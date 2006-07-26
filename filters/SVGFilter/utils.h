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
#define MM2PT(x) ((x)*2.834645669f)
#define CM2PT(x) ((x)*28.34645669f)
#define PC2PT(x) ((x)*12.0f)
#define IN2PT(x) ((x)*72.0f)
#define PX2PT(x) ((x)*1.0f)
#define PT2MM(x) ((x)*0.3527777778f)
#define PT2CM(x) ((x)*0.03527777778f)
#define PT2PC(x) ((x)*0.08333333333f)
#define PT2IN(x) ((x)*0.01388888889f)
#define PT2PX(x) ((x)*1.0f)

#ifndef M_SQRT2
#define M_SQRT2	 1.414213562f
#endif

// miscellaneous functions ---------------------------------------------------
	 
// trim whitespace from string
wxString TrimWs(const wxString& s);

// check if character is one of [0-9] or one of ".+-"
inline bool IsNumberChar(wxChar c)
{
	return ((c >= wxT('0') && c <= wxT('9')) || c == _T('+') || c == _T('-') || c == _T('.'));
}

// check if character is one of [0-9] or "."
inline bool IsNumberDigit(wxChar c)
{
	return ((c >= wxT('0') && c <= wxT('9')) || c == _T('.'));
}

// take (and remove) the first number from the string and return it
double TakeNumber(wxString& data);

// XML utilities -------------------------------------------------------------

inline bool IsEntityName(xmlNodePtr cur, const char *name)
{
	return !xmlStrcmp(cur->name, (const xmlChar *)name);
}

wxString GetStringProperty(xmlNodePtr cur, const char *name);
double GetDoubleProperty(xmlNodePtr cur, const char *name);

// measure conversion utilities ----------------------------------------------

bool IsUnitless(const wxString& measure);
bool IsMeasure(const wxString& measure);
double MeasureToMillipoints(const wxString& measure);
double MeasureToUserUnits(const wxString& measure);

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
