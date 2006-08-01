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
#define STYLE_STROKE_WIDTH			00002000
#define STYLE_STROKE_LINECAP		00004000
#define STYLE_STROKE_LINEJOIN		00010000
#define STYLE_STROKE_OPACITY		00020000
#define STYLE_STROKE_ALL			(STYLE_STROKE_COLOUR|STYLE_STROKE_WIDTH|STYLE_STROKE_LINECAP| \
									 STYLE_STROKE_LINEJOIN|STYLE_STROKE_OPACITY)

#define STYLE_STOP_COLOUR		    00100000
#define STYLE_STOP_OPACITY			00200000
#define STYLE_STOP_ALL				(STYLE_STOP_COLOUR|STYLE_STOP_OPACITY)

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
		m_strokeColour = wxColour(0,0,0);
		m_strokeOpacity = 1.0;
		m_strokeWidth = 1;
		m_stopColour = wxColour(0,0,0);
		m_stopOpacity = 0.0;
		m_fillGradient = NULL;
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
		m_strokeColour = copy.m_strokeColour;
		m_strokeOpacity = copy.m_strokeOpacity;
		m_strokeWidth = copy.m_strokeWidth;
		m_stopColour = copy.m_stopColour;
		m_stopOpacity = copy.m_stopOpacity;
		m_fillGradient = copy.m_fillGradient;
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

	bool IsStopColourDefined() const { return m_defined & STYLE_STOP_COLOUR; }
	wxColour GetStopColour() const { return m_stopColour; }
	void SetStopColour(const wxColour& col) {
		m_defined |= STYLE_STOP_COLOUR;
		m_stopColour = col;
	}

	bool IsStopOpacityDefined() const { return m_defined & STYLE_STOP_OPACITY; }
	double GetStopOpacity() const { return m_stopOpacity; }
	void SetStopOpacity(double value) {
		m_defined |= STYLE_STOP_OPACITY;
		m_stopOpacity = value;
	}

	bool IsFillGradientDefined() const { return m_defined & STYLE_FILL_GRADIENT; }
	Gradient* GetFillGradient() const { return m_fillGradient; }
	void SetFillGradient(Gradient* value) {
		m_defined |= STYLE_FILL_GRADIENT;
		m_fillGradient = value;
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
	wxColour	m_stopColour;
	double		m_stopOpacity;
	UINT32		m_defined;
};

WX_DECLARE_LIST(Style, StyleList);

#endif // !STYLES_H
