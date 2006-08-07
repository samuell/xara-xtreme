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
	GradientStop() {
		offset = 0.0;
		stopColour = wxColour(0,0,0);
		stopOpacity = 1.0;
	}
	GradientStop(const GradientStop& copy) { *this = copy; }

	GradientStop& operator =(const GradientStop& copy) {
		offset = copy.offset;
		stopColour = copy.stopColour;
		stopOpacity = copy.stopOpacity;
		return *this;
	}

	double				offset;
	wxColour			stopColour;
	double				stopOpacity;
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
		cx = cy = r = 0.0;
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
	// radial gradient parameters
	double				cx, cy, r;
    // common gradient parameters
	GradientStopList	stops;
	// solidColor paramenters
	wxString			solidColour;
	double				solidOpacity;
};

WX_DECLARE_STRING_HASH_MAP(Gradient*, pGradientHashTable);

#endif // !GRADIENTS_H
