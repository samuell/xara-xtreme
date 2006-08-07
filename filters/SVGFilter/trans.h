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

Transformation ParseTransformations(const wxString& s, bool bScaleToMillipoints);
Transformation ParseTransformations(xmlNodePtr cur, const char* prop, bool bScaleToMillipoints);

#if SVGDEBUG
void DebugDumpTransformation(int lvl, const Transformation& trans);
#endif

#endif // !TRANS_H
