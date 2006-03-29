// $Id: fontbase.h 662 2006-03-14 21:31:49Z alex $
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
// header file for FontBase class. All other Font classes inside Camelot should be
// derived from this - it acts as an interface layer to the kernel level font manager.
// Currently derived font classes are;
// TTFont	- The TrueType font manager class
// ATMFont	- The ATM font manager class

/*
*/

#ifndef INC_FONTBASE
#define INC_FONTBASE

#include "pathtype.h"
#include "txtattr.h"
#include "ccpanose.h"

class  DocCoord;
class CharMetrics;

#define OILFONTLIMIT 2048

/********************************************************************************************
	The types of fonts the OIL font manager recognises so far
********************************************************************************************/

typedef enum FontClass {
	FC_UNDEFINED = 0,
	FC_RASTER,
	FC_DEVICE,
	FC_TRUETYPE,
	FC_ATM,
	FC_FREETYPE,

	// Add further font classes before this
	FC_ILLEGAL
};

// The kernel likes dealing with LOGFONT and ENUMLOGFONT structures, so we do it this favour
struct LOGFONT
{
	String_64 FaceName;
};
typedef LOGFONT		   *PLOGFONT, *LPLOGFONT;

struct ENUMLOGFONT
{
	LOGFONT elfLogFont;
};

/********************************************************************************************

>	class FontBase : public CCObject
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95
	Purpose:	A class which all Oil level fonts should be derived from. Its really
				an abstract class but contains some other functionaity too.

********************************************************************************************/

class FontBase : public CCObject
{
	CC_DECLARE_DYNCREATE(FontBase)
	
	public:
		FontBase();
		virtual BOOL Initialise(String_64 *pFontName);
		virtual BOOL Compare(String_64 *pFontName);

		virtual FontClass GetFontClass() { return FC_UNDEFINED; }
		virtual String_64* GetFontName() { return &TheFontName; }

		virtual void Delete();
		virtual void Dump();

	protected:
		String_64 TheFontName;
};

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

	bool CacheFontKerns(wxDC* pDC, CharDescription FontDesc, MILLIPOINT DefaultHeight, INT32 DesignSize);
	MILLIPOINT GetCharsKerning(WCHAR chLeft, WCHAR chRight);
	inline CharDescription GetFontDesc()					{ return FontDesc; }
	inline void SetFontDesc(CharDescription NewFontDesc)	{ FontDesc = NewFontDesc; }

#ifdef _DEBUG
public:
	void Dump();
#endif//_DEBUG

protected:
	CharDescription FontDesc;

protected: // Kerning data
	MillipointKerningPair*	pKernPairs;
	INT32						KernCount;
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
	static void	InvalidateKerningPairsCache();
	static MILLIPOINT GetCharsKerning(wxDC* pDC, WCHAR chLeft, WCHAR chRight,
														CharDescription& FontDesc);

#ifdef _DEBUG
public:
	static void Dump();
#endif//_DEBUG

protected:
	// this seems the only way to define constants in a class assigning values
	enum CacheInfo { NUMENTRIES=3 };

	static FontKerningPairsCacheEntry mpFontKerningPairsCacheData[NUMENTRIES];
};

/********************************************************************************************

>	class OILFontMan : public CCObject
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95
	Purpose:	The base font class controller inside the WinOil. The kernel should make
				calls to this font manager class only. The class acts as a switching layer
				calling other OIL level font managers dependent on the type of font in use.

********************************************************************************************/

class OILFontMan : public CCObject
{
	CC_DECLARE_DYNCREATE(OILFontMan)

	public:
		OILFontMan();

		// Functions each OIL font manager should take a role in.
		static BOOL IsOkToCall(FontClass Class);
		static BOOL CacheNamedFont(String_64* pFont, FontClass Class=FC_UNDEFINED, INT32 Pass=1);
		static void ValidateCache();
		static void FindClosestFont();
		static FontBase* CreateNewFont(FontClass Class, String_64* pFontName);
		static OUTLINETEXTMETRIC *GetOutlineTextMetric(FontClass Class, LOGFONT *pLogFont);

	public:
		// Character cache related functions - other font manglers can use this
		// cache for their own purposes.
		static UINT32 GetOutlineCache(DocCoord** pCoords, PathVerb** pVerbs);
		static void InitialiseOutlineCache();
		static void FinaliseOutlineCache();
		static BOOL AddMoveTo(POINT& p0);
		static BOOL AddLineTo(POINT& p0);
		static BOOL AddBezierTo(POINT& p0, POINT& p1, POINT& p2);
		static BOOL AddMoveTo(DocCoord& p0);
		static BOOL AddLineTo(DocCoord& p0);
		static BOOL AddBezierTo(DocCoord& p0, DocCoord& p1, DocCoord& p2);
		static BOOL ClosePath();

		// character hi resolution metrics stuff.
		static BOOL GetCharPath(FontClass Class,
								CharDescription& ChDesc,
								DocCoord** ppCoords,
								PathVerb** ppVerbs,
								UINT32* pNumCoords,
								wxDC* pDC);

		// checks whether a class is actually known about
		static BOOL LegalFontClass(INT32 Class, FontClass& RealClass);

		// debugging stuff
		#if _DEBUG
		static void DumpEnumLogFont(ENUMLOGFONT* lpelf);
		static void DumpLogFont(LOGFONT* lplf);
		#endif
};


/********************************************************************************************

>	class EnumFonts : public CCObject
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/95
	Purpose:	A class which allows kernel level and tool functions to inherit the ability
				to enum fonts. The class will call each OIL level font manager to enum their
				own specific set of fonts and the results will be returned to the derived
				NewFont() function. 

********************************************************************************************/

class OILEnumFonts : public CCObject
{
	CC_DECLARE_DYNCREATE( OILEnumFonts )

	public:
		OILEnumFonts();
		void Execute();
		virtual BOOL NewFont(FontClass Class, ENUMLOGFONT FAR* lpelf);
};


#endif
