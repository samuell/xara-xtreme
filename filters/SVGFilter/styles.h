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
// styles.h: This defines the class for handling the rendering styles

#ifndef STYLES_H
#define STYLES_H

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/tokenzr.h"
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

// hashing function
class wxColourHash {
public:
	wxColourHash() {}

	// compute the hash
	unsigned long operator() (const wxColour& k) const {
		return k.Red() + (k.Green() + k.Blue()<<8)<<8;
	}

	wxColourHash& operator= (const wxColourHash&) { return *this; }
};

// comparison operator
class wxColourEqual {
public:
	wxColourEqual() {}

	// compare for equality
	bool operator() (const wxColour& a, const wxColour& b) const { return a == b; }

	wxColourEqual& operator= (const wxColourEqual&) { return *this; }
};

// Hash table for storing XAR references to colours
WX_DECLARE_HASH_MAP(wxColour, INT32, wxColourHash, wxColourEqual, ColourRefHashTable);

// style flags (for IsXXXDefined())
#define STYLE_FILL_COLOUR			00000010
#define STYLE_FILL_OPACITY			00000020
#define STYLE_FILL_RULE				00000040
#define STYLE_FILL_GRADIENT			00000100
#define STYLE_FILL_ALL				(STYLE_FILL_COLOUR|STYLE_FILL_OPACITY|STYLE_FILL_RULE| \
									 STYLE_FILL_GRADIENT)

#define STYLE_STROKE_COLOUR			00001000
#define STYLE_STROKE_OPACITY		00002000
#define STYLE_STROKE_WIDTH			00004000
#define STYLE_STROKE_LINECAP		00010000
#define STYLE_STROKE_LINEJOIN		00020000
#define STYLE_STROKE_GRADIENT		00040000
#define STYLE_STROKE_ALL			(STYLE_STROKE_COLOUR|STYLE_STROKE_OPACITY|STYLE_STROKE_WIDTH| \
									 STYLE_STROKE_LINECAP|STYLE_STROKE_LINEJOIN|STYLE_STROKE_GRADIENT)

#define STYLE_STOP_OFFSET			00100000
#define STYLE_STOP_COLOUR		    00200000
#define STYLE_STOP_OPACITY			00400000
#define STYLE_STOP_ALL				(STYLE_STOP_OFFSET|STYLE_STOP_COLOUR|STYLE_STOP_OPACITY)

#define STYLE_OPACITY				01000000

struct Gradient; // forward declaration

/*
 * NB: Inside the Style class is stored also XML id information (the tag of the object):
 *     while it's not an object style in the strict sense, it's very useful to keep
 *     this information in this class.
 */

/***************************************************************************************************

>	class Style

	Author:		Sandro Sigala <sandro@sigala.it>
	Created:	10 July 2006
	Purpose:	Contains SVG styles and parses CSS/SVG colours.

***************************************************************************************************/

class Style {
public:
	// default constructor
	Style() {
		// default values
		m_fillColour = wxColour(255,255,255);
		m_fillOpacity = 1.0;
		m_fillGradient = NULL;
		m_strokeColour = wxColour(0,0,0);
		m_strokeOpacity = 1.0;
		m_strokeWidth = 1;
		m_strokeLineJoin = MitreJoin;
		m_strokeLineCap = LineCapButt;
		m_strokeGradient = NULL;
		m_opacity = 1.0;
		m_stopOffset = 0.0;
		m_stopColour = wxColour(0,0,0);
		m_stopOpacity = 0.0;
		m_defined = 0; // no defined attibute
	}
	// copy constructor
	Style(const Style& copy) {
		*this = copy;
	}

	// copy operator
	Style& operator =(const Style& copy) {
		// NB: the xml id is not copied (does not inherit from parent)
		m_fillColour = copy.m_fillColour;
		m_fillOpacity = copy.m_fillOpacity;
		m_fillGradient = copy.m_fillGradient;
		m_strokeColour = copy.m_strokeColour;
		m_strokeOpacity = copy.m_strokeOpacity;
		m_strokeWidth = copy.m_strokeWidth;
		m_strokeLineJoin = copy.m_strokeLineJoin;
		m_strokeLineCap = copy.m_strokeLineCap;
		m_strokeGradient = copy.m_strokeGradient;
		m_opacity = copy.m_opacity;
		m_stopOffset = copy.m_stopOffset;
		m_stopColour = copy.m_stopColour;
		m_stopOpacity = copy.m_stopOpacity;
		m_defined = copy.m_defined;
		return *this;
	}

	wxString GetXmlId() const { return m_xmlId; }
	void SetXmlId(const wxString& id) {
		m_xmlId = id;
	}

	bool IsFillColourDefined() const { return m_defined & STYLE_FILL_COLOUR; }
	wxColour GetFillColour() const { return m_fillColour; }
	void SetFillColour(const wxColour& col) {
		m_defined |= STYLE_FILL_COLOUR;
		m_fillColour = col;
	}

	bool IsFillOpacityDefined() const { return m_defined & STYLE_FILL_OPACITY; }
	double GetFillOpacity() const { return m_fillOpacity; }
	void SetFillOpacity(double value) {
		m_defined |= STYLE_FILL_OPACITY;
		m_fillOpacity = value;
	}

	bool IsFillGradientDefined() const { return m_defined & STYLE_FILL_GRADIENT; }
	Gradient* GetFillGradient() const { return m_fillGradient; }
	void SetFillGradient(Gradient* value) {
		m_defined |= STYLE_FILL_GRADIENT;
		m_fillGradient = value;
	}

	bool IsStrokeColourDefined() const { return m_defined & STYLE_STROKE_COLOUR; }
	wxColour GetStrokeColour() const { return m_strokeColour; }
	void SetStrokeColour(const wxColour& col) {
		m_defined |= STYLE_STROKE_COLOUR;
		m_strokeColour = col;
	}

	bool IsStrokeOpacityDefined() const { return m_defined & STYLE_STROKE_OPACITY; }
	double GetStrokeOpacity() const { return m_strokeOpacity; }
	void SetStrokeOpacity(double value) {
		m_defined |= STYLE_STROKE_OPACITY;
		m_strokeOpacity = value;
	}

	bool IsStrokeWidthDefined() const { return m_defined & STYLE_STROKE_WIDTH; }
	INT32 GetStrokeWidth() const { return m_strokeWidth; }
	void SetStrokeWidth(INT32 width) {
		m_defined |= STYLE_STROKE_WIDTH;
		m_strokeWidth = width;
	}

	bool IsStrokeLineJoinDefined() const { return m_defined & STYLE_STROKE_LINEJOIN; }
	JointType GetStrokeLineJoin() const { return m_strokeLineJoin; }
	void SetStrokeLineJoin(JointType jt ) {
		m_defined |= STYLE_STROKE_LINEJOIN;
		m_strokeLineJoin = jt;
	}

	bool IsStrokeLineCapDefined() const { return m_defined & STYLE_STROKE_LINECAP; }
	LineCapType GetStrokeLineCap() const { return m_strokeLineCap; }
	void SetStrokeLineCap(LineCapType lct ) {
		m_defined |= STYLE_STROKE_LINECAP;
		m_strokeLineCap = lct;
	}

	bool IsStrokeGradientDefined() const { return m_defined & STYLE_STROKE_GRADIENT; }
	Gradient* GetStrokeGradient() const { return m_strokeGradient; }
	void SetStrokeGradient(Gradient* value) {
		m_defined |= STYLE_STROKE_GRADIENT;
		m_strokeGradient = value;
	}

	bool IsOpacityDefined() const { return m_defined & STYLE_OPACITY; }
	double GetOpacity() const { return m_opacity; }
	void SetOpacity(double value) {
		m_defined |= STYLE_OPACITY;
		m_opacity = value;
	}

	bool IsStopColourDefined() const { return m_defined & STYLE_STOP_COLOUR; }
	wxColour GetStopColour() const { return m_stopColour; }
	void SetStopColour(const wxColour& col) {
		m_defined |= STYLE_STOP_COLOUR;
		m_stopColour = col;
	}

	bool IsStopOffsetDefined() const { return m_defined & STYLE_STOP_OFFSET; }
	double GetStopOffset() const { return m_stopOffset; }
	void SetStopOffset(double value) {
		m_defined |= STYLE_STOP_OFFSET;
		m_stopOffset = value;
	}

	bool IsStopOpacityDefined() const { return m_defined & STYLE_STOP_OPACITY; }
	double GetStopOpacity() const { return m_stopOpacity; }
	void SetStopOpacity(double value) {
		m_defined |= STYLE_STOP_OPACITY;
		m_stopOpacity = value;
	}

	static void PrepareColourHashTable();
	static wxColour ParseColour(const wxString& s);

private:
	wxString	m_xmlId;
	wxColour	m_fillColour;
	double		m_fillOpacity;
	Gradient*	m_fillGradient;
	wxColour	m_strokeColour;
	double		m_strokeOpacity;
	INT32		m_strokeWidth;
	JointType	m_strokeLineJoin;
	LineCapType	m_strokeLineCap;
	Gradient*	m_strokeGradient;
	double		m_opacity;
	double		m_stopOffset;
	wxColour	m_stopColour;
	double		m_stopOpacity;
	UINT32		m_defined;
};

WX_DECLARE_LIST(Style, StyleList);

#endif // !STYLES_H
