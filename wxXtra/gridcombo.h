///////////////////////////////////////////////////////////////////////////////
// Name:        gridcombo.h
// Purpose:     A grid grid combo
// Author:      Mikhail Tatarnikov
// Modified by:
// Created:     2006-08-16
// RCS-ID:      $Id: $
// Copyright:   (c) 2006 Xara Ltd
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WXXTRA_GRIDCOMBO_H_
#define _WXXTRA_GRIDCOMBO_H_

#include <wx/defs.h>
//#include <wx/grid.h>
#include <vector>
#include <wx/scrolwin.h>
#include <wx/timer.h>
#include <wx/settings.h>

#if wxUSE_COMBOCTRL
#include <wx/combo.h>
#else
#include "combog.h"
#endif


class wxGridCombo;



/*************************************************************************
Class         : wxGridComboPopup
Base Class    : public wxScrolledWindow
				public wxComboPopup
Author        : Mikhail Tatarnikov
Description   : A base class for wxComboPopup implementation implementing
				a grid-like popup (multicolmn as opposint to a standard one-column
				list box popup).
Pure Virtual  : DrawItem - you have to override it and do the actuall cell paint in it.
Known Issues  : None
Usage Notes   : All the information that pertain to the control as a whole (a combobox and popup)
				is stored in the combobox itself. A number of access functions (e.g. GetColumns)
				are provided.
				An item can be in selected or highlighted (when the mouse is hovered over the item)
				state.
Override Notes: None
**************************************************************************/
class wxGridComboPopup : public wxScrolledWindow,
						 public wxComboPopup
{
	friend class wxGridCombo;
public:
	enum EDrawFlags
	{
		keSelected	   = 0x0001,	// The item is selected.
		keHighlight	   = 0x0002,	// The item is hovered.
		keComboControl = 0x0004,	// The paint the item in the combo control itself.
		keDisabled	   = 0x0008		// The item is disabled. For now can be applied for the combo
									// control itself only.
	};
	
public:

	wxGridComboPopup(wxGridCombo* pCombo);

    virtual void Init();
    virtual bool Create(wxWindow* pwndParent);

    virtual void	  OnShow();
    virtual wxSize	  GetAdjustedSize(int iMinWidth, int iPrefHeight, int iMaxHeight);
    virtual wxWindow* GetControl() {return this;}

	// Drawing functions.
	virtual void DrawItem(wxDC& dc, const wxRect& rect, int iItem, int iFlags) = 0;
    virtual void PaintComboControl(wxDC& dc, const wxRect& rect);

	virtual void SetSelected(int iItem);
	virtual int	 GetSelected() const;


	// Helper function
	inline int	  GetColumns() const;
	inline int	  GetRows() const;
	inline int	  GetItemsNum() const;
	inline wxSize GetItemSize() const;


    // Gets displayed string representation of the value.
    // Since we are owner-drawn control, the string doen't matter much.
    virtual wxString GetStringValue() const {return wxString();}

protected:
	virtual void DrawItem(wxDC& dc, const wxRect& rect, int iCol, int iRow, bool bSelected);
	virtual void UpdateScrollers();
	virtual void UpdateColumnsNum();
	virtual void ChangeSelection(int iOldSelected, int iNewSelected);
	virtual void SendComboBoxEvent(int iSelection);

	wxRect  GetItemRect(int iItem);
	wxPoint ItemToGrid(int iItem);
	int		ItemFromPoint(wxPoint ptClient);
	void	InvalidateItem(int iItem);
	void 	ClientToScrolledVirtual(int* pX, int* pY);

	// Closes the popup and sends notig
    void DismissWithEvent();

	// Message handlers.
	virtual void OnDraw(wxDC& dc);
	void OnSize(wxSizeEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnKey(wxKeyEvent& event);
    void OnLeftClick(wxMouseEvent& event);
	void OnTimer(wxTimerEvent& event);

protected:
	wxGridCombo* m_pCombo;
	int			 m_iHighlighed;
	wxTimer		 m_tmrHighlited;	// Need a timer to spot the moment the mouse leaves popup (to dehighlight an item).
private:
    DECLARE_EVENT_TABLE()
};












struct CGridComboUserData
{
	virtual ~CGridComboUserData(){};
};



/*************************************************************************
Class         : wxGridCombo
Base Class    : public wxComboCtrl
Author        : Mikhail Tatarnikov
Description   : wxWidgit control extension that implements grid-like behaivor. Uses wxGridComboPopup
				or its derivitives.
Pure Virtual  : None
Known Issues  : None
Usage Notes   : None
Override Notes: 
**************************************************************************/
class WXDLLIMPEXP_ADV wxGridCombo : public wxComboCtrl
{
//	friend class wxGridComboPopup;
public:

	wxGridCombo(){Init();}

	wxGridCombo(wxWindow* pwndParent,
				wxWindowID wiID = wxID_ANY,
				const wxPoint& crptPos = wxDefaultPosition,
				const wxSize& crszSize = wxDefaultSize,
				long lStyle = 0,
				const wxString& crstrName = wxT("wxSliderComboBox"));

	bool Create(wxWindow* pwndParent,
				wxWindowID wiID = wxID_ANY,
				const wxPoint& crptPos = wxDefaultPosition,
				const wxSize& crszSize = wxDefaultSize,
				long lStyle = 0,
				const wxString& crstrName = wxT("wxSliderComboBox"));

	virtual ~wxGridCombo();


	virtual void AddItem(CGridComboUserData* pUserData);
	virtual CGridComboUserData* GetUserData(int iIndex);

	virtual void Clear();
	virtual void DeleteUserData();


	void SetColumns(int iCols);
	int	GetColumns() const {return m_iColumns;}
	int	GetRows();

	void SetSelected(int iSelected);
	int  GetSelected() const;

	int	GetItemsNum() const {return m_vecUserData.size();}

	void   SetItemSize(wxSize szItem);
	wxSize GetItemSize() const {return m_szItem;}


protected:
	void Init();

protected:

	std::vector<CGridComboUserData*> m_vecUserData;

private:
	int m_iColumns;

	int m_iSelected;

	wxSize m_szItem;	// The item size. In grid we can have items with the same size only. TODO: allow rows/columns with different sizes?

	DECLARE_DYNAMIC_CLASS(wxGridCombo)
};

#endif //_WXXTRA_GRIDCOMBO_H_
