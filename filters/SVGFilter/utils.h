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
#define MM2PT(x) ((x)/25.4*72.0)
#define CM2PT(x) ((x)/2.54*72.0)
#define PC2PT(x) ((x)*12.0)
#define IN2PT(x) ((x)*72.0)
#define PX2PT(x) ((x)*0.8)

#ifndef M_SQRT2
#define M_SQRT2	 1.41421356237309504880
#endif

#ifndef M_PI
#define M_PI	 3.14159265358979323846
#endif

inline double deg2rad(double x) { return x*M_PI/180.0; }
inline double rad2deg(double x) { return x*180.0/M_PI; }

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

template <class T>
void mySwap(T& a, T& b)
{
	T c = a;
	a = b;
	b = c;
}

template <class T> T myMin(T a, T b) { return a < b ? a : b; }
template <class T> T myMax(T a, T b) { return a > b ? a : b; }

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

void AddProperty(xmlNodePtr cur, const wxString& sName, const wxString& sValue);
wxString GetStringProperty(xmlNodePtr cur, const char *name, const wxString& def = _T(""));
double GetDoubleProperty(xmlNodePtr cur, const char *name, double def = 0.0);
double GetClampedDoubleProperty(xmlNodePtr cur, const char *name, double def = 0.0);
wxColour GetColourProperty(xmlNodePtr cur, const char *name, wxColour def = wxColour());

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
	PointD operator+ (const PointD& p) const { return PointD(x+p.x, y+p.y); }
	PointD operator* (double d) const { return PointD(x*d, y*d); }
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
  Geometry (non necessary up-right):

  p11      p12
  +--------+
  |        |
  |        |
  +--------+
  p21      p22
  
 */
struct RectD {
	RectD() {}
	RectD(double x11, double y11, double x12, double y12,
		  double x21, double y21, double x22, double y22)
		: p11(x11, y11), p12(x12, y12), p21(x21, y21), p22(x22, y22) {}
	RectD(const PointD& p1, const PointD& p2) {
		PointD pA = p1;
		PointD pB = p2;
// 		if (pA.x > pB.x)
// 			mySwap(pA.x, pB.x);
// 		if (pA.y > pB.y)
// 			mySwap(pA.y, pB.y);
		p11 = pA;
		p12 = PointD(pB.x, pA.y);
		p21 = PointD(pA.x, pB.y);
		p22 = pB;
	}
	RectD(const RectD& copy) { *this = copy; }

	RectD& operator= (const RectD& copy) {
		p11 = copy.p11; p12 = copy.p12; p21 = copy.p21; p22 = copy.p22;
		return *this;
	}
	bool operator== (const RectD& r) const {
		return (p11==r.p11 && p12==r.p12 && p21==r.p21 && p22==r.p22);
	}

	bool IsUpright() const {
		return (p11.x == p21.x && p12.x == p22.x && p11.y == p12.y && p21.y == p22.y);
	}

	PointD LowerCoord() const {
		PointD p = p11;
		if (p12.x < p.x) p.x = p12.x;
		if (p21.x < p.x) p.x = p21.x;
		if (p22.x < p.x) p.x = p22.x;
		if (p12.y < p.y) p.y = p12.y;
		if (p21.y < p.y) p.y = p21.y;
		if (p22.y < p.y) p.y = p22.y;
		return p;
	}
	PointD HigherCoord() const {
		PointD p = p11;
		if (p12.x > p.x) p.x = p12.x;
		if (p21.x > p.x) p.x = p21.x;
		if (p22.x > p.x) p.x = p22.x;
		if (p12.y > p.y) p.y = p12.y;
		if (p21.y > p.y) p.y = p21.y;
		if (p22.y > p.y) p.y = p22.y;
		return p;
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
