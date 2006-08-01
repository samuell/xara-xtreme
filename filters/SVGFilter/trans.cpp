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
// trans.cpp: This implements the class for handling transformations

#include "trans.h"

// define list of Transformation elements
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(TransformationList);

#if SVGDEBUG
void DebugDumpTransformation(int lvl, const Transformation& trans);
#endif

Transformation& Transformation::operator *=(const Transformation& t)
{
	const double a11 = data[0];
	const double a12 = data[1];
	const double a13 = data[2];
	const double a21 = data[3];
	const double a22 = data[4];
	const double a23 = data[5];
	const double b11 = t.data[0];
	const double b12 = t.data[1];
	const double b13 = t.data[2];
	const double b21 = t.data[3];
	const double b22 = t.data[4];
	const double b23 = t.data[5];

#if SVGDEBUG
	svgtrace(DBGTRACE_TRANS, "original trans: ");
	DebugDumpTransformation(DBGTRACE_TRANS, *this);
	svgtrace(DBGTRACE_TRANS, "applying trans: ");
	DebugDumpTransformation(DBGTRACE_TRANS, t);
#endif

	data[0] = a11*b11 + a12*b21;
	data[1] = a11*b12 + a12*b22;
	data[2] = a11*b13 + a12*b23 + a13;
	data[3] = a21*b11 + a22*b21;
	data[4] = a21*b12 + a22*b22;
	data[5] = a21*b13 + a22*b23 + a23;

#if SVGDEBUG
	svgtrace(DBGTRACE_TRANS, "result: ");
	DebugDumpTransformation(DBGTRACE_TRANS, *this);
#endif

	return *this;
}

void Transformation::ApplyToCoordinate(double x, double y, double* pRX, double* pRY) const
{
	const double a11 = data[0];
	const double a12 = data[1];
	const double a13 = data[2];
	const double a21 = data[3];
	const double a22 = data[4];
	const double a23 = data[5];
	if (pRX) *pRX = a11*x + a12*y + a13;
	if (pRY) *pRY = a21*x + a22*y + a23;
}

void Transformation::ApplyToMeasure(double x, double* pRX) const
{
	const double a11 = data[0];
//	const double a12 = data[1];
//	const double a13 = data[2];
//	const double a21 = data[3];
//	const double a22 = data[4];
//	const double a23 = data[5];
	if (pRX) *pRX = a11*x; // + a12*y;
}

Transformation Transformation::CreateTranslate(double x, double y)
{
	Transformation trans;
	trans.data[2] = x;
	trans.data[5] = y;
	return trans;
}

Transformation Transformation::CreateScale(double x, double y)
{
	Transformation trans;
	trans.data[0] = x;
	trans.data[4] = y;
	return trans;
}

Transformation Transformation::CreateRotate(double a)
{
	Transformation trans;
	double sinA = sin(a);
	double cosA = cos(a);
	trans.data[0] = cosA;
	trans.data[1] = -sinA;
	trans.data[3] = sinA;
	trans.data[4] = cosA;
	return trans;
}

Transformation Transformation::CreateSkewX(double a)
{
	Transformation trans;
	trans.data[1] = tan(a);
	return trans;
}

Transformation Transformation::CreateSkewY(double a)
{
	Transformation trans;
	trans.data[3] = tan(a);
	return trans;
}

Transformation Transformation::CreateMatrix(double d[6])
{
	Transformation trans;
	trans.data[0] = d[0];
	trans.data[1] = d[2];
	trans.data[2] = d[4];
	trans.data[3] = d[1];
	trans.data[4] = d[3];
	trans.data[5] = d[5];
	return trans;
}

#if SVGDEBUG
void DebugDumpTransformation(int lvl, const Transformation& trans)
{
	svgtrace(lvl, "trans data[0..5] = [ ");
	for (int i = 0; i < 6; ++i)
		svgtrace(lvl, "%.4f ", trans.data[i]);
	svgtrace(lvl, "], ");
	svgtrace(lvl, "size = [%.2f %.2f]\n", trans.size.x, trans.size.y);
}
#endif
