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


// Header file for the GalleryBar class

#ifndef INC_GALBAR
#define INC_GALBAR

//#include "dockbar.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "basebar.h"

#ifndef WIN32
#include "dlgmgr.h"		// for DLGTEMPLATE etc
#endif

/********************************************************************************************

>	class GalleryBar : public BaseBar

	Author:		Alex Bligh
	Created:	22/6/94
	Purpose:	A subclass of BaseBar that is the oil representation of a Gallery
	SeeAlso:	GalleryBar

********************************************************************************************/

class GalleryBar : public BaseBar
{
public:
	DECLARE_DYNCREATE( GalleryBar )

	GalleryBar();
	CSize GetGallerySize(){ return GallerySize;}
	// Generated message map functions
	//{{AFX_MSG(GalleryBar)
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
//	afx_msg BOOL OnNcActivate(BOOL bActive);
//	afx_msg INT32 OnMouseActivate( CWnd* pDesktopWnd, UINT32 nHitTest, UINT32 message );
//	afx_msg void OnActivate( UINT32 nState, CWnd* pWndOther, BOOL bMinimized );
//	afx_msg void OnActivateApp( BOOL  bActive, HTASK  hTask );   
//	afx_msg void OnPaint();
	afx_msg UINT32 OnNcHitTest(CPoint point);
	afx_msg void OnMouseMove(UINT32 nFlags, CPoint point);
//	afx_msg void OnNcRButtonDown(UINT32 nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT32 nHitTest, CPoint point);
//	afx_msg void OnLButtonDown(UINT32 nFlags, CPoint point);
//	afx_msg void OnLButtonUp(UINT32 nFlags, CPoint point);
//	afx_msg void OnLButtonDblClk( UINT32 nFlags, CPoint point );
//	afx_msg void OnParentNotify( UINT32 message, LPARAM lParam );  
//	afx_msg void OnDestroy();
	afx_msg BOOL OnMouseWheel(UINT32 nFlags, short zDelta, CPoint pt);
//	afx_msg void OnSize(UINT32 nType, INT32 cx, INT32 cy);
	//}}AFX_MSG

	afx_msg LRESULT GetBarSize(WPARAM DockType , LPARAM ControlSize);
	afx_msg LRESULT SetGallerySize(WPARAM cx, LPARAM cy);
	afx_msg LRESULT DoResize(WPARAM , LPARAM);
	afx_msg LRESULT SetControlFocus(WPARAM , LPARAM State);
	DECLARE_MESSAGE_MAP()

protected:
	void PaintWin95BarNonClient(BOOL IsActive);
	
	// Format this bar according to its dock type
	virtual BOOL FormatBar(BOOL MoveWindows,ForceControlSize Force);
	
	// Format a Horizontal bar from the kernel BarItem list
	virtual BOOL FormatHorizontalBar(BOOL MoveWindows,ForceControlSize Force);

   	// Format a Vertical bar from the kernel BarItem list
	virtual BOOL FormatVerticalBar(BOOL MoveWindows,ForceControlSize Force);
	
	// Size the actual window
	virtual CRect SetBarSize(DockBarType BarType,BOOL DoRedraw = TRUE);

	// Adjust the size of a bar post format
	virtual void AdjustFormattedBarSize(CSize* BarSize);

	// Position and Format controls
	virtual BOOL PositionControls(BOOL DoRedraw = TRUE);

	// Create any extra controls
	virtual INT32 CreateExtraControls();

	// Position any extra controls
	virtual void PositionExtraControls(CSize BarSize, INT32 xpos, INT32 ypos);

	virtual BOOL SetStatusLineText();
	virtual BOOL GetStatusText(String_256 * StatusText);

	virtual CRect GetFormatRect(DockBarType BarType);

private:
	CSize	GallerySize;
	HWND	ListBoxHWND;
	CFont * ListFont;
};

#endif
