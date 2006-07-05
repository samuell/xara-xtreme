// $Id$
/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
// The following line makes normalize.pl skip type fixing
/* SKIPFIXTYPES: START */

/////////////////////////////////////////////////////////////////////////////
// Name:        odcombo.cpp
// Purpose:     wxOwnerDrawnComboBox, wxVListBoxComboPopup
// Author:      Jaakko Salli
// Modified by:
// Created:     Apr-30-2006
// RCS-ID:
// Copyright:   (c) 2005 Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/wx.h>

#include "odcombo.h"
// Only compile stuff if this is not already in WX
#if wxXTRA_ODCOMBOBOX

#include <wx/renderer.h>

// ============================================================================
// implementation
// ============================================================================


// ----------------------------------------------------------------------------
// wxVListBoxComboPopup is a wxVListBox customized to act as a popup control
//
// ----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(wxVListBoxComboPopup, wxVListBox)
    EVT_MOTION(wxVListBoxComboPopup::OnMouseMove)
    EVT_KEY_DOWN(wxVListBoxComboPopup::OnKey)
    EVT_LEFT_UP(wxVListBoxComboPopup::OnLeftClick)
END_EVENT_TABLE()


void wxVListBoxComboPopup::Init()
/*                                           : wxVListBox(),
                                             wxComboPopup(combo)*/
{
    m_widestWidth = 0;
    m_avgCharWidth = 0;
    m_baseImageWidth = 0;
    m_itemHeight = 0;
    m_value = -1;
    m_itemHover = -1;
    m_clientDataItemsType = wxClientData_None;
}

bool wxVListBoxComboPopup::Create(wxWindow* parent)
{
    if ( !wxVListBox::Create(parent,
                             wxID_ANY,
                             wxDefaultPosition,
                             wxDefaultSize,
                             wxBORDER_SIMPLE | wxLB_INT_HEIGHT | wxWANTS_CHARS) )
        return false;

    m_useFont = m_combo->GetFont();

    wxVListBox::SetItemCount(m_strings.GetCount());

    // TODO: Move this to SetFont
    m_itemHeight = GetCharHeight() + 0;

    return true;
}

wxVListBoxComboPopup::~wxVListBoxComboPopup()
{
    Clear();
}

bool wxVListBoxComboPopup::LazyCreate()
{
    // NB: There is a bug with wxVListBox that can be avoided by creating
    //     it later (bug causes empty space to be shown if initial selection
    //     is at the end of a list longer than the control can show at once).
    return true;
}

// paint the control itself
void wxVListBoxComboPopup::PaintComboControl( wxDC& dc, const wxRect& rect )
{
    if ( !(m_combo->GetWindowStyle() & wxODCB_STD_CONTROL_PAINT) )
    {
        m_combo->DrawFocusBackground(dc,rect,0);
        if ( m_value >= 0 )
        {
            OnDrawItem(dc,rect,m_value,wxODCB_PAINTING_CONTROL);
            return;
        }
    }

    wxComboPopup::PaintComboControl(dc,rect);
}

void wxVListBoxComboPopup::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const
{
    // TODO: Maybe this code could be moved to wxVListBox::OnPaint?
    dc.SetFont(m_useFont);

    // Set correct text colour for selected items
    if ( wxVListBox::GetSelection() == (int) n )
        dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT) );
    else
        dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT) );

    OnDrawItem(dc,rect,(int)n,0);
}

wxCoord wxVListBoxComboPopup::OnMeasureItem(size_t WXUNUSED(n)) const
{
    /*
    int itemHeight = m_combo->OnMeasureListItem(n);
    if ( itemHeight < 0 )
        itemHeight = m_itemHeight;
    */
    return m_itemHeight;
}

wxCoord wxVListBoxComboPopup::OnMeasureItemWidth(size_t WXUNUSED(n)) const
{
    //return OnMeasureListItemWidth(n);
    return -1;
}

void wxVListBoxComboPopup::OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const
{
    // we need to render selected and current items differently
    if ( IsCurrent(n) )
    {
        m_combo->DrawFocusBackground( dc, rect, wxCONTROL_ISSUBMENU|wxCONTROL_SELECTED );
    }
    //else: do nothing for the normal items
}

// This is called from wxVListBoxComboPopup::OnDrawItem, with text colour and font prepared
void wxVListBoxComboPopup::OnDrawItem( wxDC& dc, const wxRect& rect, int item, int flags ) const
{
    if ( flags & wxODCB_PAINTING_CONTROL )
    {
        dc.DrawText( m_combo->GetValue(),
                     rect.x + m_combo->GetTextIndent(),
                     (rect.height-dc.GetCharHeight())/2 + rect.y );
    }
    else
    {
        dc.DrawText( GetString(item), rect.x + 2, rect.y );
    }
}

void wxVListBoxComboPopup::DismissWithEvent()
{
    int selection = wxVListBox::GetSelection();

    Dismiss();

    wxString valStr;
    if ( selection != wxNOT_FOUND )
        valStr = m_strings[selection];
    else
        valStr = wxEmptyString;

    m_value = selection;

    if ( valStr != m_combo->GetValue() )
        m_combo->SetValue(valStr);

    SendComboBoxEvent(selection);
}

void wxVListBoxComboPopup::SendComboBoxEvent( int selection )
{
    wxCommandEvent evt(wxEVT_COMMAND_COMBOBOX_SELECTED,m_combo->GetId());

    evt.SetEventObject(m_combo);

    evt.SetInt(selection);

    // Set client data, if any
    if ( selection >= 0 && (int)m_clientDatas.GetCount() > selection )
    {
        void* clientData = m_clientDatas[selection];
        if ( m_clientDataItemsType == wxClientData_Object )
            evt.SetClientObject((wxClientData*)clientData);
        else
            evt.SetClientData(clientData);
    }

    m_combo->GetEventHandler()->AddPendingEvent(evt);
}

// returns true if key was consumed
bool wxVListBoxComboPopup::HandleKey( int keycode, bool saturate )
{
    int value = m_value;
    int itemCount = GetCount();

    if ( keycode == WXK_DOWN || keycode == WXK_RIGHT )
    {
        value++;
    }
    else if ( keycode == WXK_UP || keycode == WXK_LEFT )
    {
        value--;
    }
    else if ( keycode == WXK_PAGEDOWN )
    {
        value+=10;
    }
    else if ( keycode == WXK_PAGEUP )
    {
        value-=10;
    }
    /*
    else if ( keycode == WXK_END )
    {
        value = itemCount-1;
    }
    else if ( keycode == WXK_HOME )
    {
        value = 0;
    }
    */
    else
        return false;

    if ( saturate )
    {
        if ( value >= itemCount )
            value = itemCount - 1;
        else if ( value < 0 )
            value = 0;
    }
    else
    {
        if ( value >= itemCount )
            value -= itemCount;
        else if ( value < 0 )
            value += itemCount;
    }

    if ( value == m_value )
        // Even if value was same, don't skip the event
        // (good for consistency)
        return true;

    m_value = value;

    if ( value >= 0 )
        m_combo->SetValue(m_strings[value]);

    SendComboBoxEvent(m_value);

    return true;
}

void wxVListBoxComboPopup::OnComboDoubleClick()
{
    // Cycle on dclick (disable saturation to allow true cycling).
    if ( !::wxGetKeyState(WXK_SHIFT) )
        HandleKey(WXK_DOWN,false);
    else
        HandleKey(WXK_UP,false);
}

void wxVListBoxComboPopup::OnComboKeyEvent( wxKeyEvent& event )
{
    // Saturated key movement on
    if ( !HandleKey(event.GetKeyCode(),true) )
        event.Skip();
}

void wxVListBoxComboPopup::OnPopup()
{
    // *must* set value after size is set (this is because of a vlbox bug)
    wxVListBox::SetSelection(m_value);
}

void wxVListBoxComboPopup::OnMouseMove(wxMouseEvent& event)
{
    // Move selection to cursor if it is inside the popup
    int itemHere = GetItemAtPosition(event.GetPosition());
    if ( itemHere >= 0 )
        wxVListBox::SetSelection(itemHere);

    event.Skip();
}

void wxVListBoxComboPopup::OnLeftClick(wxMouseEvent& WXUNUSED(event))
{
    DismissWithEvent();
}

void wxVListBoxComboPopup::OnKey(wxKeyEvent& event)
{
    // Select item if ENTER is pressed
    if ( event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER )
    {
        DismissWithEvent();
    }
    // Hide popup if ESC is pressed
    else if ( event.GetKeyCode() == WXK_ESCAPE )
        Dismiss();
    else
        event.Skip();
}

void wxVListBoxComboPopup::CheckWidth( int pos )
{
    wxCoord x = OnMeasureItemWidth(pos);

    if ( x < 0 )
    {
        if ( !m_useFont.Ok() )
            m_useFont = m_combo->GetFont();

        wxCoord y;
        m_combo->GetTextExtent(m_strings[pos], &x, &y, 0, 0, &m_useFont);
        x += 4;
    }

    if ( m_widestWidth < x )
    {
        m_widestWidth = x;
    }
}

void wxVListBoxComboPopup::Insert( const wxString& item, int pos )
{
    // Need to change selection?
    wxString strValue;
    if ( !(m_combo->GetWindowStyle() & wxCB_READONLY) &&
         m_combo->GetValue() == item )
    {
        m_value = pos;
    }

    m_strings.Insert(item,pos);

    if ( IsCreated() )
        wxVListBox::SetItemCount( wxVListBox::GetItemCount()+1 );

    // Calculate width
    CheckWidth(pos);
}

int wxVListBoxComboPopup::Append(const wxString& item)
{
    int pos = (int)m_strings.GetCount();

    if ( m_combo->GetWindowStyle() & wxCB_SORT )
    {
        // Find position
        // TODO: Could be optimized with binary search
        wxArrayString strings = m_strings;
        unsigned int i;

        for ( i=0; i<strings.GetCount(); i++ )
        {
            if ( item.Cmp(strings.Item(i)) < 0 )
            {
                pos = (int)i;
                break;
            }
        }
    }

    Insert(item,pos);

    return pos;
}

void wxVListBoxComboPopup::Clear()
{
    wxASSERT(m_combo);

    m_strings.Empty();

    ClearClientDatas();

    if ( IsCreated() )
        wxVListBox::SetItemCount(0);
}

void wxVListBoxComboPopup::ClearClientDatas()
{
    if ( m_clientDataItemsType == wxClientData_Object )
    {
        size_t i;
        for ( i=0; i<m_clientDatas.GetCount(); i++ )
            delete (wxClientData*) m_clientDatas[i];
    }

    m_clientDatas.Empty();
}

void wxVListBoxComboPopup::SetItemClientData( int n,
                                              void* clientData,
                                              wxClientDataType clientDataItemsType )
{
    // It should be sufficient to update this variable only here
    m_clientDataItemsType = clientDataItemsType;

    m_clientDatas.SetCount(n+1,NULL);
    m_clientDatas[n] = clientData;
    CheckWidth(n);
}

void* wxVListBoxComboPopup::GetItemClientData(int n) const
{
    if ( (int)m_clientDatas.GetCount() > n )
        return m_clientDatas[n];

    return NULL;
}

void wxVListBoxComboPopup::Delete( int item )
{
    // Remove client data, if set
    if ( m_clientDatas.GetCount() )
    {
        if ( m_clientDataItemsType == wxClientData_Object )
            delete (wxClientData*) m_clientDatas[item];

        m_clientDatas.RemoveAt(item);
    }

    m_strings.RemoveAt(item);

    if ( IsCreated() )
        wxVListBox::SetItemCount( wxVListBox::GetItemCount()-1 );
}

int wxVListBoxComboPopup::FindString(const wxString& s) const
{
    return m_strings.Index(s);
}

int wxVListBoxComboPopup::GetCount() const
{
    return m_strings.GetCount();
}

wxString wxVListBoxComboPopup::GetString( int item ) const
{
    return m_strings[item];
}

void wxVListBoxComboPopup::SetString( int item, const wxString& str )
{
    m_strings[item] = str;
}

wxString wxVListBoxComboPopup::GetStringValue() const
{
    if ( m_value >= 0 )
        return m_strings[m_value];
    return wxEmptyString;
}

void wxVListBoxComboPopup::SetSelection( int item )
{
    // This seems to be necessary (2.5.3 w/ MingW atleast)
    if ( item < -1 || item >= (int)m_strings.GetCount() )
        item = -1;

    m_value = item;

    if ( IsCreated() )
        wxVListBox::SetSelection(item);
}

int wxVListBoxComboPopup::GetSelection() const
{
    return m_value;
}

void wxVListBoxComboPopup::SetStringValue( const wxString& value )
{
    int index = m_strings.Index(value);

    m_value = index;

    if ( index >= -1 && index < (int)wxVListBox::GetItemCount() )
        wxVListBox::SetSelection(index);
}

wxSize wxVListBoxComboPopup::GetAdjustedSize( int minWidth, int prefHeight, int maxHeight )
{
    int height = 250;

    if ( m_strings.GetCount() )
    {
        if ( prefHeight > 0 )
            height = prefHeight;

        if ( height > maxHeight )
            height = maxHeight;

        int totalHeight = GetTotalHeight(); // + 3;
        if ( height >= totalHeight )
        {
            height = totalHeight;
        }
        else
        {
            // Adjust height to a multiple of the height of the first item
            // NB: Calculations that take variable height into account
            //     are unnecessary.
            int fih = GetLineHeight(0);
            int shown = height/fih;
            height = shown * fih;
        }
    }
    else
        height = 50;

    // Take scrollbar into account in width calculations
    int widestWidth = m_widestWidth + wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
    return wxSize(minWidth > widestWidth ? minWidth : widestWidth,
                  height+2);
}

//void wxVListBoxComboPopup::Populate( int n, const wxString choices[] )
void wxVListBoxComboPopup::Populate( const wxArrayString& choices )
{
    int i;

    int n = choices.GetCount();

    for ( i=0; i<n; i++ )
    {
        const wxString& item = choices.Item(i);
        m_strings.Add(item);
        CheckWidth(i);
    }

    if ( IsCreated() )
        wxVListBox::SetItemCount(n);

    // Sort the initial choices
    if ( m_combo->GetWindowStyle() & wxCB_SORT )
        m_strings.Sort();

    // Find initial selection
    wxString strValue = m_combo->GetValue();
    if ( strValue.Length() )
        m_value = m_strings.Index(strValue);
}

// ----------------------------------------------------------------------------
// wxOwnerDrawnComboBox
// ----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(wxOwnerDrawnComboBox, wxComboControl)
END_EVENT_TABLE()


IMPLEMENT_DYNAMIC_CLASS2(wxOwnerDrawnComboBox, wxComboControl, wxControlWithItems)

void wxOwnerDrawnComboBox::Init()
{
    m_popupInterface = NULL;
}

bool wxOwnerDrawnComboBox::Create(wxWindow *parent,
                                  wxWindowID id,
                                  const wxString& value,
                                  const wxPoint& pos,
                                  const wxSize& size,
                                  long style,
                                  const wxValidator& validator,
                                  const wxString& name)
{
    return wxComboControl::Create(parent,id,value,pos,size,style,validator,name);
}

wxOwnerDrawnComboBox::wxOwnerDrawnComboBox(wxWindow *parent,
                                           wxWindowID id,
                                           const wxString& value,
                                           const wxPoint& pos,
                                           const wxSize& size,
                                           const wxArrayString& choices,
                                           long style,
                                           const wxValidator& validator,
                                           const wxString& name)
    : wxComboControl()
{
    Init();

    Create(parent,id,value,pos,size,choices,style, validator, name);
}

bool wxOwnerDrawnComboBox::Create(wxWindow *parent,
                                  wxWindowID id,
                                  const wxString& value,
                                  const wxPoint& pos,
                                  const wxSize& size,
                                  const wxArrayString& choices,
                                  long style,
                                  const wxValidator& validator,
                                  const wxString& name)
{
    m_initChs = choices;
    //wxCArrayString chs(choices);

    //return Create(parent, id, value, pos, size, chs.GetCount(),
    //              chs.GetStrings(), style, validator, name);
    return Create(parent, id, value, pos, size, 0,
                  NULL, style, validator, name);
}

bool wxOwnerDrawnComboBox::Create(wxWindow *parent,
                                  wxWindowID id,
                                  const wxString& value,
                                  const wxPoint& pos,
                                  const wxSize& size,
                                  int n,
                                  const wxString choices[],
                                  long style,
                                  const wxValidator& validator,
                                  const wxString& name)
{

    if ( !Create(parent, id, value, pos, size, style,
                 validator, name) )
    {
        return false;
    }

    int i;
    for ( i=0; i<n; i++ )
        m_initChs.Add(choices[i]);

    return true;
}

wxOwnerDrawnComboBox::~wxOwnerDrawnComboBox()
{
    if ( m_popupInterface )
        m_popupInterface->ClearClientDatas();
}

void wxOwnerDrawnComboBox::SetPopupControl( wxComboPopup* popup )
{
    if ( !popup )
    {
        popup = new wxVListBoxComboPopup();
    }

    wxComboControl::SetPopupControl(popup);

    wxASSERT(popup);
    m_popupInterface = (wxVListBoxComboPopup*) popup;

    // Add initial choices to the wxVListBox
    if ( !m_popupInterface->GetCount() )
    {
        //m_popupInterface->Populate(m_initChs.GetCount(),m_initChs.GetStrings());
        m_popupInterface->Populate(m_initChs);
        m_initChs.Clear();
    }
}

// ----------------------------------------------------------------------------
// wxOwnerDrawnComboBox item manipulation methods
// ----------------------------------------------------------------------------

void wxOwnerDrawnComboBox::Clear()
{
    EnsurePopupControl();

    m_popupInterface->Clear();

    if (GetTextCtrl())
        GetTextCtrl()->SetValue(wxEmptyString);
}

void wxOwnerDrawnComboBox::Delete(int n)
{
    wxCHECK_RET( (n >= 0) && (n < GetCount()), _T("invalid index in wxOwnerDrawnComboBox::Delete") );

    if ( GetSelection() == (int) n )
        SetValue(wxEmptyString);

    m_popupInterface->Delete(n);
}

int wxOwnerDrawnComboBox::GetCount() const
{
    wxASSERT_MSG( m_popupInterface, wxT("no popup interface") );
    return m_popupInterface->GetCount();
}

wxString wxOwnerDrawnComboBox::GetString(int n) const
{
    wxCHECK_MSG( (n >= 0) && (n < GetCount()), wxEmptyString, _T("invalid index in wxOwnerDrawnComboBox::GetString") );
    return m_popupInterface->GetString(n);
}

void wxOwnerDrawnComboBox::SetString( int n, const wxString& s)
{
    wxCHECK_RET( (n >= 0) && (n < GetCount()), _T("invalid index in wxOwnerDrawnComboBox::SetString") );
    m_popupInterface->SetString(n,s);
}

int wxOwnerDrawnComboBox::FindString(const wxString& s) const
{
    wxASSERT_MSG( m_popupInterface, wxT("no popup interface") );
    return m_popupInterface->FindString(s);
}

void wxOwnerDrawnComboBox::Select(int n)
{
    wxCHECK_RET( (n >= -1) && (n < (int)GetCount()), _T("invalid index in wxOwnerDrawnComboBox::Select") );
    EnsurePopupControl();

    m_popupInterface->SetSelection(n);

    wxString str;
    if ( n >= 0 )
        str = m_popupInterface->GetString(n);

    // Refresh text portion in control
    if ( m_text )
        m_text->SetValue( str );
    else
        m_valueString = str;

    Refresh();
}

int wxOwnerDrawnComboBox::GetSelection() const
{
    wxASSERT( m_popupInterface );
    return m_popupInterface->GetSelection();
}

int wxOwnerDrawnComboBox::DoAppend(const wxString& item)
{
    wxASSERT( m_popupInterface );
    return m_popupInterface->Append(item);
}

int wxOwnerDrawnComboBox::DoInsert(const wxString& item, int pos)
{
    wxCHECK_MSG(!(GetWindowStyle() & wxCB_SORT), -1, wxT("can't insert into sorted list"));
    wxCHECK_MSG((pos>=0) && (pos<=GetCount()), -1, wxT("invalid index"));

    m_popupInterface->Insert(item,pos);

    return pos;
}

void wxOwnerDrawnComboBox::DoSetItemClientData(int n, void* clientData)
{
    wxASSERT(m_popupInterface);
    m_popupInterface->SetItemClientData(n,clientData,m_clientDataItemsType);
}

void* wxOwnerDrawnComboBox::DoGetItemClientData(int n) const
{
    wxASSERT(m_popupInterface);
    return m_popupInterface->GetItemClientData(n);
}

void wxOwnerDrawnComboBox::DoSetItemClientObject(int n, wxClientData* clientData)
{
    DoSetItemClientData(n, (void*) clientData);
}

wxClientData* wxOwnerDrawnComboBox::DoGetItemClientObject(int n) const
{
    return (wxClientData*) DoGetItemClientData(n);
}

#ifdef WXXTRA_COMBO_XML_HANDLERS
IMPLEMENT_DYNAMIC_CLASS(wxOwnerDrawnComboBoxXmlHandler, wxXmlResourceHandler)

wxOwnerDrawnComboBoxXmlHandler::wxOwnerDrawnComboBoxXmlHandler()
: wxXmlResourceHandler() , m_insideBox(false)
{
    XRC_ADD_STYLE(wxCB_SIMPLE);
    XRC_ADD_STYLE(wxCB_SORT);
    XRC_ADD_STYLE(wxCB_READONLY);
    XRC_ADD_STYLE(wxCB_DROPDOWN);
    XRC_ADD_STYLE(wxODCB_STD_CONTROL_PAINT);
    XRC_ADD_STYLE(wxCC_SPECIAL_DCLICK);
    XRC_ADD_STYLE(wxCC_ALT_KEYS);
    XRC_ADD_STYLE(wxCC_STD_BUTTON);
    AddWindowStyles();
}

wxObject *wxOwnerDrawnComboBoxXmlHandler::DoCreateResource()
{
    if( m_class == wxT("wxOwnerDrawnComboBox"))
    {
        // find the selection
        long selection = GetLong( wxT("selection"), -1 );

        // need to build the list of strings from children
        m_insideBox = true;
        CreateChildrenPrivately(NULL, GetParamNode(wxT("content")));
        wxString *strings = (wxString *) NULL;
        if (strList.GetCount() > 0)
        {
            strings = new wxString[strList.GetCount()];
            int count = strList.GetCount();
            for (int i = 0; i < count; i++)
                strings[i]=strList[i];
        }

        XRC_MAKE_INSTANCE(control, wxOwnerDrawnComboBox)

        control->Create(m_parentAsWindow,
                        GetID(),
                        GetText(wxT("value")),
                        GetPosition(), GetSize(),
                        strList.GetCount(),
                        strings,
                        GetStyle(),
                        wxDefaultValidator,
                        GetName());

        control->SetPopupControl(NULL);

        wxSize ButtonSize=GetSize(wxT("buttonsize"));

        if (ButtonSize != wxDefaultSize)
            control->SetButtonPosition(ButtonSize.GetWidth(), ButtonSize.GetHeight());

        if (selection != -1)
            control->SetSelection(selection);

        SetupWindow(control);

        if (strings != NULL)
            delete[] strings;
        strList.Clear();    // dump the strings

        return control;
    }
    else
    {
        // on the inside now.
        // handle <item>Label</item>

        // add to the list
        wxString str = GetNodeContent(m_node);
        if (m_resource->GetFlags() & wxXRC_USE_LOCALE)
            str = wxGetTranslation(str);
        strList.Add(str);

        return NULL;
    }
}

bool wxOwnerDrawnComboBoxXmlHandler::CanHandle(wxXmlNode *node)
{
// Avoid GCC bug
//    return (IsOfClass(node, wxT("wxOwnerDrawnComboBox")) ||
//           (m_insideBox && node->GetName() == wxT("item")));
	bool fOurClass = node->GetPropVal(wxT("class"), wxEmptyString) == wxT("wxOwnerDrawnComboBox");
    return (fOurClass ||
           (m_insideBox && node->GetName() == wxT("item")));
}


#endif

#endif // wxXTRA_OWNERDRAWNCOMBOBOX
