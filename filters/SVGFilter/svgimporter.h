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

    bool ParseGradientStopsEntity(xmlNodePtr cur, Gradient* pGradient);
	bool ParseLinearGradientEntity(xmlNodePtr cur);
	bool ParseRadialGradientEntity(xmlNodePtr cur);
	bool ParseDefsEntity(xmlNodePtr cur);
	bool ParsePathData(const wxString& data, PathDataVector& pathVector);
	bool ParsePathEntity(xmlNodePtr cur);
	bool ParseRectEntity(xmlNodePtr cur);
	bool ParseEllipseEntity(xmlNodePtr cur, bool bIsCircle);
	bool ParseLineEntity(xmlNodePtr cur);
	bool ParsePolylineData(const wxString& data, PointDVector& coordVector);
	bool ParsePolylineEntity(xmlNodePtr cur);
	bool ParsePolygonEntity(xmlNodePtr cur);
	bool ParseGroupEntity(xmlNodePtr cur);
	bool ParseEntitiesTree(xmlNodePtr cur);

	// transformations -------------------------------------------------------
	
	void PushTransformations();
	void PopTransformations();
	Transformation GetCurrentTransformation() const;
	Transformation& GetCurrentTransformation();

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
	Style ParseGradientStyle(xmlNodePtr cur) const;
    void ImportFillStyleFromDefs(Style& style, const wxString& sXmlId) const;
    void ImportStrokeStyleFromDefs(Style& style, const wxString& sXmlId) const;
	Gradient* FindGradientInDefs(const wxString& sXmlId) const;

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
