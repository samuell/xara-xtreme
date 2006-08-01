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
// gradients.h: This defines the class for handling the gradients

#ifndef GRADIENTS_H
#define GRADIENTS_H

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

// libxml2 library
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>

#ifndef LIBXML_READER_ENABLED
#error xmlreader must be enabled in libxml2!
#endif

#include "styles.h"

/***************************************************************************************************

>	struct GradientStop

	Author:		Sandro Sigala <sandro@sigala.it>
	Created:	25 July 2006
	Purpose:	Contains SVG gradients stop parameters.

***************************************************************************************************/

struct GradientStop {
	GradientStop() { offset = 0.0; stopColour = wxColour(0,0,0); }
	GradientStop(const GradientStop& copy) { *this = copy; }

	GradientStop& operator =(const GradientStop& copy) {
		offset = copy.offset;
		stopColour = copy.stopColour;
		return *this;
	}

	double				offset;
	wxColour			stopColour;
};

WX_DECLARE_LIST(GradientStop, GradientStopList);

/***************************************************************************************************

>	struct Gradient

	Author:		Sandro Sigala <sandro@sigala.it>
	Created:	25 July 2006
	Purpose:	Contains SVG gradients parameters.

***************************************************************************************************/

struct Gradient {
	enum Type { SolidColour, Linear, Radial };
	enum Units { UserSpaceOnUse, ObjectBoundingBox };

	Gradient() {
		type = Linear;
		units = ObjectBoundingBox;
		x1 = y1 = x2 = y2 = 0.0;
		stops.DeleteContents(true);
	}
	Gradient(const Gradient& copy) { *this = copy; }
	~Gradient() {}

	Gradient& operator =(const Gradient& copy);

	Type				type;
	wxString			xmlId;
	Units				units;
	// linear gradient parameters
	double				x1, y1, x2, y2;
	GradientStopList	stops;
	// radial gradient parameters
	double				cx, cy, r;
	// solidColor paramenters
	wxString			solidColour;
	double				solidOpacity;
};

WX_DECLARE_STRING_HASH_MAP(Gradient*, pGradientHashTable);

#endif // !GRADIENTS_H