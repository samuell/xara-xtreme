
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XRC

#include "xh_gridcombo.h"

#include "gridcombo.h"

IMPLEMENT_DYNAMIC_CLASS(wxGridComboXmlHandler, wxXmlResourceHandler)

wxGridComboXmlHandler::wxGridComboXmlHandler()
                        :wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxCB_SIMPLE);
    XRC_ADD_STYLE(wxCB_READONLY);
    XRC_ADD_STYLE(wxCB_DROPDOWN);

    AddWindowStyles();
}

/******************************************************************************
Function  : *wxGridComboXmlHandler::DoCreateResource
Author    : Mikhail Tatarnikov
Purpose   : Creates new wxGridCombo and initializes it with values from the
			resources.
Returns   : wxObject* - the brand new initialized object.
Exceptions: None
Parameters: None
Notes     : 
******************************************************************************/
wxObject* wxGridComboXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(pControl, wxGridCombo)

    // Grid like stuff
    if(HasParam(wxT("columns")))
    {
        pControl->SetColumns(GetLong(wxT("columns")));
    }

    if(HasParam(wxT("itemsize")))
	{
		wxSize sizItemSize = GetSize(wxT("itemsize"));
		pControl->SetItemSize(sizItemSize);
	}

    pControl->Create(m_parentAsWindow,
                    GetID(),
                    GetPosition(), GetSize(),
                    GetStyle(),
                    GetName());


    // ComboCtrl like stuff
    wxSize ButtonSize=GetSize(wxT("buttonsize"));
    if (ButtonSize != wxDefaultSize)
	    pControl->SetButtonPosition(ButtonSize.GetWidth(), ButtonSize.GetHeight());

    SetupWindow(pControl);

    return pControl;
}

/******************************************************************************
Function  : wxGridComboXmlHandler::CanHandle
Author    : Mikhail Tatarnikov
Purpose   : Checks if an XML node corresponds to our wxGridCombo control.
Returns   : bool - true if wxGridCombo can be created from the node, false otherwise.
Exceptions: 
Parameters: [in] wxXmlNode* node - resource XML node to test.
Notes     : 
******************************************************************************/
bool wxGridComboXmlHandler::CanHandle(wxXmlNode* node)
{
#if wxCHECK_VERSION(2,7,0)

    return (IsOfClass(node, wxT("wxGridCombo")));

#else

//  Avoid GCC bug - this fails on certain GCC 3.3 and 3.4 builds for an unknown reason
//  it is believed to be related to the fact IsOfClass is inline, and node->GetPropVal
//  gets passed an invalid "this" pointer. On 2.7, the function is out of line, so the
//  above should work fine. This code is left in here so this file can easily be used
//  in a version backported to 2.6. All we are doing here is expanding the macro

    bool bOurClass = node->GetPropVal(wxT("class"), wxEmptyString) == wxT("wxGridCombo");
    return bOurClass;
#endif
}

#endif // wxUSE_XRC
