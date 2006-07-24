/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_slidrcombo.cpp
// Purpose:     XRC resource for wxSliderCombo
// Author:      Mart Raudsepp - Based on src/xrc/xh_odcombo.cpp
// Created:     2006/07/24
// RCS-ID:      $Id: $
// Copyright:   (c) 2006 Mart Raudsepp
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XRC //&& wxUSE_SLIDERCOMBO

#include "xh_slidrcombo.h"
#include <wx/intl.h>
#include <wx/textctrl.h>

#include "slidercombo.h"
#include <wx/slider.h> // For wxSL_* styles

IMPLEMENT_DYNAMIC_CLASS(wxSliderComboXmlHandler, wxXmlResourceHandler)

wxSliderComboXmlHandler::wxSliderComboXmlHandler()
                        :wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxCB_READONLY);
    XRC_ADD_STYLE(wxCB_DROPDOWN);
    XRC_ADD_STYLE(wxTE_PROCESS_ENTER);
    XRC_ADD_STYLE(wxSL_HORIZONTAL);
    XRC_ADD_STYLE(wxSL_VERTICAL);
    XRC_ADD_STYLE(wxSL_LABELS);
    XRC_ADD_STYLE(wxSL_INVERSE);
    AddWindowStyles();
}

wxObject *wxSliderComboXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(control, wxSliderCombo)

    control->Create(m_parentAsWindow,
                    GetID(),
                    GetLong(wxT("value")),
                    GetPosition(), GetSize(),
                    GetStyle(),
                    GetName());

    SetupWindow(control);

    return control;
}

bool wxSliderComboXmlHandler::CanHandle(wxXmlNode *node)
{
#if wxCHECK_VERSION(2,7,0)

    return (IsOfClass(node, wxT("wxSliderCombo")));

#else

//  Avoid GCC bug - this fails on certain GCC 3.3 and 3.4 builds for an unknown reason
//  it is believed to be related to the fact IsOfClass is inline, and node->GetPropVal
//  gets passed an invalid "this" pointer. On 2.7, the function is out of line, so the
//  above should work fine. This code is left in here so this file can easily be used
//  in a version backported to 2.6. All we are doing here is expanding the macro

    bool fOurClass = node->GetPropVal(wxT("class"), wxEmptyString) == wxT("wxSliderCombo");
    return (fOurClass);
#endif
}

#endif // wxUSE_XRC //&& wxUSE_SLIDERCOMBO
