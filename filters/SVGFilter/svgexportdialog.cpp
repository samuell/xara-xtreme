/* @@tag:xara-cn@@ DO NOT MODIFY THIS LINE
================================XARAHEADERSTART===========================

               SVGFilter, XAR <--> SVG plugin filter for XaraLX
                    Copyright (C) 2006 Xara Group Ltd.
       Copyright on certain contributions may be held in joint with their
              respective authors. See AUTHORS file for details.

LICENSE TO USE AND MODIFY SOFTWARE
----------------------------------

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of the Xara Group Ltd. nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MARKS
-----

Xara, Xara LX, Xara X, Xara X/Xtreme, Xara Xtreme, the Xtreme and Xara
designs are registered or unregistered trademarks, design-marks, and/or
service marks of Xara Group Ltd. All rights in these marks are reserved.

      Xara Group Ltd, Gaddesden Place, Hemel Hempstead, HP2 6EX, UK.
                        http://www.xara.com/

=================================XARAHEADEREND============================
*/
// svgexportdialog.cpp: This implements the XAR --> SVG export dialogbox

#include "svgfilterui.h"
#include "svgexportdialog.h"

////@begin XPM images
/* XPM */
static char *w3c_small2_xpm[] = {
/* columns rows colors chars-per-pixel */
"55 36 125 2",
"X  c #FCFEFC",
"'  c #74A2CC",
"9  c #1466A4",
"3  c #D4D2D4",
"r. c #8C8E8C",
"b  c #DCE6EC",
".. c #548ABC",
"O  c #DCE6F4",
"Y  c #1C6EA4",
"z  c #1C1E1C",
"o. c #5C92BC",
"#. c #DCDADC",
"4  c #949694",
"o  c #E4EEF4",
"N  c #6C6A6C",
"I  c #C4D6E4",
"&  c #649AC4",
"q  c #045E9C",
"{  c #E4E2E4",
"V  c #9C9E9C",
";. c #A4BEDC",
"q. c #4482B4",
"p  c #747274",
"[  c #2C2E2C",
"T  c #CCDEEC",
"*. c #6CA2C4",
"t  c #040204",
"D  c #ACC6DC",
"<. c #ECEAEC",
"-. c #CCD2D4",
"e  c #A4A6A4",
":  c #4C8ABC",
"   c #7C7A7C",
"E  c #343634",
"U  c #B4CEDC",
"A  c #2C72AC",
"@. c #B4CEE4",
" . c #5492BC",
"`  c #F4F2F4",
"2. c #ACAEAC",
"m  c #94B6D4",
"h  c #347AAC",
"+. c #848284",
"L  c #BCD6E4",
")  c #141214",
"%  c #FCFAFC",
"c  c #9CBED4",
"%. c #1462A4",
"=. c #DCE2E4",
"f  c #9CBEDC",
"k  c #3C82B4",
".  c #8C8A8C",
"<  c #444644",
"P  c #7CA6CC",
"6. c #1C6AA4",
"1  c #1C1A1C",
"2  c #BCBEBC",
"(  c #949294",
"0  c #E4EAF4",
"&. c #84AECC",
"x  c #4C4E4C",
"^  c #2472AC",
"y  c #242224",
"G  c #6496BC",
"e. c #C4C6C4",
"@  c #ECF2F4",
"0. c #6496C4",
"*  c #045A9C",
",. c #9C9A9C",
"$  c #ECF2FC",
"w  c #8CB6D4",
"1. c #545654",
"8. c #2C2A2C",
"]  c #CCDAEC",
">  c #6C9EC4",
"g  c #0C629C",
"5. c #CCCECC",
"H  c #F4FAFC",
"S  c #0C62A4",
"|  c #4C86B4",
"X. c #A4A2A4",
"}  c #D4E2EC",
"3. c #343234",
"!  c #74A6CC",
"8  c #146AA4",
"M  c #B4CADC",
"4. c #D4D6D4",
"~  c #548EBC",
",  c #ACAAAC",
"K  c #646664",
"n  c #DCEAF4",
"B  c #3C3A3C",
"7  c #BCD2E4",
"v  c #5C96BC",
"d  c #3C7EAC",
"u  c #B4B2B4",
"i  c #6C6E6C",
"Q  c #3C7EB4",
"s  c #C4DAEC",
"W  c #E4E6E4",
"5  c #A4C2DC",
"6  c #4486B4",
"R  c #BCBABC",
"7. c #040604",
":. c #ACCADC",
"O. c #246EAC",
"J  c #ECEEEC",
"#  c #ECEEF4",
"/  c #8CB2D4",
"j  c #2C76AC",
"C  c #545254",
"Z  c #0C0E0C",
"_  c #F4F6F4",
"$. c #6C9AC4",
"a  c #0C5E9C",
"l  c #CCCACC",
"+  c #F4F6FC",
"-  c #94BAD4",
"F  c #347EAC",
"9. c #848684",
";  c #347EB4",
"w. c #D4DEEC",
">. c #5C5A5C",
"=  c #74A2C4",
"r  c #141614",
/* pixels */
"  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .   ",
". X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X . ",
". X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X . ",
". X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X . ",
". X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X . ",
". X o O O + X X X X X @ O O # X X X X X X X X @ O O O O O O O O O O O $ X X X X X % % % X X X X % X X X X X . ",
". X & * * = X X X X X - * * ; X X X X X X X + : * * * * * * * * * * * > % X X X , < 1 < 2 X X 3 4 X X X X X . ",
". X 5 * * 6 $ X X X X 7 8 * 9 0 X X X X X X 0 9 * * * * * * * * * * q w X X % e r t t t y u % i p X X X X X . ",
". X @ a * 8 s X X X X $ d * q f X X X X X X 5 * * g h h h h j q * * k # X X l z t t t t t 1 , z x X X X X X . ",
". X X k * * c X X X X X & * * & % X X X X X v * * & b n n n m * * q M X X X N t t B 4 V C Z z t . X X X X X . ",
". X X m * * 6 X X X X X 5 * * j n X X X X o A * S D X X X @ F * * G H X X J z t K J X X % e Z y J X X X X X . ",
". X X L 8 * 9 + X X X X P * * a I X X X X U S * Y 0 X X X m q * Y T X X X R Z E W X X X X %   . X X X X X X . ",
". X X $ Q * q M X X X + 6 * * * ! X X X X ! * * ~ X X X O ^ * * / X X X X ( ) V X X X X X X _ ` X X X X X X . ",
". X X X = * * ' X X X ] Y * * * h X X X @ Q * * f X X X = * * ; o X X X X   [ { X X X X X X X X X X X X X X . ",
". X X X 7 * * Q o X X - q * * * 9 } X X } q * 9 } X X I 9 * S D X X X X X p K X X X X X X X X X X X X X X X . ",
". X X X + 9 * 8 I X % ~ * * j * q - X X / * * | $ X +  .* * q ..I X X X X p X.X X X X X X X X X X X X X X X . ",
". X X X X o.* * / X $ 9 * O.c * * o.H % 6 * q P X X } a * * * * 9 c % X X +.l X X X X X X X X X X X X X X X . ",
". X X X X c q * 6 X @.* *  .O O.* j O } Y * 9 L X X 0 ~ 6 ^ q * * S 7 X X e #.X X X X X X X X X X X X X X X . ",
". X X X X T O.* S @ $.* q w % & * * @.f a * ^ + X X X X X + - Y * * k $ X #.#.X X X X X X X X X X X X X X X . ",
". X X X X H 6 * q / F * S } X f q * > v * * > X X X X X X X X 5 S * %.D X % W X X X X X X X X X X X X X X X . ",
". X X X X X / * * ^ 9 * h X X } ^ * 9 O.* * M X X X X X X X X + j * * P X X X X X X X X X X X X X X X X X X . ",
". X X X X X T * * * * * &.X X H 6 * * * * ^ n X X X X X X X X X ~ * * o.H X X X X X X X X X X X X X X X X X . ",
". X X X X X X ^ * * * g 7 X X X / * * * * o.+ X X X X X X X X X > * * : + X X X X X X X X X X X X X X X X X . ",
". X X X X X % *.* * * Q o X X X T q * * q - X X X X X X X X X X  .* * o.=.X X X X X X X X X X X { X X X X X . ",
". X X X X X X D S * * > X X X X + j * * 8 } X X o M + X X X X % h * * *.-.#.X X X X X X X X X W K ` X X X X . ",
". X X X X X X b j * q @.X X X X % ! * * Q X X ;.O.S :.X X X X 7 S * a f W 4 ` X X X X X X X { >.[ #.X X X X . ",
". X X X X X X X  .* 9 @ X X X X X D g * P X X 5 g * j ] X X # : * * Y } X ,.  <.X X X X X ` 1.t < <.#.2.2 % . ",
". X X X X X X X f * o.X X X X X X n ^ g ] X X ` 6 * * 9 o.= j q * * ' X X _ C 3., 4.{ #.X.E t 1 5.% . +.X.-.. ",
". X X X X X X X 0 q c X X X X X X X ..h o X X X I 6.* * * * * * * j n X X X 3 [ 7.8.E 8.7.t r V X % 9.X.X.3 . ",
". X X X X X X X H 0.} X X X X X X X f *.% X X X X L q.S * * * g : } X X X X X { >.z t t Z B l X X X l e V ` . ",
". X X X X X X X X w.+ X X X X X X X # I X X X X X X # 5 ! & ! D @ X X X X X X X _ e.,.r.2.{ X X X X X % X X . ",
". X X X X X X X X X X X X X X X X X X X X X X X X X X X X % X X X X X X X X X X X X X X X X X X X X X X X X . ",
". X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X . ",
". X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X . ",
". X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X . ",
"  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .   "
};

////@end XPM images

IMPLEMENT_DYNAMIC_CLASS(SVGExportDialog, wxDialog)
BEGIN_EVENT_TABLE(SVGExportDialog, wxDialog)

////@begin SVGExportDialog event table entries
////@end SVGExportDialog event table entries

END_EVENT_TABLE()

SVGExportDialog::SVGExportDialog()
{
}

SVGExportDialog::SVGExportDialog(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
    Create(parent, id, caption, pos, size, style);
}

bool SVGExportDialog::Create(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
////@begin SVGExportDialog member initialisation
    m_SVGVersionComboBox = NULL;
    m_VerboseCheckBox = NULL;
////@end SVGExportDialog member initialisation

////@begin SVGExportDialog creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end SVGExportDialog creation
    return true;
}

void SVGExportDialog::CreateControls()
{
////@begin SVGExportDialog content construction
    SVGExportDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_RIGHT|wxLEFT|wxTOP, 5);

    wxBitmap itemStaticBitmap4Bitmap(itemDialog1->GetBitmapResource(wxT("images/w3c-small2.png")));
    wxStaticBitmap* itemStaticBitmap4 = new wxStaticBitmap( itemDialog1, wxID_STATIC, itemStaticBitmap4Bitmap, wxDefaultPosition, wxSize(55, 36), 0 );
    itemBoxSizer3->Add(itemStaticBitmap4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer5Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Settings"));
    wxStaticBoxSizer* itemStaticBoxSizer5 = new wxStaticBoxSizer(itemStaticBoxSizer5Static, wxVERTICAL);
    itemBoxSizer2->Add(itemStaticBoxSizer5, 1, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer5->Add(itemBoxSizer6, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);

    wxStaticText* itemStaticText7 = new wxStaticText( itemDialog1, wxID_STATIC, _("SVG &specification:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemStaticText7, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString m_SVGVersionComboBoxStrings[] = {
        _("SVG 1.1"),
        _("SVG 1.2")
    };
    m_SVGVersionComboBox = new wxComboBox( itemDialog1, ID_SPECIFICATION_COMBOBOX, _("SVG 1.2"), wxDefaultPosition, wxDefaultSize, 2, m_SVGVersionComboBoxStrings, wxCB_READONLY );
    m_SVGVersionComboBox->SetStringSelection(_("SVG 1.2"));
    itemBoxSizer6->Add(m_SVGVersionComboBox, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer5->Add(itemBoxSizer9, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_VerboseCheckBox = new wxCheckBox( itemDialog1, ID_CHECKBOX, _("&Verbose output"), wxDefaultPosition, wxDefaultSize, 0 );
    m_VerboseCheckBox->SetValue(false);
    itemBoxSizer9->Add(m_VerboseCheckBox, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer11 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer11, 0, wxALIGN_RIGHT|wxALL, 5);
    wxButton* itemButton12 = new wxButton( itemDialog1, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton12->SetDefault();
    itemStdDialogButtonSizer11->AddButton(itemButton12);

    wxButton* itemButton13 = new wxButton( itemDialog1, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer11->AddButton(itemButton13);

    itemStdDialogButtonSizer11->Realize();

////@end SVGExportDialog content construction
}

bool SVGExportDialog::ShowToolTips()
{
    return true;
}

wxBitmap SVGExportDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin SVGExportDialog bitmap retrieval
    wxUnusedVar(name);
    if (name == _T("images/w3c-small2.png"))
    {
        wxBitmap bitmap(w3c_small2_xpm);
        return bitmap;
    }
    return wxNullBitmap;
////@end SVGExportDialog bitmap retrieval
}

wxIcon SVGExportDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin SVGExportDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end SVGExportDialog icon retrieval
}
