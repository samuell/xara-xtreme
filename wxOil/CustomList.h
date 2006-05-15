// $Id: wxOil/CustomList.h, 1, 01-Jan-2006, Anonymous $
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
#if !defined(AFX_CUSTOMLIST_H__32AE552D_C07B_4C09_9030_AB20E842FFD6__INCLUDED_)
#define AFX_CUSTOMLIST_H__32AE552D_C07B_4C09_9030_AB20E842FFD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomList.h : header file
//

class CCustomListScrollableArea;
class CCustomListRowWnd;

/////////////////////////////////////////////////////////////////////////////
// CCustomList - Custom List Control class. Hosts a vertical scrollbar and
// a CCustomListScrollableArea

class CCustomList : public CWnd
{
	friend class CCustomListScrollableArea;
	friend class CCustomListRowWnd;
public:
//	constants
	static const INT32 MAXCOLUMNS ;
	static const INT32 MAXROWS ;
	static const INT32 FONTHEIGHT;
	static const INT32 ROWHEIGHT ;
	static const INT32 COLOUR_PATCH_WIDTH ;
	static const INT32 COLOUR_PATCH_HEIGHT;
	static const CString WNDCLASSNAME;


public:
// constructor & destructor
	CCustomList();
	virtual ~CCustomList();
	static BOOL RegisterWindowClass();
	static LRESULT CALLBACK EXPORT CustomWindowProc(HWND hWnd, UINT32 nMsg, WPARAM wParam,LPARAM lParam);
	void SetColumnWidth(INT32 colnum,INT32 offset);

	// The following block of public methods is the API actually used by Camelot. It is
	// almost identical to the original CCListGadget interface which it replaces

	static CCustomList*	GetGadget(CWindowID parentID, CGadgetID gadgetID);
	BOOL CreateCustomHeader (UINT32 bitmapID);

	BOOL GetSwitchState(UINT32 itemIndex, UINT32 switchIndex) const;
	INT32	 GetItemCount() const;
	BOOL GetItemString(StringBase& itemString, UINT32 itemIndex, UINT32 columnIndex) const;
	INT32	 GetSelectedItemIndex() const;

	BOOL AddItem(StringBase& itemString, KernelBitmap* pItemImage = 0);
	BOOL AddItem(StringBase& itemString, UINT32 bitmapEnabledID, UINT32 bitmapDisabledID);
	BOOL AddRefsItem(UINT32 idStatusBitmap, StringBase& strItemName, StringBase& strDetails);
	BOOL AddColourListItem(StringBase& colourName, INT32 red, INT32 green, INT32 blue, BOOL IsSpotColour = FALSE);
	BOOL SetSwitchState(BOOL state, UINT32 itemIndex, UINT32 switchIndex);
	void SetSelectedItemIndex(INT32 NewSel);
	BOOL SetItemString(StringBase& itemString, UINT32 itemIndex, UINT32 columnIndex);
	BOOL SetEnabled(BOOL enabled);
	BOOL DeleteAllItems();

protected:
	//{{AFX_VIRTUAL(CCustomList)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCustomList)
	afx_msg INT32 OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnVScroll(UINT32 nSBCode, UINT32 nPos, CScrollBar* pScrollBar);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnMouseWheel(UINT32 nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	
	void NewScrollableArea();	// creates a new scrollable area wnd
	CScrollBar*					GetVScrollBar()		const {return m_VScrollBar;}     // get vertical scrollbar
	CCustomListScrollableArea*	GetScrollableArea() const {return m_ScrollableArea;} // get the scrollable area 

	CCustomListScrollableArea*	m_ScrollableArea;			// the scrollable area wnd
	CScrollBar*					m_VScrollBar;				// the scrollbar
	INT32*						m_ColumnOffsetsArray;		// array of column offsets
	CBitmap						m_hHeaderBitmap;			// header bitmap 
	CStatic						m_hHeader;					// header window 
};

/////////////////////////////////////////////////////////////////////////////
// CCustomListScrollableArea window .  Represents the scrollable area and
// hosts the list row wnds.

class CCustomListScrollableArea : public CWnd
{
	friend class CCustomListRowWnd;
	friend class CCustomList;
	;
protected:
	// constructor, destructor
	CCustomListScrollableArea(CCustomList* parent);
	virtual ~CCustomListScrollableArea();

	CCustomListRowWnd*	AddRow();
	void				SelectRow(INT32 RowNum);
	void				HandleScrollMessage(UINT32 nSBCode, UINT32 nPos);
	CCustomListRowWnd*	GetRow(INT32 row);
	INT32					GetHeight() const {return m_RowCount * CCustomList::ROWHEIGHT ;}



protected:
	// Generated message map functions
	//{{AFX_VIRTUAL(CCustomListScrollableArea)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	//{{AFX_MSG(CCustomListScrollableArea)
	afx_msg void OnSize(UINT32 nType, INT32 cx, INT32 cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CCustomListRowWnd** m_ListRowsArray;		// the array of row wnd objects
	INT32					m_ScrollPos ;			// the current scroll pos
	INT32					m_CurrentSelectedRow;	// the currently selected row
	CFont				m_Font;					// the font
	INT32					m_RowCount;				// current row count 
	CCustomList* m_Parent;						// the parent CCustomList
};

/////////////////////////////////////////////////////////////////////////////
// CCustomListRowWnd window. A single row object within a scrollable area
// object. Contains text, controls and bitmaps in column positions.

class CCustomListRowWnd : public CWnd
{
	friend CCustomListScrollableArea; // grant full access to my parent
public:
	// construct with the parent Scrollable Area
	CCustomListRowWnd	(INT32 i, CCustomListScrollableArea* parent);
	virtual ~CCustomListRowWnd();

	void AddText(INT32 col, CString text);
	void SetText(INT32 col, CString text);
	void AddCheck(INT32 col);
	void AddBitmap(INT32 col, HBITMAP bmp1, HBITMAP bmp2, DWORD dwBackColour=0xFFFFFFFF);
	void SetChecked(INT32 col,BOOL checked);
	bool IsChecked(INT32 col) const;
	CString GetText(INT32 col) const;

private:
	HBRUSH GetBackgroundBrush(){return (m_Selected ? m_BackBrushSel:m_BackBrush ); }

	const INT32 m_RowNum;						// index of this row in the list
	CCustomListScrollableArea* m_Parent;	// pointer to our parent
	CBrush m_BackBrush;						// background brush
	CBrush m_BackBrushSel;					// background brush to use when selected
	CObject** m_ColumnObjects;				// array of our column aligned objects
	bool m_Selected;						// true if this row is selected


protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomListRowWnd)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Generated message map functions

	//{{AFX_MSG(CCustomListRowWnd)
	afx_msg void OnLButtonUp(UINT32 nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT32 nCtlColor);
	afx_msg void OnLButtonDblClk(UINT32 nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT32 nChar, UINT32 nRepCnt, UINT32 nFlags);
	afx_msg UINT32 OnGetDlgCode();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMLIST_H__32AE552D_C07B_4C09_9030_AB20E842FFD6__INCLUDED_)
