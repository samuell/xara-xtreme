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
// import.cpp: This implements the SVG --> XAR backend

#include "svgfilter.h"
#include "svgimporter.h"
#include "utils.h"
#include "version.h"

// Check for XML document correctness
bool DoCanImportInternal(const wxString& sFileName)
{
	bool IsCompatible = true;
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile(sFileName.mb_str(wxConvUTF8));
	if (doc == NULL) {
		ReportError(_T("Failed to open input file"));
		IsCompatible = false;
		goto finish;
	}

	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		ReportError(_T("The document is empty"));
		xmlFreeDoc(doc);
		IsCompatible = false;
		goto finish;
	}
	if (!IsEntityName(cur, "svg")) {
		ReportError(_T("document root is not <svg>"));
		xmlFreeDoc(doc);
		IsCompatible = false;
		goto finish;
	}

	// All seem correct, so dispose document
	xmlFreeDoc(doc);

finish:
	wxPrintf(_T("%d\n"), IsCompatible ? 10 : 0);

	return IsCompatible;
}

// Import the XML document
bool DoImportInternal(CXarExport* pExporter, const wxString& sFileName)
{
	SVGImporter importer(pExporter, sFileName);

	if (!importer.Open())
		return false;
	if (!importer.ParseAndOutput())
		return false;

	return true;
}
