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
// svgimporter.h: This defines the SVG --> XAR parser and producer

#ifndef SVGIMPORTER_H
#define SVGIMPORTER_H

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/list.h"
#include "wx/hashmap.h"

// XAR handling library
#include "xarlib/xarlib.h"

// libxml2 library
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>

#ifndef LIBXML_READER_ENABLED
#error xmlreader must be enabled in libxml2!
#endif

#include <stdarg.h>

// for Style, StyleList and ColourRefHashTable
#include "styles.h"

// for SimpleVector
#include "utils.h"

// used for storing paths data (coordinate and verb)
struct PathData {
	PathData() : m_verb(0x00), m_coord(DocCoord(0, 0)) {}
	PathData(BYTE verb, const DocCoord& coord) : m_verb(verb), m_coord(coord) {}
	BYTE m_verb;
	DocCoord m_coord;
};
typedef SimpleVector<PathData> PathDataVector;
typedef SimpleVector<DocCoord> DocCoordVector;

struct Transformation {
	Transformation() { xf = yf = 1.0f; }
	double parentWidth, parentHeight;
	double width, height;
	double xf, yf;
};
WX_DECLARE_LIST(Transformation, TransformationList);

class SVGImporter {
public:
	SVGImporter(CXarExport* pExporter, const wxString& sFileName);
	~SVGImporter();

	bool Open();
	bool ParseAndOutput();

private:
	bool OutputXARHeader();
	bool OutputXARFooter();
	bool ParseDefinitionsTree();
	bool ParseRootTree();

	bool ParsePathEntity(xmlNodePtr cur);
	bool ParseRectEntity(xmlNodePtr cur);
	bool ParseCircleEntity(xmlNodePtr cur);
	bool ParseEllipseEntity(xmlNodePtr cur);
	bool ParseLineEntity(xmlNodePtr cur);
	bool ParsePolylineEntity(xmlNodePtr cur);
	bool ParsePolygonEntity(xmlNodePtr cur);
	bool ParseGroupEntity(xmlNodePtr cur);
	bool ParseEntitiesTree(xmlNodePtr cur);

	void GetEntityGeometry(xmlNodePtr cur, size_t size, ...);

	void PushTransformations();
	void PopTransformations();
	Transformation GetCurrentTransformation() const;
	Transformation& GetCurrentTransformation();

	void PushStyles();
	void PopStyles();
	Style GetCurrentStyle() const;
	Style& GetCurrentStyle();
	Style ParseStyle(xmlNodePtr cur, bool bIsObject) const;
	bool OutputStyles(const Style& cssStyle, UINT32 witch);

	INT32 DefineColour(const wxColour& col);
	bool ParsePathData(const wxString& data, PathDataVector& pathVector);
	bool ParsePolylineData(const wxString& data, DocCoordVector& coordVector);

	CXarExport*			m_pExporter;
	wxString			m_sFileName;
	xmlDocPtr			m_doc;
	xmlNodePtr			m_root;
	DocCoord			m_docSize;
	DocRect				m_viewBox;
	TransformationList	m_trans;
	StyleList			m_styles;
	ColourRefHashTable  m_colourRef;
};

#endif // !SVGIMPORTER_H
