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
// Header file for FreeType Font manager. This font manager provides access to
// scalable fonts supported by FreeType (via Pango, fontconfig and FreeType)

/*
*/

#ifndef INC_FTFONTS
#define INC_FTFONTS

#include "fontbase.h"

// Forward definitions
class OILEnumFonts;

/********************************************************************************************
>	class FTFont : public FontBase
	   		
	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	22/02/06
	Purpose:	A class description of a font managed by FreeType.
********************************************************************************************/

class FTFont : public FontBase
{
	CC_DECLARE_DYNCREATE(FTFont)

public:
	 FTFont(FontClass fc = FC_UNDEFINED);
	~FTFont();

	virtual FontClass GetFontClass() { return m_FontClass; };
	virtual void Dump();
 private:
	// fonts managed by FreeType can be TrueType fonts or Type1 fonts
	FontClass m_FontClass;
};

/********************************************************************************************

>	class FTFontCacheEntry
	   		
	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	19/05/06
	Purpose:	an entry in the new font list cache - used as the value part of a key/value
				pair where the key is the face name wrapped in an ENUMLOGFONT structure

********************************************************************************************/

class FTFontCacheEntry
{
 public:
	FontClass m_Class;
	BOOL m_Referenced;               // used during recaching

	FTFontCacheEntry(FontClass Class, BOOL Referenced):
		m_Class(Class), m_Referenced(Referenced) {}
	FTFontCacheEntry(): m_Class(FC_UNDEFINED), m_Referenced(FALSE) {}
};

// we need to be able to compare ENUMLOGFONTs so we can use them as the keys of a map
bool operator<(ENUMLOGFONT &e1, ENUMLOGFONT &e2);

// Our font list cache is simply a map from font name to class (plus flags).
// This allows us to add/list entries easily and find out whether a specific
// font is in the cache in logarithmic time (for incremental cache updates).
// Using ENUMLOGFONTs as keys allows us to enumerate fonts more efficiently
// because the Kernel expects to be passed ENUMLOGFONT structures.
typedef map<ENUMLOGFONT,FTFontCacheEntry> FTFontCache;

/********************************************************************************************

>	class FTFontMan : public CCObject
	   		
	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	22/02/06
	Purpose:	A static class to allow access to fonts managed by FreeType.

********************************************************************************************/

class FTFontMan : public CCObject
{
	CC_DECLARE_DYNCREATE(FTFontMan)

	friend class OILFontMan;

// constructor
public:
	FTFontMan();

	static BOOL     GetAscentDescent(CharDescription& ChDesc, INT32* pAscent, INT32* pDescent);
	static BOOL     GetCharWidth(CharDescription& ChDesc, TCHAR FirstChar, TCHAR LastChar, INT32* pCharWidthsBuf);
    static INT32    GetCharsKerning(CharDescription& FontDesc, TCHAR LeftChar, TCHAR RightChar);

// the support interface (functions in class OILFontMan will use these)
private:
	static BOOL		IsOkToCall();
	static BOOL		CacheNamedFont(String_64* pFont);
	static BOOL		CacheCompatibleFont(String_64* pFont);
	static void		ValidateCache();
	static void		FindClosestFont();
	static FTFont*	CreateNewFont(String_64* pFontName, FontClass Class);

	// extract the Panose information
	static OUTLINETEXTMETRIC* GetOutlineTextMetric(LOGFONT *pLogFont);

	// get a vector representation of a glyph
	static BOOL 	GetCharOutline(CharDescription& ChDesc, 
								   DocCoord** ppCoords,
								   PathVerb** ppVerbs,
								   UINT32* pNumCoords,
								   wxDC* pDC);
// utterly private
private:
	static BOOL CacheFontCore(String_64* pFont, BOOL compatible);

	// the font list cache - a cache of enumerated fonts
	// do not confuse this with the kernel's font cache
	// (which holds the fonts that have been used)
	static FTFontCache* m_cache;

// function called by class EnumFonts as a method for the kernel to enumerate all the installed fonts
public:
	static void		EnumAllFonts(OILEnumFonts* pClass);

	// get a font class for a face name (FC_TRUETYPE or FC_ATM) - returns FC_UNDEFINED
	// for non-scalable or virtual fonts or fonts that are not present
	static FontClass GetFontClass(String_64& FaceName);

	// routines for the font list cache - a cache of enumerated fonts
	// do not confuse this with the kernel's font cache
	static void AddFontToCache(ENUMLOGFONT& EnumLogFont);
	static BOOL FontIsCached(ENUMLOGFONT &EnumLogFont);
	static void UpdateCache();
};

#endif
