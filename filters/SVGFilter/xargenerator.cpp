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

	return ok;
}

bool XARGenerator::OutputFooter()
{
	bool ok = true;

	// End of the layer
	ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	// Write the end of file record
	CXaraFileRecord Rec(0);
	ok = Rec.Reinit(TAG_ENDOFFILE, 0);
	ok = m_pExporter->WriteZeroSizedRecord(TAG_ENDOFFILE);

	return ok;
}

bool XARGenerator::OutputPathEntity(const Style& style, PathDataVector& pathVector)
{
	bool ok = true;
	CXaraFileRecord Rec(0);

	if (style.IsFillColourDefined()) {
		if (style.IsStrokeColourDefined())
			Rec.Reinit(TAG_PATH_FILLED_STROKED, TAG_PATH_SIZE);
		else
			Rec.Reinit(TAG_PATH_FILLED, TAG_PATH_SIZE);
	} else // if (style.IsStrokeColourDefined())
		Rec.Reinit(TAG_PATH_STROKED, TAG_PATH_SIZE);

	Rec.WriteUINT32(pathVector.GetCount());
	for (unsigned int i = 0; i < pathVector.GetCount(); ++i)
		Rec.WriteBYTE(pathVector[i].m_verb);
	for (unsigned int i = 0; i < pathVector.GetCount(); ++i)
		Rec.WriteCoord(pathVector[i].m_coord);
	ok = m_pExporter->WriteRecord(&Rec);

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);
	OutputStyles(style, DocRect() /* XXX */, STYLE_FILL_ALL|STYLE_STROKE_ALL);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	return ok;
}

bool XARGenerator::OutputUprightRect(const RectD& r, double fRoundAxis, DocRect& boundings)
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
	boundings = DocRect(DocCoord(dcPos.x-dcSize.x, dcPos.y-dcSize.y), dcSize);

	bool ok = true;
	CXaraFileRecord Rec(0);
	if (fRoundAxis == 0.0) {
		// non-rounded rectangle
#if SVGDEBUG
		svgtrace(DBGTRACE_SHAPES, "upright rect: x=%d y=%d w=%d h=%d\n", dcPos.x, dcPos.y, dcSize.x, dcSize.y);
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
		svgtrace(DBGTRACE_SHAPES, "upright rounded rect: x=%d y=%d w=%d h=%d f=%.2f\n", dcPos.x, dcPos.y, dcSize.x, dcSize.y, fRound);
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

bool XARGenerator::OutputComplexRect(const RectD& r, double fRoundAxis, DocRect& boundings)
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
	boundings = DocRect(); // XXX

	bool ok = true;
	CXaraFileRecord Rec(0);
	if (fRoundAxis == 0.0) {
		// non-rounded rectangle
#if SVGDEBUG
		svgtrace(DBGTRACE_SHAPES, "complex rect: x=%d y=%d M=%d,%d m=%d,%d\n", dcPos.x, dcPos.y, dcMaj.x, dcMaj.y, dcMin.x, dcMin.y);
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
		svgtrace(DBGTRACE_SHAPES, "complex rounded rect: x=%d y=%d M=%d,%d m=%d,%d f=%.2f\n", dcPos.x, dcPos.y, dcMaj.x, dcMaj.y, dcMin.x, dcMin.y, fRound);
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

bool XARGenerator::OutputRectEntity(const Style& style, const RectD& r, double fRoundAxis)
{
	bool ok = true;

	DocRect boundings;
	if (r.IsUpright())
		OutputUprightRect(r, fRoundAxis, boundings);
	else
		OutputComplexRect(r, fRoundAxis, boundings);

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);
	OutputStyles(style, boundings, STYLE_FILL_ALL|STYLE_STROKE_ALL);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	return ok;
}

bool XARGenerator::OutputUprightEllipse(const RectD& r, DocRect& boundings)
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
	boundings = DocRect(DocCoord(dcPos.x-dcSize.x, dcPos.y-dcSize.y), dcSize);

	bool ok = true;
	CXaraFileRecord Rec(0);
#if SVGDEBUG
	svgtrace(DBGTRACE_SHAPES, "upright ellipse: x=%d y=%d w=%d h=%d\n", dcPos.x, dcPos.y, dcSize.x, dcSize.y);
#endif
 	Rec.Reinit(TAG_ELLIPSE_SIMPLE, TAG_ELLIPSE_SIMPLE_SIZE);
 	ok = Rec.WriteCoord(dcPos);
 	ok = Rec.WriteINT32(dcSize.x);
 	ok = Rec.WriteINT32(dcSize.y);
 	ok = m_pExporter->WriteRecord(&Rec);

	return ok;
}

bool XARGenerator::OutputComplexEllipse(const RectD& r, DocRect& boundings)
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
	boundings = DocRect(); // XXX

	bool ok = true;
	CXaraFileRecord Rec(0);
#if SVGDEBUG
	svgtrace(DBGTRACE_SHAPES, "complex ellipse: x=%d y=%d M=%d,%d m=%d,%d\n", dcPos.x, dcPos.y, dcMaj.x, dcMaj.y, dcMin.x, dcMin.y);
#endif
	Rec.Reinit(TAG_ELLIPSE_COMPLEX, TAG_ELLIPSE_COMPLEX_SIZE);
	ok = Rec.WriteCoord(dcPos);
	ok = Rec.WriteCoord(dcMaj);
	ok = Rec.WriteCoord(dcMin);
	ok = m_pExporter->WriteRecord(&Rec);

	return ok;
}

bool XARGenerator::OutputEllipseEntity(const Style& style, const RectD& r)
{
	bool ok = true;

	DocRect boundings;
	if (r.IsUpright())
		ok = OutputUprightEllipse(r, boundings);
	else
		ok = OutputComplexEllipse(r, boundings);

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);
	OutputStyles(style, boundings, STYLE_FILL_ALL|STYLE_STROKE_ALL);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	return ok;
}

bool XARGenerator::OutputLineEntity(const Style& style, const PointD& p1, const PointD& p2)
{
	// truncate floating point values to INT32
	DocCoord dcPos1((INT32)p1.x, m_docSize.y - (INT32)p1.y);
	DocCoord dcPos2((INT32)p2.x, m_docSize.y - (INT32)p2.y);
	DocRect boundings = DocRect(); // XXX
	
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

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);
	OutputStyles(style, boundings, STYLE_STROKE_ALL);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	return ok;
}

bool XARGenerator::OutputPolylineEntity(const Style& style, const DocCoordVector& coordVector)
{
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
	OutputStyles(style, DocRect() /* XXX */, STYLE_STROKE_ALL);
	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_UP);

	return ok;
}

bool XARGenerator::OutputPolygonEntity(const Style& style, DocCoordVector& coordVector)
{
	bool ok = true;
	CXaraFileRecord Rec(0);

	if (style.IsFillColourDefined()) {
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
	for (unsigned int i = 0; i < coordVector.GetCount(); ++i)
		Rec.WriteCoord(coordVector[i]);
	ok = m_pExporter->WriteRecord(&Rec);

	/***/ ok = m_pExporter->WriteZeroSizedRecord(TAG_DOWN);
	OutputStyles(style, DocRect() /* XXX */, STYLE_FILL_ALL|STYLE_STROKE_ALL);
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

bool XARGenerator::OutputStyles(const Style& style, const DocRect& boundings, UINT32 witch)
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

	// XXX TODO to save XAR redundancy, we should look
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
		} else {
			m_pExporter->WriteZeroSizedRecord(TAG_FLATFILL_NONE);
#if SVGDEBUG
			svgtrace(DBGTRACE_STYLES, "no fill colour\n");
#endif
		}
	}

	if (witch & STYLE_FILL_GRADIENT && style.IsFillGradientDefined()) {
		Gradient* pGradient = style.GetFillGradient();
		GradientStopList& stops = pGradient->stops;
		
#if SVGDEBUG
		svgtrace(DBGTRACE_STYLES, "using fill gradient '%s'\n", (const char *)pGradient->xmlId.mb_str(wxConvUTF8));
#endif

		if (pGradient->type == Gradient::Linear) {
			if (stops.GetCount() == 1 || stops.GetCount() == 2) {
				wxColour col1, col2;
				double fOffset1, fOffset2;
				if (stops.GetCount() == 1) {
					GradientStop* pStop1 = stops.GetFirst()->GetData();
					col2 = col1 = pStop1->stopColour;
					fOffset1 = 0.0;
					fOffset2 = 0.0;
				} else { // stops.GetCount() == 2
					GradientStop* pStop1 = stops.GetFirst()->GetData();
					GradientStop* pStop2 = stops.Item(1)->GetData();
					col1 = pStop1->stopColour;
					col2 = pStop2->stopColour;
					fOffset1 = pStop1->offset;
					fOffset2 = pStop2->offset;
				}

				DocCoord p1 = boundings.LowCorner();
				DocCoord p2 = p1;
				double fWidth = boundings.Width();
				p1.x += (INT32)(fWidth * fOffset1);
				p2.x += (INT32)(fWidth * fOffset2);

				UINT32 iRecNo1 = DefineColour(col1);
				UINT32 iRecNo2 = DefineColour(col2);

				Rec.Reinit(TAG_LINEARFILL, 24); // XXX what is fill profile?
				ok = Rec.WriteCoord(p1);
				ok = Rec.WriteCoord(p2);
				ok = Rec.WriteReference(iRecNo1);
				ok = Rec.WriteReference(iRecNo2);
				// XXX write fill profile
				ok = m_pExporter->WriteRecord(&Rec);
			} else if (stops.GetCount() > 2) {
				// we have more than 2 colours
				size_t size = stops.GetCount();
				double* fvOffsets = new double[size];
				UINT32* ivRecNo = new UINT32[size];

				DocCoord p1 = boundings.LowCorner();
				DocCoord p2 = p1;
				p2.x += boundings.Width();

				for (unsigned int i = 0; i < size; ++i) {
					GradientStop* pStop = stops.Item(i)->GetData();
					ivRecNo[i] = DefineColour(pStop->stopColour);
					fvOffsets[i] = pStop->offset;
					//fprintf(stderr, "%d ==== %.2f %d\n", i, ivRecNo[i], fvOffsets[i]);
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

				delete[] fvOffsets;
				delete[] ivRecNo;
			}
		} else if (pGradient->type == Gradient::Radial) {
			// XXX
		}
	}
	
	if (witch & STYLE_FILL_OPACITY && style.IsFillOpacityDefined()) {
		if (style.GetFillOpacity() < 1.0f) {
			BYTE bOpacity = (BYTE)((1.0f-style.GetFillOpacity())*255.0f);
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
