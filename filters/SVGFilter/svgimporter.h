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
// for GradientStop, Gradient, pGradientHashTable
#include "gradients.h"
// for Transformation and TransformationList
#include "trans.h"
// for XARGenerator
#include "xargenerator.h"
	 
/***************************************************************************************************

>	class SVGImporter

	Author:		Sandro Sigala <sandro@sigala.it>
	Created:	1 July 2006
	Purpose:    Handles all the parsing of SVG documents and conversion to XAR documents.

***************************************************************************************************/

class SVGImporter {
public:
	SVGImporter(CXarExport* pExporter, const wxString& sFileName);
	~SVGImporter();

	bool Open();
	bool ParseAndOutput();

private:
	// SVG/XML entities parsers ----------------------------------------------

	bool ParseRootTree();

	bool ParseLinearGradientEntity(xmlNodePtr cur);
	bool ParseDefsEntity(xmlNodePtr cur);
	bool ParsePathData(const Transformation& trans, const wxString& data, PathDataVector& pathVector);
	bool ParsePathEntity(xmlNodePtr cur);
	bool ParseRectEntity(xmlNodePtr cur);
	bool ParseEllipseEntity(xmlNodePtr cur, bool bIsCircle);
	bool ParseLineEntity(xmlNodePtr cur);
	bool ParsePolylineData(const Transformation& trans, const wxString& data, DocCoordVector& coordVector);
	bool ParsePolylineEntity(xmlNodePtr cur);
	bool ParsePolygonEntity(xmlNodePtr cur);
	bool ParseGroupEntity(xmlNodePtr cur);
	bool ParseEntitiesTree(xmlNodePtr cur);

	// transformations -------------------------------------------------------
	
	void PushTransformations();
	void PopTransformations();
	Transformation GetCurrentTransformation() const;
	Transformation& GetCurrentTransformation();
	Transformation ParseTransformations(xmlNodePtr cur) const;

	double MeasureToMillipoints(const Transformation& trans, const wxString& sMeasure) const;
	PointD GetTransformedCoordinate(const Transformation& trans,
									const wxString& sX, const wxString& sY) const;
	PointD GetTransformedCoordinate(xmlNodePtr cur, const Transformation& trans,
									const char* pX, const char* pY) const;
	RectD GetTransformedRectangle(const Transformation& trans,
								  const wxString& sX, const wxString& sY,
								  const wxString& sWidth, const wxString sHeight) const;
	RectD GetTransformedRectangle(xmlNodePtr cur, const Transformation& trans,
								  const char* pX, const char* pY,
								  const char* pW, const char* pH) const;
	RectD GetTransformedCircle(const Transformation& trans,
							   const wxString& sX, const wxString& sY, const wxString& sR) const;
	RectD GetTransformedCircle(xmlNodePtr cur, const Transformation& trans,
							   const char* pX, const char* pY, const char* pR) const;
	RectD GetTransformedEllipse(const Transformation& trans,
								const wxString& sX, const wxString& sY,
								const wxString& sRX, const wxString& sRY) const;
	RectD GetTransformedEllipse(xmlNodePtr cur, const Transformation& trans,
								const char* pX, const char* pY,
								const char* pRX, const char* pRY) const;
	double GetTransformedMeasure(const Transformation& trans, const wxString& sMeasure) const;
	double GetTransformedMeasure(xmlNodePtr cur, const Transformation& trans, const char* pM) const;

	// styles ----------------------------------------------------------------

	void PushStyles();
	void PopStyles();
	Style GetCurrentStyle() const;
	Style& GetCurrentStyle();
	Style ParseStyle(xmlNodePtr cur, const Transformation& trans, bool bIsObject) const;
	void ImportStyleFromDefs(Style& style, const wxString& sXmlId) const;

	XARGenerator*		m_pGenerator;
	wxString			m_sFileName;
	xmlDocPtr			m_doc;
	xmlNodePtr			m_root;
	DocCoord			m_docSize;
	TransformationList	m_trans;
	StyleList			m_styles;
	pGradientHashTable	m_gradients;
};

#endif // !SVGIMPORTER_H
