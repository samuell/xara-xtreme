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

// The ScreenCamView class - subclass of ScreenView used for screen display.

#ifndef INC_SCRCAMVW
#define INC_SCRCAMVW

#include "camview.h"
//#include "draginfo.h"
//#include "dragtrgt.h"
//#include "ink.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "clikmods.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "scrvw.h"

/*

class CCPrintInfo;
class PrintControl;
class OILHorizontalRuler;
class OILVerticalRuler;
class OriginGadget;
*/
class ExternalClipboard;

/********************************************************************************************

>	class ScreenCamView : public CCamView

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Purpose:	A subclass of CCamView which is the class actually used for screen
				display.
	SeeAlso:	CCamView; PrintCamView

********************************************************************************************/

class ScreenCamView : public ScreenView
{
////////////////////////////////////////////////////////////////////////////////
// A contained canvas to emulate MFC style views
protected:
	class ScreenCamCanvas :
		public ScreenView::ScreenCanvas
	{
	public:
		ScreenCamCanvas( wxView *v, wxMDIChildFrame *frame, const wxPoint& pos, const wxSize& size, 
			/* TYPENOTE: Correct */ long style ) : ScreenCanvas( v, frame, pos, size, style )
		{
		}
		
	////////////////////////////////////////////////////////////////////////////////////////////////
	// Message response functions.
	protected:
		void 	OnSize( wxSizeEvent &event );

	private:
		DECLARE_EVENT_TABLE()
	};

	// Creator shim, that creates a canvas suitable for this view
	virtual CCamCanvas* CreateCanvas( wxView *v, wxMDIChildFrame *frame, const wxPoint& pos, 
		const wxSize& size, /* TYPENOTE: Correct */ long style )
	{
		return new ScreenCamCanvas( v, frame, pos, size, style );
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////
// Creation and destruction.
public:
	virtual BOOL IsScreenCamView() { return TRUE; }

////////////////////////////////////////////////////////////////////////////
// OLE drag and drop
public:
PORTNOTE("other","Removed OLE usage")
#ifndef EXCLUDE_FROM_XARALX
#if (_OLE_VER >= 0x200)
	virtual DROPEFFECT OnDragEnter( COleDataObject* pDataObject, DWORD dwKeyState,
									CPoint point );
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState,
									CPoint point );
	virtual void OnDragLeave();

	virtual BOOL OnDrop(COleDataObject* pDataObject,
						DROPEFFECT dropEffect, CPoint point);
#endif

private:
#if (_OLE_VER >= 0x200)
	virtual void HandleOleDragOutOfView(CPoint point);
	COleDropTarget		m_DropTarget;		// OLE drop Target
	BOOL				m_DropFormatOK;		// Is the data format OK for dropping?
	ExternalClipboard*	m_pTempClipboard;	// Our drag and drop clipboard
	BOOL				m_ClipBoardSwapped;	// TRUE if we had to swap to the DragAndDrop clipboard
#endif
#endif
	
////////////////////////////////////////////////////////////////////////////////////////////////
// Message response functions.
protected:
    DECLARE_DYNAMIC_CLASS( ScreenCamView )
    DECLARE_EVENT_TABLE()

    bool OnCreate( wxDocument* doc, /* TYPENOTE: Correct */ long flags );
	void OnSize( wxSizeEvent& event );
};



#endif  // INC_SCRCAMVW
