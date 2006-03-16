// $Id: oilruler.h 662 2006-03-14 21:31:49Z alex $
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

#ifndef __AFXWIN_H__
#include <afxwin.h>
#endif
#ifndef __AFXDLGS_H__
#include <afxdlgs.h>
#endif

#include <afxpriv.h>
#include "guides.h"

class WinCoord;
class WinRect;
class DovView;
class ScreenCamView;
class RulerBase;
class OILHorizontalRuler;

#define RULER_BORDERS    1
#define MAX_RULER_DIGITS 3

enum MouseFollowerRenderType { RenderOn, RenderOff }; 

// WEBSTER - markn 15/1/97
// No rulers in Webster
#ifndef WEBSTER
#ifndef EXCLUDE_FROM_RALPH

/********************************************************************************************
>	class LegendLabel : public CWnd

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/94
	Purpose:	This is the class for the "set origin" gadget, 
********************************************************************************************/

class LegendLabel : public CWnd
{
	friend class  OILHorizontalRuler;
	DECLARE_DYNAMIC(LegendLabel)

public:
	LegendLabel();
	virtual	~LegendLabel();
	BOOL Create(CWnd* pParentWindow);
	
	void ShowLabel(BOOL show);
	BOOL PositionLegend();

	CPoint ClientToParentClient(CPoint point);

	BOOL GetStatusLineText(String_256* pText, CPoint MousePos, HWND hWnd);

protected:
	BOOL SetLegendText(StringBase* pText);
	BOOL SetLegendSize();

	//{{AFX_MSG(LegendLabel)
		afx_msg void OnPaint();   	
		afx_msg void OnLButtonDown(UINT32 nFlags, CPoint point);
		afx_msg void OnRButtonUp(UINT32 nFlags, CPoint point);
		afx_msg void OnLButtonDblClk(UINT32 nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT32 nFlags, CPoint point);
   	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	UINT32 LegendWidth;
	UINT32 LegendHeight;
	String_256 LegendText;
};


/********************************************************************************************
>	class OILRuler : public CWnd

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/94
	Purpose:	This is the base class for Horizontal and Vertical rulers,
				It looks after the basic's - creating windows etc..
				and handles all mouse events fo the rulers 
*********************************************************************************************/

class OILRuler : public CWnd
{
	DECLARE_DYNAMIC(OILRuler)

public:
	OILRuler();
	virtual	~OILRuler();
	virtual BOOL PostCreate() =0;
	BOOL Create(CWnd* pParentWindow);	
	void LinkToKernel(RulerBase* pKRuler) { pKernelRuler=pKRuler; }

	static BOOL GetMinGraticuleSpacing(OilRect* pSpacing, DocView* pDocView);
	static BOOL GetTextSize(OilRect* pTextSize, StringBase* pText, DocView* pDocView);
	static BOOL GetTextSize(WinRect* pTextSize, StringBase* pText);

	CPoint  ClientToOtherClient(CWnd* pOtherCWnd, CPoint point);
	WinRect ClientToOtherClient(CWnd* pOtherCWnd, WinRect  WR);

	OilCoord ClientToOil(DocView* pDocView, CPoint point);
	OilRect  ClientToOil(DocView* pDocView, WinRect  WR);

	virtual BOOL DrawMajorGraticule(OilCoord GratOilPos, StringBase* pGratLabel) =0;
	virtual BOOL DrawMinorGraticule(OilCoord GratOilPos, INT32 ExtraSize=0);

	BOOL PaintMouseFollower(OilCoord OilPos, DocView* pDocView, MouseFollowerRenderType RenderType);
	BOOL  DrawMouseFollower(OilCoord OilPos, DocView* pDocView, MouseFollowerRenderType RenderType, CDC* pDC);

	BOOL GetStatusLineText(String_256* pText, CPoint MousePos, HWND hWnd);

	void ShowRuler(BOOL show);
	void UpdateRuler();

	static BOOL Init();
	static void Deinit();
	static INT32 GetRenderWidth() { return RenderWidth; }
	static UINT32 GetWidth()       { return RulerWidth; }

protected:
	virtual BOOL IsHorizontal() =0;
	virtual BOOL StartDrag(UINT32 nFlags,CPoint point);

	//{{AFX_MSG(OILRuler)
		afx_msg void OnPaint();   	
   		afx_msg void OnLButtonDown(UINT32 nFlags, CPoint point);
		afx_msg void OnRButtonUp(UINT32 nFlags, CPoint point);
		afx_msg void OnLButtonDblClk(UINT32 nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT32 nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	static String_256 *FontName;		// read from .ini file
	static INT32        FontSize;

protected:
	static INT32 RenderWidth;	// renderable width of ruler, determined by the font specified in the .ini
	static UINT32 RulerWidth;		// offset to next bar - NOT NECESSARILY THE PHYSICAL OR LOGICAL WIDTH OF THE RULER!
	static INT32 CharHeight;
	static INT32 CharWidth;

	// vars to pass info from OIL OnPaint() to low level OIL render funtions, transparent to intermediate kernel code
	static CPaintDC* pPaintDC;
	static DocView*  pPaintDocView;
	static CSize     RulerToDocOffset;

	static OpGuidelineParam NewGuidelineParam;

	ScreenCamView* pParentWnd;
	RulerBase*     pKernelRuler;

	BOOL InDrag;
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
	DECLARE_DYNAMIC(OILHorizontalRuler)

public:
	OILHorizontalRuler();
	~OILHorizontalRuler();
	virtual BOOL PostCreate();

	void CalcPosFromParentClient(LPRECT lpRect);
	void ScrollRuler(INT32 amount);

	virtual BOOL DrawMajorGraticule(OilCoord GratOilPos, StringBase* pGratLabel);

	BOOL PositionLegend();
	BOOL SetLegendText(StringBase* pText);

	LegendLabel* GetpLegendLabel() { return pLegend; }

protected:
	virtual BOOL StartDrag(UINT32 nFlags,CPoint point);
	virtual BOOL IsHorizontal(){ return TRUE ;};

	//{{AFX_MSG(OILHorizontalRuler)
   	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

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
	DECLARE_DYNAMIC(OILVerticalRuler)

public:
	OILVerticalRuler();
	virtual BOOL PostCreate();

	void CalcPosFromParentClient(LPRECT lpRect);
	void ScrollRuler(INT32 amount);

	virtual BOOL DrawMajorGraticule(OilCoord GratOilPos, StringBase* pGratLabel);

protected:
	virtual BOOL StartDrag(UINT32 nFlags,CPoint point);
	virtual BOOL IsHorizontal(){ return FALSE ;};

	//{{AFX_MSG(OILVerticalRuler)
   	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/********************************************************************************************
>	class OriginGadget : public CWnd

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/94
	Purpose:	This is the class for the "set origin" gadget, 
********************************************************************************************/

class OriginGadget : public CWnd
{
	DECLARE_DYNAMIC(OriginGadget)

public:
	OriginGadget();
	virtual	~OriginGadget();
	BOOL Create(CWnd* pParentWindow);
	
	void ShowGadget(BOOL show);
	void CalcPosFromParentClient(LPRECT lpRect);

	BOOL GetStatusLineText(String_256* pText, CPoint MousePos, HWND hWnd);
	static TCHAR* BubbleHelpCallBack(HWND hWnd, UINT32 PaneIndex, void* UserData);
	BOOL PlotMaskedBitmap(CDC* destDC, CBitmap* srcBitmap, INT32 SrcOffset,
								INT32 xPlotOffset,INT32 yPlotOffset, INT32 width, INT32 height);
protected:
	//{{AFX_MSG(OriginGadget)
		afx_msg void OnPaint();   	
		afx_msg void OnLButtonDown(UINT32 nFlags, CPoint point);
		afx_msg void OnRButtonUp(UINT32 nFlags, CPoint point);
		afx_msg void OnLButtonDblClk(UINT32 nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT32 nFlags, CPoint point);
   	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	ScreenCamView* pParentWnd;

	CBitmap* pGlyph;
};

#endif
#endif	// WEBSTER
#endif	// INC_OILRULERS
