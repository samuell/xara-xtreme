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
// oil ruler class

#ifndef INC_OILRULERS
#define INC_OILRULERS

#include "guides.h"
#include "camview.h"

class WinCoord;
class WinRect;
class DovView;
class RulerBase;
class OILHorizontalRuler;
class CRenderWnd;

#define MAX_RULER_DIGITS 3

enum MouseFollowerRenderType { RenderOn, RenderOff }; 

// WEBSTER - markn 15/1/97
// No rulers in Webster
#ifndef WEBSTER
#ifndef EXCLUDE_FROM_RALPH

/********************************************************************************************
>	class LegendLabel : public wxWindow

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/94
	Purpose:	This is the class for the "set origin" gadget, 
********************************************************************************************/

class LegendLabel : public wxWindow
{
	friend class  OILHorizontalRuler;
	DECLARE_DYNAMIC_CLASS(LegendLabel)

public:
	LegendLabel();
	virtual	~LegendLabel();
	BOOL Create(OILHorizontalRuler* pHorzRuler, wxWindow* pParentWindow, INT32 id);
	
	void ShowLabel(BOOL show);
	BOOL PositionLegend();

	wxPoint ClientToParentClient(wxPoint point);

	BOOL GetStatusLineText(String_256* pText, WinCoord MousePos, CWindowID hWnd);

protected:
	BOOL SetLegendText(StringBase* pText);
	BOOL SetLegendSize();

protected:
	// wxWindows OIL message handlers and related functions
	void DoPaint(wxDC* pDC);

	void OnPaint(wxPaintEvent &event);

	void OnSize(wxSizeEvent &event);
	void OnLButtonDown(wxMouseEvent& event);
	void OnLButtonDblClk(wxMouseEvent& event);
	void OnRButtonUp(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);

protected:
	UINT32 LegendWidth;
	UINT32 LegendHeight;
	String_256 LegendText;
	OILHorizontalRuler* m_pHorzRuler;

	DECLARE_EVENT_TABLE()
};


/********************************************************************************************
>	class OILRuler : public wxWindow

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/94
	Purpose:	This is the base class for Horizontal and Vertical rulers,
				It looks after the basic's - creating windows etc..
				and handles all mouse events fo the rulers 
*********************************************************************************************/

class OILRuler : public wxWindow
{
	DECLARE_DYNAMIC_CLASS(OILRuler)

public:
	OILRuler();
	virtual	~OILRuler();
	virtual BOOL PostCreate() {return FALSE;}
	BOOL Create(CCamView* pOwnerView, INT32 id);
	void LinkToKernel(RulerBase* pKRuler) { pKernelRuler=pKRuler; }

	static BOOL GetMinGraticuleSpacing(OilRect* pSpacing, DocView* pDocView);
	static BOOL GetTextSize(OilRect* pTextSize, LPCTSTR str, DocView* pDocView);
	static BOOL GetTextSize(WinRect* pTextSize, LPCTSTR str);
	static void PatB(wxDC* pDC, INT32 x, INT32 y, INT32 dx, INT32 dy, wxColour rgb);

	wxPoint  ClientToOtherClient(wxWindow* pOtherCWnd, wxPoint point);
	WinRect ClientToOtherClient(wxWindow* pOtherCWnd, WinRect  WR);

	OilCoord ClientToOil(DocView* pDocView, wxPoint point);
	OilRect  ClientToOil(DocView* pDocView, WinRect  WR);

	virtual BOOL DrawMajorGraticule(OilCoord GratOilPos, LPCTSTR str) {return FALSE;}
	virtual BOOL DrawMinorGraticule(OilCoord GratOilPos, INT32 ExtraSize=0);
	BOOL HighlightSection(OilCoord Lo, OilCoord Hi);
	BOOL DrawBitmap(OilCoord Pos, ResourceID BitmapID);
	BOOL StartToolDrag(ClickModifiers Mods, OilCoord point, String_256* OpToken, OpParam* Param);

	BOOL PaintMouseFollower(OilCoord OilPos, DocView* pDocView, MouseFollowerRenderType RenderType);
	BOOL  DrawMouseFollower(OilCoord OilPos, DocView* pDocView, MouseFollowerRenderType RenderType, wxDC* pDC);

	BOOL GetStatusLineText(String_256* pText, WinCoord MousePos, CWindowID hWnd);

	virtual void ShowRuler(BOOL show);
	void UpdateRuler();

	static BOOL Init();
	static void Deinit();
	static INT32 GetRenderWidth() { return RenderWidth; }
	static UINT32 GetWidth()       { return RulerWidth; }
	static wxFont GetRulerFont();

protected:
	virtual BOOL IsHorizontal() {return FALSE;}
	virtual BOOL StartDrag(UINT32 nFlags, wxPoint point);
	virtual BOOL HandleRulerDragEvent(UINT32 Button, UINT32 nFlags, WinCoord point, ClickType t);
	virtual BOOL HandleRulerUpEvent(UINT32 Button, WinCoord point);

	CRenderWnd* GetRenderWindow() {return m_pOwnerView ? m_pOwnerView->GetRenderWindow() : NULL;}
	void SetCurrentStates();

protected:
	// wxWindows OIL message handlers and related functions
	void DoPaint(wxDC* pDC);

	void OnPaint(wxPaintEvent &event);

//	void OnLButtonDown(wxMouseEvent& event);
//	void OnLButtonDblClk(wxMouseEvent& event);
//	void OnRButtonUp(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);

	void 	OnLButtonDown( wxMouseEvent &event );
	void 	OnLButtonDblClk( wxMouseEvent &event );
	void 	OnLButtonUp( wxMouseEvent &event );
	void 	OnMButtonDown( wxMouseEvent &event );
	void 	OnMButtonDblClk( wxMouseEvent &event );
	void 	OnMButtonUp( wxMouseEvent &event );
	void 	OnRButtonDown( wxMouseEvent &event );
	void 	OnRButtonDblClk( wxMouseEvent &event );
	void 	OnRButtonUp( wxMouseEvent &event );

	void	OnFocus( wxFocusEvent& event );

public:
	static String_256*	FontName;		// read from .ini file
	static INT32        FontSize;

protected:
	static INT32 		RenderWidth;	// renderable width of ruler, determined by the font specified in the .ini
	static UINT32 		RulerWidth;		// offset to next bar - NOT NECESSARILY THE PHYSICAL OR LOGICAL WIDTH OF THE RULER!
	static INT32 		CharHeight;
	static INT32 		CharWidth;

	// vars to pass info from OIL OnPaint() to low level OIL render funtions, transparent to intermediate kernel code
	// This is a bit NASTY!
	static wxDC* 		pPaintDC;
	static DocView*  	pPaintDocView;
	static wxSize     	RulerToDocOffset;

	static OpGuidelineParam NewGuidelineParam;

	CCamView*			m_pOwnerView;
	RulerBase*			pKernelRuler;

	ClickType			m_LastClickType;		// click type of last START_DRAG
	UINT32				m_LastClickButton;		// buttons down on last START_DRAG
	wxPoint				m_LastClickPoint;		// mouse position of last START_DRAG
	ClickModifiers		m_LastClickMods;		// ClickMods on last mouse event (NOT modified by button-up events!)
	UINT32 				m_FirstClickButton;		// Which button started the drag (if dragging).

	BOOL InDrag;

	DECLARE_EVENT_TABLE()
};


/********************************************************************************************
>	class OILHorizontalRuler : public OILRuler
	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/94
	Purpose:	This class handles the OIL-specific parts of Horizontal rulers.
********************************************************************************************/

class OILHorizontalRuler : public OILRuler
{
	friend class LegendLabel;
	DECLARE_DYNAMIC_CLASS(OILHorizontalRuler)

public:
	OILHorizontalRuler();
	~OILHorizontalRuler();
	virtual BOOL PostCreate();

	void CalcPosFromParentClient(WinRect* lpRect);
	void ScrollRuler(INT32 amount);

	virtual void ShowRuler(BOOL show);
	virtual BOOL DrawMajorGraticule(OilCoord GratOilPos, LPCTSTR str);

	BOOL PositionLegend();
	BOOL SetLegendText(StringBase* pText);

	LegendLabel* GetpLegendLabel() { return pLegend; }

protected:
	virtual BOOL StartDrag(UINT32 nFlags, wxPoint point);
	virtual BOOL IsHorizontal(){ return TRUE ;};

private:
	LegendLabel* pLegend;
};


/********************************************************************************************
>	class OILVerticalRuler : public OILRuler
	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/94
	Purpose:	This class handles the OIL-specific parts of Horizontal rulers.
********************************************************************************************/

class OILVerticalRuler : public OILRuler
{
	DECLARE_DYNAMIC_CLASS(OILVerticalRuler)

public:
	OILVerticalRuler();
	virtual BOOL PostCreate();

	void CalcPosFromParentClient(WinRect* lpRect);
	void ScrollRuler(INT32 amount);

	virtual BOOL DrawMajorGraticule(OilCoord GratOilPos, LPCTSTR str);

protected:
	virtual BOOL StartDrag(UINT32 nFlags, wxPoint point);
	virtual BOOL IsHorizontal(){ return FALSE ;};

};


/********************************************************************************************
>	class OriginGadget : public wxWindow

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/94
	Purpose:	This is the class for the "set origin" gadget, 
********************************************************************************************/

class OriginGadget : public wxWindow
{
	DECLARE_DYNAMIC_CLASS(OriginGadget)

public:
	OriginGadget();
	virtual	~OriginGadget();
	BOOL Create(CCamView* pOwnerView, INT32 id);
	
	void ShowGadget(BOOL show);
	void CalcPosFromParentClient(WinRect* lpRect);

	BOOL GetStatusLineText(String_256* pText, WinCoord MousePos, CWindowID hWnd);
#if !defined(EXCLUDE_FROM_XARALX)
	static TCHAR* BubbleHelpCallBack(CWindowID hWnd, UINT32 PaneIndex, void* UserData);
#endif

protected:
	// Windows OIL message handlers and related functions
	void DoPaint(wxDC* pDC);

	void OnPaint(wxPaintEvent &event);

	void OnLButtonDown(wxMouseEvent& event);
	void OnLButtonDblClk(wxMouseEvent& event);
	void OnRButtonUp(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);

protected:
	CCamView* m_pOwnerView;

	wxBitmap* pGlyph;

	DECLARE_EVENT_TABLE()
};

#endif
#endif	// WEBSTER
#endif	// INC_OILRULERS
