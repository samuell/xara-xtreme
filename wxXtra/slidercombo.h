///////////////////////////////////////////////////////////////////////////////
// Name:        slidercombo.h
// Purpose:     A pop-up slider combo control
// Author:      Mart Raudsepp
// Modified by:
// Created:     2006-07-05
// RCS-ID:      $Id: $
// Copyright:   (c) 2006 Mart Raudsepp
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WXXTRA_SLIDERCOMBO_H_
#define _WXXTRA_SLIDERCOMBO_H_

#include <wx/defs.h>

#if wxUSE_COMBOCTRL
#include <wx/combo.h>
#else
#include "combo.h"
#endif

#include <wx/string.h>

class wxSlider;

class WXDLLIMPEXP_ADV wxSliderCombo : public wxComboCtrl
{
public:

    wxSliderCombo() { Init(); }

    wxSliderCombo(wxWindow *parent,
                  wxWindowID id = wxID_ANY,
                  int value = 0,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxString& name = wxT("wxSliderComboBox"));

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                int value = 0,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxT("wxSliderComboBox"));

    virtual ~wxSliderCombo();

    // Suffix to automatically use after the integer value, e.g _("pix")
    // Used if SetAutomaticUpdates(false) hasn't been called.
    wxString GetValueSuffix() const;
    void SetValueSuffix(wxString& suffix);

    // Changes if the updates are done automatically (based on value and the
    // given prefix) or left for the user to do on EVT_SCROLL. Default is TRUE.
    bool GetAutomaticUpdates() const;
    void SetAutomaticUpdates(bool);

    // Duplicate the wxSlider API

    // From wxSlider whatever is necessary... Events are the most important here
    int GetSliderValue() const;
    void SetSliderValue(int value);

    // Default without having called this is 0..100
    void SetRange(int minValue, int maxValue);

    int GetSliderMin() const;
    int GetSliderMax() const;
    void SetSliderMin( int minValue );
    void SetSliderMax( int maxValue );

    // the line/page size is the increment by which the slider moves when
    // cursor arrow key/page up or down are pressed (clicking the mouse is like
    // pressing PageUp/Down) and are by default set to 1 and 1/10 of the range
    void SetLineSize(int lineSize);
    void SetPageSize(int pageSize);
    int GetLineSize() const;
    int GetPageSize() const;

    // these methods get/set the length of the slider pointer in pixels
    void SetThumbLength(int lenPixels);
    int GetThumbLength() const;

    // warning: most of subsequent methods are currently only implemented in
    //          wxMSW under Win95 and are silently ignored on other platforms

    void SetTickFreq(int n, int pos);
    int GetTickFreq() const;
    void ClearTicks();
    void SetTick(int tickPos);

    void ClearSel();
    int GetSelEnd() const;
    int GetSelStart() const;
    void SetSelection(int min, int max);

    // Do we need floating point support? In this case this could be used to
    // select it (and add some other floating point methods)
    /* void SetRange(double minValue, double maxValue); */
    // or the alternative (discussed in the end) with a better name:
    /* void SetMappingConstant(double display_map_value); */

protected:
    wxSlider* m_slider;
    wxString m_suffix;
    bool m_automatic_updates;

private:
    void Init();

    DECLARE_DYNAMIC_CLASS(wxSliderCombo)
};

#endif //_WX_SLIDERCOMBO_H_
