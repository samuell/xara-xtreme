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
// svgfilter.cpp: This implements the XAR <--> SVG command line interface

#include "svgfilter.h"

bool s_bDoProgress = false;
bool s_bDoVerbose = false;
bool s_bDoCompress = false;
#if SVGDEBUG
long iDebugTraceLevel = 0;
#endif

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

static int s_LastProgress = 0;

void ReportProgress(int Progress)
{
	if (s_bDoProgress)
	{
		if (Progress > 100)
			Progress = 100;

		if (Progress > s_LastProgress)
		{
			wxFprintf(stderr, _T("PROGRESS:%d%%\n"), Progress);
			s_LastProgress = Progress;
		}
	}
}

// Read previous XML from -x param (if empty or illegal then filter must generate default)
// Write new XML to -x param
// Filename being exported to given by -f param

static bool DoPrepareExport(const wxCmdLineParser& parser)
{
	wxString sFileName;
	if (!parser.Found(_T("f"), &sFileName))
	{
		ReportError(_T("A filename must be specified with -f"));
		return false;
	}

	wxString sXMLFileName;
	if (!parser.Found(_T("x"), &sXMLFileName))
	{
		ReportError(_T("An XML filename must be specified with -x"));
		return false;
	}

	// This need extending to handle the XML passing
	if (!DoPrepareExportInternal(sFileName, sXMLFileName))
	{
		return false;
	}

	return true;
}

// Read Xar format data from stdin
// Read XML from -x param
// Filename being exported to given by -f param

static bool DoExport(const wxCmdLineParser& parser)
{
	wxString sFileName;
	if (!parser.Found(_T("f"), &sFileName))
	{
		ReportError(_T("A filename must be specified with -f"));
		return false;
	}

	wxString sXMLFileName;
	if (!parser.Found(_T("x"), &sXMLFileName))
	{
		ReportError(_T("An XML filename must be specified with -x"));
		return false;
	}

	// First we ask the library to create a Xar importer
	CXarImport* pImporter = XarLibrary::CreateImporter();
	bool bRetVal = true;

	// Call PrepareImport passing stdin as the source
	if (pImporter->PrepareImport())
	{
		if (!DoExportInternal(pImporter, sFileName, sXMLFileName, s_bDoCompress))
		{
			bRetVal = false;
		}
	}
	else
	{
		ReportError(_T("PrepareImport failed"));
		bRetVal = false;
	}

	delete pImporter;

	return bRetVal;
}


// Filename to be tested given by -f param
// Compatibility rating written to stdout

static bool DoCanImport(const wxCmdLineParser& parser)
{
	wxString sFileName;
	if (!parser.Found(_T("f"), &sFileName))
	{
		ReportError(_T("A filename must be specified with -f"));
		return false;
	}

	if (!DoCanImportInternal(sFileName))
	{
		return false;
	}

	return true;
}

// Filename to be imported given by -f param
// Xar format data to be written to stdout

static bool DoImport(const wxCmdLineParser& parser)
{
	wxString sFileName;
	if (!parser.Found(_T("f"), &sFileName))
	{
		ReportError(_T("A filename must be specified with -f"));
		return false;
	}

#if SVGDEBUG
	wxString sDebugTraceLevel;
	if (parser.Found(_T("T"), &sDebugTraceLevel))
	{
		sDebugTraceLevel.ToLong(&iDebugTraceLevel);
		fprintf(stderr, "debug trace level %ld enabled\n", iDebugTraceLevel);
	}
#endif

	// Ask the library to create a Xar Exporter object
	CXarExport* pExporter = XarLibrary::CreateExporter();
	bool bRetVal = true;

	// Call StartExport passing stdout as where to write to
	if (pExporter->StartExport())
	{
		// Call DoImportInternal to actually do the work
		if (!DoImportInternal(pExporter, sFileName))
		{
			bRetVal = false;
		}
	}
	else
	{
		ReportError(_T("StartExport failed"));
		bRetVal = false;
	}

	// Delete the exporter object to clean up
	delete pExporter;

	return bRetVal;
}


static bool Run(const wxCmdLineParser& parser)
{
	// Read the various global flags
	s_bDoProgress = parser.Found(_T("g"));
	s_bDoVerbose = parser.Found(_T("v"));
	s_bDoCompress = parser.Found(_T("z"));

	bool bPrepareExport = parser.Found(_T("p"));
	bool bExport = parser.Found(_T("e"));
	bool bCanImport = parser.Found(_T("c"));
	bool bImport = parser.Found(_T("i"));

	bool bOnlyOneError = false;
	bool bRetVal = true;

	// There is almost certainly a much nicer way of doing this
	// When I find it or get around to deriving my own wxCmdLineParser
	// then I'll change it
	if (bPrepareExport)
	{
		if (bExport || bCanImport || bImport)
		{
			bOnlyOneError = true;
		}
		else
		{
			bRetVal = DoPrepareExport(parser);
		}
	}
	else if (bExport)
	{
		if (bCanImport || bImport)
		{
			bOnlyOneError = true;
		}
		else
		{
			bRetVal = DoExport(parser);
		}
	}
	else if (bCanImport)
	{
		if (bImport)
		{
			bOnlyOneError = true;
		}
		else
		{
			bRetVal = DoCanImport(parser);
		}
	}
	else if (bImport)
	{
		bRetVal = DoImport(parser);
	}
	else
	{
		bOnlyOneError = true;
	}

	if (bOnlyOneError)
	{
		ReportError(_T("Exactly one of -p, -e, -c or -i must be specified"));
		bRetVal = false;
	}

	return bRetVal;
}


void ShowCmdLine(const wxCmdLineParser& parser)
{
	wxString s(_T("\n"));
	s << _T("Verbose:\t") << (parser.Found(_T("v")) ? _T("yes") : _T("no")) << _T('\n');
	s << _T("PrepareExport:\t") << (parser.Found(_T("p")) ? _T("yes") : _T("no")) << _T('\n');
	s << _T("Export:\t\t") << (parser.Found(_T("e")) ? _T("yes") : _T("no")) << _T('\n');
	s << _T("CanImport:\t") << (parser.Found(_T("c")) ? _T("yes") : _T("no")) << _T('\n');
	s << _T("Import:\t\t") << (parser.Found(_T("i")) ? _T("yes") : _T("no")) << _T('\n');
	s << _T("(De)compress:\t\t") << (parser.Found(_T("z")) ? _T("yes") : _T("no")) << _T('\n');

	wxString strVal;
	if ( parser.Found(_T("f"), &strVal) )
		s << _T("File:\t") << strVal << _T('\n');

	if ( parser.Found(_T("x"), &strVal) )
		s << _T("XML File:\t") << strVal << _T('\n');

	wxLogMessage(s);
}


/****************************************************************************

>	int main(int argc, char* argv[])

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/2005

	Inputs:		argc		- number of parameters on command line
				argv[]		- array of parameter strings
	Returns:
	Purpose:	The main entry point of the code.

****************************************************************************/

int main(int argc, char* argv[])
{
#if wxUSE_UNICODE
	wxChar **wxArgv = new wxChar *[argc + 1];

	{
		int n;

		for (n = 0; n < argc; n++ )
		{
			wxMB2WXbuf warg = wxConvertMB2WX(argv[n]);
			wxArgv[n] = wxStrdup(warg);
		}

		wxArgv[n] = NULL;
	}
#else // !wxUSE_UNICODE
	#define wxArgv argv
#endif // wxUSE_UNICODE/!wxUSE_UNICODE

	wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");

	wxInitializer initializer;
	if ( !initializer )
	{
		fprintf(stderr, "Failed to initialize the wxWidgets library, aborting.\n");
		return -1;
	}

	// Default to OK exit value
	int RetVal = 0;

	static const wxCmdLineEntryDesc cmdLineDesc[] =
	{
		{ wxCMD_LINE_SWITCH, _T("h"), _T("help"), _T("show this help message"),
			wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
		{ wxCMD_LINE_SWITCH, _T("v"), _T("verbose"), _T("be verbose") },
		{ wxCMD_LINE_SWITCH, _T("g"), _T("progress"), _T("give progress") },
		{ wxCMD_LINE_SWITCH, _T("p"), _T("prepareexport"), _T("prepare export") },
		{ wxCMD_LINE_SWITCH, _T("e"), _T("export"), _T("do export") },
		{ wxCMD_LINE_SWITCH, _T("c"), _T("canimport"), _T("can import") },
		{ wxCMD_LINE_SWITCH, _T("i"), _T("import"), _T("do import") },
		{ wxCMD_LINE_SWITCH, _T("z"), _T("compress"), _T("(de)compress (output) input file") },
		{ wxCMD_LINE_OPTION, _T("f"), _T("file"),  _T("input/output file") },
		{ wxCMD_LINE_OPTION, _T("x"), _T("xmlfile"),  _T("xml configuration file") },
#if SVGDEBUG
		{ wxCMD_LINE_OPTION, _T("T"), _T("tracelevel"),  _T("enable debug") },
#endif
		{ wxCMD_LINE_NONE }
	};

	wxCmdLineParser parser(cmdLineDesc, argc, wxArgv);

	switch ( parser.Parse() )
	{
		case -1:
			wxLogMessage(_T("Help was given, terminating."));
			break;

		case 0:
//			ShowCmdLine(parser);

			if (Run(parser))
				RetVal = 0;
			else
				RetVal = 1;
			break;

		default:
			RetVal = 1;
			break;
	}

#if wxUSE_UNICODE
	{
		for ( int n = 0; n < argc; n++ )
			free(wxArgv[n]);

		delete [] wxArgv;
	}
#endif // wxUSE_UNICODE

	wxUnusedVar(argc);
	wxUnusedVar(argv);

	// May need to use exit(RetVal) here if the OS ignores the return value from main
	return RetVal;
}
