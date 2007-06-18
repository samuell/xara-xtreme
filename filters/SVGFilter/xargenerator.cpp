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
// svgimporter.cpp: This implements the XAR producer

#include "svgfilter.h"
#include "xargenerator.h"
#include "utils.h"
#include "version.h"

bool XARGenerator::OutputHeader()
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
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);

	if (m_docSize != DocCoord(0, 0)) {
		ok = Rec.Reinit(TAG_SPREADINFORMATION, TAG_SPREADINFORMATION_SIZE);
		ok = Rec.WriteUINT32(m_docSize.x);		// width
		ok = Rec.WriteUINT32(m_docSize.y);		// height
		ok = Rec.WriteUINT32(10000);			// margin
		ok = Rec.WriteUINT32(0);				// bleed
		ok = Rec.WriteBYTE(2);					// flags (1:shadow)
		ok = m_pExporter->WriteRecord(&Rec);
	}

	ok = Rec.Reinit(TAG_VIEWPORT, TAG_VIEWPORT_SIZE);
	ok = Rec.WriteCoord(DocCoord(0, 0));		// bottom-left
	ok = Rec.WriteCoord(DocCoord(m_docSize.x, m_docSize.y));// top-right
	ok = m_pExporter->WriteRecord(&Rec);

	// Write a layer
	ok = m_pExporter->WriteZeroSizedRecord(TAG_LAYER);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);

	ok = Rec.Reinit(TAG_LAYERDETAILS, TAG_LAYERDETAILS_SIZE);
	ok = Rec.WriteBYTE(1|4|8);					// flags (0:visible, 2:printable, 3:active)
	ok = Rec.WriteUnicode(_T("Layer 1"));
	ok = m_pExporter->WriteRecord(&Rec);

	return ok;
}

bool XARGenerator::OutputFooter()
{
	bool ok = true;

	// End of layer
	ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	// End of spread
	ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	// Write end of file record
	ok = m_pExporter->WriteZeroSizedRecord(TAG_ENDOFFILE);

	return ok;
}

bool XARGenerator::OutputPathEntity(const Style& style, const Transformation& trans, PathDataVector& pathVector)
{
	bool ok = true;
	CXaraFileRecord Rec(0);

	if (style.IsFillColourDefined() || style.IsFillGradientDefined()) {
		if (style.IsStrokeColourDefined())
			Rec.Reinit(TAG_PATH_FILLED_STROKED, TAG_PATH_SIZE);
		else
			Rec.Reinit(TAG_PATH_FILLED, TAG_PATH_SIZE);
	} else // if (style.IsStrokeColourDefined())
		Rec.Reinit(TAG_PATH_STROKED, TAG_PATH_SIZE);

	Rec.WriteUINT32(pathVector.GetCount());
	for (unsigned int i = 0; i < pathVector.GetCount(); ++i)
		Rec.WriteBYTE(pathVector[i].m_verb);
	for (unsigned int i = 0; i < pathVector.GetCount(); ++i) {
		PointD p = pathVector[i].m_coord;
		Rec.WriteCoord(DocCoord((INT32)p.x, m_docSize.y - (INT32)p.y));
	}
	ok = m_pExporter->WriteRecord(&Rec);

	RectD boundings = GetMaximumBoundings(pathVector);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);
	OutputStyles(style, trans, boundings, STYLE_FILL_ALL|STYLE_STROKE_ALL|STYLE_OPACITY);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	return ok;
}

bool XARGenerator::OutputUprightRect(const RectD& r, double fRoundAxis)
{
	// no rotation or skew was applied
	double fX = r.p11.x;
	double fY = r.p11.y;
	double fWidth = r.p12.x - r.p11.x;
	double fHeight = r.p21.y - r.p11.y;

	// adjust coordinates
	fX += fWidth/2.0;
	fY = m_docSize.y - fY - fHeight/2.0;

	// determine circumscribed ellipse axes
	fWidth *= M_SQRT2;
	fHeight *= M_SQRT2;

	// truncate floating point values to INT32
	DocCoord dcPos((INT32)fX, (INT32)fY);
	DocCoord dcSize((INT32)fWidth, (INT32)fHeight);

	bool ok = true;
	CXaraFileRecord Rec(0);
	if (fRoundAxis == 0.0) {
		// non-rounded rectangle
#if SVGDEBUG
		svgtrace(DBGTRACE_SHAPES, "translated upright rect: x=%d y=%d w=%d h=%d\n", dcPos.x, dcPos.y, dcSize.x, dcSize.y);
#endif
		Rec.Reinit(TAG_RECTANGLE_SIMPLE, TAG_RECTANGLE_SIMPLE_SIZE);
		ok = Rec.WriteCoord(dcPos);
		ok = Rec.WriteINT32(dcSize.x);
		ok = Rec.WriteINT32(dcSize.y);
		ok = m_pExporter->WriteRecord(&Rec);
	} else {
		// rounded rectangle
		double fRound = (fRoundAxis*2.0) / dcSize.x;
#if SVGDEBUG
		svgtrace(DBGTRACE_SHAPES, "translated upright rounded rect: x=%d y=%d w=%d h=%d f=%.2f\n", dcPos.x, dcPos.y, dcSize.x, dcSize.y, fRound);
#endif
		Rec.Reinit(TAG_RECTANGLE_SIMPLE_ROUNDED, TAG_RECTANGLE_SIMPLE_ROUNDED_SIZE);
		ok = Rec.WriteCoord(dcPos);
		ok = Rec.WriteINT32(dcSize.x);
		ok = Rec.WriteINT32(dcSize.y);
		ok = Rec.WriteDOUBLE(fRound);
		ok = m_pExporter->WriteRecord(&Rec);
	}

	return ok;
}

bool XARGenerator::OutputComplexRect(const RectD& r, double fRoundAxis)
{
	// complex (non-upright) rectangle
	double fX = (r.p12.x + r.p21.x)*0.5;
	double fY = (r.p12.y + r.p21.y)*0.5;
	double fMaX = (r.p12.x + r.p22.x)*0.5;
	double fMaY = (r.p12.y + r.p22.y)*0.5;
	double fMiX = (r.p12.x + r.p11.x)*0.5;
	double fMiY = (r.p12.y + r.p11.y)*0.5;

	// adjust coordinates
	fY = m_docSize.y - fY;
	fMaY = m_docSize.y - fMaY;
	fMiY = m_docSize.y - fMiY;
	// axes are relative to center
	fMaX -= fX;
	fMaY -= fY;
	fMiX -= fX;
	fMiY -= fY;
	// determine circumscribed ellipse axes
	fMaX *= M_SQRT2;
	fMaY *= M_SQRT2;
	fMiX *= M_SQRT2;
	fMiY *= M_SQRT2;

	// truncate floating point values to INT32
	DocCoord dcPos((INT32)fX, (INT32)fY);
	DocCoord dcMaj((INT32)fMaX, (INT32)fMaY);
	DocCoord dcMin((INT32)fMiX, (INT32)fMiY);

	bool ok = true;
	CXaraFileRecord Rec(0);
	if (fRoundAxis == 0.0) {
		// non-rounded rectangle
#if SVGDEBUG
		svgtrace(DBGTRACE_SHAPES, "translated complex rect: x=%d y=%d M=%d,%d m=%d,%d\n", dcPos.x, dcPos.y, dcMaj.x, dcMaj.y, dcMin.x, dcMin.y);
#endif
		Rec.Reinit(TAG_RECTANGLE_COMPLEX, TAG_RECTANGLE_COMPLEX_SIZE);
		ok = Rec.WriteCoord(dcPos);
		ok = Rec.WriteCoord(dcMaj);
		ok = Rec.WriteCoord(dcMin);
		ok = m_pExporter->WriteRecord(&Rec);
	} else {
		// rounded rectangle
		double fRound = fRoundAxis / dcMaj.x;
#if SVGDEBUG
		svgtrace(DBGTRACE_SHAPES, "translated complex rounded rect: x=%d y=%d M=%d,%d m=%d,%d f=%.2f\n", dcPos.x, dcPos.y, dcMaj.x, dcMaj.y, dcMin.x, dcMin.y, fRound);
#endif
		Rec.Reinit(TAG_RECTANGLE_COMPLEX_ROUNDED, TAG_RECTANGLE_COMPLEX_ROUNDED_SIZE);
		ok = Rec.WriteCoord(dcPos);
		ok = Rec.WriteCoord(dcMaj);
		ok = Rec.WriteCoord(dcMin);
		ok = Rec.WriteDOUBLE(fRound);
		ok = m_pExporter->WriteRecord(&Rec);
	}

	return ok;
}

bool XARGenerator::OutputRectEntity(const Style& style, const Transformation& trans, const RectD& r, double fRoundAxis)
{
	bool ok = true;

#if SVGDEBUG
	svgtrace(DBGTRACE_SHAPES, "rect: x1=%.2f y1=%.2f x2=%.2f y2=%.2f\n", r.p11.x, r.p11.y, r.p22.x, r.p22.y);
#endif

	if (r.IsUpright())
		OutputUprightRect(r, fRoundAxis);
	else
		OutputComplexRect(r, fRoundAxis);

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);
	OutputStyles(style, trans, r, STYLE_FILL_ALL|STYLE_STROKE_ALL|STYLE_OPACITY);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	return ok;
}

bool XARGenerator::OutputUprightEllipse(const RectD& r)
{
	// no rotation or skew was applied
	double fX = r.p11.x;
	double fY = r.p11.y;
	double fWidth = r.p12.x - r.p11.x;
	double fHeight = r.p21.y - r.p11.y;

	// adjust coordinates
	fX += fWidth/2.0;
	fY = m_docSize.y - fY - fHeight/2.0;

	// truncate floating point values to INT32
	DocCoord dcPos((INT32)fX, (INT32)fY);
	DocCoord dcSize((INT32)fWidth, (INT32)fHeight);

	bool ok = true;
	CXaraFileRecord Rec(0);
#if SVGDEBUG
	svgtrace(DBGTRACE_SHAPES, "translated upright ellipse: x=%d y=%d w=%d h=%d\n", dcPos.x, dcPos.y, dcSize.x, dcSize.y);
#endif
 	Rec.Reinit(TAG_ELLIPSE_SIMPLE, TAG_ELLIPSE_SIMPLE_SIZE);
 	ok = Rec.WriteCoord(dcPos);
 	ok = Rec.WriteINT32(dcSize.x);
 	ok = Rec.WriteINT32(dcSize.y);
 	ok = m_pExporter->WriteRecord(&Rec);

	return ok;
}

bool XARGenerator::OutputComplexEllipse(const RectD& r)
{
	// complex (non-upright) ellipse
	double fX = (r.p12.x + r.p21.x)*0.5;
	double fY = (r.p12.y + r.p21.y)*0.5;
	double fMaX = (r.p12.x + r.p22.x)*0.5;
	double fMaY = (r.p12.y + r.p22.y)*0.5;
	double fMiX = (r.p12.x + r.p11.x)*0.5;
	double fMiY = (r.p12.y + r.p11.y)*0.5;

	// adjust coordinates
	fY = m_docSize.y - fY;
	fMaY = m_docSize.y - fMaY;
	fMiY = m_docSize.y - fMiY;
	// axes are relative to center
	fMaX -= fX;
	fMaY -= fY;
	fMiX -= fX;
	fMiY -= fY;

	// truncate floating point values to INT32
	DocCoord dcPos((INT32)fX, (INT32)fY);
	DocCoord dcMaj((INT32)fMaX, (INT32)fMaY);
	DocCoord dcMin((INT32)fMiX, (INT32)fMiY);

	bool ok = true;
	CXaraFileRecord Rec(0);
#if SVGDEBUG
	svgtrace(DBGTRACE_SHAPES, "translated complex ellipse: x=%d y=%d M=%d,%d m=%d,%d\n", dcPos.x, dcPos.y, dcMaj.x, dcMaj.y, dcMin.x, dcMin.y);
#endif
	Rec.Reinit(TAG_ELLIPSE_COMPLEX, TAG_ELLIPSE_COMPLEX_SIZE);
	ok = Rec.WriteCoord(dcPos);
	ok = Rec.WriteCoord(dcMaj);
	ok = Rec.WriteCoord(dcMin);
	ok = m_pExporter->WriteRecord(&Rec);

	return ok;
}

bool XARGenerator::OutputEllipseEntity(const Style& style, const Transformation& trans, const RectD& r)
{
	bool ok = true;

	if (r.IsUpright())
		ok = OutputUprightEllipse(r);
	else
		ok = OutputComplexEllipse(r);

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);
	OutputStyles(style, trans, r, STYLE_FILL_ALL|STYLE_STROKE_ALL|STYLE_OPACITY);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	return ok;
}

bool XARGenerator::OutputLineEntity(const Style& style, const Transformation& trans, const PointD& p1, const PointD& p2)
{
	// truncate floating point values to INT32
	DocCoord dcPos1((INT32)p1.x, m_docSize.y - (INT32)p1.y);
	DocCoord dcPos2((INT32)p2.x, m_docSize.y - (INT32)p2.y);

#if SVGDEBUG
	svgtrace(DBGTRACE_SHAPES, "line: x1=%d y1=%d x2=%d y2=%d\n", dcPos1.x, dcPos1.y, dcPos2.x, dcPos2.y);
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

	RectD boundings = RectD(p1, p2);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);
	OutputStyles(style, trans, boundings, STYLE_STROKE_ALL|STYLE_OPACITY);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	return ok;
}

bool XARGenerator::OutputPolylineEntity(const Style& style, const Transformation& trans, const PointDVector& coordVector)
{
	bool ok = true;
	CXaraFileRecord Rec(0);

	Rec.Reinit(TAG_PATH_STROKED, TAG_PATH_SIZE);
	Rec.WriteUINT32(coordVector.GetCount());
	Rec.WriteBYTE(0x06); // moveto
	for (unsigned int i = 1; i < coordVector.GetCount(); ++i)
		Rec.WriteBYTE(0x02); // lineto
	for (unsigned int i = 0; i < coordVector.GetCount(); ++i) {
		PointD p = coordVector[i];
		Rec.WriteCoord(DocCoord((INT32)p.x, m_docSize.y - (INT32)p.y));
	}
	ok = m_pExporter->WriteRecord(&Rec);

	RectD boundings = GetMaximumBoundings(coordVector);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);
	OutputStyles(style, trans, boundings, STYLE_STROKE_ALL|STYLE_OPACITY);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	return ok;
}

bool XARGenerator::OutputPolygonEntity(const Style& style, const Transformation& trans, const PointDVector& coordVector)
{
	bool ok = true;
	CXaraFileRecord Rec(0);

	if (style.IsFillColourDefined() || style.IsFillGradientDefined()) {
		if (style.IsStrokeColourDefined())
			Rec.Reinit(TAG_PATH_FILLED_STROKED, TAG_PATH_SIZE);
		else
			Rec.Reinit(TAG_PATH_FILLED, TAG_PATH_SIZE);
	} else // if (style.IsStrokeColourDefined())
		Rec.Reinit(TAG_PATH_STROKED, TAG_PATH_SIZE);
	Rec.WriteUINT32(coordVector.GetCount());
	Rec.WriteBYTE(0x06); // moveto
	for (unsigned int i = 1; i < coordVector.GetCount() - 1; ++i)
		Rec.WriteBYTE(0x02); // lineto
	Rec.WriteBYTE(0x03); // lineto + closepath
	for (unsigned int i = 0; i < coordVector.GetCount(); ++i) {
		PointD p = coordVector[i];
		Rec.WriteCoord(DocCoord((INT32)p.x, m_docSize.y - (INT32)p.y));
	}
	ok = m_pExporter->WriteRecord(&Rec);

	RectD boundings = GetMaximumBoundings(coordVector);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);
	OutputStyles(style, trans, boundings, STYLE_FILL_ALL|STYLE_STROKE_ALL|STYLE_OPACITY);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	return ok;
}

bool XARGenerator::EnterGroup()
{
	bool ok = true;
	ok = m_pExporter->WriteZeroSizedRecord(TAG_GROUP);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);
	return ok;
}

bool XARGenerator::LeaveGroup()
{
	bool ok = true;
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);
	return ok;
}

void XARGenerator::DetermineLinearGradientPoints(const Gradient* pGradient, const Transformation& trans,
												 const RectD& boundings, DocCoord& p1, DocCoord& p2)
{
	if (pGradient->units == Gradient::ObjectBoundingBox) {
		PointD pLower = boundings.LowerCoord();
		PointD pHigher = boundings.HigherCoord();
		p1 = DocCoord((INT32)pLower.x, m_docSize.y - (INT32)pLower.y);
		p2 = DocCoord((INT32)pHigher.x, m_docSize.y - (INT32)pHigher.y);
#ifdef SVGDEBUG
		svgtrace(DBGTRACE_GRADIENTS, "using ObjectBoundingBox %d,%d %d,%d\n", p1.x, p1.y, p2.x, p2.y);
#endif
	} else { // Gradient->units == Gradient::UserSpaceOnUse
		double fX1 = pGradient->x1;
		double fY1 = pGradient->y1;
		double fX2 = pGradient->x2;
		double fY2 = pGradient->y2;
		trans.ApplyToCoordinate(fX1, fY1, &fX1, &fY1);
		trans.ApplyToCoordinate(fX2, fY2, &fX2, &fY2);
		p1 = DocCoord((INT32)fX1, m_docSize.y - (INT32)fY1);
		p2 = DocCoord((INT32)fX2, m_docSize.y - (INT32)fY2);
#if SVGDEBUG
		svgtrace(DBGTRACE_GRADIENTS, "using UserSpaceOnUse %d,%d %d,%d\n", p1.x, p1.y, p2.x, p2.y);
#endif
	}
}

void XARGenerator::DetermineRadialGradientPoints(const Gradient* pGradient, const Transformation& trans,
								const RectD& boundings, DocCoord& pC, DocCoord& pMaj, DocCoord& pMin)
{
	if (pGradient->units == Gradient::ObjectBoundingBox) {
		PointD p1m = (boundings.p11+boundings.p12)*0.5;
		PointD p2m = (boundings.p21+boundings.p22)*0.5;
		PointD p1M = (boundings.p11+boundings.p21)*0.5;
		PointD p2M = (boundings.p12+boundings.p22)*0.5;
		double fCX = p1M.x + (p2M.x-p1M.x)*pGradient->cx;
		double fCY = p2m.y + (p1m.y-p2m.y)*pGradient->cy;
		double fXM = fCX + (p2M.x-p1M.x)*pGradient->r;
		double fYM = fCY;
		double fXm = fCX;
		double fYm = fCY + (p1m.y-p2m.y)*pGradient->r;
		pC = DocCoord((INT32)fCX, (INT32)fCY);
		pMaj = DocCoord((INT32)fXM, m_docSize.y - (INT32)fYM);
		pMin = DocCoord((INT32)fXm, m_docSize.y - (INT32)fYm);
#ifdef SVGDEBUG
		svgtrace(DBGTRACE_GRADIENTS, "using ObjectBoundingBox %d,%d %d,%d (%d,%d)\n", pMaj.x, pMaj.y, pMin.x, pMin.y, pC.x, pC.y);
#endif
	} else { // Gradient->units == Gradient::UserSpaceOnUse
		double fX = pGradient->cx;
		double fY = pGradient->cy;
		double fR = pGradient->r;
		double fX1 = fX + fR;
		double fY1 = fY;
		double fX2 = fX;
		double fY2 = fY + fR;
		trans.ApplyToCoordinate(fX, fY, &fX, &fY);
		trans.ApplyToCoordinate(fX1, fY1, &fX1, &fY1);
		trans.ApplyToCoordinate(fX2, fY2, &fX2, &fY2);
		pC = DocCoord((INT32)fX,  m_docSize.y - (INT32)fY);
		pMaj = DocCoord((INT32)fX1, m_docSize.y - (INT32)fY1);
		pMin = DocCoord((INT32)fX2, m_docSize.y - (INT32)fY2);
#if SVGDEBUG
		svgtrace(DBGTRACE_GRADIENTS, "using UserSpaceOnUse %d,%d %d,%d (%d,%d)\n", pMaj.x, pMaj.y, pMin.x, pMin.y, pC.x, pC.y);
#endif
	}
}

bool XARGenerator::OutputFillLinearGradient(const Style& style, const Transformation& trans, const RectD& boundings)
{
	bool ok = true;
	CXaraFileRecord Rec(0);

	const Gradient* pGradient = style.GetFillGradient();
	const GradientStopList& stops = pGradient->stops;

	if (stops.GetCount() < 1) {
#if SVGDEBUG
		svgtrace(DBGTRACE_GRADIENTS, "gradient '%s' has no stop points, giving up\n", (const char *)pGradient->xmlId.mb_str(wxConvUTF8));
#endif
		return false;
	}

#if SVGDEBUG
	svgtrace(DBGTRACE_GRADIENTS, "using linear fill gradient '%s' with %d colours\n", (const char *)pGradient->xmlId.mb_str(wxConvUTF8), stops.GetCount());
	svgtrace(DBGTRACE_GRADIENTS, "gradient trans: ");
	DebugDumpTransformation(DBGTRACE_GRADIENTS, trans);
#endif

    // start and end points of gradient
	DocCoord p1, p2;

	DetermineLinearGradientPoints(pGradient, trans, boundings, p1, p2);

	if (pGradient->units == Gradient::ObjectBoundingBox) {
		// first and last offsets of gradient
		double fOffsetFirst, fOffsetLast;

		if (stops.GetCount() == 1) {
			fOffsetFirst = 0.0;
			fOffsetLast = 1.0;
		} else { // stops.GetCount() > 1
			GradientStop* pStop1 = stops.GetFirst()->GetData();
			GradientStop* pStop2 = stops.GetLast()->GetData();
			fOffsetFirst = pStop1->offset;
			fOffsetLast = pStop2->offset;
		}

#if 0
		// determine vector of direction of object gradient
		double fDX=1.0, fDY=0.0;
		trans.ApplyToCoordinate(fDX, fDY, &fDX, &fDY);
		double fMod = sqrt(fDX*fDX+fDY*fDY);
		fDX /= fMod;
		fDY /= fMod;
#if SVGDEBUG
		svgtrace(DBGTRACE_GRADIENTS, "direction of gradient: %.2f %.2f\n", fDX, fDY);
#endif
#endif

		PointD pLower = boundings.LowerCoord();
		PointD pHigher = boundings.HigherCoord();
		double fWidth = fabs(pHigher.x - pLower.x);
		//double fHeight = fabs(pHigher.x - pLower.x);
		p1.x += (INT32)(fWidth * fOffsetFirst);
		//p1.y += (INT32)(fHeight * fOffsetFirst);
		p2.x -= (INT32)(fWidth * (1.0 - fOffsetLast));
		//p2.y -= (INT32)(fHeight * (1.0 - fOffsetLast));
#ifdef SVGDEBUG
		svgtrace(DBGTRACE_GRADIENTS, "new ObjectBoundingBox %d,%d %d,%d\n", p1.x, p1.y, p2.x, p2.y);
#endif
	}

	if (stops.GetCount() < 3) { // 1 or 2 stops
		wxColour col1, col2;
		double fOpacity1, fOpacity2;
		if (stops.GetCount() == 1) {
			GradientStop* pStop1 = stops.GetFirst()->GetData();
			col2 = col1 = pStop1->stopColour;
			fOpacity1 = fOpacity2 = pStop1->stopOpacity;
		} else { // stops.GetCount() == 2
			GradientStop* pStop1 = stops.GetFirst()->GetData();
			GradientStop* pStop2 = stops.GetLast()->GetData();
			col1 = pStop1->stopColour;
			col2 = pStop2->stopColour;
			fOpacity1 = pStop1->stopOpacity;
			fOpacity2 = pStop2->stopOpacity;
		}

#if SVGDEBUG
		svgtrace(DBGTRACE_GRADIENTS, "stop points: %d, %d and %d, %d\n", p1.x, p1.y, p2.x, p2.y);
#endif

		UINT32 iRecNo1 = DefineColour(col1);
		UINT32 iRecNo2 = DefineColour(col2);

		Rec.Reinit(TAG_LINEARFILL, TAG_LINEARFILL_SIZE);
		ok = Rec.WriteCoord(p1);
		ok = Rec.WriteCoord(p2);
		ok = Rec.WriteReference(iRecNo1);
		ok = Rec.WriteReference(iRecNo2);
		ok = Rec.WriteDOUBLE(0.0); // bias
		ok = Rec.WriteDOUBLE(0.0); // gain
		ok = m_pExporter->WriteRecord(&Rec);

		if (fOpacity1 != 1.0 || fOpacity2 != 1.0) {
			// the fill has also alpha transparency
			BYTE bOpacity1 = (BYTE)((1.0-fOpacity1)*255.0);
			BYTE bOpacity2 = (BYTE)((1.0-fOpacity2)*255.0);

			Rec.Reinit(TAG_LINEARTRANSPARENTFILL, TAG_LINEARTRANSPARENTFILL_SIZE);
			ok = Rec.WriteCoord(p1);
			ok = Rec.WriteCoord(p2);
			ok = Rec.WriteBYTE(bOpacity1);
			ok = Rec.WriteBYTE(bOpacity2);
			ok = Rec.WriteBYTE(0x01); // mix
			ok = Rec.WriteDOUBLE(0.0); // bias
			ok = Rec.WriteDOUBLE(0.0); // gain
			ok = m_pExporter->WriteRecord(&Rec);

		}
	} else { // stops.GetCount() > 2
		// we have more than 2 colours
		size_t size = stops.GetCount();
		double* fvOffsets = new double[size];
		UINT32* ivRecNo = new UINT32[size];
		double* fvOpacity = new double[size];

		bool bHaveTransparency = false;
		for (unsigned int i = 0; i < size; ++i) {
			GradientStop* pStop = stops.Item(i)->GetData();
			ivRecNo[i] = DefineColour(pStop->stopColour);
			fvOffsets[i] = pStop->offset;
			fvOpacity[i] = pStop->stopOpacity;
			if (pStop->stopOpacity < 1.0)
				bHaveTransparency = true;
		}

		Rec.Reinit(TAG_LINEARFILLMULTISTAGE, TAG_LINEARFILLMULTISTAGE_SIZE);
		ok = Rec.WriteCoord(p1);
		ok = Rec.WriteCoord(p2);
		ok = Rec.WriteReference(ivRecNo[0]);
		ok = Rec.WriteReference(ivRecNo[size-1]);
		ok = Rec.WriteINT32((INT32)size-2);
		for (unsigned int i = 1; i < size - 1; ++i) {
			ok = Rec.WriteDOUBLE(fvOffsets[i]);
			ok = Rec.WriteReference(ivRecNo[i]);
		}
		ok = m_pExporter->WriteRecord(&Rec);

		if (bHaveTransparency) {
			if (size == 3) {
				DocCoord pM;
				// calculate middle point position
				pM.x = p1.x + (INT32)((p2.x-p1.x)*fvOffsets[1]);
				pM.y = p1.y + (INT32)((p2.y-p1.y)*fvOffsets[1]);

				// hack: avoid aligned points, otherwise the fill will not work
				if (pM.y == p1.y) {
					pM.y -= 10;
				}

				BYTE bOpacity1 = (BYTE)((1.0-fvOpacity[0])*255.0);
				BYTE bOpacityM = (BYTE)((1.0-fvOpacity[1])*255.0);
				BYTE bOpacity2 = (BYTE)((1.0-fvOpacity[2])*255.0);

				Rec.Reinit(TAG_THREECOLTRANSPARENTFILL, TAG_THREECOLTRANSPARENTFILL_SIZE);
				ok = Rec.WriteCoord(p1);
				ok = Rec.WriteCoord(pM);
				ok = Rec.WriteCoord(p2);
				ok = Rec.WriteBYTE(bOpacity1);
				ok = Rec.WriteBYTE(bOpacityM);
				ok = Rec.WriteBYTE(bOpacity2);
				ok = Rec.WriteBYTE(0x01); // mix
				ok = m_pExporter->WriteRecord(&Rec);

			} else { // size > 3
				// XXX the maximum number of transparent positions in XAR files is 4
				DocCoord pM1, pM2;
				// calculate middle point position
				pM1.x = p1.x + (INT32)((p2.x-p1.x)*fvOffsets[1]); // XXX should get the best offsets
				pM1.y = p1.y + (INT32)((p2.y-p1.y)*fvOffsets[1]); // not simply the first two
				pM2.x = p1.x + (INT32)((p2.x-p1.x)*fvOffsets[2]);
				pM2.y = p1.y + (INT32)((p2.y-p1.y)*fvOffsets[2]);

				// hack: avoid aligned points, otherwise the fill will not work
				if (pM1.y == p1.y) {
					pM1.y -= 10;
				}
				if (pM2.y == p1.y) {
					pM2.y -= 20;
				}

				BYTE bOpacity1  = (BYTE)((1.0-fvOpacity[0])*255.0);
				BYTE bOpacityM1 = (BYTE)((1.0-fvOpacity[1])*255.0);
				BYTE bOpacityM2 = (BYTE)((1.0-fvOpacity[2])*255.0);
				BYTE bOpacity2  = (BYTE)((1.0-fvOpacity[size-1])*255.0);

				Rec.Reinit(TAG_FOURCOLTRANSPARENTFILL, TAG_FOURCOLTRANSPARENTFILL_SIZE);
				ok = Rec.WriteCoord(p1);
				ok = Rec.WriteCoord(pM1);
				ok = Rec.WriteCoord(pM2);
				//ok = Rec.WriteCoord(p2); // XXX this is wrong
				ok = Rec.WriteBYTE(bOpacity1);
				ok = Rec.WriteBYTE(bOpacityM1);
				ok = Rec.WriteBYTE(bOpacityM2);
				ok = Rec.WriteBYTE(bOpacity2);
				ok = Rec.WriteBYTE(0x01); // mix
				ok = m_pExporter->WriteRecord(&Rec);
			}
		}

		delete[] fvOffsets;
		delete[] ivRecNo;
		delete[] fvOpacity;
	}

	return ok;
}

bool XARGenerator::OutputFillRadialGradient(const Style& style, const Transformation& trans, const RectD& boundings)
{
	bool ok = true;
	CXaraFileRecord Rec(0);

	const Gradient* pGradient = style.GetFillGradient();
	const GradientStopList& stops = pGradient->stops;

	if (stops.GetCount() < 1) {
#if SVGDEBUG
		svgtrace(DBGTRACE_GRADIENTS, "gradient '%s' has no stop points, giving up\n", (const char *)pGradient->xmlId.mb_str(wxConvUTF8));
#endif
		return false;
	}

#if SVGDEBUG
	svgtrace(DBGTRACE_GRADIENTS, "using radial fill gradient '%s' with %d colours\n", (const char *)pGradient->xmlId.mb_str(wxConvUTF8), stops.GetCount());
	svgtrace(DBGTRACE_GRADIENTS, "gradient trans: ");
	DebugDumpTransformation(DBGTRACE_GRADIENTS, trans);
#endif

    // center, major and minor axis points of gradient
	DocCoord pC, pMaj, pMin;
	DetermineRadialGradientPoints(pGradient, trans, boundings, pC, pMaj, pMin);

	if (stops.GetCount() < 3) { // 1 or 2 stops
		wxColour col1, col2;
		double fOpacity1, fOpacity2;
		if (stops.GetCount() == 1) {
			GradientStop* pStop1 = stops.GetFirst()->GetData();
			col2 = col1 = pStop1->stopColour;
			fOpacity1 = fOpacity2 = pStop1->stopOpacity;
		} else { // stops.GetCount() == 2
			GradientStop* pStop1 = stops.GetFirst()->GetData();
			GradientStop* pStop2 = stops.GetLast()->GetData();
			col1 = pStop1->stopColour;
			col2 = pStop2->stopColour;
			fOpacity1 = pStop1->stopOpacity;
			fOpacity2 = pStop2->stopOpacity;
		}

#if SVGDEBUG
		svgtrace(DBGTRACE_GRADIENTS, "stop points: %d, %d and %d, %d\n", pMaj.x, pMaj.y, pMin.x, pMin.y);
#endif

		UINT32 iRecNo1 = DefineColour(col1);
		UINT32 iRecNo2 = DefineColour(col2);

		Rec.Reinit(TAG_ELLIPTICALFILL, TAG_ELLIPTICALFILL_SIZE);
		ok = Rec.WriteCoord(pC);
		ok = Rec.WriteCoord(pMaj);
		ok = Rec.WriteCoord(pMin);
		ok = Rec.WriteReference(iRecNo1);
		ok = Rec.WriteReference(iRecNo2);
		ok = Rec.WriteDOUBLE(0.0); // bias
		ok = Rec.WriteDOUBLE(0.0); // gain
		ok = m_pExporter->WriteRecord(&Rec);

		if (fOpacity1 != 1.0 || fOpacity2 != 1.0) {
			// the fill has also alpha transparency
			BYTE bOpacity1 = (BYTE)((1.0-fOpacity1)*255.0);
			BYTE bOpacity2 = (BYTE)((1.0-fOpacity2)*255.0);

			Rec.Reinit(TAG_ELLIPTICALTRANSPARENTFILL, TAG_ELLIPTICALTRANSPARENTFILL_SIZE);
			ok = Rec.WriteCoord(pC);
			ok = Rec.WriteCoord(pMaj);
			ok = Rec.WriteCoord(pMin);
			ok = Rec.WriteBYTE(bOpacity1);
			ok = Rec.WriteBYTE(bOpacity2);
			ok = Rec.WriteBYTE(0x01); // mix
			ok = Rec.WriteDOUBLE(0.0); // bias
			ok = Rec.WriteDOUBLE(0.0); // gain
			ok = m_pExporter->WriteRecord(&Rec);
		}
	} else { // stops.GetCount() > 2
		// XXX ?
	}

	return true;
}

bool XARGenerator::OutputStyles(const Style& style, const Transformation& trans, const RectD& boundings, UINT32 witch)
{
	bool ok = true;
	CXaraFileRecord Rec(0);

	wxString sXmlId = style.GetXmlId();
	if (!sXmlId.IsEmpty()) {
		// XXX how to output object labels in XAR?

#if SVGDEBUG
		svgtrace(DBGTRACE_SHAPES, "object id: %s\n", (const char *)sXmlId.mb_str(wxConvUTF8));
#endif
	}

	// XXX TODO to avoid XAR redundancy, we should look
	// if the styles we are outputting are already the default
	// in Xara's stack

	if (witch & STYLE_FILL_COLOUR) {
		if (style.IsFillColourDefined()) {
			wxColour col = style.GetFillColour();
			if (col.Ok()) {
				UINT32 iRecNo = DefineColour(col);

				Rec.Reinit(TAG_FLATFILL, TAG_FLATFILL_SIZE);
				ok = Rec.WriteReference(iRecNo);
				ok = m_pExporter->WriteRecord(&Rec);

#if SVGDEBUG
				svgtrace(DBGTRACE_STYLES, "fill colour %d,%d,%d\n", col.Red(), col.Green(), col.Blue());
#endif
			} else {
				m_pExporter->WriteZeroSizedRecord(TAG_FLATFILL_NONE);
#if SVGDEBUG
				svgtrace(DBGTRACE_STYLES, "no fill colour\n");
#endif
			}
		} else if (!style.IsFillGradientDefined()) {
			m_pExporter->WriteZeroSizedRecord(TAG_FLATFILL_NONE);
#if SVGDEBUG
			svgtrace(DBGTRACE_STYLES, "no fill colour\n");
#endif
		}
	}

	if (witch & STYLE_FILL_GRADIENT && style.IsFillGradientDefined()) {
		Gradient* pGradient = style.GetFillGradient();

		if (pGradient->type == Gradient::Linear) {
			OutputFillLinearGradient(style, trans, boundings);
		} else if (pGradient->type == Gradient::Radial) {
			OutputFillRadialGradient(style, trans, boundings);
		}
	}

	if (witch & STYLE_FILL_OPACITY && style.IsFillOpacityDefined()) {
		double opacity = style.GetFillOpacity();
		if (opacity < 1.0) {
			BYTE bOpacity = (BYTE)((1.0-opacity)*255.0);

			Rec.Reinit(TAG_FLATTRANSPARENTFILL, TAG_FLATTRANSPARENTFILL_SIZE);
			ok = Rec.WriteBYTE(bOpacity);
			ok = Rec.WriteBYTE(0x01);
			ok = m_pExporter->WriteRecord(&Rec);
		}
	}

	if (witch & STYLE_STROKE_COLOUR) {
		if (style.IsStrokeColourDefined()) {
			wxColour col = style.GetStrokeColour();
			if (col.Ok()) {
				UINT32 iRecNo = DefineColour(col);

				Rec.Reinit(TAG_LINECOLOUR, TAG_LINECOLOUR_SIZE);
				ok = Rec.WriteReference(iRecNo);
				ok = m_pExporter->WriteRecord(&Rec);

#if SVGDEBUG
				svgtrace(DBGTRACE_STYLES, "stroke colour %d,%d,%d\n", col.Red(), col.Green(), col.Blue());
#endif
			} else {
				m_pExporter->WriteZeroSizedRecord(TAG_LINECOLOUR_NONE);
#if SVGDEBUG
				svgtrace(DBGTRACE_STYLES, "no stroke colour\n");
#endif
			}
		} else {
			m_pExporter->WriteZeroSizedRecord(TAG_LINECOLOUR_NONE);
#if SVGDEBUG
			svgtrace(DBGTRACE_STYLES, "no stroke colour\n");
#endif
		}
	}

	if (witch & STYLE_STROKE_WIDTH && style.IsStrokeWidthDefined()) {
		UINT32 iStrokeWidth = style.GetStrokeWidth();
		Rec.Reinit(TAG_LINEWIDTH, TAG_LINEWIDTH_SIZE);
		ok = Rec.WriteINT32(iStrokeWidth);
		ok = m_pExporter->WriteRecord(&Rec);
	}

	if (witch & STYLE_STROKE_LINEJOIN && style.IsStrokeLineJoinDefined()) {
		JointType jt=style.GetStrokeLineJoin();
		Rec.Reinit(TAG_JOINSTYLE, TAG_JOINSTYLE_SIZE);
		ok = Rec.WriteBYTE(BYTE(jt));
		ok = m_pExporter->WriteRecord(&Rec);

#if SVGDEBUG
		switch(jt)
		{
			case MitreJoin:
				svgtrace(DBGTRACE_STYLES, "stroke join mitre\n");
			break;
			case BevelledJoin:
				svgtrace(DBGTRACE_STYLES, "stroke join bevel\n");
			break;
			case RoundJoin:
				svgtrace(DBGTRACE_STYLES, "stroke join round\n");
			break;
		}
#endif

	}

	if (witch & STYLE_STROKE_LINECAP && style.IsStrokeLineCapDefined()) {
		LineCapType lct=style.GetStrokeLineCap();
		Rec.Reinit(TAG_STARTCAP, TAG_STARTCAP_SIZE);
		ok = Rec.WriteBYTE(BYTE(lct));
		ok = m_pExporter->WriteRecord(&Rec);

		Rec.Reinit(TAG_ENDCAP, TAG_ENDCAP_SIZE);
		ok = Rec.WriteBYTE(BYTE(lct));
		ok = m_pExporter->WriteRecord(&Rec);

#if SVGDEBUG
		switch(lct)
		{
			case LineCapButt:
				svgtrace(DBGTRACE_STYLES, "stroke cap butt\n");
			break;
			case LineCapRound:
				svgtrace(DBGTRACE_STYLES, "stroke cap round\n");
			break;
			case LineCapSquare:
				svgtrace(DBGTRACE_STYLES, "stroke cap square\n");
			break;
		}
#endif

	}

	if (witch & STYLE_STROKE_OPACITY && style.IsStrokeOpacityDefined()) {
		double opacity = style.GetStrokeOpacity();
		if (opacity < 1.0) {
			BYTE bOpacity = (BYTE)((1.0-opacity)*255.0);
			Rec.Reinit(TAG_LINETRANSPARENCY, TAG_LINETRANSPARENCY_SIZE);
			ok = Rec.WriteBYTE(bOpacity);
			ok = Rec.WriteBYTE(0x01); // mix
			ok = m_pExporter->WriteRecord(&Rec);
		}
	}

	if (witch & STYLE_OPACITY && style.IsOpacityDefined()) {
		double fOpacity = style.GetOpacity();
		if (fOpacity < 1.0) {
			double fFillOpacity = fOpacity;
			double fStrokeOpacity = fOpacity;

			if (style.IsFillOpacityDefined())
				fFillOpacity *= style.GetFillOpacity();
			if (style.IsStrokeOpacityDefined())
				fStrokeOpacity *= style.GetStrokeOpacity();

			BYTE bFillOpacity   = (BYTE)((1.0-fFillOpacity)*255.0);
			BYTE bStrokeOpacity = (BYTE)((1.0-fStrokeOpacity)*255.0);

			Rec.Reinit(TAG_FLATTRANSPARENTFILL, TAG_FLATTRANSPARENTFILL_SIZE);
			ok = Rec.WriteBYTE(bFillOpacity);
			ok = Rec.WriteBYTE(0x01); // mix
			ok = m_pExporter->WriteRecord(&Rec);
			Rec.Reinit(TAG_LINETRANSPARENCY, TAG_LINETRANSPARENCY_SIZE);
			ok = Rec.WriteBYTE(bStrokeOpacity);
			ok = Rec.WriteBYTE(0x01); // mix
			ok = m_pExporter->WriteRecord(&Rec);
		}
	}

	return ok;
}

bool XARGenerator::OutputOpacity(double fOpacity)
{
	bool ok = true;
	CXaraFileRecord Rec(0);

	if (fOpacity < 1.0) {
		BYTE bOpacity = (BYTE)((1.0-fOpacity)*255.0);

		Rec.Reinit(TAG_FLATTRANSPARENTFILL, TAG_FLATTRANSPARENTFILL_SIZE);
		ok = Rec.WriteBYTE(bOpacity);
		ok = Rec.WriteBYTE(0x01); // mix
		ok = m_pExporter->WriteRecord(&Rec);
	}

	return ok;
}

INT32 XARGenerator::DefineColour(const wxColour& col)
{
	INT32 iRef;

#if SVGDEBUG
	svgtrace(DBGTRACE_STYLES, "looking up colour: #%.2x%.2x%.2x\n", col.Red(), col.Green(), col.Blue());
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
		svgtrace(DBGTRACE_STYLES, "definecolour: #%.2x%.2x%.2x (recid=%d)\n", col.Red(), col.Green(), col.Blue(), iRef);
#endif
	} else {
		// already defined: take from the hash table
		iRef = m_colourRef[col];

#if SVGDEBUG
		svgtrace(DBGTRACE_STYLES, "definecolour: recycle recid=%d\n", iRef);
#endif
	}

	return iRef;
}

RectD XARGenerator::GetMaximumBoundings(const PointDVector& coordVector)
{
	PointD p1(1e50, 1e50); // the maximum values
	PointD p2(-1e50, -1e50);
	for (unsigned int i = 0; i < coordVector.GetCount(); ++i) {
		const PointD& p = coordVector[i];
		if (p.x < p1.x)
			p1.x = p.x;
		if (p.y < p1.y)
			p1.y = p.y;
		if (p.x > p2.x)
			p2.x = p.x;
		if (p.y > p2.y)
			p2.y = p.y;
	}

	return RectD(p1, p2);
}

RectD XARGenerator::GetMaximumBoundings(const PathDataVector& pathVector)
{
	PointD p1(1e50, 1e50); // the maximum values
	PointD p2(-1e50, -1e50);
	for (unsigned int i = 0; i < pathVector.GetCount(); ++i) {
		const PointD& p = pathVector[i].m_coord;
		if (p.x < p1.x)
			p1.x = p.x;
		if (p.y < p1.y)
			p1.y = p.y;
		if (p.x > p2.x)
			p2.x = p.x;
		if (p.y > p2.y)
			p2.y = p.y;
	}

	return RectD(p1, p2);
}
