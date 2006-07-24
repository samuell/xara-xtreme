/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_odcombo.h
// Purpose:     XML resource handler for wxOwnerDrawnComboBox
// Author:      Alex Bligh - based on wx/xrc/xh_combo.h
// Created:     2006/06/19
// RCS-ID:      $Id: xh_odcombo.h,v 1.1 2006/06/20 12:26:05 ABX Exp $
// Copyright:   (c) 2006 Alex Bligh
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_SLIDERCOMBO_H_
#define _WX_XH_SLIDERCOMBO_H_

#include <wx/xrc/xmlres.h>

//#if wxUSE_SLIDERCOMBO

class WXDLLIMPEXP_XRC wxSliderComboXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxSliderComboBoxHandler)
public:
    wxSliderComboXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

//#endif

#endif // _WX_XH_SLIDERCOMBO_H_
