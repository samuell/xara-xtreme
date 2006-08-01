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
// utils.h: This defines various common utility functions

#ifndef UTILS_H
#define UTILS_H

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// XAR handling library
#include "xarlib/xarlib.h"

// libxml2 library
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>

#ifndef LIBXML_READER_ENABLED
#error xmlreader must be enabled in libxml2!
#endif

#include <string.h>

// Useful units conversion macros
#define MM2PT(x) ((x)*2.834645669)
#define CM2PT(x) ((x)*28.34645669)
#define PC2PT(x) ((x)*12.0)
#define IN2PT(x) ((x)*72.0)
#define PX2PT(x) ((x)*1.0)
#define PT2MM(x) ((x)*0.3527777778)
#define PT2CM(x) ((x)*0.03527777778)
#define PT2PC(x) ((x)*0.08333333333)
#define PT2IN(x) ((x)*0.01388888889)
#define PT2PX(x) ((x)*1.0)

#ifndef M_SQRT2
#define M_SQRT2	 1.414213562
#endif

// debug ---------------------------------------------------------------------

#if SVGDEBUG
#define DBGTRACE_STYLES			0001
#define DBGTRACE_TRANS			0002
#define DBGTRACE_TRANS_STACK	0004
#define DBGTRACE_PATHS			0010
#define DBGTRACE_SHAPES			0020
#define DBGTRACE_GROUPS			0040
#define DBGTRACE_GRADIENTS		0100
void svgtrace(int lvl, const char *fmt, ...);
#endif

// miscellaneous functions ---------------------------------------------------

// trim whitespace from string
wxString TrimWs(const wxString& s);

// check if character is one of [0-9] or one of ".+-"
inline bool IsNumberChar(wxChar c)
{
	return ((c >= wxT('0') && c <= wxT('9')) || c == _T('+') || c == _T('-') || c == _T('.'));
}

// check if character is one of [0-9] or one of ".eE"
inline bool IsNumberDigit(wxChar c)
{
	return ((c >= wxT('0') && c <= wxT('9')) || c == _T('.') || c == _T('e') || c == _T('E'));
}

// take (and remove) the first number from the string and return it
double TakeNumber(wxString& data);
inline double TakeMilliNumber(wxString& data) { return TakeNumber(data)*1000.0; }

// XML utilities -------------------------------------------------------------

inline bool IsEntityName(xmlNodePtr cur, const char *name)
{
	return !xmlStrcasecmp(cur->name, (const xmlChar *)name);
}

inline bool IsPropertyDefined(xmlNodePtr cur, const char *name)
{
	xmlChar *value = xmlGetProp(cur, (const xmlChar *)name);
	return (value != NULL);
}

wxString GetStringProperty(xmlNodePtr cur, const char *name);
double GetDoubleProperty(xmlNodePtr cur, const char *name);
#if 0 // XXX unused
bool IsAnyPropertyZero(xmlNodePtr cur, size_t size, ...);
#endif

// measure conversion utilities ----------------------------------------------

bool IsRelativeMeasure(const wxString& measure);
bool IsAbsoluteMeasure(const wxString& measure);
double AbsoluteMeasureToMillipoints(const wxString& measure);

// implements a point (double) -----------------------------------------------

struct PointD {
	PointD() : x(0.0f), y(0.0f) {}
	PointD(double x1, double y1) : x(x1), y(y1) {}
	PointD(const PointD& copy) { x = copy.x; y = copy.y; }

	PointD& operator= (const PointD& copy) { x = copy.x; y = copy.y; return *this; }
	bool operator== (const PointD& p) const { return (x==p.x && y==p.y); }

	double x, y;
};

// implements a coordinate (double) ------------------------------------------

struct CoordD {
	CoordD() {}
	CoordD(double x1, double y1, double x2, double y2) : a(x1, y1), b(x2, y2) {}
	CoordD(const CoordD& copy) { a = copy.a; b = copy.b; }

	CoordD& operator= (const CoordD& copy) { a = copy.a; b = copy.b; return *this; }
	bool operator== (const CoordD& p) const { return (a==p.a && b==p.b); }

	PointD a, b;
};

// implements a rectangle (double) -------------------------------------------

/*
 Geometry:

 p11      p12
 +--------+
 |        |
 |        |
 +--------+
 p21      p22


 Some coordinates are duplicated (e.g.: pll.y==p12.y and p21.y==p22.y)
 but thanks to this applying transformations is really simple.
*/

struct RectD {
	RectD() {}
	RectD(double x11, double y11, double x12, double y12,
		  double x21, double y21, double x22, double y22)
		: p11(x11, y11), p12(x12, y12), p21(x21, y21), p22(x22, y22) {}
	RectD(const RectD& copy) { *this = copy; }

	RectD& operator= (const RectD& copy) {
		p11 = copy.p11; p12 = copy.p12; p21 = copy.p21; p22 = copy.p22;
		return *this;
	}
	bool operator== (const RectD& r) const {
		return (p11==r.p11 && p12==r.p12 && p21==r.p21 && p22==r.p22);
	}

	bool IsUpright() const {
		// check only the first two coordinates
		return (p11.x == p21.x && p11.y == p12.y);
	}

	PointD p11, p12, p21, p22;
};

// implements an extremely simple and fast vector ----------------------------

template <class T>
class SimpleVector {
public:
	SimpleVector();
	~SimpleVector();

	T& operator[] (size_t n) { wxASSERT(n < m_count); return m_data[n]; }
	T operator[] (size_t n) const { wxASSERT(n < m_count); return m_data[n]; }

	size_t GetCount() const { return m_count; }
	void Append(const T& value);

private:
	T *m_data;
	size_t m_size;		// current allocated space
	size_t m_count;		// number of objects
};

template <class T>
SimpleVector<T>::SimpleVector()
{
	m_size = 16;
	m_count = 0;
	m_data = new T[m_size]();
}

template <class T>
SimpleVector<T>::~SimpleVector()
{
	delete[] m_data;
	m_data = NULL;
}

template <class T>
void SimpleVector<T>::Append(const T& value)
{
	if (m_count + 1 >= m_size) {
		// no more space, reallocate data buffer
		m_size *= 2;
		T *newdata = new T[m_size]();
		memcpy(newdata, m_data, sizeof(T)*m_count);
		delete[] m_data;
		m_data = newdata;
	}
	m_data[m_count++] = value;
}

#endif // !UTILS_H
