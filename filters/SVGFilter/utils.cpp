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
// utils.cpp: This implements various common utility functions

#include "utils.h"
#include "styles.h"

// trim whitespace from string
wxString TrimWs(const wxString& s)
{
	wxString ret = s;
	return ret.Trim(true).Trim(false);
}

// take (and remove) the first number from the string and return it
// XXX should be more clever parsing floating point numbers
// (e.g. bogus 1.2e2.1e3 is currently accepted)
double TakeNumber(wxString& data)
{
	wxString s;
	data = TrimWs(data);
	if (data.IsEmpty())
		return 0.0; // some error
	if (IsNumberChar(data[0])) {
		s += data[0];
		data = data.Mid(1);
		while (data.Length() > 0 && IsNumberDigit(data[0])) {
			s += data[0];
			data = data.Mid(1);
		}
	}
	double f;
	s.ToDouble(&f);
	return f;
}

void AddProperty(xmlNodePtr cur, const wxString& sName, const wxString& sValue)
{
	xmlSetProp(cur, (const xmlChar *)(const char *)sName.mb_str(wxConvUTF8),
			   (const xmlChar *)(const char *)sValue.mb_str(wxConvUTF8));
}

// take the property named "name"
wxString GetStringProperty(xmlNodePtr cur, const char *name, const wxString& def)
{
	wxString s = def;
	xmlChar *value = xmlGetProp(cur, (const xmlChar *)name);
	if (value != NULL) {
		s = wxString::FromAscii((const char *)value);
		xmlFree(value);
	}
	return s;
}

// take the property named "name" and convert to double
double GetDoubleProperty(xmlNodePtr cur, const char *name, double def)
{
	double x = def;
	xmlChar *value = xmlGetProp(cur, (const xmlChar *)name);
	if (value != NULL) {
		x = atof((const char *)value);
		xmlFree(value);
	}
	return x;
}

// take the property named "name" and convert to double and clamp to range [0,1]
double GetClampedDoubleProperty(xmlNodePtr cur, const char *name, double def)
{
	double x = def;
	xmlChar *value = xmlGetProp(cur, (const xmlChar *)name);
	if (value != NULL) {
		wxString s = wxString::FromAscii((const char *)value);
		xmlFree(value);

		s = TrimWs(s);
		if (s.Right(1) == _T("%")) {
			// strip "%"
			s = s.RemoveLast(1).Trim();
			s.ToDouble(&x);
			x *= 0.01;
		} else {
			s.ToDouble(&x);
		}
		if (x < 0.0)
			x = 0.0;
		else if (x > 1.0)
			x = 1.0;
	}
	return x;
}

// take the property named "name" and convert to a colour
wxColour GetColourProperty(xmlNodePtr cur, const char *name, wxColour def)
{
	wxColour col = def;
	xmlChar *value = xmlGetProp(cur, (const xmlChar *)name);
	if (value != NULL) {
		wxString s = wxString::FromAscii((const char *)value);
		col = Style::ParseColour(s);
		xmlFree(value);
	}
	return col;
}

// is the measure relative?
bool IsRelativeMeasure(const wxString& measure)
{
	wxString s = measure;
	// remove whitespace and convert to lowercase
	s = TrimWs(s).MakeLower();
	return (s.IsNumber() || s.Right(1) == _T("%"));
}

// is the string an absolute measure?
bool IsAbsoluteMeasure(const wxString& measure)
{
	wxString s = measure;
	// remove whitespace and convert to lowercase and take last 2 chars
	s = TrimWs(s).MakeLower().Right(2);
	return (s == _T("cm") || s == _T("mm") || s == _T("in") || s == _T("pc") || s == _T("px"));
}

// convert measure to millipoints
double AbsoluteMeasureToMillipoints(const wxString& measure)
{
	wxString s = measure;
	// remove whitespace and convert to lowercase
	s = TrimWs(s).MakeLower();

	double f;
	if (s.Right(2) == _T("cm")) {
		s = s.RemoveLast(2).Trim();
		s.ToDouble(&f);
		f = CM2PT(f)/0.8*1000.0; // Convert to millipoints
	} else if (s.Right(2) == _T("mm")) {
		s = s.RemoveLast(2).Trim();
		s.ToDouble(&f);
		f = MM2PT(f)/0.8*1000.0; // Convert to millipoints
	} else if (s.Right(2) == _T("in")) {
		s = s.RemoveLast(2).Trim();
		s.ToDouble(&f);
		f = IN2PT(f)/0.8*1000.0; // Convert to millipoints
	} else if (s.Right(2) == _T("pc")) {
		s = s.RemoveLast(2).Trim();
		s.ToDouble(&f);
		f = PC2PT(f)/0.8*1000.0; // Convert to millipoints
	} else if (s.Right(2) == _T("px")) {
		s = s.RemoveLast(2).Trim();
		s.ToDouble(&f);
		f = PX2PT(f)/0.8*1000.0; // Convert to millipoints
	} else { // anything else
		if (s.Right(2) == _T("pt"))
			s = s.RemoveLast(2).Trim();
		s.ToDouble(&f);
		f *= 1000.0;
	}

	return f;
}

#if SVGDEBUG
extern long iDebugTraceLevel; // from svgfilter.cpp

void svgtrace(int lvl, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if (lvl & iDebugTraceLevel)
		vfprintf(stderr, fmt, ap);
	va_end(ap);
}
#endif
