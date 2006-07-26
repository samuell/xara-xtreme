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

// define list of transformations
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(TransformationList);

SVGImporter::SVGImporter(CXarExport* pExporter, const wxString& sFileName)
{
	m_pExporter = pExporter;
	m_sFileName = sFileName;
	m_doc = NULL;
	m_root = NULL;

	// Prepare hash table containing CSS colours ("red", "navy", etc.)
	Style::PrepareColourHashTable();

	Transformation *pTrans = new Transformation();
	m_trans.Append(pTrans);

	Style* pStyle = new Style();
	m_styles.Append(pStyle);
}

SVGImporter::~SVGImporter()
{
	// dispose xml document
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
}

bool SVGImporter::Open()
{
	m_doc = xmlParseFile(m_sFileName.mb_str(wxConvUTF8));
	if (m_doc == NULL) {
		// XXX Set an appropriate error here
		fprintf(stderr, "Failed to open input file.\n");
		return false;
	}

	m_root = xmlDocGetRootElement(m_doc);
	if (m_root == NULL) {
		// XXX Set an appropriate error here
		fprintf(stderr, "The document is empty\n");
		xmlFreeDoc(m_doc);
		m_doc = NULL;
		return false;
	}
	if (!IsEntityName(m_root, "svg")) {
		// XXX Set an appropriate error here
		fprintf(stderr, "document root is not <svg>\n");
		xmlFreeDoc(m_doc);
		m_doc = NULL;
		return false;
	}

	// Parse width and height attributes
	wxString sWidth = GetStringProperty(m_root, "width");
	wxString sHeight = GetStringProperty(m_root, "height");
	if (!sWidth.IsEmpty() && !sHeight.IsEmpty()) {
		double fWidth = MeasureToMillipoints(sWidth);
		double fHeight = MeasureToMillipoints(sHeight);
		INT32 iWidth = (INT32)fWidth;
		INT32 iHeight = (INT32)fHeight;

		m_docSize = DocCoord(iWidth, iHeight);
	} else {
		// no size or no valid size specified, let's suppose an A4 paper
		INT32 iWidth = (INT32)(MM2PT(210.0f)*1000.0f);
		INT32 iHeight = (INT32)(MM2PT(297.0f)*1000.0f);

		m_docSize = DocCoord(iWidth, iHeight);
	}

	wxString sViewbox = GetStringProperty(m_root, "viewBox");
	if (!sViewbox.IsEmpty() && TrimWs(sViewbox) != _T("none")) {
		// parse the four space or comma separated values
		wxString sVec[4];
		wxStringTokenizer tkz(sViewbox, _T(" ,"));
		for (int i = 0; i < 4 && tkz.HasMoreTokens(); ++i) {
			wxString token = tkz.GetNextToken();
			sVec[i] = token;
		}

		double fX, fY, fWidth, fHeight;
		sVec[0].ToDouble(&fX);
		sVec[1].ToDouble(&fY);
		sVec[2].ToDouble(&fWidth);
		sVec[3].ToDouble(&fHeight);
		INT32 iX = (INT32)fX;
		INT32 iY = (INT32)fY;
		INT32 iWidth = (INT32)fWidth;
		INT32 iHeight = (INT32)fHeight;

		m_viewBox = DocRect(DocCoord(iX, iY), iWidth, iHeight);
	} else {
		// no viewbox or no valid viewbox specified

		double fWidth = GetDoubleProperty(m_root, "width");
		double fHeight = GetDoubleProperty(m_root, "height");
		if (fWidth > 0.0f && fHeight > 0.0f) {
			// get viewbox geometry from document width and height
			INT32 iWidth = (INT32)fWidth;
			INT32 iHeight = (INT32)fHeight;

			m_viewBox = DocRect(0, 0, iWidth, iHeight);
		} else {
			m_viewBox = DocRect(0, 0, m_docSize.x, m_docSize.y);
		}
	}

#if SVGDEBUG
	fprintf(stderr, "document width: %d, height: %d\n", m_docSize.x, m_docSize.y);
	fprintf(stderr, "viewBox width: %d, height: %d\n", m_viewBox.Width(), m_viewBox.Height());
#endif

	return true;
}

bool SVGImporter::OutputXARHeader()
{
	bool ok = true;

	// Create a CXaraFileRecord object for the document header record
	CXaraFileRecord Rec(TAG_FILEHEADER);
	ok = Rec.Init();
	if (ok) ok = Rec.WriteBuffer((BYTE*)"CXN", 3);
	if (ok) ok = Rec.WriteUINT32(123);				// File size
	if (ok) ok = Rec.WriteUINT32(0);				// Native/Web link ID
	if (ok) ok = Rec.WriteUINT32(0);				// Precompression flags
	if (ok) ok = Rec.WriteASCII(_T("SVGFilter"));	// Producer
	if (ok) ok = Rec.WriteASCII(VERSION_TSTRING);	// Producer version
	if (ok) ok = Rec.WriteASCII(_T(""));			// Producer build
	if (ok) ok = m_pExporter->WriteRecord(&Rec);

	// If any part of the header writing fails then return our own specific error
	if (!ok)
	{
		// XXX Set an appropriate error here
		return false;
	}

	// XXX From this point on the error handling becomes a bit thin

	ok = m_pExporter->WriteZeroSizedRecord(TAG_SPREAD);
	if (m_docSize != DocCoord(0, 0)) {
		ok = Rec.Reinit(TAG_SPREADINFORMATION, TAG_SPREADINFORMATION_SIZE);
		ok = Rec.WriteUINT32(m_docSize.x); // width
		ok = Rec.WriteUINT32(m_docSize.y); // height
		ok = Rec.WriteUINT32(10000);	   // margin
		ok = Rec.WriteUINT32(0);		   // bleed
		ok = Rec.WriteBYTE(2);			   // flags (shadow: on)
		ok = m_pExporter->WriteRecord(&Rec);
	}

	// Write a layer
	ok = m_pExporter->WriteZeroSizedRecord(TAG_LAYER);
	ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);

	ok = Rec.Reinit(TAG_LAYERDETAILS, -1);
	ok = Rec.WriteBYTE(13);
	ok = Rec.WriteUnicode(_T("Layer 1"));
	ok = m_pExporter->WriteRecord(&Rec);

	return true;
}

bool SVGImporter::OutputXARFooter()
{
	bool ok = true;

	// End of the layer
	ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	// Write the end of file record
	CXaraFileRecord Rec(0);
	ok = Rec.Reinit(TAG_ENDOFFILE, 0);
	ok = m_pExporter->WriteZeroSizedRecord(TAG_ENDOFFILE);

	return true;
}

void SVGImporter::GetEntityGeometry(xmlNodePtr cur, size_t size, ...)
{
	va_list ap;
	va_start(ap, size);
	for (unsigned int i = 0; i < size; ++i) {
		const char *pName = va_arg(ap, const char *);
		double *pValue = va_arg(ap, double *);
		wxString sValue = GetStringProperty(cur, pName);
		double f;
		if (IsMeasure(sValue)) {
			f = MeasureToMillipoints(sValue);
		} else {
			f = MeasureToUserUnits(sValue);
			if (m_viewBox.Width() != 0)
				f = f*m_docSize.x/m_viewBox.Width();
		}
		*pValue = f;
	}
	va_end(ap);
}

bool SVGImporter::ParsePathData(const wxString& data, PathDataVector& pathVector)
{
	wxString s = data;
	DocCoord dcLast(0,0);

	// replace commas with whitespace
	s.Replace(_T(","), _T(" "));

	while (!s.Trim(false).IsEmpty()) {
		wxChar c = s[0];
		if (c == _T('M') || c == _T('m')) {
			// moveto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX = TakeNumber(s);
				double fY = TakeNumber(s);
				if (m_viewBox.Width() != 0) {
					fX = fX/m_viewBox.Width()*m_docSize.x;
					fY = fY/m_viewBox.Width()*m_docSize.x;
				}

#if SVGDEBUG
				fprintf(stderr, "path: %c %.2f %.2f\n", action, fX, fY);
#endif

				INT32 iX = (INT32)fX;
				INT32 iY = (INT32)fY;
				if (action == _T('m')) { // relative
					iX += dcLast.x;
					iY += dcLast.y;
				}
				dcLast.x = iX;
				dcLast.y = iY;

				pathVector.Append(PathData(0x06, DocCoord(iX, m_docSize.y - iY))); // moveto
			}
		} else if (c == _T('L') || c == _T('l')) {
			// lineto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX = TakeNumber(s);
				double fY = TakeNumber(s);
				if (m_viewBox.Width() != 0) {
					fX = fX/m_viewBox.Width()*m_docSize.x;
					fY = fY/m_viewBox.Width()*m_docSize.x;
				}

#if SVGDEBUG
				fprintf(stderr, "path: %c %.2f %.2f\n", action, fX, fY);
#endif

				INT32 iX = (INT32)fX;
				INT32 iY = (INT32)fY;
				if (action == _T('l')) { // relative
					iX += dcLast.x;
					iY += dcLast.y;
				}
				dcLast.x = iX;
				dcLast.y = iY;

				pathVector.Append(PathData(0x02, DocCoord(iX, m_docSize.y - iY))); // lineto
			}
		} else if (c == _T('H') || c == _T('h')) {
			// horizontal lineto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX = TakeNumber(s);
				fprintf(stderr, "%c coord: %.2f\n", action, fX);
				if (m_viewBox.Width() != 0) {
					fX = fX/m_viewBox.Width()*m_docSize.x;
				}

#if SVGDEBUG
				fprintf(stderr, "path: %c %.2f\n", action, fX);
#endif

				INT32 iX = (INT32)fX;
				INT32 iY = dcLast.y;
				if (action == _T('h')) {
					iX += dcLast.x;
				}
				dcLast.x = iX;

				pathVector.Append(PathData(0x02, DocCoord(iX, m_docSize.y - iY))); // lineto
			}
		} else if (c == _T('V') || c == _T('v')) {
			// vertical lineto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fY = TakeNumber(s);
				if (m_viewBox.Width() != 0) {
					fY = fY/m_viewBox.Width()*m_docSize.x;
				}

#if SVGDEBUG
				fprintf(stderr, "path: %c %.2f\n", action, fY);
#endif

				INT32 iX = dcLast.x;
				INT32 iY = (INT32)fY;
				if (action == _T('v')) {
					iY += dcLast.y;
				}
				dcLast.y = iY;

				pathVector.Append(PathData(0x02, DocCoord(iX, m_docSize.y - iY))); // lineto
			}
		} else if (c == _T('C') || c == _T('c')) {
			// Bézier cubic curveto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX1 = TakeNumber(s);
				double fY1 = TakeNumber(s);
				double fX2 = TakeNumber(s);
				double fY2 = TakeNumber(s);
				double fX = TakeNumber(s);
				double fY = TakeNumber(s);
				if (m_viewBox.Width() != 0) {
					fX1 = fX1/m_viewBox.Width()*m_docSize.x;
					fY1 = fY1/m_viewBox.Width()*m_docSize.x;
					fX2 = fX2/m_viewBox.Width()*m_docSize.x;
					fY2 = fY2/m_viewBox.Width()*m_docSize.x;
					fX = fX/m_viewBox.Width()*m_docSize.x;
					fY = fY/m_viewBox.Width()*m_docSize.x;
				}

#if SVGDEBUG
				fprintf(stderr, "bezier: %c %.2f %.2f %.2f %.2f %.2f %.2f\n", action, fX1, fY1, fX2, fY2, fX, fY);
#endif

				INT32 iX1 = (INT32)fX1;
				INT32 iY1 = (INT32)fY1;
				INT32 iX2 = (INT32)fX2;
				INT32 iY2 = (INT32)fY2;
				INT32 iX = (INT32)fX;
				INT32 iY = (INT32)fY;
				if (action == _T('c')) {
					iX1 += dcLast.x;
					iY1 += dcLast.y;
					iX2 += dcLast.x;
					iY2 += dcLast.y;
					iX += dcLast.x;
					iY += dcLast.y;
				}
				dcLast.x = iX;
				dcLast.y = iY;

				pathVector.Append(PathData(0x04, DocCoord(iX1, m_docSize.y - iY1))); // bezierto
				pathVector.Append(PathData(0x04, DocCoord(iX2, m_docSize.y - iY2))); // bezierto
				pathVector.Append(PathData(0x04, DocCoord(iX, m_docSize.y - iY))); // bezierto
			}
		} else if (c == _T('S') || c == _T('s')) {
			// Bézier smooth cubic curveto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX = TakeNumber(s);
				double fY = TakeNumber(s);
				if (m_viewBox.Width() != 0) {
					fX = fX/m_viewBox.Width()*m_docSize.x;
					fY = fY/m_viewBox.Width()*m_docSize.x;
				}

#if SVGDEBUG
				fprintf(stderr, "bezier: %c %.2f %.2f\n", action, fX, fY);
#endif

				// XXX ?
			}
		} else if (c == _T('Q') || c == _T('q')) {
			// Bézier quadratic curveto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX1 = TakeNumber(s);
				double fY1 = TakeNumber(s);
				double fX = TakeNumber(s);
				double fY = TakeNumber(s);
				if (m_viewBox.Width() != 0) {
					fX1 = fX/m_viewBox.Width()*m_docSize.x;
					fY1 = fY/m_viewBox.Width()*m_docSize.x;
					fX = fX/m_viewBox.Width()*m_docSize.x;
					fY = fY/m_viewBox.Width()*m_docSize.x;
				}

#if SVGDEBUG
				fprintf(stderr, "bezier: %c %.2f %.2f %.2f %.2f\n", action, fX1, fY1, fX, fY);
#endif

				INT32 iX1 = (INT32)fX1;
				INT32 iY1 = (INT32)fY1;
				INT32 iX = (INT32)fX;
				INT32 iY = (INT32)fY;
				if (action == _T('c')) {
					dcLast.x += iX;
					dcLast.y += iY;
				} else if (action == _T('C')) {
					dcLast.x = iX;
					dcLast.y = iY;
				}

				// XXX ?
			}
		} else if (c == _T('T') || c == _T('t')) {
			// Bézier smooth quadratic curveto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX = TakeNumber(s);
				double fY = TakeNumber(s);
				if (m_viewBox.Width() != 0) {
					fX = fX/m_viewBox.Width()*m_docSize.x;
					fY = fY/m_viewBox.Width()*m_docSize.x;
				}

#if SVGDEBUG
				fprintf(stderr, "bezier: %c %.2f %.2f\n", action, fX, fY);
#endif

				INT32 iX = (INT32)fX;
				INT32 iY = (INT32)fY;
				if (action == _T('c')) {
					dcLast.x += iX;
					dcLast.y += iY;
				} else if (action == _T('C')) {
					dcLast.x = iX;
					dcLast.y = iY;
				}

				// XXX ?
			}
		} else if (c == _T('Z') || c == _T('z')) {
			// closepath
			s = s.Mid(1);

#if SVGDEBUG
			fprintf(stderr, "closepath\n");
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
	double fX, fY, fWidth, fHeight;

	GetEntityGeometry(cur, 4, "x", &fX, "y", &fY, "width", &fWidth, "height", &fHeight);

	// adjust coordinates
	fX += fWidth/2;
	fY = m_docSize.y - fY - fHeight/2;

	// truncate floating point values to INT32
	DocCoord dcPos((INT32)fX, (INT32)fY);
	DocCoord dcSize((INT32)fWidth, (INT32)fHeight);

#if SVGDEBUG
	fprintf(stderr, "path: %d %d %d %d\n", dcPos.x, dcPos.y, dcSize.x, dcSize.y);
#endif

	PathDataVector pathVector;
	wxString sPathData = GetStringProperty(cur, "d");
	ParsePathData(sPathData, pathVector);
	if (pathVector.GetCount() < 2)
		return false;

	bool ok = true;
	CXaraFileRecord Rec(0);

	PushStyles();
	Style& cssStyle = GetCurrentStyle(); // reference to the current style
	cssStyle = ParseStyle(cur, true);

	if (cssStyle.IsFillColourDefined()) {
		if (cssStyle.IsStrokeColourDefined())
			Rec.Reinit(TAG_PATH_FILLED_STROKED, TAG_PATH_SIZE);
		else
			Rec.Reinit(TAG_PATH_FILLED, TAG_PATH_SIZE);
	} else // if (cssStyle.IsStrokeColourDefined())
		Rec.Reinit(TAG_PATH_STROKED, TAG_PATH_SIZE);

	Rec.WriteUINT32(pathVector.GetCount());
	for (unsigned int i = 0; i < pathVector.GetCount(); ++i)
		Rec.WriteBYTE(pathVector[i].m_verb);
	for (unsigned int i = 0; i < pathVector.GetCount(); ++i)
		Rec.WriteCoord(pathVector[i].m_coord);
	ok = m_pExporter->WriteRecord(&Rec);

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);

	OutputStyles(cssStyle, STYLE_FILL_COLOUR|STYLE_FILL_OPACITY|STYLE_STROKE_COLOUR|STYLE_STROKE_WIDTH);
	PopStyles();

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	return true;
}

bool SVGImporter::ParseRectEntity(xmlNodePtr cur)
{
	double fX, fY, fWidth, fHeight;

	GetEntityGeometry(cur, 4, "x", &fX, "y", &fY, "width", &fWidth, "height", &fHeight);
	if (fWidth == 0.0f || fHeight == 0.0f)
		return false;

	// adjust coordinates
	fX += fWidth/2.0f;
	fY = m_docSize.y - fY - fHeight/2.0f;

	// determine circumscribed ellipse axis
	fWidth *= M_SQRT2;
	fHeight *= M_SQRT2;

	// truncate floating point values to INT32
	DocCoord dcPos((INT32)fX, (INT32)fY);
	DocCoord dcSize((INT32)fWidth, (INT32)fHeight);

#if SVGDEBUG
	fprintf(stderr, "rect: x=%d y=%d w=%d h=%d\n", dcPos.x, dcPos.y, dcSize.x, dcSize.y);
#endif

	bool ok = true;
	CXaraFileRecord Rec(0);

	Rec.Reinit(TAG_RECTANGLE_SIMPLE, TAG_RECTANGLE_SIMPLE_SIZE);
	ok = Rec.WriteCoord(dcPos);
	ok = Rec.WriteINT32(dcSize.x);
	ok = Rec.WriteINT32(dcSize.y);
	ok = m_pExporter->WriteRecord(&Rec);

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);

	PushStyles();
	Style& cssStyle = GetCurrentStyle(); // reference to the current style
	cssStyle = ParseStyle(cur, true);
	OutputStyles(cssStyle, STYLE_FILL_COLOUR|STYLE_FILL_OPACITY|STYLE_STROKE_COLOUR|STYLE_STROKE_WIDTH);
	PopStyles();

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	return true;
}

bool SVGImporter::ParseCircleEntity(xmlNodePtr cur)
{
	double fCX, fCY, fR;

	GetEntityGeometry(cur, 3, "cx", &fCX, "cy", &fCY, "r", &fR);
	if (fR == 0.0f)
		return false;

	// adjust coordinates
	fCY = m_docSize.y - fCY;
	fR *= 2.0f;

	// truncate floating point values to INT32
	DocCoord dcPos((INT32)fCX, (INT32)fCY);
	INT32 iR = (INT32)fR;

#if SVGDEBUG
	fprintf(stderr, "circle: cx=%d cy=%d r=%d\n", dcPos.x, dcPos.y, iR);
#endif

	bool ok = true;
	CXaraFileRecord Rec(0);

	Rec.Reinit(TAG_ELLIPSE_SIMPLE, TAG_ELLIPSE_SIMPLE_SIZE);
	ok = Rec.WriteCoord(dcPos);
	ok = Rec.WriteINT32(iR);
	ok = Rec.WriteINT32(iR);
	ok = m_pExporter->WriteRecord(&Rec);

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);

	PushStyles();
	Style& cssStyle = GetCurrentStyle(); // reference to the current style
	cssStyle = ParseStyle(cur, true);
	OutputStyles(cssStyle, STYLE_FILL_COLOUR|STYLE_FILL_OPACITY|STYLE_STROKE_COLOUR|STYLE_STROKE_WIDTH);
	PopStyles();

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);


	return true;
}

bool SVGImporter::ParseEllipseEntity(xmlNodePtr cur)
{
	double fCX, fCY, fRX, fRY;

	GetEntityGeometry(cur, 4, "cx", &fCX, "cy", &fCY, "rx", &fRX, "ry", &fRY);
	if (fRX == 0.0f || fRY == 0.0f)
		return false;

	// adjust coordinates
	fCY = m_docSize.y - fCY;
	fRX *= 2.0f;
	fRY *= 2.0f;

	// truncate floating point values to INT32
	DocCoord dcPos((INT32)fCX, (INT32)fCY);
	DocCoord dcSize((INT32)fRX, (INT32)fRY);

#if SVGDEBUG
	fprintf(stderr, "ellipse: cx=%d cy=%d rx=%d ry=%d\n", dcPos.x, dcPos.y, dcSize.x, dcSize.y);
#endif

	bool ok = true;
	CXaraFileRecord Rec(0);

	Rec.Reinit(TAG_ELLIPSE_SIMPLE, TAG_ELLIPSE_SIMPLE_SIZE);
	ok = Rec.WriteCoord(dcPos);
	ok = Rec.WriteINT32(dcSize.x);
	ok = Rec.WriteINT32(dcSize.y);
	ok = m_pExporter->WriteRecord(&Rec);

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);

	PushStyles();
	Style& cssStyle = GetCurrentStyle(); // reference to the current style
	cssStyle = ParseStyle(cur, true);
	OutputStyles(cssStyle, STYLE_FILL_COLOUR|STYLE_FILL_OPACITY|STYLE_STROKE_COLOUR|STYLE_STROKE_WIDTH);
	PopStyles();

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	return true;
}

bool SVGImporter::ParseLineEntity(xmlNodePtr cur)
{
	double fX1, fY1, fX2, fY2;

	GetEntityGeometry(cur, 4, "x1", &fX1, "y1", &fY1, "x2", &fX2, "y2", &fY2);
	if (fX1 == fX2 && fY1 == fY2)
		return false;

	// adjust coordinates
	fY1 = m_docSize.y - fY1;
	fY2 = m_docSize.y - fY2;

	// truncate floating point values to INT32
	DocCoord dcPos1((INT32)fX1, (INT32)fY1);
	DocCoord dcPos2((INT32)fX2, (INT32)fY2);

#if SVGDEBUG
	fprintf(stderr, "line: x1=%d y1=%d x2=%d y2=%d\n", dcPos1.x, dcPos1.y, dcPos2.x, dcPos2.y);
#endif

	bool ok = true;
	CXaraFileRecord Rec(0);

	Rec.Reinit(TAG_PATH_STROKED, TAG_PATH_SIZE);
	Rec.WriteUINT32(2);	 // two coordinates
	Rec.WriteBYTE(0x06); // moveto
	Rec.WriteBYTE(0x02); // lineno
	Rec.WriteCoord(dcPos1);
	Rec.WriteCoord(dcPos2);
	ok = m_pExporter->WriteRecord(&Rec);

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);

	PushStyles();
	Style& cssStyle = GetCurrentStyle(); // reference to the current style
	cssStyle = ParseStyle(cur, true);
	OutputStyles(cssStyle, STYLE_FILL_COLOUR|STYLE_FILL_OPACITY|STYLE_STROKE_COLOUR|STYLE_STROKE_WIDTH);
	PopStyles();

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	return true;
}

bool SVGImporter::ParsePolylineData(const wxString& data, DocCoordVector& coordVector)
{
	wxString s = data;

	// replace commas with whitespace
	s.Replace(_T(","), _T(" "));

	while (!s.Trim(false).IsEmpty()) {
		if (IsNumberChar(s[0])) {
			while (IsNumberChar(s[0])) {
				double fX = TakeNumber(s);
				double fY = TakeNumber(s);
				if (m_viewBox.Width() != 0) {
					fX = fX/m_viewBox.Width()*m_docSize.x;
					fY = fY/m_viewBox.Width()*m_docSize.x;
				}
				fY = m_docSize.y - fY;

#if SVGDEBUG
				fprintf(stderr, "polyline point: %f %f\n", fX, fY);
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
	double fX, fY, fWidth, fHeight;

	GetEntityGeometry(cur, 4, "x", &fX, "y", &fY, "width", &fWidth, "height", &fHeight);

	// adjust coordinates
	fX += fWidth/2;
	fY = m_docSize.y - fY - fHeight/2;

	// truncate floating point values to INT32
	DocCoord dcPos((INT32)fX, (INT32)fY);
	DocCoord dcSize((INT32)fWidth, (INT32)fHeight);

#if SVGDEBUG
	fprintf(stderr, "polyline: %d %d %d %d\n", dcPos.x, dcPos.y, dcSize.x, dcSize.y);
#endif

	DocCoordVector coordVector;
	wxString sPathData = GetStringProperty(cur, "points");
	ParsePolylineData(sPathData, coordVector);
	if (coordVector.GetCount() < 2)
		return false;

	bool ok = true;
	CXaraFileRecord Rec(0);

	Rec.Reinit(TAG_PATH_STROKED, TAG_PATH_SIZE);
	Rec.WriteUINT32(coordVector.GetCount());
	Rec.WriteBYTE(0x06); // moveto
	for (unsigned int i = 1; i < coordVector.GetCount(); ++i)
		Rec.WriteBYTE(0x02); // lineto
	for (unsigned int i = 0; i < coordVector.GetCount(); ++i)
		Rec.WriteCoord(coordVector[i]);
	ok = m_pExporter->WriteRecord(&Rec);

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);

	PushStyles();
	Style& cssStyle = GetCurrentStyle(); // reference to the current style
	cssStyle = ParseStyle(cur, true);
	OutputStyles(cssStyle, STYLE_FILL_COLOUR|STYLE_FILL_OPACITY|STYLE_STROKE_COLOUR|STYLE_STROKE_WIDTH);
	PopStyles();

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	return true;
}

bool SVGImporter::ParsePolygonEntity(xmlNodePtr cur)
{
	double fX, fY, fWidth, fHeight;

	GetEntityGeometry(cur, 4, "x", &fX, "y", &fY, "width", &fWidth, "height", &fHeight);

	// adjust coordinates
	fX += fWidth/2;
	fY = m_docSize.y - fY - fHeight/2;

	// truncate floating point values to INT32
	DocCoord dcPos((INT32)fX, (INT32)fY);
	DocCoord dcSize((INT32)fWidth, (INT32)fHeight);

#if SVGDEBUG
	fprintf(stderr, "polygon: %d %d %d %d\n", dcPos.x, dcPos.y, dcSize.x, dcSize.y);
#endif

	DocCoordVector coordVector;
	wxString sPathData = GetStringProperty(cur, "points");
	ParsePolylineData(sPathData, coordVector);
	if (coordVector.GetCount() < 3)
		return false;

	bool ok = true;
	CXaraFileRecord Rec(0);

	PushStyles();
	Style& cssStyle = GetCurrentStyle(); // reference to the current style
	cssStyle = ParseStyle(cur, true);

	if (cssStyle.IsFillColourDefined()) {
		if (cssStyle.IsStrokeColourDefined())
			Rec.Reinit(TAG_PATH_FILLED_STROKED, TAG_PATH_SIZE);
		else
			Rec.Reinit(TAG_PATH_FILLED, TAG_PATH_SIZE);
	} else // if (cssStyle.IsStrokeColourDefined())
		Rec.Reinit(TAG_PATH_STROKED, TAG_PATH_SIZE);
	Rec.WriteUINT32(coordVector.GetCount());
	Rec.WriteBYTE(0x06); // moveto
	for (unsigned int i = 1; i < coordVector.GetCount() - 1; ++i)
		Rec.WriteBYTE(0x02); // lineto
	Rec.WriteBYTE(0x03); // lineto + closepath
	for (unsigned int i = 0; i < coordVector.GetCount(); ++i)
		Rec.WriteCoord(coordVector[i]);
	ok = m_pExporter->WriteRecord(&Rec);

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);

	OutputStyles(cssStyle, STYLE_FILL_COLOUR|STYLE_FILL_OPACITY|STYLE_STROKE_COLOUR|STYLE_STROKE_WIDTH);
	PopStyles();

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	return true;
}

bool SVGImporter::ParseGroupEntity(xmlNodePtr cur)
{
	xmlNodePtr child;

	PushStyles();
	Style& cssStyle = GetCurrentStyle(); // reference to the current style
	cssStyle = ParseStyle(cur, false);

	// Points to the first child
	child = cur->xmlChildrenNode;

#if SVGDEBUG
	static int groupid = 0;
	fprintf(stderr, "opengroup: %d\n", ++groupid);
#endif

	bool ok = true;
	ok = m_pExporter->WriteZeroSizedRecord(TAG_GROUP);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);
	ok = ParseEntitiesTree(child);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

#if SVGDEBUG
	fprintf(stderr, "closegroup: %d\n", groupid--);
#endif

	PopStyles();

	return ok;
}

bool SVGImporter::ParseDefinitionsTree()
{
	// XXX parse the <defs> entity for colours, fills etc.
	// and put into a hashtable for later lookup
	return true;
}

bool SVGImporter::ParseEntitiesTree(xmlNodePtr cur)
{
	while (cur != NULL) {
		if (IsEntityName(cur, "path")) {
			ParsePathEntity(cur);
		} else if (IsEntityName(cur, "rect")) {
			ParseRectEntity(cur);
		} else if (IsEntityName(cur, "circle")) {
			ParseCircleEntity(cur);
		} else if (IsEntityName(cur, "ellipse")) {
			ParseEllipseEntity(cur);
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

	// Points to first <svg> child
	cur = m_root->xmlChildrenNode;

	return ParseEntitiesTree(cur);
}

bool SVGImporter::ParseAndOutput()
{
	if (!ParseDefinitionsTree())
		return false;
	if (!OutputXARHeader())
		return false;
	if (!ParseRootTree())
		return false;
	if (!OutputXARFooter())
		return false;
	return true;
}

void SVGImporter::PushTransformations()
{
	wxASSERT(m_trans.GetCount() > 0);
	Transformation* pTrans = m_trans.GetLast()->GetData();
	m_trans.Append(new Transformation(*pTrans));
}

void SVGImporter::PopTransformations()
{
	wxASSERT(m_trans.GetCount() > 1);
	wxTransformationListNode* pNode = m_trans.GetLast();
	Transformation* pTrans = pNode->GetData();
	m_trans.DeleteNode(pNode);
	delete pTrans;
}

Transformation SVGImporter::GetCurrentTransformation() const
{
	wxASSERT(m_trans.GetCount() > 0);
	Transformation* pTrans = m_trans.GetLast()->GetData();
	return *pTrans;
}

Transformation& SVGImporter::GetCurrentTransformation()
{
	wxASSERT(m_trans.GetCount() > 0);
	Transformation* pTrans = m_trans.GetLast()->GetData();
	return *pTrans;
}

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
	Style* pStyle = m_styles.GetLast()->GetData();
	return *pStyle;
}

Style& SVGImporter::GetCurrentStyle()
{
	wxASSERT(m_styles.GetCount() > 0);
	Style* pStyle = m_styles.GetLast()->GetData();
	return *pStyle;
}

Style SVGImporter::ParseStyle(xmlNodePtr cur, bool bIsObject) const
{
	wxString sStyle = GetStringProperty(cur, "style");
	Style cssStyle = GetCurrentStyle();

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
			cssStyle.SetFillColour(Style::ParseColour(sValue));
		} else if (sIdent == _T("fill-opacity")) {
			double f;
			sValue.ToDouble(&f);
			cssStyle.SetFillOpacity(f);
		} else if (sIdent == _T("stroke")) {
			cssStyle.SetStrokeColour(Style::ParseColour(sValue));
		} else if (sIdent == _T("stroke-opacity")) {
			double f;
			sValue.ToDouble(&f);
			cssStyle.SetStrokeOpacity(f);
		} else if (sIdent == _T("stroke-width")) {
			double fWidth;
			sValue.ToDouble(&fWidth);
			if (m_viewBox.Width() != 0)
				fWidth = fWidth/m_viewBox.Width()*m_docSize.x;
			cssStyle.SetStrokeWidth((INT32)fWidth);
		} else if (sIdent == _T("opacity")) {
			double f;
			sValue.ToDouble(&f);
			cssStyle.SetFillOpacity(f);
			cssStyle.SetStrokeOpacity(f);
		}
	}

	// parse the attributes alone (not in style)
	wxString sValue;
	sValue = GetStringProperty(cur, "fill");
	if (!sValue.IsEmpty())
		cssStyle.SetFillColour(Style::ParseColour(sValue));

	sValue = GetStringProperty(cur, "fill-opacity");
	if (!sValue.IsEmpty()) {
		double f;
		sValue.ToDouble(&f);
		cssStyle.SetFillOpacity(f);
	}

	sValue = GetStringProperty(cur, "stroke");
	if (!sValue.IsEmpty())
		cssStyle.SetStrokeColour(Style::ParseColour(sValue));

	sValue = GetStringProperty(cur, "stroke-opacity");
	if (!sValue.IsEmpty()) {
		double f;
		sValue.ToDouble(&f);
		cssStyle.SetStrokeOpacity(f);
	}

	sValue = GetStringProperty(cur, "opacity");
	if (!sValue.IsEmpty()) {
		double f;
		sValue.ToDouble(&f);
		cssStyle.SetFillOpacity(f);
		cssStyle.SetStrokeOpacity(f);
	}

	sValue = GetStringProperty(cur, "stroke-width");
	if (!sValue.IsEmpty()) {
		double fWidth;
		sValue.ToDouble(&fWidth);
		if (m_viewBox.Width() != 0)
			fWidth = fWidth/m_viewBox.Width()*m_docSize.x;
		cssStyle.SetStrokeWidth((INT32)fWidth);
	}

	// for objects, the default stroke width is 1
	if (bIsObject && !cssStyle.IsStrokeWidthDefined()) {
		double fWidth = 1.0f;
		if (m_viewBox.Width() != 0) {
			fWidth = fWidth/m_viewBox.Width()*m_docSize.x;
		}
		INT32 iWidth = (INT32)fWidth;
		cssStyle.SetStrokeWidth(iWidth);
	}

	return cssStyle;
}

bool SVGImporter::OutputStyles(const Style& cssStyle, UINT32 witch)
{
	bool ok = true;
	CXaraFileRecord Rec(0);

	// XXX TODO to save XAR redundancy, we have to look
	// if the styles are the same as the current ones

	if (witch & STYLE_FILL_COLOUR) {
		if (cssStyle.IsFillColourDefined()) {
			wxColour col = cssStyle.GetFillColour();
			if (col.Ok()) {
				UINT32 iRecNo = DefineColour(col);

				Rec.Reinit(TAG_FLATFILL, TAG_FLATFILL_SIZE);
				ok = Rec.WriteReference(iRecNo);
				ok = m_pExporter->WriteRecord(&Rec);

#if SVGDEBUG
				fprintf(stderr, "fill colour %d,%d,%d\n", col.Red(), col.Green(), col.Blue());
#endif
			} else {
				m_pExporter->WriteZeroSizedRecord(TAG_FLATFILL_NONE);
#if SVGDEBUG
				fprintf(stderr, "no fill colour\n");
#endif
			}
		} else {
			m_pExporter->WriteZeroSizedRecord(TAG_FLATFILL_NONE);
#if SVGDEBUG
			fprintf(stderr, "no fill colour\n");
#endif
		}
	}

	if (witch & STYLE_FILL_OPACITY && cssStyle.IsFillOpacityDefined()) {
		if (cssStyle.GetFillOpacity() < 1.0f) {
			BYTE bOpacity = (BYTE)((1.0f-cssStyle.GetFillOpacity())*255.0f);
			Rec.Reinit(TAG_FLATTRANSPARENTFILL, TAG_FLATTRANSPARENTFILL_SIZE);
			ok = Rec.WriteBYTE(bOpacity);
			ok = Rec.WriteBYTE(0x02); // XXX stained glass, witch one is ok for SVG?
			ok = m_pExporter->WriteRecord(&Rec);
		}
	}

	if (witch & STYLE_STROKE_COLOUR) {
		if (cssStyle.IsStrokeColourDefined()) {
			wxColour col = cssStyle.GetStrokeColour();
			if (col.Ok()) {
				UINT32 iRecNo = DefineColour(col);

				Rec.Reinit(TAG_LINECOLOUR, TAG_LINECOLOUR_SIZE);
				ok = Rec.WriteReference(iRecNo);
				ok = m_pExporter->WriteRecord(&Rec);

#if SVGDEBUG
				fprintf(stderr, "stroke colour %d,%d,%d\n", col.Red(), col.Green(), col.Blue());
#endif
			} else {
				m_pExporter->WriteZeroSizedRecord(TAG_LINECOLOUR_NONE);
#if SVGDEBUG
				fprintf(stderr, "no stroke colour\n");
#endif
			}
		} else {
			m_pExporter->WriteZeroSizedRecord(TAG_LINECOLOUR_NONE);
#if SVGDEBUG
			fprintf(stderr, "no stroke colour\n");
#endif
		}
	}

	if (witch & STYLE_STROKE_WIDTH && cssStyle.IsStrokeWidthDefined()) {
		UINT32 iStrokeWidth = cssStyle.GetStrokeWidth();
		Rec.Reinit(TAG_LINEWIDTH, TAG_LINEWIDTH_SIZE);
		ok = Rec.WriteINT32(iStrokeWidth);
		ok = m_pExporter->WriteRecord(&Rec);
	}

	return ok;
}

INT32 SVGImporter::DefineColour(const wxColour& col)
{
	INT32 iRef;

#if SVGDEBUG
	fprintf(stderr, "looking up colour: #%.2x%.2x%.2x\n", col.Red(), col.Green(), col.Blue());
#endif

	if (m_colourRef.find(col) == m_colourRef.end()) {
		// colour not yet defined, put a definition in XAR file
		bool ok = true;
		CXaraFileRecord Rec(0);

		Rec.Reinit(TAG_DEFINERGBCOLOUR, TAG_DEFINERGBCOLOUR_SIZE);
		ok = Rec.WriteBYTE(col.Red());
		ok = Rec.WriteBYTE(col.Green());
		ok = Rec.WriteBYTE(col.Blue());
		UINT32 uiRef;
		ok = m_pExporter->WriteRecord(&Rec, &uiRef);
		iRef = uiRef;

		// add to hash table
		m_colourRef[col] = iRef;

#if SVGDEBUG
		fprintf(stderr, "definecolour: #%.2x%.2x%.2x (recid=%d)\n", col.Red(), col.Green(), col.Blue(), iRef);
#endif
	} else {
		// already defined: take from the hash table
		iRef = m_colourRef[col];

#if SVGDEBUG
		fprintf(stderr, "definecolour: recycle recid=%d\n", iRef);
#endif
	}

	return iRef;
}
