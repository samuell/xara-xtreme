///////////////////////////////////////////////////////////////////////////////
// Name:        slidercombo.cpp
// Purpose:     A pop-up slider combo control
// Author:      Mart Raudsepp
// Modified by:
// Created:     2006-07-06
// RCS-ID:      $Id: $
// Copyright:   (c) 2006 Mart Raudsepp
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "slidercombo.h"

#include <wx/slider.h>

class wxSliderComboPopup : public wxSlider, public wxComboPopup
{
public:

    virtual void Init()
    {
    }

    virtual bool Create( wxWindow* parent )
    {
        long style = m_combo->GetWindowStyleFlag();
        style &= wxSL_HORIZONTAL | wxSL_VERTICAL | wxSL_LABELS | wxSL_INVERSE;
        return wxSlider::Create(parent, wxID_ANY,
                                50, 0, 100,
                                wxDefaultPosition, wxDefaultSize,
                                style | wxRAISED_BORDER);
    }

    virtual void OnShow()
    {
    }

    virtual wxSize GetAdjustedSize( int minWidth,
                                    int WXUNUSED(prefHeight),
                                    int maxHeight )
    {
        wxSize sz = GetBestSize();
        return wxSize(wxMax(120, minWidth), wxMin(sz.GetHeight(), maxHeight));
    }

    virtual wxWindow *GetControl() { return this; }

    virtual void SetStringValue( const wxString& s )
    {
    }

    virtual wxString GetStringValue() const
    {
        // FIXME: Does this get called in wxGTK? On wxMSW this gets called on popup close
        return m_combo->GetValue();
    }

protected:
    void OnSliderScrollEvent(wxScrollEvent& event)
    {
        wxScrollEvent ev( event.GetEventType(),
                          m_combo->GetId(),
                          event.GetPosition(),
                          event.GetOrientation() );
        ev.SetEventObject( m_combo );
        m_combo->GetEventHandler()->ProcessEvent( ev );
        if (event.GetEventType() == wxEVT_SCROLL_THUMBRELEASE)
            m_combo->HidePopup();
    }

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxSliderComboPopup, wxSlider)
    EVT_SCROLL(wxSliderComboPopup::OnSliderScrollEvent)
END_EVENT_TABLE()


IMPLEMENT_DYNAMIC_CLASS(wxSliderCombo, wxComboCtrl)

void wxSliderCombo::Init()
{
    m_slider = NULL;
    m_suffix = wxEmptyString;
    m_automatic_updates = true;
}

wxSliderCombo::wxSliderCombo(wxWindow *parent, wxWindowID id, int value,
                             const wxPoint& pos, const wxSize& size,
                             long style, const wxString& name)
// FIXME: initial string and validator should maybe be in our ctor too?
{
    Init();
    Create(parent, id, value, pos, size, style, name);
}

bool wxSliderCombo::Create(wxWindow *parent, wxWindowID id,
                           int value,
                           const wxPoint& pos, const wxSize& size,
                           long style, const wxString& name)
{
    if (wxComboCtrl::Create(parent, id, wxEmptyString, pos, size, style, wxDefaultValidator, name))
    {
        wxSliderComboPopup* sliderpopup = new wxSliderComboPopup();
        m_slider = sliderpopup;
        SetPopupControl(sliderpopup);
        m_slider->SetValue(value);
        return true;
    }
    return false;
}

wxSliderCombo::~wxSliderCombo()
{
}

wxString wxSliderCombo::GetValueSuffix() const
{
    return m_suffix;
}

void wxSliderCombo::SetValueSuffix(wxString& suffix)
{
    m_suffix = suffix;
}

bool wxSliderCombo::GetAutomaticUpdates() const
{
    return m_automatic_updates;
}

void wxSliderCombo::SetAutomaticUpdates(bool automatic_updates)
{
    m_automatic_updates = automatic_updates;
}

int wxSliderCombo::GetSliderValue() const
{
    return m_slider->GetValue();
}

void wxSliderCombo::SetSliderValue(int value)
{
    m_slider->SetValue(value);
}

void wxSliderCombo::SetRange(int minValue, int maxValue)
{
    m_slider->SetRange(minValue, maxValue);
    // FIXME: Perhaps need to sync with text control
}

int wxSliderCombo::GetSliderMin() const
{
    return m_slider->GetMin();
}

int wxSliderCombo::GetSliderMax() const
{
    return m_slider->GetMax();
}

void wxSliderCombo::SetSliderMin( int minValue )
{
    m_slider->SetMin(minValue);
}

void wxSliderCombo::SetSliderMax( int maxValue )
{
    m_slider->SetMax(maxValue);
}

void wxSliderCombo::SetLineSize(int lineSize)
{
    m_slider->SetLineSize(lineSize);
}

void wxSliderCombo::SetPageSize(int pageSize)
{
    m_slider->SetPageSize(pageSize);
}

int wxSliderCombo::GetLineSize() const
{
    return m_slider->GetLineSize();
}

int wxSliderCombo::GetPageSize() const
{
    return m_slider->GetPageSize();
}

void wxSliderCombo::SetThumbLength(int lenPixels)
{
    m_slider->SetThumbLength(lenPixels);
}

int wxSliderCombo::GetThumbLength() const
{
    return m_slider->GetThumbLength();
}

void wxSliderCombo::SetTickFreq(int n, int pos)
{
    m_slider->SetTickFreq(n, pos);
}

int wxSliderCombo::GetTickFreq() const
{
    return m_slider->GetTickFreq();
}

void wxSliderCombo::ClearTicks()
{
    m_slider->ClearTicks();
}

void wxSliderCombo::SetTick(int tickPos)
{
    m_slider->SetTick(tickPos);
}

void wxSliderCombo::ClearSel()
{
    m_slider->ClearSel();
}

int wxSliderCombo::GetSelEnd() const
{
    return m_slider->GetSelEnd();
}

int wxSliderCombo::GetSelStart() const
{
    return m_slider->GetSelStart();
}

void wxSliderCombo::SetSelection(int min, int max)
{
    m_slider->SetSelection(min,max);
}
