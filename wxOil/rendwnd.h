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
//
// This file declares the CRendWnd class, into which all document rendering
// is done.
//
/*
 *  */

#ifndef RENDWND__INC
#define RENDWND__INC

// A private message
#define WM_RENDERVIEW	(WM_USER + 200)

#define OPTOKEN_TOGGLEDOUBLEBUFFER		TEXT("ToggleDoubleBuffer")

class CCamView;

/////////////////////////////////////////////////////////////////////////////
// CRenderWnd window

/***************************************************************************************
>	class CRenderWnd : public wxWindow

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Purpose:	This OIL class is rendered into.  It passes all input to its parent
				CCamView window.
				
****************************************************************************************/
class CRenderWnd : public wxWindow
{
	DECLARE_DYNAMIC_CLASS(CRenderWnd)

public:
	CRenderWnd(CCamView* pView = NULL);
	virtual ~CRenderWnd();

/////////////////////////////////////////////////////////////////////////////
// Construction and destruction.

public:
	virtual BOOL Create(const wxRect& rect, wxWindow* parent, UINT32 id);
	void SetView(CCamView* pView) { m_pView = pView; }
	CCamView* GetView(void) { return(m_pView); }

	static void SetDoubleBuffer (BOOL DoubleBuffer);
	static BOOL GetDoubleBuffer () {return m_DoubleBuffer;}

/////////////////////////////////////////////////////////////////////////////
// Generated message map functions

protected:
	void OnPaint( wxPaintEvent &evnt );
	void OnLButtonDown( wxMouseEvent &evnt );
	void OnLButtonDblClk( wxMouseEvent &evnt );
	void OnLButtonUp( wxMouseEvent &evnt );
	void OnMouseMove( wxMouseEvent &evnt );
	void OnRButtonDown( wxMouseEvent &evnt );
	void OnRButtonDblClk( wxMouseEvent &evnt );
	void OnRButtonUp( wxMouseEvent &evnt );
	void OnMButtonDown( wxMouseEvent &evnt );
	void OnMButtonDblClk( wxMouseEvent &evnt );
	void OnMButtonUp( wxMouseEvent &evnt );
	void OnMouseWheel( wxMouseEvent &evnt );

    void OnSize ( wxSizeEvent &event );
    void OnErase ( wxEraseEvent &event );

	void OnSetCursor( wxSetCursorEvent &event );

#if defined(__WXGTK__)
	void OnEnter( wxMouseEvent &event );
	void OnLeave( wxMouseEvent &event );
#endif

	void OnKey ( wxKeyEvent & event);

protected:
	CCamView* m_pView;

	static BOOL m_DoubleBuffer;
	static void ReflectDoubleBufferingInChildren(wxWindow * pWindow);

	DECLARE_EVENT_TABLE()
};


#if WIN32

const INT32 MAX_RECT_LIST=4;

typedef struct
{
	RGNDATAHEADER rdh;
	RECT rectlist[MAX_RECT_LIST];
} CCRGNDATA;

#endif

UINT32 GetRectangleList( wxDC* pCDC, LPRECT *lplpRect );

/////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	class OpToggleDoubleBuffer : public Operation

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14 Mar 2006
	Purpose:	This class represents the OpToggleDoubleBuffer operation.
				Creating an instance of this class and calling its "Do" function will toggle
				the state of the flag controlling whether double buffering is used for
				rendering on GTK

********************************************************************************************/

class OpToggleDoubleBuffer: public Operation
{
	CC_DECLARE_DYNCREATE( OpToggleDoubleBuffer )
	
public:
	OpToggleDoubleBuffer();											// Constructor
	~OpToggleDoubleBuffer();										// Destructor

	static BOOL		Init();									// Register an OpDescriptor
	void			Do(OpDescriptor*);						// "Do" function
	static OpState	GetState(String_256*, OpDescriptor*);	// Read the state of an operation
};


#endif	// RENDWND__INC
