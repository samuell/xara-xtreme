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
// trans.h: This defines the class for handling transformations

#ifndef TRANS_H
#define TRANS_H

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

// XAR handling library
#include "xarlib/xarlib.h"

// for PointD
#include "utils.h"

/***************************************************************************************************

>	struct Transformation

	Author:		Sandro Sigala <sandro@sigala.it>
	Created:	20 July 2006
	Purpose:	Contains transformation matrix and applies SVG transformations to coordinates.

***************************************************************************************************/

struct Transformation {
	Transformation() {
		// default is an identity matrix
		data[0] = 1.0; // x scale
		data[1] = 0.0;
		data[2] = 0.0;
		data[3] = 0.0;
		data[4] = 1.0; // y scale
		data[5] = 0.0;
	}
	// copy constructor
	Transformation(const Transformation& copy) {
		*this = copy;
	}

	// copy operator
	Transformation& operator =(const Transformation& copy) {
		memcpy(data, copy.data, sizeof(double)*6);
		size = copy.size;
		return *this;
	}

	Transformation& operator *=(const Transformation& t);

	// is the tranformation only composed by translation and scale?
	bool IsSimpleTransformation() const {
		// check items a12 & a21
		return data[1] == 0.0 && data[3] == 0.0;
	}

	void ApplyToCoordinate(double x, double y, double* pRX, double* pRY) const;
	void ApplyToMeasure(double x, double* pX) const;
	
	static Transformation CreateTranslate(double x, double y);
	static Transformation CreateScale(double x, double y);
	static Transformation CreateRotate(double a);
	static Transformation CreateSkewX(double a);
	static Transformation CreateSkewY(double a);
	static Transformation CreateMatrix(double d[6]);

	// 3x3 matrix simplified to 6 elements vector
	// (line 3 of matrix is always [0 0 1])
	double data[6];

	// width and height of the parent object
	PointD size;
};

WX_DECLARE_LIST(Transformation, TransformationList);

#endif // !TRANS_H
