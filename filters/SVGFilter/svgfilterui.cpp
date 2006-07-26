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
// svgfilterui.h: This implements the XAR <--> SVG GUI interface

#include "svgfilterui.h"
#include "svgexportdialog.h"

static bool s_bDoVerbose = false;

// Define a new application class
class MyApp : public wxApp
{
public:
	virtual void OnInitCmdLine(wxCmdLineParser& parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
	virtual int OnRun();

public:
	bool m_bDoExport;
	bool m_bCancelled;
	wxString m_sOutFile;
	wxString m_sXMLFile;
};

IMPLEMENT_APP(MyApp)

void MyApp::OnInitCmdLine(wxCmdLineParser& parser)
{
	static const wxCmdLineEntryDesc cmdLineDesc[] =
	{
		{ wxCMD_LINE_SWITCH, _T("h"), _T("help"),	 _T("show this help message"),
			wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
		{ wxCMD_LINE_SWITCH, _T("v"), _T("verbose"), _T("be verbose") },
		{ wxCMD_LINE_OPTION, _T("f"), _T("file"),	 _T("output filename") },
		{ wxCMD_LINE_OPTION, _T("x"), _T("xmlfile"), _T("xml configuration filename") },
		{ wxCMD_LINE_SWITCH, _T("i"), _T("import"),	 _T("specify import action") },
		{ wxCMD_LINE_SWITCH, _T("e"), _T("export"),  _T("specify export action") },
		{ wxCMD_LINE_NONE }
	};

	parser.SetDesc(cmdLineDesc);
}

bool MyApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
	s_bDoVerbose = parser.Found(_T("v"));

	if (!parser.Found(_T("f"), &m_sOutFile))
	{
		ReportError(_T("A filename must be specified with -f"));
		return false;
	}

	if (!parser.Found(_T("x"), &m_sXMLFile))
	{
		ReportError(_T("An XML filename must be specified with -x"));
		return false;
	}
	if (!parser.Found(_T("i")) && !parser.Found(_T("e")))
	{
		ReportError(_T("An action must be specified with -i or -e"));
		return false;
	}
	m_bDoExport = parser.Found(_T("e"));

	// Open the output file for writing
	wxFileOutputStream OutFile(m_sXMLFile);
	if (!OutFile.Ok())
	{
		ReportError(_T("Failed to open output XML file."));
		// Set an appropriate error here
		return false;
	}
	wxTextOutputStream OutText(OutFile);

	OutText << _T("<XPFilterConfig>\n");
	OutText << _T("<Private ");

	m_bCancelled = false;
	if (m_bDoExport) {
		SVGExportDialog *dlg = new SVGExportDialog(NULL);
		if (dlg->ShowModal() != wxID_OK) {
			// user cancelled export
			m_bCancelled = true;
		} else {
			// output exporter settings
			if (dlg->m_SVGVersionComboBox->GetSelection() == 0)
				OutText << _T("version=\"1.1\" ");
			else
				OutText << _T("version=\"1.2\" ");
			if (dlg->m_VerboseCheckBox->IsChecked())
				OutText << _T("verbose=\"1\" ");
			else
				OutText << _T("verbose=\"0\" ");
		}
		dlg->Destroy();
	}
	OutText << _T("/>\n");
	OutText << _T("</XPFilterConfig>\n");

    return true;
}

int MyApp::OnRun()
{
	if (m_bCancelled)	// User cancelled export dialog
		return 1;

	return 0;			// Worked
}

void ReportError(const wxChar* pStr)
{
	wxFprintf(stderr, _T("ERROR:%s\n"), pStr);
}

void ReportWarning(const wxChar* pStr)
{
	wxFprintf(stderr, _T("WARNING:%s\n"), pStr);
}

void ReportMessage(const wxChar* pStr)
{
	if (s_bDoVerbose)
		wxFprintf(stderr, _T("MESSAGE:%s\n"), pStr);
}
