// $Id$
/* @@tag:xara-cn@@ DO NOT MODIFY THIS LINE
================================XARAHEADERSTART===========================
 
               SVGFilter, XAR <--> SVG plugin filter for XaraLX
                    Copyright (C) 2006 Xara Group Ltd.
       Copyright on certain contributions may be held in joint with their
              respective authors. See AUTHORS file for details.

LICENSE TO USE AND MODIFY SOFTWARE
----------------------------------

This file is part of Xara LX.

Xara LX is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as published
by the Free Software Foundation.

Xara LX and its component source files are distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with Xara LX (see the file GPL in the root directory of the
distribution); if not, write to the Free Software Foundation, Inc., 51
Franklin St, Fifth Floor, Boston, MA  02110-1301 USA


ADDITIONAL RIGHTS
-----------------

Conditional upon your continuing compliance with the GNU General Public
License described above, Xara Group Ltd grants to you certain additional
rights. 

The additional rights are to use, modify, and distribute the software
together with the wxWidgets library, the wxXtra library, and the "CDraw"
library and any other such library that any version of Xara LX relased
by Xara Group Ltd requires in order to compile and execute, including
the static linking of that library to XaraLX. In the case of the
"CDraw" library, you may satisfy obligation under the GNU General Public
License to provide source code by providing a binary copy of the library
concerned and a copy of the license accompanying it.

Nothing in this section restricts any of the rights you have under
the GNU General Public License.


SCOPE OF LICENSE
----------------

This license applies to this program (XaraLX) and its constituent source
files only, and does not necessarily apply to other Xara products which may
in part share the same code base, and are subject to their own licensing
terms.

This license does not apply to files in the wxXtra directory, which
are built into a separate library, and are subject to the wxWindows
license contained within that directory in the file "WXXTRA-LICENSE".

This license does not apply to the binary libraries (if any) within
the "libs" directory, which are subject to a separate license contained
within that directory in the file "LIBS-LICENSE".


ARRANGEMENTS FOR CONTRIBUTION OF MODIFICATIONS
----------------------------------------------

Subject to the terms of the GNU Public License (see above), you are
free to do whatever you like with your modifications. However, you may
(at your option) wish contribute them to Xara's source tree. You can
find details of how to do this at:
  http://www.xaraxtreme.org/developers/

Prior to contributing your modifications, you will need to complete our
contributor agreement. This can be found at:
  http://www.xaraxtreme.org/developers/contribute/

Please note that Xara will not accept modifications which modify any of
the text between the start and end of this header (marked
XARAHEADERSTART and XARAHEADEREND).


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
