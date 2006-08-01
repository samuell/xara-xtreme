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
// svgexportdialog.h: This defines the XAR --> SVG export dialogbox

#ifndef SVGEXPORTDIALOG_H
#define SVGEXPORTDIALOG_H

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 10000
#define SYMBOL_SVGEXPORTDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxDIALOG_MODAL
#define SYMBOL_SVGEXPORTDIALOG_TITLE _("SVG Export Settings")
#define SYMBOL_SVGEXPORTDIALOG_IDNAME ID_DIALOG
#define SYMBOL_SVGEXPORTDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_SVGEXPORTDIALOG_POSITION wxDefaultPosition
#define ID_SPECIFICATION_COMBOBOX 10001
#define ID_CHECKBOX 10002
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/***************************************************************************************************

>	class SVGExportDialog

	Author:		Sandro Sigala <sandro@sigala.it>
	Created:	20 June 2006
	Purpose:	Export (SVG --> XAR) dialogbox with the common user settings.

***************************************************************************************************/

class SVGExportDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS(SVGExportDialog)
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    SVGExportDialog();
    SVGExportDialog(wxWindow* parent, wxWindowID id = SYMBOL_SVGEXPORTDIALOG_IDNAME, const wxString& caption = SYMBOL_SVGEXPORTDIALOG_TITLE, const wxPoint& pos = SYMBOL_SVGEXPORTDIALOG_POSITION, const wxSize& size = SYMBOL_SVGEXPORTDIALOG_SIZE, long style = SYMBOL_SVGEXPORTDIALOG_STYLE);

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = SYMBOL_SVGEXPORTDIALOG_IDNAME, const wxString& caption = SYMBOL_SVGEXPORTDIALOG_TITLE, const wxPoint& pos = SYMBOL_SVGEXPORTDIALOG_POSITION, const wxSize& size = SYMBOL_SVGEXPORTDIALOG_SIZE, long style = SYMBOL_SVGEXPORTDIALOG_STYLE);

    /// Creates the controls and sizers
    void CreateControls();

////@begin SVGExportDialog event handler declarations

////@end SVGExportDialog event handler declarations

////@begin SVGExportDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end SVGExportDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin SVGExportDialog member variables
    wxComboBox* m_SVGVersionComboBox;
    wxCheckBox* m_VerboseCheckBox;
////@end SVGExportDialog member variables
};

#endif // !SVGEXPORTDIALOG_H
