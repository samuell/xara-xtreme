/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_odcombo.h
// Purpose:     XML resource handler for wxOwnerDrawnComboBox
// Author:      Alex Bligh - based on wx/xrc/xh_combo.h
// Created:     2006/06/19
// RCS-ID:      $Id: xh_odcombo.h,v 1.1 2006/06/20 12:26:05 ABX Exp $
// Copyright:   (c) 2006 Alex Bligh
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_GRIDCOMBO_H_
#define _WX_XH_GRIDCOMBO_H_


#include <wx/xrc/xmlres.h>


/*************************************************************************
Class         : wxGridComboXmlHandler
Base Class    : public wxXmlResourceHandler
Author        : Mikhail Tatarnikov
Description   : Creator class for wxGridCombo based on XML resources.
Pure Virtual  : None
Known Issues  : None
Usage Notes   : None
Override Notes: None
**************************************************************************/
class WXDLLIMPEXP_XRC wxGridComboXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxGridComboXmlHandler)
public:
    wxGridComboXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};


#endif // _WX_XH_GRIDCOMBO_H_
