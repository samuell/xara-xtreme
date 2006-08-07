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
// export.cpp: This implements the XAR --> SVG backend

#include "svgfilter.h"
#include "version.h"

// Should we output verbose SVG?
static bool s_bVerboseSVG = false;
// The specification version of the SVG output
enum SVGVersion { SVGVersion_11, SVGVersion_12 };
static SVGVersion s_SVGVersion = SVGVersion_11;

// Here we keep data during XAR record parsing
class ExportData
{
public:
	wxTextOutputStream *pOutText;
};

bool DoPrepareExportInternal(const wxString& sFileName, const wxString& sXMLFileName)
{
	// nothing to do here
	return true;
}

// Cycle thru each XAR record
BOOL HandleRecord(void* pMagic, CXaraFileRecord* pRecord)
{
	ExportData* pData = (ExportData*)pMagic;
	wxTextOutputStream& OutText = *pData->pOutText;

	// Get the Tag of the record
	UINT32 Tag = pRecord->GetTag();

	OutText << _T("<!-- entity tag=\"") << Tag << _T("\" -->\n");
	// XXX just to see if it works, of course units are wrong
	switch (Tag) {
		case TAG_RECTANGLE_SIMPLE:
		{
			DocCoord Centre;
			INT32 MajorAxis;
			INT32 MinorAxis;
			pRecord->ReadCoord(&Centre);
			pRecord->ReadINT32(&MajorAxis);
			pRecord->ReadINT32(&MinorAxis);
			OutText << wxString::Format(_T("<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" />\n"),
										Centre.x, Centre.y, MajorAxis, MinorAxis);
		}
	}

	return true;
}

// read the settings from the xml capatibility file (<Private/> section)
static bool ParseXmlCapatibilityFile(const wxString& sXMLFileName)
{
    xmlTextReaderPtr reader;
    int ret;

    reader = xmlReaderForFile(sXMLFileName.mb_str(wxConvUTF8), NULL, 0);
    if (reader != NULL) {
		ret = xmlTextReaderRead(reader);
		while (ret == 1) {
			const xmlChar *name;

			// get the entity name
			name = xmlTextReaderConstName(reader);

			if (!xmlStrcmp(name, (const xmlChar *)"Private")) {
				const xmlChar *attribute;

				// get the value of "version" attribute
				attribute = xmlTextReaderGetAttribute(reader, (const xmlChar *)"version");
				if (attribute != NULL) {
					if (!xmlStrcmp(attribute, (const xmlChar *)"1.2"))
						s_SVGVersion = SVGVersion_12;
				}

				// get the value of "verbose" attribute
				attribute = xmlTextReaderGetAttribute(reader, (const xmlChar *)"verbose");
				if (attribute != NULL) {
					if (!xmlStrcmp(attribute, (const xmlChar *)"1"))
						s_bVerboseSVG = true;
				}

				break;	// stop parsing
			}
			// get next item
			ret = xmlTextReaderRead(reader);
		}

		// dispose xmlReader
		xmlFreeTextReader(reader);
    } else {
		ReportError(_T("Unable to open xml capatibility file"));
		return false;
    }

	return true;
}

bool DoExportInternal(CXarImport* pImporter, const wxString& sFileName, const wxString& sXMLFileName, bool bCompress)
{
	if (!ParseXmlCapatibilityFile(sXMLFileName))
		return false;

	// Open the output file for writing
	wxFileOutputStream OutFile(sFileName);
	if (!OutFile.Ok())
	{
		ReportError(_T("Failed to open output file."));
		// Set an appropriate error here
		return false;
	}

	wxOutputStream *pOutStream;
	// Make pOutStream point to the file stream or add another
	// compressed stream filter layer on top of it,
	// depending on the bCompress flag.
	if (bCompress) {
		pOutStream = new wxZlibOutputStream(OutFile, wxZ_BEST_COMPRESSION, wxZLIB_GZIP);
		// we will delete pOutStream later
	} else {
		pOutStream = &OutFile;
	}

	// Open text stream
	wxTextOutputStream OutText(*pOutStream);

	ExportData MyData;
	MyData.pOutText = &OutText;

	// Set up our record handler function
	if (!pImporter->SetHandler(&MyData, ::HandleRecord))
	{
		ReportError(_("SetHandler failed"));
		// Set an appropriate error here
		return false;
	}

	// Output standard XML header
	OutText << _T("<?xml version=\"1.0\" standalone=\"no\"?>\n");

	// DTD is specified only for version 1.1
	if (s_SVGVersion == SVGVersion_11)
		OutText	<< _T("<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n")
				<< _T("\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n");

//<svg width="10cm" height="5cm" viewBox="0 0 1000 500"
//     xmlns="http://www.w3.org/2000/svg" version="1.1">

	OutText << _T("<svg xmlns=\"http://www.w3.org/2000/svg\"");
	// output version attribute
	if (s_SVGVersion == SVGVersion_11)
		OutText << _T(" version=\"1.1\"");
	else // s_SVGVersion == SVGVersion_12
		OutText << _T(" version=\"1.2\"");
	OutText << _T(">\n");

	wxDateTime date = wxDateTime::Now();
	OutText << _T("<!-- Generator: Xara Xtreme 2.0, SVG Export Plug-In Version: ") << VERSION_TSTRING << _T(" -->\n")
			<< _T("<!-- Generated ") << date.FormatISODate() << _T(" ") << date.FormatISOTime() << _T(" -->\n")
			<< _T("<!-- not very useful exporter for now... -->\n");
	if (s_bVerboseSVG)
		OutText << _T("<!-- verbose mode enabled -->\n");

	// And do the import (cycles with HandleRecord() function)
	if (!pImporter->DoImport())
	{
		ReportError(_T("Failed to parse Xar data."));
		// Set an appropriate error here
		return false;
	}

	OutText << _T("</svg>\n");

	// Delete the compress stream if compression is enabled
	if (bCompress)
	{
		delete pOutStream;
	}

	return true;
}
