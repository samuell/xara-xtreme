// $Id$
// $Id$
/* @@tag:xara-cn@@ DO NOT MODIFY THIS LINE
================================XARAHEADERSTART===========================
 
               Xara LX, a vector drawing and manipulation program.
                    Copyright (C) 1993-2006 Xara Group Ltd.
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

// Implementation-specific definitions for fontbase.cpp. Not meant to
// included by any other file.

#include "fontbase.h"
#include <map>

/********************************************************************************************
>	class FontMetricsCacheEntry : public CC_CLASS_MEMDUMP
	   		
	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/96
	Purpose:	Holds a cache of char widths, ascent, descent and em for a particular font.
********************************************************************************************/

class FontMetricsCacheEntry: public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(FontMetricsCacheEntry);
	
public:
	FontMetricsCacheEntry();

	BOOL CacheFontMetrics(wxDC* pDC, CharDescription FontDesc, MILLIPOINT DefaultHeight, INT32 DesignSize);

	void CheckCharWidthsSameAsABCWidths(wxDC* pDC, CharDescription FontDesc);	// debug test

	inline static BOOL CharInCacheRange(WCHAR ch) { return (ch>=FIRSTCHAR && ch<=LASTCHAR); }

	inline INT32 GetCharWidthFromCache(WCHAR ch) { return pCharWidths[ch-FIRSTCHAR]; }

	inline MILLIPOINT      GetFontEmWidth() { return FontEmWidth; }
	inline MILLIPOINT      GetFontAscent()  { return FontAscent; }
	inline MILLIPOINT      GetFontDescent() { return FontDescent; }
	inline CharDescription GetFontDesc()    { return FontDesc; }

	inline void SetFontEmWidth(MILLIPOINT NewFontEmWidth) { FontEmWidth = NewFontEmWidth; }
	inline void SetFontAscent( MILLIPOINT NewFontAscent)  { FontAscent  = NewFontAscent; }
	inline void SetFontDescent(MILLIPOINT NewFontDescent) { FontDescent = NewFontDescent; }
	inline void SetFontDesc(CharDescription NewFontDesc)  { FontDesc    = NewFontDesc; }

protected:
	// this seems the only way to define constants in a class assigning values
	enum CharRange { FIRSTCHAR=32, LASTCHAR=127, NUMCHARS=(LASTCHAR-FIRSTCHAR+1) };

	MILLIPOINT pCharWidths[NUMCHARS];
	MILLIPOINT FontEmWidth;
	MILLIPOINT FontAscent;
	MILLIPOINT FontDescent;
	CharDescription FontDesc;
};

/********************************************************************************************
>	class FontMetricsCache : public CC_CLASS_MEMDUMP
	   		
	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/96
	Purpose:	Holds a cache of char widths, ascent, descent and em for a particular font.
********************************************************************************************/

class FontMetricsCache: public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(FontMetricsCache);
	
public:
	static void InvalidateCharMetrics();
	static BOOL GetCharMetrics(wxDC* pDC, WCHAR ch, CharDescription& FontDesc, CharMetrics* pCharMetrics);

protected:
	// this seems the only way to define constants in a class assigning values
	enum CacheInfo { NUMENTRIES=3 };

	static FontMetricsCacheEntry mpFontMetricsData[NUMENTRIES];
};


/********************************************************************************************

>	class CharOutlineCache : public CC_CLASS_MEMDUMP
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95
	Purpose:	A cache for a character outline. This cache can only be accessed via the
				OILFontMan character outline calls

********************************************************************************************/

class CharOutlineCache : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(CharOutlineCache);
	
	friend class OILFontMan;
	
	private:
		#if _DEBUG
		static void Dump();
		#endif

		static DocCoord CacheCoords[OILFONTLIMIT];
		static PathVerb CacheVerbs[OILFONTLIMIT];
		static UINT32 CacheSize;
		static DocCoord LastMoveTo;
};

// Forward decleration
struct MillipointKerningPair;

/********************************************************************************************
>	class FontKerningPairsCacheEntry : public CC_CLASS_MEMDUMP
	   		
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/2000
	Purpose:	Holds a cache of kerning pairs for a font
********************************************************************************************/

class FontKerningPairsCacheEntry : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(FontKerningPairsCacheEntry);
	
public:
	FontKerningPairsCacheEntry();		// constructor
	~FontKerningPairsCacheEntry();		// destructor

	void Reinitialise(CharDescription& NewFontDesc);

	MILLIPOINT GetCharsKerning(WCHAR chLeft, WCHAR chRight);
	inline CharDescription GetFontDesc()					{ return FontDesc; }
	inline void SetFontDesc(CharDescription NewFontDesc)	{ FontDesc = NewFontDesc; }

#ifdef _DEBUG
public:
	void Dump();
#endif //_DEBUG

protected:
	CharDescription FontDesc;

protected: // Kerning data
	std::map<UINT32,INT32>* m_pPairsCacheMap;

private:
	// declare private copy constructor and assignment operator to prevent
	// copy/assignment operations - the methods are not implemented
    FontKerningPairsCacheEntry(const FontKerningPairsCacheEntry&);
	FontKerningPairsCacheEntry& operator=(const FontKerningPairsCacheEntry&);
};


/********************************************************************************************

>	class FontKerningPairsCache : public CC_CLASS_MEMDUMP
	   		
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/2000
	Purpose:	A cache for the kerning pairs for a font

********************************************************************************************/

class FontKerningPairsCache : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(FontKerningPairsCache);
	
public:
	static MILLIPOINT GetCharsKerning(wxDC* pDC, WCHAR chLeft, WCHAR chRight,
									  CharDescription& FontDesc);

#ifdef _DEBUG
public:
	static void Dump();
#endif//_DEBUG

protected:
	// this seems the only way to define constants in a class assigning values
	enum CacheInfo { NUMENTRIES=3 };

	static FontKerningPairsCacheEntry m_FontKerningPairsCacheData[NUMENTRIES];
};

