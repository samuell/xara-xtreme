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
// svgimporter.cpp: This implements the SVG --> XAR parser and producer

#include "svgfilter.h"
#include "svgimporter.h"
#include "utils.h"
#include "version.h"

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
		fDocWidth = MM2PT(210.0)*1000.0;
		fDocHeight = MM2PT(297.0)*1000.0;
	}

	trans.size = PointD(fDocWidth, fDocHeight);
	m_docSize = DocCoord((INT32)fDocWidth, (INT32)fDocHeight);

#if SVGDEBUG
	svgtrace(DBGTRACE_TRANS, "document width: %d, height: %d\n", m_docSize.x, m_docSize.y);
	DebugDumpTransformation(DBGTRACE_TRANS, trans);
#endif

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

		double fFactor = (fWidth/fDocWidth) / (fHeight/fDocHeight);
		if (fFactor > 1.0) {
			double space = fHeight*(fFactor - 1.0)/2.0;
#if SVGDEBUG
			svgtrace(DBGTRACE_TRANS, "X > Y in factor, translating Y by %.2f*%.2f=%.2f\n", fHeight, fFactor, space);
#endif
			trans *= Transformation::CreateTranslate(0.0, space);
		}
	}

	return true;
}

/*----------------------------------------------------------------------------
 *
 * XML/SVG parsing functions
 *
 *----------------------------------------------------------------------------*/

// the arc handling code is from XSVG (BSD license)

/*
 * Copyright 2002 USC/Information Sciences Institute
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * Information Sciences Institute not be used in advertising or
 * publicity pertaining to distribution of the software without
 * specific, written prior permission.  Information Sciences Institute
 * makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * INFORMATION SCIENCES INSTITUTE DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL INFORMATION SCIENCES
 * INSTITUTE BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
 * OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

static void PathDefineArcSegment(PathDataVector& pathVector,
								 double fXC, double fYC, double fTh0, double fTh1,
								 double fRX, double fRY, double fXRot)
{
    double sinTh = sin(fXRot);
    double cosTh = cos(fXRot);

    double a00 =  cosTh*fRX;
    double a01 = -sinTh*fRY;
    double a10 =  sinTh*fRX;
    double a11 =  cosTh*fRY;

    double thHalf = 0.5*(fTh1 - fTh0);
    double t = (8.0/3.0)*sin(thHalf*0.5)*sin(thHalf*0.5)/sin(thHalf);
    double x1 = fXC + cos(fTh0) - t*sin(fTh0);
    double y1 = fYC + sin(fTh0) + t*cos(fTh0);
    double x3 = fXC + cos(fTh1);
    double y3 = fYC + sin(fTh1);
    double x2 = x3 + t*sin(fTh1);
    double y2 = y3 - t*cos(fTh1);

	PointD p1(a00*x1 + a01*y1, a10*x1 + a11*y1);
	PointD p2(a00*x2 + a01*y2, a10*x2 + a11*y2);
	PointD p3(a00*x3 + a01*y3, a10*x3 + a11*y3);

	pathVector.Append(PathData(0x04, p1)); // bezierto
	pathVector.Append(PathData(0x04, p2)); // bezierto
	pathVector.Append(PathData(0x04, p3)); // bezierto
}

static void PathDefineArc(PathDataVector& pathVector,
						  double fRX, double fRY, double fXRot, bool bLargeArcFlag, bool bSweepFlag,
						  double fX1, double fY1, double fX2, double fY2)
{
    double sin_th = sin(fXRot);
    double cos_th = cos(fXRot);
    double a00 =  cos_th/fRX;
    double a01 =  sin_th/fRX;
    double a10 = -sin_th/fRY;
    double a11 =  cos_th/fRY;
    double x0  = a00*fX2 + a01*fY2;
    double y0  = a10*fX2 + a11*fY2;
    double x1  = a00*fX1 + a01*fY1;
    double y1  = a10*fX1 + a11*fY1;
    // (x0, y0) is current point in transformed coordinate space.
    // (x1, y1) is new point in transformed coordinate space.

	//  The arc fits a unit-radius circle in this space.
    double d = (x1 - x0)*(x1 - x0) + (y1 - y0)*(y1 - y0);
    double sfactor_sq = 1.0/d - 0.25;
    if (sfactor_sq < 0.0)
		sfactor_sq = 0.0;
    double sfactor = sqrt(sfactor_sq);
    if (bSweepFlag == bLargeArcFlag)
		sfactor = -sfactor;

	double xc = 0.5*(x0 + x1) - sfactor*(y1 - y0);
    double yc = 0.5*(y0 + y1) + sfactor*(x1 - x0);
    // (xc, yc) is center of the circle.

    double th0 = atan2(y0 - yc, x0 - xc);
    double th1 = atan2(y1 - yc, x1 - xc);
    double th_arc = th1 - th0;
    if (th_arc < 0.0 && bSweepFlag)
        th_arc += 2*M_PI;
    else if (th_arc > 0.0 && !bSweepFlag)
        th_arc -= 2*M_PI;

	int n_segs = (int)ceil(fabs(th_arc/(M_PI*0.5 + 0.001)));

	for (int i = 0; i < n_segs; ++i) {
		double fTh0 = th0 + i*th_arc/n_segs;
		double fTh1 = th0 + (i + 1)*th_arc/n_segs;
		PathDefineArcSegment(pathVector, xc, yc, fTh0, fTh1, fRX, fRY, fXRot);
    }
}

bool SVGImporter::ParsePathData(const wxString& data, PathDataVector& pathVector)
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
				if (action == _T('m')) { // relative
					fX += pLast.x;
					fY += pLast.y;
				}
				pLast.x = fX;
				pLast.y = fY;

#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "path: %c %.2f %.2f\n", action, fX, fY);
#endif
				pathVector.Append(PathData(0x06, PointD(fX, fY))); // moveto
			}
		} else if (c == _T('L') || c == _T('l')) {
			// lineto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX = TakeMilliNumber(s);
				double fY = TakeMilliNumber(s);
				if (action == _T('l')) { // relative
					fX += pLast.x;
					fY += pLast.y;
				}
				pLast.x = fX;
				pLast.y = fY;

#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "path: %c %.2f %.2f\n", action, fX, fY);
#endif
				pathVector.Append(PathData(0x02, PointD(fX, fY))); // lineto
			}
		} else if (c == _T('H') || c == _T('h')) {
			// horizontal lineto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX = TakeMilliNumber(s);
				double fY = pLast.y;
				if (action == _T('h')) {
					fX += pLast.x;
				}
				pLast.x = fX;

#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "path: %c %.2f %.2f\n", action, fX, fY);
#endif
				pathVector.Append(PathData(0x02, PointD(fX, fY))); // lineto
			}
		} else if (c == _T('V') || c == _T('v')) {
			// vertical lineto
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX = pLast.x;
				double fY = TakeMilliNumber(s);
				if (action == _T('v')) {
					fY += pLast.y;
				}
				pLast.y = fY;

#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "path: %c %.2f %.2f\n", action, fY, fY);
#endif
				pathVector.Append(PathData(0x02, PointD(fX, fY))); // lineto
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

#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "bezier: %c %.2f %.2f %.2f %.2f %.2f %.2f\n", action, fX1, fY1, fX2, fY2, fX, fY);
#endif
				pathVector.Append(PathData(0x04, PointD(fX1, fY1))); // bezierto
				pathVector.Append(PathData(0x04, PointD(fX2, fY2))); // bezierto
				pathVector.Append(PathData(0x04, PointD(fX,  fY))); // bezierto
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

#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "bezier: %c %.2f %.2f %.2f %.2f %.2f %.2f\n", action, fX1, fY1, fX2, fY2, fX, fY);
#endif
				pathVector.Append(PathData(0x04, PointD(fX1, fY1))); // bezierto
				pathVector.Append(PathData(0x04, PointD(fX2, fY2))); // bezierto
				pathVector.Append(PathData(0x04, PointD(fX,  fY))); // bezierto
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

#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "bezier: %c %.2f %.2f %.2f %.2f\n", action, fX2, fY2, fX, fY);
#endif
				pathVector.Append(PathData(0x04, PointD(fXc1, fYc1))); // bezierto
				pathVector.Append(PathData(0x04, PointD(fXc2, fYc2))); // bezierto
				pathVector.Append(PathData(0x04, PointD(fX,   fY))); // bezierto
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
				if (action == _T('t')) {
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

#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "bezier: %c %.2f %.2f\n", action, fX, fY);
#endif
				pathVector.Append(PathData(0x04, PointD(fXc1, fYc1))); // bezierto
				pathVector.Append(PathData(0x04, PointD(fXc2, fYc2))); // bezierto
				pathVector.Append(PathData(0x04, PointD(fX,   fY))); // bezierto
			}
		} else if (c == _T('A') || c == _T('a')) {
			// elliptic arc
			wxChar action = c;
			s = s.Mid(1);
			while (IsNumberChar(TrimWs(s)[0])) {
				double fX1 = pLast.x;
				double fY1 = pLast.y;
				double fRX = TakeMilliNumber(s);
				double fRY = TakeMilliNumber(s);
				double fXRot = deg2rad(TakeNumber(s));
				bool bLargeArcFlag = (int)TakeNumber(s);
				bool bSweepFlag = (int)TakeNumber(s);
				double fX2 = TakeMilliNumber(s);
				double fY2 = TakeMilliNumber(s);

				if (action == _T('a')) {
					fX2 += pLast.x;
					fY2 += pLast.y;
				}
				pLast.x = fX2;
				pLast.y = fY2;

#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "arc: %c %.2f %.2f %.2f %d %d %.2f %.2f (%.2f %.2f)\n",
						 action, fRX, fRY, fXRot, bLargeArcFlag, bSweepFlag, fX2, fY2, fX1, fY1);
#endif
				PathDefineArc(pathVector, fRX, fRY, fXRot, bLargeArcFlag, bSweepFlag, fX2, fY2, fX1, fY1);
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
				ReportWarning(_("Unexpected 'closepath' in path"));
			}
		} else {
			ReportWarning(wxString::Format(_("Unexpected character '%c' in path"), c));
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
	trans *= ParseTransformations(cur, "transform", true);

#if SVGDEBUG
	svgtrace(DBGTRACE_SHAPES, "path\n");
#endif

	wxString sPathData = GetStringProperty(cur, "d");
	ParsePathData(sPathData, pathVector);
	if (pathVector.GetCount() < 2) { // bogus path
		/*T*/ PopTransformations();
		return false;
	}
	for (unsigned int i = 0; i < pathVector.GetCount(); ++i) {
		PointD& p = pathVector[i].m_coord;
		trans.ApplyToCoordinate(p.x, p.y, &p.x, &p.y);
	}

	/*S*/ PushStyles();
	Style& style = GetCurrentStyle(); // reference to the current style
	style = ParseStyle(cur, trans, true);
	m_pGenerator->OutputPathEntity(style, trans, pathVector);
	/*S*/ PopStyles();

	/*T*/ PopTransformations();

	return true;
}

bool SVGImporter::ParseRectEntity(xmlNodePtr cur)
{
	RectD r;

	/*T*/ PushTransformations();
	Transformation& trans = GetCurrentTransformation();
	trans *= ParseTransformations(cur, "transform", true);

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
	m_pGenerator->OutputRectEntity(style, trans, r, fRoundAxis);
	/*S*/ PopStyles();

	/*T*/ PopTransformations();

	return true;
}

bool SVGImporter::ParseEllipseEntity(xmlNodePtr cur, bool bIsCircle)
{
	RectD r;

	/*T*/ PushTransformations();
	Transformation& trans = GetCurrentTransformation();
	trans *= ParseTransformations(cur, "transform", true);

	if (bIsCircle)
		r = GetTransformedCircle(cur, trans, "cx", "cy", "r");
	else
		r = GetTransformedEllipse(cur, trans, "cx", "cy", "rx", "ry");

	/*S*/ PushStyles();
	Style& style = GetCurrentStyle(); // reference to the current style
	style = ParseStyle(cur, trans, true);
	m_pGenerator->OutputEllipseEntity(style, trans, r);
	/*S*/ PopStyles();

	/*T*/ PopTransformations();

	return true;
}

bool SVGImporter::ParseLineEntity(xmlNodePtr cur)
{
	PointD p1, p2;

	/*T*/ PushTransformations();
	Transformation& trans = GetCurrentTransformation();
	trans *= ParseTransformations(cur, "transform", true);

	p1 = GetTransformedCoordinate(cur, trans, "x1", "y1");
	p2 = GetTransformedCoordinate(cur, trans, "x2", "y2");

	/*S*/ PushStyles();
	Style& style = GetCurrentStyle(); // reference to the current style
	style = ParseStyle(cur, trans, true);
	m_pGenerator->OutputLineEntity(style, trans, p1, p2);
	/*S*/ PopStyles();

	/*T*/ PopTransformations();

	return true;
}

bool SVGImporter::ParsePolylineData(const wxString& data, PointDVector& coordVector)
{
	wxString s = data;

	// replace commas with whitespace
	s.Replace(_T(","), _T(" "));

	while (!s.Trim(false).IsEmpty()) {
		wxChar c = s[0];
		if (IsNumberChar(c)) {
			while (s.Length() > 0 && IsNumberChar(s[0])) {
				double fX = TakeMilliNumber(s);
				double fY = TakeMilliNumber(s);

#if SVGDEBUG
				svgtrace(DBGTRACE_PATHS, "polyline point: %f %f\n", fX, fY);
#endif

				coordVector.Append(PointD(fX, fY));
				s.Trim(false); // trim whitespace on the left
			}
		} else {
			ReportWarning(wxString::Format(_("Unexpected character '%c' in polyline"), c));
			// eat character
			s = s.Mid(1);
		}
	}

	return true;
}

bool SVGImporter::ParsePolylineEntity(xmlNodePtr cur)
{
	PointDVector coordVector;

	/*T*/ PushTransformations();
	Transformation& trans = GetCurrentTransformation();
	trans *= ParseTransformations(cur, "transform", true);

#if SVGDEBUG
	svgtrace(DBGTRACE_SHAPES, "polyline\n");
#endif

	wxString sPathData = GetStringProperty(cur, "points");
	ParsePolylineData(sPathData, coordVector);
	if (coordVector.GetCount() < 2) { // bogus polyline
		/*T*/ PopTransformations();
		return false;
	}
	for (unsigned int i = 0; i < coordVector.GetCount(); ++i) {
		PointD& p = coordVector[i];
		trans.ApplyToCoordinate(p.x, p.y, &p.x, &p.y);
	}

	/*S*/ PushStyles();
	Style& style = GetCurrentStyle(); // reference to the current style
	style = ParseStyle(cur, trans, true);
	m_pGenerator->OutputPolylineEntity(style, trans, coordVector);
	/*S*/ PopStyles();

	/*T*/ PopTransformations();

	return true;
}

bool SVGImporter::ParsePolygonEntity(xmlNodePtr cur)
{
	PointDVector coordVector;

	/*T*/ PushTransformations();
	Transformation& trans = GetCurrentTransformation();
	trans *= ParseTransformations(cur, "transform", true);

#if SVGDEBUG
	svgtrace(DBGTRACE_SHAPES, "polygon\n");
#endif

	wxString sPathData = GetStringProperty(cur, "points");
	ParsePolylineData(sPathData, coordVector);
	if (coordVector.GetCount() < 2) { // bogus polygon
		/*T*/ PopTransformations();
		return false;
	}
	for (unsigned int i = 0; i < coordVector.GetCount(); ++i) {
		PointD& p = coordVector[i];
		trans.ApplyToCoordinate(p.x, p.y, &p.x, &p.y);
	}

	/*S*/ PushStyles();
	Style& style = GetCurrentStyle(); // reference to the current style
	style = ParseStyle(cur, trans, true);
	m_pGenerator->OutputPolygonEntity(style, trans, coordVector);
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
	trans *= ParseTransformations(cur, "transform", true);

	/*S*/ PushStyles();
	Style& style = GetCurrentStyle(); // reference to the current style
	style = ParseStyle(cur, trans, false);

	bool ok = true;
	/***/ ok = m_pGenerator->EnterGroup();

	ok = ParseEntitiesTree(child);

	if (style.IsOpacityDefined()) {
		ok = m_pGenerator->OutputOpacity(style.GetOpacity());
	}

	/***/ ok = m_pGenerator->LeaveGroup();

	/*S*/ PopStyles();

	/*T*/ PopTransformations();

#if SVGDEBUG
	svgtrace(DBGTRACE_GROUPS, "closegroup: %d\n", groupid--);
#endif

	return ok;
}

bool SVGImporter::ParseGradientStopsEntity(xmlNodePtr cur, Gradient* pGradient)
{
	// scan all the elements
	while (cur != NULL) {
		if (IsEntityName(cur, "stop")) {
			Style style = ParseGradientStyle(cur);

			double offset = style.GetStopOffset();
			wxColour stopColour = style.GetStopColour();
			double stopOpacity = style.GetStopOpacity();

#if SVGDEBUG
			svgtrace(DBGTRACE_GRADIENTS, "gradient stop: %.2f (%d,%d,%d)\n", offset, stopColour.Red(), stopColour.Green(), stopColour.Blue());
#endif

			GradientStop* pStop = new GradientStop();
			pStop->offset = offset;
			pStop->stopColour = stopColour;
			pStop->stopOpacity = stopOpacity;
			pGradient->stops.Append(pStop);
		}
		cur = cur->next;
	}

	return true;
}


bool SVGImporter::ParseLinearGradientEntity(xmlNodePtr cur)
{
	Style style = ParseGradientStyle(cur);
	wxString sXmlId = style.GetXmlId();
	if (sXmlId.IsEmpty()) { // no id specified, useless gradient
		return false;
	}

	Gradient* pGradient = new Gradient;

	wxString sValue = GetStringProperty(cur, "href");
	if (!sValue.IsEmpty()) {
		// inherit values from another gradient
#if SVGDEBUG
		svgtrace(DBGTRACE_GRADIENTS, "inherit gradient values from: %s\n", (const char *)sValue.mb_str(wxConvUTF8));
#endif
		if (sValue.Left(1) == _T("#"))
			sValue = sValue.Mid(1);
		Gradient* pGradientCopy = FindGradientInDefs(sValue);
		if (pGradientCopy != NULL) {
			*pGradient = *pGradientCopy;
		} else {
#if SVGDEBUG
			svgtrace(DBGTRACE_GRADIENTS, "inherit from nonexistent %s\n", (const char *)sValue.mb_str(wxConvUTF8));
#endif
		}
	}

	pGradient->type = Gradient::Linear;
	pGradient->xmlId = sXmlId;
	pGradient->x1 = 0.0;
	pGradient->y1 = 0.0;
	pGradient->x2 = 1.0;
	pGradient->y2 = 1.0;

	
	if (IsPropertyDefined(cur, "x1")) {
		pGradient->x1 = GetDoubleProperty(cur, "x1");
	}
	if (IsPropertyDefined(cur, "y1")) {
		pGradient->y1 = GetDoubleProperty(cur, "y1");
	}
	if (IsPropertyDefined(cur, "x2")) {
		pGradient->x2 = GetDoubleProperty(cur, "x2");
	}
	if (IsPropertyDefined(cur, "y2")) {
		pGradient->y2 = GetDoubleProperty(cur, "y2");
	}

	wxString sUnits = GetStringProperty(cur, "gradientUnits");
	if (sUnits == _T("userSpaceOnUse")) {
		pGradient->units = Gradient::UserSpaceOnUse;
		pGradient->x1 *= 1000.0;
		pGradient->y1 *= 1000.0;
		pGradient->x2 *= 1000.0;
		pGradient->y2 *= 1000.0;
	}

	wxString sTrans = GetStringProperty(cur, "gradientTransform");
	if (!sTrans.IsEmpty()) {
		Transformation trans = ParseTransformations(cur, "gradientTransform", false);
		trans.ApplyToCoordinate(pGradient->x1, pGradient->y1, &pGradient->x1, &pGradient->y1);
		trans.ApplyToCoordinate(pGradient->x2, pGradient->y2, &pGradient->x2, &pGradient->y2);
	}

#if SVGDEBUG
	svgtrace(DBGTRACE_GRADIENTS, "linear gradient: '%s' p1=%.2f,%.2f p2=%.2f,%.2f\n",
			 (const char *)sXmlId.mb_str(wxConvUTF8), pGradient->x1, pGradient->y1, pGradient->x2, pGradient->y2);
#endif

	// Points to first <linearGradient /> child
	cur = cur->xmlChildrenNode;

	if (pGradient->stops.GetCount() < 1)
		ParseGradientStopsEntity(cur, pGradient);

	// add to hash table
	m_gradients[sXmlId] = pGradient;

	return true;
}

bool SVGImporter::ParseRadialGradientEntity(xmlNodePtr cur)
{
	Style style = ParseGradientStyle(cur);
	wxString sXmlId = style.GetXmlId();
	if (sXmlId.IsEmpty()) { // no id specified, useless gradient
		return false;
	}

	Gradient* pGradient = new Gradient;

	wxString sValue = GetStringProperty(cur, "href");
	if (!sValue.IsEmpty()) {
		// inherit values from another gradient
#if SVGDEBUG
		svgtrace(DBGTRACE_GRADIENTS, "inherit gradient values from: %s\n", (const char *)sValue.mb_str(wxConvUTF8));
#endif
		if (sValue.Left(1) == _T("#"))
			sValue = sValue.Mid(1);
		Gradient* pGradientCopy = FindGradientInDefs(sValue);
		if (pGradientCopy != NULL) {
			*pGradient = *pGradientCopy;
		} else {
#if SVGDEBUG
			svgtrace(DBGTRACE_GRADIENTS, "nonexistent %s\n", (const char *)sValue.mb_str(wxConvUTF8));
#endif
		}
	}

	pGradient->type = Gradient::Radial;
	pGradient->xmlId = sXmlId;
	pGradient->cx = 0.5;
	pGradient->cy = 0.5;
	pGradient->r  = 0.5;

	if (IsPropertyDefined(cur, "cx")) {
		pGradient->cx = GetDoubleProperty(cur, "cx");
	}
	if (IsPropertyDefined(cur, "cy")) {
		pGradient->cy = GetDoubleProperty(cur, "cy");
	}
	if (IsPropertyDefined(cur, "r")) {
		pGradient->r = GetDoubleProperty(cur, "r");
	}

	wxString sUnits = GetStringProperty(cur, "gradientUnits");
	if (sUnits == _T("userSpaceOnUse")) {
		pGradient->units = Gradient::UserSpaceOnUse;
		pGradient->cx *= 1000.0;
		pGradient->cy *= 1000.0;
		pGradient->r *= 1000.0;
	}

	wxString sTrans = GetStringProperty(cur, "gradientTransform");
	if (!sTrans.IsEmpty()) {
		Transformation trans = ParseTransformations(cur, "gradientTransform", false);
		trans.ApplyToCoordinate(pGradient->cx, pGradient->cy, &pGradient->cx, &pGradient->cy);
		trans.ApplyToMeasure(pGradient->r, &pGradient->r);
	}

#if SVGDEBUG
	svgtrace(DBGTRACE_GRADIENTS, "radial gradient: '%s' c=%.2f,%.2f r=%.2f\n",
			 (const char *)sXmlId.mb_str(wxConvUTF8), pGradient->cx, pGradient->cy, pGradient->r);
#endif

	// Points to first <radialGradient /> child
	cur = cur->xmlChildrenNode;

	if (pGradient->stops.GetCount() < 1)
		ParseGradientStopsEntity(cur, pGradient);

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
		} else if (IsEntityName(cur, "radialGradient")) {
			ParseRadialGradientEntity(cur);
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
			f *= trans.size.x * 0.01; // XXX not correct
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

static void PushStylesIntoNode(xmlNodePtr cur)
{
	wxString sStyle = GetStringProperty(cur, "style");

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

		// append the property to the xml tree
		AddProperty(cur, sIdent, sValue);
	}
}

Style SVGImporter::ParseStyle(xmlNodePtr cur, const Transformation& trans, bool bIsObject) const
{
	Style style = GetCurrentStyle(); // import style defaults from parent

	PushStylesIntoNode(cur);

	// id="" and xml:id="" properties
	wxString sValue = GetStringProperty(cur, "id");
	if (!sValue.IsEmpty()) {
#if SVGDEBUG
		svgtrace(DBGTRACE_STYLES, "got object id: %s\n", (const char *)sValue.mb_str(wxConvUTF8));
#endif
		style.SetXmlId(sValue);
	}

// 	sValue = GetStringProperty(cur, "href");
// 	if (!sValue.IsEmpty()) {
// 		// inherit values from <defs /> section
// 		if (sValue.Left(1) == _T("#"))
// 			sValue = sValue.Mid(1);
// 		ImportStyleFromDefs(style, sValue);
// 	}

	sValue = TrimWs(GetStringProperty(cur, "fill"));
	if (!sValue.IsEmpty()) {
		if (sValue.Left(5) == _T("url(#")) {
			// inherit values from <defs /> section
			sValue = sValue.Mid(5);				  // skip "url(#"
			sValue.Truncate(sValue.Length() - 1); // skip last ")"
			ImportFillStyleFromDefs(style, sValue);
		} else {
			style.SetFillColour(Style::ParseColour(sValue));
		}
	}

	if (IsPropertyDefined(cur, "fill-opacity")) {
		double fOpacity = GetClampedDoubleProperty(cur, "fill-opacity");
		style.SetFillOpacity(fOpacity);
	}

	sValue = GetStringProperty(cur, "stroke");
	if (!sValue.IsEmpty()) {
		if (sValue.Left(5) == _T("url(#")) {
			// inherit values from <defs /> section
			sValue = sValue.Mid(5);				  // skip "url(#"
			sValue.Truncate(sValue.Length() - 1); // skip last ")"
			ImportStrokeStyleFromDefs(style, sValue);
		} else {
			style.SetStrokeColour(Style::ParseColour(sValue));
		}
	}
		
	if (IsPropertyDefined(cur, "stroke-opacity")) {
		double fOpacity = GetClampedDoubleProperty(cur, "stroke-opacity");
		style.SetStrokeOpacity(fOpacity);
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

	sValue = GetStringProperty(cur, "stroke-linejoin");
	if (!sValue.IsEmpty()) {
		if(sValue.CmpNoCase(_T("miter"))==0) {
			style.SetStrokeLineJoin(MitreJoin);
		}
		else if(sValue.CmpNoCase(_T("bevel"))==0) {
			style.SetStrokeLineJoin(BevelledJoin);
		}
		else if(sValue.CmpNoCase(_T("round"))==0) {
			style.SetStrokeLineJoin(RoundJoin);
		}
	}

	if (IsPropertyDefined(cur, "opacity")) {
		double fOpacity = GetClampedDoubleProperty(cur, "opacity");
		style.SetOpacity(fOpacity);
	}

	return style;
}

Style SVGImporter::ParseGradientStyle(xmlNodePtr cur) const
{
	Style style;

	PushStylesIntoNode(cur);
	
	// id="" and xml:id="" properties
	wxString sValue = GetStringProperty(cur, "id");
	if (!sValue.IsEmpty()) {
#if SVGDEBUG
		svgtrace(DBGTRACE_STYLES, "got object id: %s\n", (const char *)sValue.mb_str(wxConvUTF8));
#endif
		style.SetXmlId(sValue);
	}

	if (IsPropertyDefined(cur, "offset")) {
		double offset = GetClampedDoubleProperty(cur, "offset");
		style.SetStopOffset(offset);
	}

	if (IsPropertyDefined(cur, "stop-color")) {
		wxString sStopColour = GetStringProperty(cur, "stop-color");
		wxColour stopColour = Style::ParseColour(sStopColour);
		style.SetStopColour(stopColour);
	}

	if (IsPropertyDefined(cur, "stop-opacity")) {
		double stopOpacity = GetClampedDoubleProperty(cur, "stop-opacity");
		style.SetStopOpacity(stopOpacity);
	}

	return style;
}

void SVGImporter::ImportFillStyleFromDefs(Style& style, const wxString& sXmlId) const
{
	Gradient* pGradient = FindGradientInDefs(sXmlId);
	if (pGradient != NULL) {
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

void SVGImporter::ImportStrokeStyleFromDefs(Style& style, const wxString& sXmlId) const
{
	Gradient* pGradient = FindGradientInDefs(sXmlId);
	if (pGradient != NULL) {
#if SVGDEBUG
		svgtrace(DBGTRACE_STYLES, "importing style '%s' from defs\n", (const char *)sXmlId.mb_str(wxConvUTF8));
#endif
		style.SetStrokeGradient(pGradient);
	} else {
#if SVGDEBUG
		svgtrace(DBGTRACE_STYLES, "tried to import non-existing style '%s' from defs\n", (const char *)sXmlId.mb_str(wxConvUTF8));
#endif
	}
}

Gradient* SVGImporter::FindGradientInDefs(const wxString& sXmlId) const
{
	typedef pGradientHashTable::const_iterator I;
	I i = m_gradients.find(sXmlId);
	if (i != m_gradients.end()) {
		Gradient* pGradient = i->second;
		return pGradient;
	}
	return NULL;
}
