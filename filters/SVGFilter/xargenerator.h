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
// svgimporter.h: This defines the XAR generator

#ifndef XARGENERATOR_H
#define XARGENERATOR_H

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/hashmap.h"

// XAR handling library
#include "xarlib/xarlib.h"

// for Style, StyleList and ColourRefHashTable
#include "styles.h"
// for GradientStop, Gradient, pGradientHashTable
#include "gradients.h"
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

/***************************************************************************************************

>	class XARGenerator

	Author:		Sandro Sigala <sandro@sigala.it>
	Created:	28 July 2006
	Purpose:    Produces XAR documents.

***************************************************************************************************/

class XARGenerator {
public:
	XARGenerator(CXarExport* pExporter) { m_pExporter = pExporter; }
	~XARGenerator() {}

	void SetDocumentSize(const DocCoord& size) { m_docSize = size; }

	bool OutputHeader();
	bool OutputFooter();
	bool OutputPathEntity(const Style& style, PathDataVector& pathVector);
	bool OutputRectEntity(const Style& style, const RectD& r, double fRoundAxis);
	bool OutputEllipseEntity(const Style& style, const RectD& r);
	bool OutputLineEntity(const Style& style, const PointD& p1, const PointD& p2);
	bool OutputPolylineEntity(const Style& style, const DocCoordVector& coordVector);
	bool OutputPolygonEntity(const Style& style, DocCoordVector& coordVector);
	bool EnterGroup();
	bool LeaveGroup();

private:
	bool OutputUprightRect(const RectD& r, double fRoundAxis, DocRect& boundings);
	bool OutputComplexRect(const RectD& r, double fRoundAxis, DocRect& boundings);
	bool OutputUprightEllipse(const RectD& r, DocRect& boundings);
	bool OutputComplexEllipse(const RectD& r, DocRect& boundings);
	bool OutputStyles(const Style& style, const DocRect& boundings, UINT32 witch);

	INT32 DefineColour(const wxColour& col);

	CXarExport*			m_pExporter;
	DocCoord			m_docSize;
	ColourRefHashTable  m_colourRef;
};

#endif // !XARGENERATOR_H
