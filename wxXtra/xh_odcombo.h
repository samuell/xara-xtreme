/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_odcombo.h
// Purpose:     XML resource handler for wxOwnerDrawnComboBox
// Author:      Alex Bligh - based on wx/xrc/xh_combo.h
// Created:     2006/06/19
// RCS-ID:      $Id: xh_odcombo.h,v 1.1 2006/06/20 12:26:05 ABX Exp $
// Copyright:   (c) 2006 Alex Bligh
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WXXTRA_XH_ODCOMBO_H_
#define _WXXTRA_XH_ODCOMBO_H_

#include <wx/wx.h>

#if wxUSE_ODCOMBOBOX
#undef wxXTRA_XH_ODCOMBO
#include <wx/xrc/xh_odcombo.h>
#else
#define wxXTRA_XH_ODCOMBO 1

#include <wx/xrc/xmlres.h>

class WXDLLIMPEXP_XRC wxOwnerDrawnComboBoxXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxOwnerDrawnComboBoxXmlHandler)
public:
    wxOwnerDrawnComboBoxXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
private:
    bool m_insideBox;
    wxArrayString strList;
};

#endif // wxUSE_ODCOMBOBOX

#endif // _WXXTA_XH_ODCOMBO_H_
