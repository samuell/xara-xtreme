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
// styles.cpp: This implements the class for handling the rendering styles

#include "svgfilter.h"
#include "styles.h"
#include "utils.h"

// define list of Style elements
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(StyleList);

// static table of CSS colours
// contains standard and non-standard colours (standard are marked)
static const struct CSSTextColour {
	const char *name;
	unsigned int colour;
} sCSSColourTable[] = {
	{ "aliceblue",				0xf0f8ff },
	{ "antiquewhite",			0xfaebd7 },
	{ "aqua",					0x00ffff }, // std
	{ "aquamarine",				0x7fffd4 },
	{ "azure",					0xf0ffff },
	{ "beige",					0xf5f5dc },
	{ "bisque",					0xffe4c4 },
	{ "black",					0x000000 }, // std
	{ "blanchedalmond",			0xffebcd },
	{ "blue",					0x0000ff }, // std
	{ "blueviolet",				0x8a2be2 },
	{ "brown",					0xa52a2a },
	{ "burlywood",				0xdeb887 },
	{ "cadetblue",				0x5f9ea0 },
	{ "chartreuse",				0x7fff00 },
	{ "chocolate",				0xd2691e },
	{ "coral",					0xff7f50 },
	{ "cornflowerblue",			0x6495ed },
	{ "cornsilk",				0xfff8dc },
	{ "crimson",				0xdc143c },
	{ "cyan",					0x00ffff },
	{ "darkcyan",				0x008b8b },
	{ "darkgoldenrod",			0xb8860b },
	{ "darkgray",				0xa9a9a9 },
	{ "darkgreen",				0x006400 },
	{ "darkkhaki",				0xbdb76b },
	{ "darkmagenta",			0x8b008b },
	{ "darkolivegreen",			0x556b2f },
	{ "darkorange",				0xff8c00 },
	{ "darkorchid",				0x9932cc },
	{ "darkred",				0x8b0000 },
	{ "darksalmon",				0xe9967a },
	{ "darkseagreen",			0x8fbc8f },
	{ "darkslateblue",			0x483d8b },
	{ "darkslategray",			0x2f4f4f },
	{ "darkturquoise",			0x00ced1 },
	{ "darkviolet",				0x9400d3 },
	{ "deeppink",				0xff1493 },
	{ "deepskyblue",			0x00bfff },
	{ "dimgray",				0x696969 },
	{ "dodgerblue",				0x1e90ff },
	{ "feldspar",				0xd19275 },
	{ "firebrick",				0xb22222 },
	{ "floralwhite",			0xfffaf0 },
	{ "forestgreen",			0x228b22 },
	{ "fuchsia",				0xff00ff }, // std
	{ "gainsboro",				0xdcdcdc },
	{ "ghostwhite",				0xf8f8ff },
	{ "gold",					0xffd700 },
	{ "goldenrod",				0xdaa520 },
	{ "gray",					0x808080 }, // std
	{ "green",					0x008000 }, // std
	{ "greenyellow",			0xadff2f },
	{ "honeydew",				0xf0fff0 },
	{ "hotpink",				0xff69b4 },
	{ "indianred",				0xcd5c5c },
	{ "indigo",					0x4b0082 },
	{ "ivory",					0xfffff0 },
	{ "khaki",					0xf0e68c },
	{ "lavender",				0xe6e6fa },
	{ "lavenderblush",			0xfff0f5 },
	{ "lawngreen",				0x7cfc00 },
	{ "lemonchiffon",			0xfffacd },
	{ "lightblue",				0xadd8e6 },
	{ "lightcoral",				0xf08080 },
	{ "lightcyan",				0xe0ffff },
	{ "lightgoldenrodyellow",	0xfafad2 },
	{ "lightgrey",				0xd3d3d3 },
	{ "lightgreen",				0x90ee90 },
	{ "lightpink",				0xffb6c1 },
	{ "lightsalmon",			0xffa07a },
	{ "lightseagreen",			0x20b2aa },
	{ "lightskyblue",			0x87cefa },
	{ "lightslateblue",			0x8470ff },
	{ "lightslategray",			0x778899 },
	{ "lightsteelblue",			0xb0c4de },
	{ "lightyellow",			0xffffe0 },
	{ "lime",					0x00ff00 }, // std
	{ "limegreen",				0x32cd32 },
	{ "linen",					0xfaf0e6 },
	{ "magenta",				0xff00ff },
	{ "maroon",					0x800000 }, // std
	{ "mediumaquamarine",		0x66cdaa },
	{ "mediumblue",				0x0000cd },
	{ "mediumorchid",			0xba55d3 },
	{ "mediumpurple",			0x9370d8 },
	{ "mediumseagreen",			0x3cb371 },
	{ "mediumslateblue",		0x7b68ee },
	{ "mediumspringgreen",		0x00fa9a },
	{ "mediumturquoise",		0x48d1cc },
	{ "mediumvioletred",		0xc71585 },
	{ "midnightblue",			0x191970 },
	{ "mintcream",				0xf5fffa },
	{ "mistyrose",				0xffe4e1 },
	{ "moccasin",				0xffe4b5 },
	{ "navajowhite",			0xffdead },
	{ "navy",					0x000080 }, // std
	{ "oldlace",				0xfdf5e6 },
	{ "olive",					0x808000 }, // std
	{ "olivedrab",				0x6b8e23 },
	{ "orange",					0xffa500 },
	{ "orangered",				0xff4500 },
	{ "orchid",					0xda70d6 },
	{ "palegoldenrod",			0xeee8aa },
	{ "palegreen",				0x98fb98 },
	{ "paleturquoise",			0xafeeee },
	{ "palevioletred",			0xd87093 },
	{ "papayawhip",				0xffefd5 },
	{ "peachpuff",				0xffdab9 },
	{ "peru",					0xcd853f },
	{ "pink",					0xffc0cb },
	{ "plum",					0xdda0dd },
	{ "powderblue",				0xb0e0e6 },
	{ "purple",					0x800080 }, // std
	{ "red",					0xff0000 }, // std
	{ "rosybrown",				0xbc8f8f },
	{ "royalblue",				0x4169e1 },
	{ "saddlebrown",			0x8b4513 },
	{ "salmon",					0xfa8072 },
	{ "sandybrown",				0xf4a460 },
	{ "seagreen",				0x2e8b57 },
	{ "seashell",				0xfff5ee },
	{ "sienna",					0xa0522d },
	{ "silver",					0xc0c0c0 }, // std
	{ "skyblue",				0x87ceeb },
	{ "slateblue",				0x6a5acd },
	{ "slategray",				0x708090 },
	{ "snow",					0xfffafa },
	{ "springgreen",			0x00ff7f },
	{ "steelblue",				0x4682b4 },
	{ "tan",					0xd2b48c },
	{ "teal",					0x008080 }, // std
	{ "thistle",				0xd8bfd8 },
	{ "tomato",					0xff6347 },
	{ "turquoise",				0x40e0d0 },
	{ "violet",					0xee82ee },
	{ "violetred",				0xd02090 },
	{ "wheat",					0xf5deb3 },
	{ "white",					0xffffff }, // std
	{ "whitesmoke",				0xf5f5f5 },
	{ "yellow",					0xffff00 }, // std
	{ "yellowgreen",			0x9acd32 },
};

WX_DECLARE_STRING_HASH_MAP(wxColour, CSSColourHashType);
// here we store all the known CSS colour names
static CSSColourHashType sCSSColourHash;

// push all the known colours into the hash table
void Style::PrepareColourHashTable()
{
	size_t size = sizeof(sCSSColourTable) / sizeof(sCSSColourTable[0]);
	for (unsigned int i = 0; i < size; ++i) {
		wxString sName = wxString::FromAscii(sCSSColourTable[i].name);
		unsigned int ulColour = sCSSColourTable[i].colour;
	    BYTE bR = (ulColour >> 16) & 255;
	    BYTE bG = (ulColour >> 8) & 255;
	    BYTE bB = (ulColour & 255);
		sCSSColourHash[sName] = wxColour(bR, bG, bB);
	}
}

// translate a SVG/CSS colour into a wxWidgets colour
wxColour Style::ParseColour(const wxString& sColour)
{
	wxString s = sColour;
	s = TrimWs(s);

	if (s.IsEmpty())
		return wxColour(0,0,0);
	else if (s == _T("none"))
		return wxColour(); // return invalid colour (means no color)
	else if (s.Left(1) == _T("#")) {
		// hexadecimal colour definition
		s = TrimWs(s.Mid(1)); // strip '#', then whitespace
		if (s.Length() == 6) { // #rrggbb
			wxString sR = s.Mid(0, 2);
			wxString sG = s.Mid(2, 2);
			wxString sB = s.Mid(4, 2);
			long lR, lG, lB;
			sR.ToLong(&lR, 16);
			sG.ToLong(&lG, 16);
			sB.ToLong(&lB, 16);

			return wxColour(lR, lG, lB);
		} else if (s.Length() == 3) { // #rgb
			wxString sR = s.Mid(0, 1);
			wxString sG = s.Mid(1, 1);
			wxString sB = s.Mid(2, 2);
			// transform from "rgb" to "rrggbb"
			sR += sR;
			sG += sG;
			sB += sB;
			long lR, lG, lB;
			sR.ToLong(&lR, 16);
			sG.ToLong(&lG, 16);
			sB.ToLong(&lB, 16);

			return wxColour(lR, lG, lB);
		} else {
			// XXX print warning: invalid colour
		}
	} else if (s.Left(4) == _T("rgb(")) { // "rgb(a, b, c)" colour
		s = s.Mid(4); // strip "rgb("
		if (s.Right(1) == _T(")")) // strip last ")"
			s.Truncate(s.Length() - 1);
		wxStringTokenizer tkz(s, _T(","));
		if (tkz.CountTokens() == 3) {
			wxString sR = TrimWs(tkz.GetNextToken());
			wxString sG = TrimWs(tkz.GetNextToken());
			wxString sB = TrimWs(tkz.GetNextToken());
			if (sR.Right(1) == _T("%") && sG.Right(1) == _T("%") && sB.Right(1) == _T("%"))	{
				// percentage colour "rgb(r%, g%, b%)"
				sR.Truncate(sR.Length() - 1); // strip last "%"
				sG.Truncate(sG.Length() - 1);
				sB.Truncate(sB.Length() - 1);

				double fR, fG, fB;
				sR.ToDouble(&fR);
				sG.ToDouble(&fG);
				sB.ToDouble(&fB);
				fR *= 2.55f; // scale from [0,100] to [0,255]
				fG *= 2.55f;
				fB *= 2.55f;

				return wxColour((unsigned char)fR, (unsigned char)fG, (unsigned char)fB);
			} else {
				// integer colour "rgb(r, g, b)"
				long lR, lG, lB;
				sR.ToLong(&lR);
				sG.ToLong(&lG);
				sB.ToLong(&lB);

				return wxColour(lR, lG, lB);
			}
		}
	} else {
		// colour name, lookup the hash table
		wxColour colour = sCSSColourHash[sColour];
		if (colour.Ok()) {
			return colour;
		}
	}

	wxString msg = wxString::Format(_("invalid colour %s"), (const char *)sColour.mb_str(wxConvUTF8));
	ReportWarning(msg);

	// parse error: return an black colour
	return wxColour(0,0,0);
}
