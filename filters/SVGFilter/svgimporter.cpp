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
// svgimporter.cpp: This implements the SVG --> XAR parser and producer

#include "svgfilter.h"
#include "svgimporter.h"
#include "utils.h"
#include "version.h"

#if SVGDEBUG
void DebugDumpTransformation(int lvl, const Transformation& trans);		// from trans.cpp
#endif

/*----------------------------------------------------------------------------
 *
 * Constructor/destructor
 *
 *----------------------------------------------------------------------------*/

SVGImporter::SVGImporter(CXarExport* pExporter, const wxString& sFileName)
{
	m_sFileName = sFileName;
	m_doc = NULL;
	m_root = NULL;

	// Create XAR generator object
	m_pGenerator = new XARGenerator(pExporter);

	// Prepare hash table containing CSS colours ("red", "navy", etc.)
	Style::PrepareColourHashTable();

	// Append the root transformation: will contain the document geometry
	Transformation *pTrans = new Transformation();
	m_trans.Append(pTrans);

	// Append the root style: will contain the default style
	Style* pStyle = new Style();
	m_styles.Append(pStyle);
}

SVGImporter::~SVGImporter()
{
	// Dispose XAR generator object
	delete m_pGenerator;

	// Dispose XML document
	if (m_doc != NULL)
		xmlFreeDoc(m_doc);

	wxASSERT(m_trans.GetCount() == 1);
	wxTransformationListNode* pTNode = m_trans.GetLast();
	Transformation* pTrans = pTNode->GetData();
	m_trans.DeleteNode(pTNode);
	delete pTrans;

	wxASSERT(m_styles.GetCount() == 1);
	wxStyleListNode* pSNode = m_styles.GetLast();
	Style* pStyle = pSNode->GetData();
	m_styles.DeleteNode(pSNode);
	delete pStyle;

	// flush gradients hashtable
	typedef pGradientHashTable::iterator I;
	for (I i = m_gradients.begin(); i != m_gradients.end(); ++i) {
		Gradient* pGradient = i->second;
		delete pGradient;
    }
	m_gradients.clear();
}

bool SVGImporter::Open()
{
	m_doc = xmlParseFile(m_sFileName.mb_str(wxConvUTF8));
	if (m_doc == NULL) {
		ReportError(_T("Failed to open input file"));
		return false;
	}

	m_root = xmlDocGetRootElement(m_doc);
	if (m_root == NULL) {
		ReportError(_T("The document is empty"));
		xmlFreeDoc(m_doc);
		m_doc = NULL;
		return false;
	}
	if (!IsEntityName(m_root, "svg")) {
		ReportError(_T("document root is not <svg>"));
		xmlFreeDoc(m_doc);
		m_doc = NULL;
		return false;
	}

	Transformation& trans = GetCurrentTransformation();

	// Parse width and height attributes

	wxString sWidth  = GetStringProperty(m_root, "width");
	wxString sHeight = GetStringProperty(m_root, "height");
	double fDocWidth, fDocHeight;

	if (!sWidth.IsEmpty() /*&& !sHeight.IsEmpty()*/) {
		fDocWidth = AbsoluteMeasureToMillipoints(sWidth);
		fDocHeight = AbsoluteMeasureToMillipoints(sHeight);
	} else {
		// no size or no valid size specified, let's suppose an A4 paper
		fDocWidth = MM2PT(210.0f)*1000.0f;
		fDocHeight = MM2PT(297.0f)*1000.0f;
	}

	trans.size = PointD(fDocWidth, fDocHeight);
	m_docSize = DocCoord((INT32)fDocWidth, (INT32)fDocHeight);

	wxString sViewbox = GetStringProperty(m_root, "viewBox");
	if (!sViewbox.IsEmpty() && TrimWs(sViewbox) != _T("none")) {
		// parse the four space or comma separated values

		// replace commas with whitespace, if any
		sViewbox.Replace(_T(","), _T(" "));

		double fX      = TakeMilliNumber(sViewbox);
		double fY      = TakeMilliNumber(sViewbox);
		double fWidth  = TakeMilliNumber(sViewbox);
		double fHeight = TakeMilliNumber(sViewbox);
		double fScaleX = 1.0;
		double fScaleY = 1.0;

		if (fWidth != 0.0) {
			fScaleX = fDocWidth / fWidth;
		}
		if (fHeight != 0.0) {
			fScaleY = fDocHeight / fHeight;
		}

		// XXX other preserveAspectRatio flags?
		wxString sPreserve = GetStringProperty(m_root, "preserveAspectRatio");
		if (TrimWs(sPreserve) != _T("none"))
			fScaleY = fScaleX;

		trans *= Transformation::CreateScale(fScaleX, fScaleY);
		trans *= Transformation::CreateTranslate(-fX, -fY);
	}

#if SVGDEBUG
	svgtrace(DBGTRACE_TRANS, "document width: %d, height: %d\n", m_docSize.x, m_docSize.y);
	DebugDumpTransformation(DBGTRACE_TRANS, trans);
#endif

	return true;
}

/*----------------------------------------------------------------------------
 *
 * XML/SVG parsing functions
 *
 *----------------------------------------------------------------------------*/

bool SVGImporter::ParsePathData(const Transformation& trans, const wxString& data, PathDataVector& pathVector)
{
	wxString s = data;
	PointD pLast(0,0);
	PointD pLastCtrl(0,0);
	bool bHaveLastCtrl = false;

	// replace commas with whitespace
	s.Replace(_T(","), _T(" "));

	while (!s.Trim(false).IsEmpty()) {
		wxChar c = s[0];
		if (c == _T('M') || c == _T('m')) {
			// moveto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX = TakeMilliNumber(s);
				double fY = TakeMilliNumber(s);

#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "path: %c %.2f %.2f\n", action, fX, fY);
#endif

				if (action == _T('m')) { // relative
					fX += pLast.x;
					fY += pLast.y;
				}
				pLast.x = fX;
				pLast.y = fY;

				trans.ApplyToCoordinate(fX, fY, &fX, &fY);
				pathVector.Append(PathData(0x06, DocCoord((INT32)fX, m_docSize.y - (INT32)fY))); // moveto
			}
		} else if (c == _T('L') || c == _T('l')) {
			// lineto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX = TakeMilliNumber(s);
				double fY = TakeMilliNumber(s);
#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "path: %c %.2f %.2f\n", action, fX, fY);
#endif
				if (action == _T('l')) { // relative
					fX += pLast.x;
					fY += pLast.y;
				}
				pLast.x = fX;
				pLast.y = fY;

				trans.ApplyToCoordinate(fX, fY, &fX, &fY);
				pathVector.Append(PathData(0x02, DocCoord((INT32)fX, m_docSize.y - (INT32)fY))); // lineto
			}
		} else if (c == _T('H') || c == _T('h')) {
			// horizontal lineto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX = TakeMilliNumber(s);
				double fY = pLast.y;
#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "path: %c %.2f\n", action, fX);
#endif
				if (action == _T('h')) {
					fX += pLast.x;
				}
				pLast.x = fX;

				trans.ApplyToCoordinate(fX, fY, &fX, &fY);
				pathVector.Append(PathData(0x02, DocCoord((INT32)fX, m_docSize.y - (INT32)fY))); // lineto
			}
		} else if (c == _T('V') || c == _T('v')) {
			// vertical lineto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX = pLast.x;
				double fY = TakeMilliNumber(s);
#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "path: %c %.2f\n", action, fY);
#endif
				if (action == _T('v')) {
					fY += pLast.y;
				}
				pLast.y = fY;

				trans.ApplyToCoordinate(fX, fY, &fX, &fY);
				pathVector.Append(PathData(0x02, DocCoord((INT32)fX, m_docSize.y - (INT32)fY))); // lineto
			}
		} else if (c == _T('C') || c == _T('c')) {
			// Bezier cubic curveto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX1 = TakeMilliNumber(s);
				double fY1 = TakeMilliNumber(s);
				double fX2 = TakeMilliNumber(s);
				double fY2 = TakeMilliNumber(s);
				double fX = TakeMilliNumber(s);
				double fY = TakeMilliNumber(s);
#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "bezier: %c %.2f %.2f %.2f %.2f %.2f %.2f\n", action, fX1, fY1, fX2, fY2, fX, fY);
#endif
				if (action == _T('c')) {
					fX1 += pLast.x;
					fY1 += pLast.y;
					fX2 += pLast.x;
					fY2 += pLast.y;
					fX += pLast.x;
					fY += pLast.y;
				}
				pLast.x = fX;
				pLast.y = fY;
				pLastCtrl.x = fX2;
				pLastCtrl.y = fY2;
				bHaveLastCtrl = true;

				trans.ApplyToCoordinate(fX1, fY1, &fX1, &fY1);
				trans.ApplyToCoordinate(fX2, fY2, &fX2, &fY2);
				trans.ApplyToCoordinate(fX, fY, &fX, &fY);
				pathVector.Append(PathData(0x04, DocCoord((INT32)fX1, m_docSize.y - (INT32)fY1))); // bezierto
				pathVector.Append(PathData(0x04, DocCoord((INT32)fX2, m_docSize.y - (INT32)fY2))); // bezierto
				pathVector.Append(PathData(0x04, DocCoord((INT32)fX, m_docSize.y - (INT32)fY))); // bezierto
			}
		} else if (c == _T('S') || c == _T('s')) {
			// Bezier smooth cubic curveto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX1; // we will calculate later as reflection of previous
				double fY1; // control point to the current point (fX, fY)
				double fX2 = TakeMilliNumber(s);
				double fY2 = TakeMilliNumber(s);
				double fX = TakeMilliNumber(s);
				double fY = TakeMilliNumber(s);

				if (bHaveLastCtrl) {
					// we have control point, i.e. there was a 'C', 'c', 'S' or 's'
					// command previously.
					fX1 = 2.0*pLast.x - pLastCtrl.x;
					fY1 = 2.0*pLast.y - pLastCtrl.y;
				} else {
					fX1 = fX;
					fY1 = fY;
				}
#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "bezier: %c %.2f %.2f %.2f %.2f %.2f %.2f\n", action, fX1, fY1, fX2, fY2, fX, fY);
#endif
				if (action == _T('s')) {
					fX2 += pLast.x;
					fY2 += pLast.y;
					fX += pLast.x;
					fY += pLast.y;
				}
				pLast.x = fX;
				pLast.y = fY;
				pLastCtrl.x = fX2;
				pLastCtrl.y = fY2;
				bHaveLastCtrl = true;

				trans.ApplyToCoordinate(fX1, fY1, &fX1, &fY1);
				trans.ApplyToCoordinate(fX2, fY2, &fX2, &fY2);
				trans.ApplyToCoordinate(fX, fY, &fX, &fY);
				pathVector.Append(PathData(0x04, DocCoord((INT32)fX1, m_docSize.y - (INT32)fY1))); // bezierto
				pathVector.Append(PathData(0x04, DocCoord((INT32)fX2, m_docSize.y - (INT32)fY2))); // bezierto
				pathVector.Append(PathData(0x04, DocCoord((INT32)fX, m_docSize.y - (INT32)fY))); // bezierto
			}
		} else if (c == _T('Q') || c == _T('q')) {
			// Bezier quadratic curveto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX1 = pLast.x;
				double fY1 = pLast.y;
				double fX2 = TakeMilliNumber(s);
				double fY2 = TakeMilliNumber(s);
				double fX = TakeMilliNumber(s);
				double fY = TakeMilliNumber(s);

				if (action == _T('q')) {
					fX1 += pLast.x;
					fY1 += pLast.y;
					fX2 += pLast.x;
					fY2 += pLast.y;
					fX += pLast.x;
					fY += pLast.y;
				}
#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "bezier: %c %.2f %.2f %.2f %.2f\n", action, fX2, fY2, fX, fY);
#endif
				pLast.x = fX;
				pLast.y = fY;
				pLastCtrl.x = fX2;
				pLastCtrl.y = fY2;
				bHaveLastCtrl = true;

				// convert to cubic Bezier
				double fXc1 = (fX1 + 2.0*fX2)/3.0;
				double fYc1 = (fY1 + 2.0*fY2)/3.0;
				double fXc2 = (2.0*fX2 + fX)/3.0;
				double fYc2 = (2.0*fY2 + fY)/3.0;

				trans.ApplyToCoordinate(fXc1, fYc1, &fXc1, &fYc1);
				trans.ApplyToCoordinate(fXc2, fYc2, &fXc2, &fYc2);
				trans.ApplyToCoordinate(fX, fY, &fX, &fY);
				pathVector.Append(PathData(0x04, DocCoord((INT32)fXc1, m_docSize.y - (INT32)fYc1))); // bezierto
				pathVector.Append(PathData(0x04, DocCoord((INT32)fXc2, m_docSize.y - (INT32)fYc2))); // bezierto
				pathVector.Append(PathData(0x04, DocCoord((INT32)fX, m_docSize.y - (INT32)fY))); // bezierto
			}
		} else if (c == _T('T') || c == _T('t')) {
			// Bezier smooth quadratic curveto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX1 = pLast.x;
				double fY1 = pLast.y;
				double fX2; // we will calculate later as reflection of previous
				double fY2; // control point to the current point (fX, fY)
				double fX = TakeMilliNumber(s);
				double fY = TakeMilliNumber(s);

				if (bHaveLastCtrl) {
					// we have control point, i.e. there was a 'Q', 'q', 'T' or 't'
					// command previously.
					fX2 = 2*fX1 - pLastCtrl.x;
					fY2 = 2*fY1 - pLastCtrl.y;
				} else {
					fX2 = fX;
					fY2 = fY;
				}
#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "bezier: %c %.2f %.2f\n", action, fX, fY);
#endif
				if (action == _T('t')) {
					fX1 += pLast.x;
					fY1 += pLast.y;
					fX2 += pLast.x;
					fY2 += pLast.y;
					fX += pLast.x;
					fY += pLast.y;
				}
				pLast.x = fX;
				pLast.y = fY;
				pLastCtrl.x = fX2;
				pLastCtrl.y = fY2;
				bHaveLastCtrl = true;

				// convert to cubic Bezier
				double fXc1 = (fX1 + 2.0*fX2)/3.0;
				double fYc1 = (fY1 + 2.0*fY2)/3.0;
				double fXc2 = (2.0*fX2 + fX)/3.0;
				double fYc2 = (2.0*fY2 + fY)/3.0;

				trans.ApplyToCoordinate(fXc1, fYc1, &fXc1, &fYc1);
				trans.ApplyToCoordinate(fXc2, fYc2, &fXc2, &fYc2);
				trans.ApplyToCoordinate(fX, fY, &fX, &fY);
				pathVector.Append(PathData(0x04, DocCoord((INT32)fXc1, m_docSize.y - (INT32)fYc1))); // bezierto
				pathVector.Append(PathData(0x04, DocCoord((INT32)fXc2, m_docSize.y - (INT32)fYc2))); // bezierto
				pathVector.Append(PathData(0x04, DocCoord((INT32)fX, m_docSize.y - (INT32)fY))); // bezierto
			}
		} else if (c == _T('Z') || c == _T('z')) {
			// closepath
			s = s.Mid(1);

#if SVGDEBUG
			svgtrace(DBGTRACE_PATHS, "closepath\n");
#endif

			if (pathVector.GetCount() > 0) {
				BYTE& b = pathVector[pathVector.GetCount()-1].m_verb;
				b |= 0x01; // add closepath flag
			} else {
				// XXX print a syntax error
			}
		} else {
				// XXX print a syntax error
			// eat character
			s = s.Mid(1);
		}
	}

	return true;
}

bool SVGImporter::ParsePathEntity(xmlNodePtr cur)
{
	PathDataVector pathVector;

	/*T*/ PushTransformations();
	Transformation& trans = GetCurrentTransformation();
	trans *= ParseTransformations(cur);

#if SVGDEBUG
	svgtrace(DBGTRACE_SHAPES, "path\n");
#endif

	wxString sPathData = GetStringProperty(cur, "d");
	ParsePathData(trans, sPathData, pathVector);
	if (pathVector.GetCount() < 2) { // bogus path
		/*T*/ PopTransformations();
		return false;
	}

	/*S*/ PushStyles();
	Style& style = GetCurrentStyle(); // reference to the current style
	style = ParseStyle(cur, trans, true);
	m_pGenerator->OutputPathEntity(style, pathVector);
	/*S*/ PopStyles();

	/*T*/ PopTransformations();

	return true;
}

bool SVGImporter::ParseRectEntity(xmlNodePtr cur)
{
	RectD r;

	/*T*/ PushTransformations();
	Transformation& trans = GetCurrentTransformation();
	trans *= ParseTransformations(cur);

	r = GetTransformedRectangle(cur, trans, "x", "y", "width", "height");
	double fRoundAxis = GetTransformedMeasure(cur, trans, "rx");
	if (fRoundAxis == 0.0)
		fRoundAxis = GetTransformedMeasure(cur, trans, "ry");
	// XXX ry is not supported, no way to specify in XAR
	// the vertical axis of ellipse using only TAG_RECTANGLE_XXX_ROUNDED
	// If rx != ry, should be simulated with a few shapes

	/*S*/ PushStyles();
	Style& style = GetCurrentStyle(); // reference to the current style
	style = ParseStyle(cur, trans, true);
	m_pGenerator->OutputRectEntity(style, r, fRoundAxis);
	/*S*/ PopStyles();

	/*T*/ PopTransformations();

	return true;
}

bool SVGImporter::ParseEllipseEntity(xmlNodePtr cur, bool bIsCircle)
{
	RectD r;

	/*T*/ PushTransformations();
	Transformation& trans = GetCurrentTransformation();
	trans *= ParseTransformations(cur);

	if (bIsCircle)
		r = GetTransformedCircle(cur, trans, "cx", "cy", "r");
	else
		r = GetTransformedEllipse(cur, trans, "cx", "cy", "rx", "ry");

	/*S*/ PushStyles();
	Style& style = GetCurrentStyle(); // reference to the current style
	style = ParseStyle(cur, trans, true);
	m_pGenerator->OutputEllipseEntity(style, r);
	/*S*/ PopStyles();

	/*T*/ PopTransformations();

	return true;
}

bool SVGImporter::ParseLineEntity(xmlNodePtr cur)
{
	PointD p1, p2;

	/*T*/ PushTransformations();
	Transformation& trans = GetCurrentTransformation();
	trans *= ParseTransformations(cur);

	p1 = GetTransformedCoordinate(cur, trans, "x1", "y1");
	p2 = GetTransformedCoordinate(cur, trans, "x2", "y2");

	/*S*/ PushStyles();
	Style& style = GetCurrentStyle(); // reference to the current style
	style = ParseStyle(cur, trans, true);
	m_pGenerator->OutputLineEntity(style, p1, p2);
	/*S*/ PopStyles();

	/*T*/ PopTransformations();

	return true;
}

bool SVGImporter::ParsePolylineData(const Transformation& trans, const wxString& data, DocCoordVector& coordVector)
{
	wxString s = data;

	// replace commas with whitespace
	s.Replace(_T(","), _T(" "));

	while (!s.Trim(false).IsEmpty()) {
		if (IsNumberChar(s[0])) {
			while (IsNumberChar(s[0])) {
				double fX = TakeMilliNumber(s);
				double fY = TakeMilliNumber(s);
				trans.ApplyToCoordinate(fX, fY, &fX, &fY);
				fY = m_docSize.y - fY;

#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "polyline point: %f %f\n", fX, fY);
#endif

				INT32 iX = (INT32)fX;
				INT32 iY = (INT32)fY;
				coordVector.Append(DocCoord(iX, iY));
				s.Trim(false); // trim whitespace on the left
			}
		} else {
			// XXX print a syntax error
			// eat character
			s = s.Mid(1);
		}
	}

	return true;
}

bool SVGImporter::ParsePolylineEntity(xmlNodePtr cur)
{
	DocCoordVector coordVector;

	/*T*/ PushTransformations();
	Transformation& trans = GetCurrentTransformation();
	trans *= ParseTransformations(cur);

#if SVGDEBUG
	svgtrace(DBGTRACE_SHAPES, "polyline\n");
#endif

	wxString sPathData = GetStringProperty(cur, "points");
	ParsePolylineData(trans, sPathData, coordVector);
	if (coordVector.GetCount() < 2) { // bogus polyline
		/*T*/ PopTransformations();
		return false;
	}

	/*S*/ PushStyles();
	Style& style = GetCurrentStyle(); // reference to the current style
	style = ParseStyle(cur, trans, true);
	m_pGenerator->OutputPolylineEntity(style, coordVector);
	/*S*/ PopStyles();

	/*T*/ PopTransformations();

	return true;
}

bool SVGImporter::ParsePolygonEntity(xmlNodePtr cur)
{
	DocCoordVector coordVector;

	/*T*/ PushTransformations();
	Transformation& trans = GetCurrentTransformation();
	trans *= ParseTransformations(cur);

#if SVGDEBUG
	svgtrace(DBGTRACE_SHAPES, "polygon\n");
#endif

	wxString sPathData = GetStringProperty(cur, "points");
	ParsePolylineData(trans, sPathData, coordVector);
	if (coordVector.GetCount() < 2) { // bogus polygon
		/*T*/ PopTransformations();
		return false;
	}

	/*S*/ PushStyles();
	Style& style = GetCurrentStyle(); // reference to the current style
	style = ParseStyle(cur, trans, true);
	m_pGenerator->OutputPolygonEntity(style, coordVector);
	/*S*/ PopStyles();

	/*T*/ PopTransformations();

	return true;
}

bool SVGImporter::ParseGroupEntity(xmlNodePtr cur)
{
	// Points to the first child
	xmlNodePtr child = cur->xmlChildrenNode;

#if SVGDEBUG
	static int groupid = 0;
	svgtrace(DBGTRACE_GROUPS, "opengroup: %d\n", ++groupid);
#endif

	/*T*/ PushTransformations();
	Transformation& trans = GetCurrentTransformation();
	trans *= ParseTransformations(cur);

	/*S*/ PushStyles();
	Style& style = GetCurrentStyle(); // reference to the current style
	style = ParseStyle(cur, trans, false);

	bool ok = true;
	ok = m_pGenerator->EnterGroup();
	ok = ParseEntitiesTree(child);
	ok = m_pGenerator->LeaveGroup();
	/*S*/ PopStyles();

	/*T*/ PopTransformations();

#if SVGDEBUG
	svgtrace(DBGTRACE_GROUPS, "closegroup: %d\n", groupid--);
#endif

	return ok;
}

bool SVGImporter::ParseLinearGradientEntity(xmlNodePtr cur)
{
	wxString sXmlId = GetStringProperty(cur, "id");
	if (sXmlId.IsEmpty())
		sXmlId = GetStringProperty(cur, "xml:id");
	sXmlId = TrimWs(sXmlId);
	if (sXmlId.IsEmpty()) { // no id specified, useless gradient
		/*T*/ PopTransformations();
		return false;
	}

	double x1 = 0.0;
	double y1 = 0.0;
	double x2 = 1.0;
	double y2 = 1.0;
	if (IsPropertyDefined(cur, "x1")) {
		x1 = GetDoubleProperty(cur, "x1");
	}
	if (IsPropertyDefined(cur, "y1")) {
		y1 = GetDoubleProperty(cur, "y1");
	}
	if (IsPropertyDefined(cur, "x2")) {
		x2 = GetDoubleProperty(cur, "x2");
	}
	if (IsPropertyDefined(cur, "y2")) {
		y2 = GetDoubleProperty(cur, "y2");
	}
	
	Gradient::Units units = Gradient::ObjectBoundingBox;
	wxString sUnits = GetStringProperty(cur, "gradientUnits");
	if (sUnits == _T("userSpaceOnUse")) {
		units = Gradient::UserSpaceOnUse;
	}

#if SVGDEBUG
	svgtrace(DBGTRACE_GRADIENTS, "gradient: '%s' %.2f %.2f %.2f %.2f\n",
			 (const char *)sXmlId.mb_str(wxConvUTF8), x1, y1, x2, y2);
#endif

	Gradient* pGradient = new Gradient;
	pGradient->type = Gradient::Linear;
	pGradient->xmlId = sXmlId;
	pGradient->units = units;
	pGradient->x1 = x1;
	pGradient->y1 = y1;
	pGradient->x2 = x2;
	pGradient->y2 = y2;

	// Points to first <linearGradient /> child
	cur = cur->xmlChildrenNode;

	// scan all the elements
	while (cur != NULL) {
		if (IsEntityName(cur, "stop")) {
			double offset = GetDoubleProperty(cur, "offset");
			wxString sStopColour = GetStringProperty(cur, "stop-color");
			wxColour stopColour = Style::ParseColour(sStopColour);
			
#if SVGDEBUG
			svgtrace(DBGTRACE_GRADIENTS, "gradient stop: %.2f (%d,%d,%d)\n", offset, stopColour.Red(), stopColour.Green(), stopColour.Blue());
#endif

			GradientStop* pStop = new GradientStop();
			pStop->offset = offset;
			pStop->stopColour = stopColour;
			pGradient->stops.Append(pStop);
		}
		cur = cur->next;
	}

	// add to hash table
	m_gradients[sXmlId] = pGradient;

	return true;
}

bool SVGImporter::ParseDefsEntity(xmlNodePtr cur)
{
	// Points to first <defs /> child
	cur = cur->xmlChildrenNode;
	
	while (cur != NULL) {
		if (IsEntityName(cur, "linearGradient")) {
			ParseLinearGradientEntity(cur);
		}
		cur = cur->next;
	}

	return true;
}

bool SVGImporter::ParseEntitiesTree(xmlNodePtr cur)
{
	while (cur != NULL) {
		if (IsEntityName(cur, "defs")) {
			ParseDefsEntity(cur);
		} else if (IsEntityName(cur, "path")) {
			ParsePathEntity(cur);
		} else if (IsEntityName(cur, "rect")) {
			ParseRectEntity(cur);
		} else if (IsEntityName(cur, "circle")) {
			ParseEllipseEntity(cur, true);
		} else if (IsEntityName(cur, "ellipse")) {
			ParseEllipseEntity(cur, false);
		} else if (IsEntityName(cur, "line")) {
			ParseLineEntity(cur);
		} else if (IsEntityName(cur, "polyline")) {
			ParsePolylineEntity(cur);
		} else if (IsEntityName(cur, "polygon")) {
			ParsePolygonEntity(cur);
		} else if (IsEntityName(cur, "g")) {
			ParseGroupEntity(cur);
		}
		cur = cur->next;
	}

	return true;
}

bool SVGImporter::ParseRootTree()
{
	xmlNodePtr cur;

	// Points to first <svg /> child
	cur = m_root->xmlChildrenNode;

	return ParseEntitiesTree(cur);
}

bool SVGImporter::ParseAndOutput()
{
	m_pGenerator->SetDocumentSize(m_docSize);
	if (!m_pGenerator->OutputHeader())
		return false;
	if (!ParseRootTree())
		return false;
	if (!m_pGenerator->OutputFooter())
		return false;
	return true;
}

/*----------------------------------------------------------------------------
 *
 * Transformations handling functions
 *
 *----------------------------------------------------------------------------*/

#if SVGDEBUG
static int transLevel = 0;
#endif

void SVGImporter::PushTransformations()
{
	wxASSERT(m_trans.GetCount() > 0);
	Transformation* pTrans = m_trans.GetLast()->GetData();
	m_trans.Append(new Transformation(*pTrans));
#if SVGDEBUG
	svgtrace(DBGTRACE_TRANS_STACK, "pushing transformations (level %d)\n", ++transLevel);
	DebugDumpTransformation(DBGTRACE_TRANS_STACK, *pTrans);
#endif
}

void SVGImporter::PopTransformations()
{
	wxASSERT(m_trans.GetCount() > 1);
	wxTransformationListNode* pNode = m_trans.GetLast();
	Transformation* pTrans = pNode->GetData();
	m_trans.DeleteNode(pNode);
	delete pTrans;
#if SVGDEBUG
	svgtrace(DBGTRACE_TRANS_STACK, "popping transformations (level %d)\n", transLevel--);
	Transformation *pOldTrans = m_trans.GetLast()->GetData();
	svgtrace(DBGTRACE_TRANS_STACK, "restoring: ");
	DebugDumpTransformation(DBGTRACE_TRANS_STACK, *pOldTrans);
#endif
}

Transformation SVGImporter::GetCurrentTransformation() const
{
	wxASSERT(m_trans.GetCount() > 0);
	wxASSERT(m_trans.GetLast() != NULL);
	Transformation* pTrans = m_trans.GetLast()->GetData();
	return *pTrans;
}

Transformation& SVGImporter::GetCurrentTransformation()
{
	wxASSERT(m_trans.GetCount() > 0);
	wxASSERT(m_trans.GetLast() != NULL);
	Transformation* pTrans = m_trans.GetLast()->GetData();
	return *pTrans;
}

Transformation SVGImporter::ParseTransformations(xmlNodePtr cur) const
{
	wxString s = GetStringProperty(cur, "transform");
	Transformation trans;

	// replace commas with whitespace, if any
	s.Replace(_T(","), _T(" "));

	s = TrimWs(s);

	while (s.Length() > 0) {
		if (s.Left(10) == _T("translate(")) {
			double fX;
			double fY;

			s = s.Mid(10); // skip "translate("

			fX = TakeMilliNumber(s);
			s = TrimWs(s);
			if (s.Left(1) == _T(")")) {
				fY = 0.0; // Y translation is implicit (equal to zero)
			} else {
				fY = TakeMilliNumber(s);
			}
			s = s.Mid(1); // skip ")"
#if SVGDEBUG
			svgtrace(DBGTRACE_TRANS, "creating translate matrix (%.4f, %.4f)\n", fX, fY);
#endif
			trans *= Transformation::CreateTranslate(fX, fY);
		} else if (s.Left(6) == _T("scale(")) {
			double fX;
			double fY;

			s = s.Mid(6); // skip "scale("

			fX = TakeNumber(s);
			s = TrimWs(s);
			if (s.Left(1) == _T(")")) {
				fY = fX; // Y scale is implicit (equal to X)
			} else {
				fY = TakeNumber(s);
			}
			s = s.Mid(1); // skip ")"
#if SVGDEBUG
			svgtrace(DBGTRACE_TRANS, "creating scale matrix (%.4f, %.4f)\n", fX, fY);
#endif
			trans *= Transformation::CreateScale(fX, fY);
		} else if (s.Left(7) == _T("rotate(")) {
			double fA;
			double fX = 0.0;
			double fY = 0.0;

			s = s.Mid(7); // skip "rotate("

			fA = TakeNumber(s)*M_PI/180.0;
			s = TrimWs(s);
			if (s.Left(1) != _T(")")) {
				// also rotation center is specified
				fX = TakeNumber(s);
				fY = TakeNumber(s);
			}
			s = s.Mid(1); // skip ")"

			if (fX != 0.0 || fY != 0.0) {
#if SVGDEBUG
				svgtrace(DBGTRACE_TRANS, "creating rotate/translate matrix %.4f (%.4f, %.4f)\n", fA, fX, fY);
#endif
				trans *= Transformation::CreateTranslate(fX, fY);
				trans *= Transformation::CreateRotate(fA);
				trans *= Transformation::CreateTranslate(-fX, -fY);
			} else {
#if SVGDEBUG
				svgtrace(DBGTRACE_TRANS, "creating rotate matrix (%.4f)\n", fA);
#endif
				trans *= Transformation::CreateRotate(fA);
			}
		} else if (s.Left(6) == _T("skewX(")) {
			double fA;

			s = s.Mid(6); // skip "skewX("

			fA = TakeNumber(s)*M_PI/180.0;
			s = TrimWs(s);
			s = s.Mid(1); // skip ")"
#if SVGDEBUG
			svgtrace(DBGTRACE_TRANS, "creating skewX matrix (%.4ff)\n", fA);
#endif
			trans *= Transformation::CreateSkewX(fA);
		} else if (s.Left(6) == _T("skewY(")) {
			double fA;

			s = s.Mid(6); // skip "skewY("

			fA = TakeNumber(s)*M_PI/180.0;
			s = TrimWs(s);
			s = s.Mid(1); // skip ")"
#if SVGDEBUG
			svgtrace(DBGTRACE_TRANS, "creating skewY matrix (%.4ff)\n", fA);
#endif
			trans *= Transformation::CreateSkewY(fA);
		} else if (s.Left(7) == _T("matrix(")) {
			double fX[6];

			s = s.Mid(7); // skip "matrix("

			for (int i = 0; i < 6; ++i)
				fX[i] = TakeNumber(s);
			s = TrimWs(s);
			s = s.Mid(1); // skip ")"
#if SVGDEBUG
			svgtrace(DBGTRACE_TRANS, "creating user matrix [%.4f %.4f %.4f %.4f %.4f %.4f]\n",
					 fX[0], fX[1], fX[2], fX[3], fX[4], fX[5]);
#endif
			trans *= Transformation::CreateMatrix(fX);
		} else {
			// skip unexpected character
			s = s.Mid(1);
		}
	}

	return trans;
}

double SVGImporter::MeasureToMillipoints(const Transformation& trans, const wxString& sMeasure) const
{
	wxString s;
	double f;

	s = TrimWs(sMeasure);

	if (IsAbsoluteMeasure(s)) {
		// absolute measure (mm, pt, etc.)
		f = AbsoluteMeasureToMillipoints(s);
	} else {
		// relative measure
		if (s.Right(1) == _T("%")) {
			// relative measure to parent

			// strip "%"
			s = s.RemoveLast(1).Trim();

			s.ToDouble(&f);
			f *= trans.size.x * 0.01;
		} else {
			if (s.Right(2) == _T("px")) {
				// strip units
				s = s.RemoveLast(2).Trim();
			}
			s.ToDouble(&f);
			f *= 1000.0;
		}
	}

	return f;
}

PointD SVGImporter::GetTransformedCoordinate(const Transformation& trans, const wxString& sX, const wxString& sY) const
{
	double fX = MeasureToMillipoints(trans, sX);
	double fY = MeasureToMillipoints(trans, sY);

	PointD p;
#if SVGDEBUG
	svgtrace(DBGTRACE_TRANS, "using trans: ");
	DebugDumpTransformation(DBGTRACE_TRANS, trans);
#endif
	trans.ApplyToCoordinate(fX, fY, &p.x, &p.y);
#if SVGDEBUG
	svgtrace(DBGTRACE_TRANS, "convert (%.2f,%.2f) --> (%.2f,%.2f)\n", fX, fY, p.x, p.y);
#endif

	return p;
}

inline RectD Transform(const Transformation& trans,
					   double fX11, double fY11, double fX12, double fY12,
					   double fX21, double fY21, double fX22, double fY22)
{
	RectD r;
#if SVGDEBUG
	svgtrace(DBGTRACE_TRANS, "using trans: ");
	DebugDumpTransformation(DBGTRACE_TRANS, trans);
#endif
	trans.ApplyToCoordinate(fX11, fY11, &r.p11.x, &r.p11.y);
	trans.ApplyToCoordinate(fX12, fY12, &r.p12.x, &r.p12.y);
	trans.ApplyToCoordinate(fX21, fY21, &r.p21.x, &r.p21.y);
	trans.ApplyToCoordinate(fX22, fY22, &r.p22.x, &r.p22.y);
#if SVGDEBUG
	svgtrace(DBGTRACE_TRANS, "convert (%.2f,%.2f) --> (%.2f,%.2f)\n", fX11, fY11, r.p11.x, r.p11.y);
	svgtrace(DBGTRACE_TRANS, "convert (%.2f,%.2f) --> (%.2f,%.2f)\n", fX12, fY12, r.p12.x, r.p12.y);
	svgtrace(DBGTRACE_TRANS, "convert (%.2f,%.2f) --> (%.2f,%.2f)\n", fX21, fY21, r.p21.x, r.p21.y);
	svgtrace(DBGTRACE_TRANS, "convert (%.2f,%.2f) --> (%.2f,%.2f)\n", fX22, fY22, r.p22.x, r.p22.y);
#endif
	return r;
}

RectD SVGImporter::GetTransformedRectangle(const Transformation& trans, const wxString& sX, const wxString& sY,
										   const wxString& sWidth, const wxString sHeight) const
{
	double fX11    = MeasureToMillipoints(trans, sX);
	double fY11    = MeasureToMillipoints(trans, sY);
	double fWidth  = MeasureToMillipoints(trans, sWidth);
	double fHeight = MeasureToMillipoints(trans, sHeight);
	double fX12    = fX11 + fWidth;
	double fY12    = fY11;
	double fX21    = fX11;
	double fY21    = fY11 + fHeight;
	double fX22    = fX11 + fWidth;
	double fY22    = fY11 + fHeight;

	return Transform(trans, fX11, fY11, fX12, fY12, fX21, fY21, fX22, fY22);
}

RectD SVGImporter::GetTransformedCircle(const Transformation& trans, const wxString& sX, const wxString& sY,
										const wxString& sR) const
{
	double fR      = MeasureToMillipoints(trans, sR);
	double fX11    = MeasureToMillipoints(trans, sX) - fR;
	double fY11    = MeasureToMillipoints(trans, sY) - fR;
	double fX12    = fX11 + fR*2.0;
	double fY12    = fY11;
	double fX21    = fX11;
	double fY21    = fY11 + fR*2.0;
	double fX22    = fX11 + fR*2.0;
	double fY22    = fY11 + fR*2.0;

	return Transform(trans, fX11, fY11, fX12, fY12, fX21, fY21, fX22, fY22);
}

RectD SVGImporter::GetTransformedEllipse(const Transformation& trans, const wxString& sX, const wxString& sY,
										 const wxString& sRX, const wxString& sRY) const
{
	double fRX     = MeasureToMillipoints(trans, sRX);
	double fRY     = MeasureToMillipoints(trans, sRY);
	double fX11    = MeasureToMillipoints(trans, sX) - fRX;
	double fY11    = MeasureToMillipoints(trans, sY) - fRY;
	double fX12    = fX11 + fRX*2.0;
	double fY12    = fY11;
	double fX21    = fX11;
	double fY21    = fY11 + fRY*2.0;
	double fX22    = fX11 + fRX*2.0;
	double fY22    = fY11 + fRY*2.0;

	return Transform(trans, fX11, fY11, fX12, fY12, fX21, fY21, fX22, fY22);
}

double SVGImporter::GetTransformedMeasure(const Transformation& trans, const wxString& sMeasure) const
{
	double f = MeasureToMillipoints(trans, sMeasure);
	double fR;

#if SVGDEBUG
	svgtrace(DBGTRACE_TRANS, "using trans: ");
	DebugDumpTransformation(DBGTRACE_TRANS, trans);
#endif
	trans.ApplyToMeasure(f, &fR);
#if SVGDEBUG
	svgtrace(DBGTRACE_TRANS, "convert %.4f --> %.4f\n", f, fR);
#endif

	return fR;
}

PointD SVGImporter::GetTransformedCoordinate(xmlNodePtr cur, const Transformation& trans,
											 const char* pX, const char* pY) const
{
	wxString sX = GetStringProperty(cur, pX);
	wxString sY = GetStringProperty(cur, pY);
	return GetTransformedCoordinate(trans, sX, sY);
}

RectD SVGImporter::GetTransformedRectangle(xmlNodePtr cur, const Transformation& trans,
										   const char* pX, const char* pY,
										   const char* pW, const char* pH) const
{
	wxString sX      = GetStringProperty(cur, pX);
	wxString sY      = GetStringProperty(cur, pY);
	wxString sWidth  = GetStringProperty(cur, pW);
	wxString sHeight = GetStringProperty(cur, pH);
	return GetTransformedRectangle(trans, sX, sY, sWidth, sHeight);
}

RectD SVGImporter::GetTransformedCircle(xmlNodePtr cur, const Transformation& trans,
										const char* pX, const char* pY, const char* pR) const
{
	wxString sX      = GetStringProperty(cur, pX);
	wxString sY      = GetStringProperty(cur, pY);
	wxString sR      = GetStringProperty(cur, pR);
	return GetTransformedCircle(trans, sX, sY, sR);
}

RectD SVGImporter::GetTransformedEllipse(xmlNodePtr cur, const Transformation& trans,
										 const char* pX, const char* pY,
										 const char* pRX, const char* pRY) const
{
	wxString sX      = GetStringProperty(cur, pX);
	wxString sY      = GetStringProperty(cur, pY);
	wxString sRX     = GetStringProperty(cur, pRX);
	wxString sRY     = GetStringProperty(cur, pRY);
	return GetTransformedEllipse(trans, sX, sY, sRX, sRY);
}

double SVGImporter::GetTransformedMeasure(xmlNodePtr cur, const Transformation& trans, const char* pM) const
{
	wxString s = GetStringProperty(cur, pM);
	return GetTransformedMeasure(trans, s);
}

/*----------------------------------------------------------------------------
 *
 * Styles handling functions
 *
 *----------------------------------------------------------------------------*/

void SVGImporter::PushStyles()
{
	wxASSERT(m_styles.GetCount() > 0);
	Style* pStyle = m_styles.GetLast()->GetData();
	m_styles.Append(new Style(*pStyle));
}

void SVGImporter::PopStyles()
{
	wxASSERT(m_styles.GetCount() > 1);
	wxStyleListNode* pNode = m_styles.GetLast();
	Style* pStyle = pNode->GetData();
	m_styles.DeleteNode(pNode);
	delete pStyle;
}

Style SVGImporter::GetCurrentStyle() const
{
	wxASSERT(m_styles.GetCount() > 0);
	wxASSERT(m_styles.GetLast() != NULL);
	Style* pStyle = m_styles.GetLast()->GetData();
	return *pStyle;
}

Style& SVGImporter::GetCurrentStyle()
{
	wxASSERT(m_styles.GetCount() > 0);
	wxASSERT(m_styles.GetLast() != NULL);
	Style* pStyle = m_styles.GetLast()->GetData();
	return *pStyle;
}

Style SVGImporter::ParseStyle(xmlNodePtr cur, const Transformation& trans, bool bIsObject) const
{
	wxString sStyle = GetStringProperty(cur, "style");
	Style style = GetCurrentStyle(); // import style defaults from parent

	sStyle = TrimWs(sStyle);

	// split the style="first: value1; second: value2" attribute into tokens
	// like "first: value1" and "second: value2"
	wxStringTokenizer tkz(sStyle, _T(";"));
	while (tkz.HasMoreTokens()) {
		wxString sToken = TrimWs(tkz.GetNextToken());

		// split the style into name and value, e.g:
		// "first: value1" --> "first" and "value1"
		wxString sIdent = TrimWs(sToken.Before(_T(':')));
		wxString sValue = TrimWs(sToken.After(_T(':')));

		if (sIdent == _T("fill")) {
			if (sValue.Left(5) == _T("url(#")) {
				// inherit values from <defs /> section
				sValue = sValue.Mid(5);				  // skip "url(#"
				sValue.Truncate(sValue.Length() - 1); // skip last ")"
				ImportStyleFromDefs(style, sValue);
			} else {
				style.SetFillColour(Style::ParseColour(sValue));
			}
		} else if (sIdent == _T("fill-opacity")) {
			double f;
			sValue.ToDouble(&f);
			style.SetFillOpacity(f);
		} else if (sIdent == _T("stroke")) {
			style.SetStrokeColour(Style::ParseColour(sValue));
		} else if (sIdent == _T("stroke-opacity")) {
			double f;
			sValue.ToDouble(&f);
			style.SetStrokeOpacity(f);
		} else if (sIdent == _T("stroke-width")) {
			double fWidth = GetTransformedMeasure(trans, sValue);
			style.SetStrokeWidth((INT32)fWidth);
		} else if (sIdent == _T("opacity")) {
			double f;
			sValue.ToDouble(&f);
			style.SetFillOpacity(f);
			style.SetStrokeOpacity(f);
		} else if (sIdent == _T("stop-color")) {
			style.SetStopColour(Style::ParseColour(sValue));
		} else if (sIdent == _T("stop-opacity")) {
			double f;
			sValue.ToDouble(&f);
			style.SetStopOpacity(f);
		}
	}

	// parse the attributes alone (not in style="" property)
	wxString sValue;

	// id="" and xml:id="" properties
	sValue = GetStringProperty(cur, "id");
	if (sValue.IsEmpty()) {
		sValue = GetStringProperty(cur, "xml:id");
	}
	if (!sValue.IsEmpty()) {
#if SVGDEBUG
		svgtrace(DBGTRACE_STYLES, "got object id: %s\n", (const char *)sValue.mb_str(wxConvUTF8));
#endif
		style.SetXmlId(sValue);
	}

	sValue = GetStringProperty(cur, "xlink:href");
	if (!sValue.IsEmpty()) {
		// inherit values from <defs /> section
		ImportStyleFromDefs(style, sValue);
	}

	sValue = TrimWs(GetStringProperty(cur, "fill"));
	if (!sValue.IsEmpty()) {
		if (sValue.Left(5) == _T("url(#")) {
			// inherit values from <defs /> section
			sValue = sValue.Mid(5);				  // skip "url(#"
			sValue.Truncate(sValue.Length() - 1); // skip last ")"
			ImportStyleFromDefs(style, sValue);
		} else {
			style.SetFillColour(Style::ParseColour(sValue));
		}
	}

	sValue = GetStringProperty(cur, "fill-opacity");
	if (!sValue.IsEmpty()) {
		double f;
		sValue.ToDouble(&f);
		style.SetFillOpacity(f);
	}

	sValue = GetStringProperty(cur, "stroke");
	if (!sValue.IsEmpty())
		style.SetStrokeColour(Style::ParseColour(sValue));

	sValue = GetStringProperty(cur, "stroke-opacity");
	if (!sValue.IsEmpty()) {
		double f;
		sValue.ToDouble(&f);
		style.SetStrokeOpacity(f);
	}

	sValue = GetStringProperty(cur, "opacity");
	if (!sValue.IsEmpty()) {
		double f;
		sValue.ToDouble(&f);
		style.SetFillOpacity(f);
		style.SetStrokeOpacity(f);
	}

	sValue = GetStringProperty(cur, "stroke-width");
	if (!sValue.IsEmpty()) {
		double fWidth = GetTransformedMeasure(trans, sValue);
		style.SetStrokeWidth((INT32)fWidth);
	}

	// for objects, the default stroke width is 1
	if (bIsObject && !style.IsStrokeWidthDefined()) {
		double fWidth = GetTransformedMeasure(trans, _T("1"));
		style.SetStrokeWidth((INT32)fWidth);
	}

	return style;
}

void SVGImporter::ImportStyleFromDefs(Style& style, const wxString& sXmlId) const
{
	typedef pGradientHashTable::const_iterator I;
	I i = m_gradients.find(sXmlId);
	if (i != m_gradients.end()) {
		Gradient* pGradient = i->second;
#if SVGDEBUG
		svgtrace(DBGTRACE_STYLES, "importing style '%s' from defs\n", (const char *)sXmlId.mb_str(wxConvUTF8));
#endif
		style.SetFillGradient(pGradient);
	} else {
#if SVGDEBUG
		svgtrace(DBGTRACE_STYLES, "tried to import non-existing style '%s' from defs\n", (const char *)sXmlId.mb_str(wxConvUTF8));
#endif
	}
}
