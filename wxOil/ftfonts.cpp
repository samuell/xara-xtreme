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
// implementation file for the FreeType font manager - will compile to an
// empty object file on non-GTK builds

/*
*/

#include "camtypes.h"
DECLARE_SOURCE( "$Revision$" );

// we do not compile any of the functions on non-wxGTK builds
#if defined(__WXGTK__)

#include "ftfonts.h"
#include "fontman.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "textfuns.h"
#include "oilpanse.h"

#include <pango/pango.h>
#include <pango/pangofc-font.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_TRUETYPE_TABLES_H
#include FT_TYPE1_TABLES_H

#ifdef __WXGTK20__
#include <gtk/gtk.h>
extern GtkWidget *wxGetRootWindow();
#endif

CC_IMPLEMENT_DYNCREATE( FTFontMan, CCObject )
CC_IMPLEMENT_DYNCREATE( FTFont, FontBase )

#define new CAM_DEBUG_NEW     

// static class members
FTFontCache* FTFontMan::m_cache = NULL;

// forward declarations
static PangoContext* GetPangoContext();
static BOOL ToASCII(TCHAR* src, char* buffer, UINT32 len);
static BOOL GetPangoFcFontAndFreeTypeFaceForFaceName(String_64* pFaceName,
													 PangoFcFont** ppPangoFcFont, FT_Face* ppFreeTypeFace);

// compare ENUMLOGFONT structures - needed to allow use to use ENUMLOGFONTs as map keys
bool operator<(const ENUMLOGFONT &e1, const ENUMLOGFONT &e2)
{
	return e1.elfLogFont.FaceName.CompareTo(e2.elfLogFont.FaceName, FALSE) < 0;
}

bool operator==(const ENUMLOGFONT &e1, const ENUMLOGFONT &e2)
{
	return e1.elfLogFont.FaceName.CompareTo(e2.elfLogFont.FaceName, FALSE) == 0;
}

/********************************************************************************************

>	FTFont::FTFont()

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	18/05/06
	Purpose:	Constructor

********************************************************************************************/

FTFont::FTFont(FontClass fc)
{
	m_FontClass = fc;
}

/********************************************************************************************

>	FTFont::~FTFont()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95				
	Purpose:	destructor for the FTFont class

********************************************************************************************/

FTFont::~FTFont()
{
}

/********************************************************************************************

>	void FTFont::Dump()

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	18/05/06
	Purpose:	Dump the contents of this cache entry out

********************************************************************************************/

void FTFont::Dump()
{
	FontBase::Dump();
	if (m_FontClass == FC_TRUETYPE)
		TRACE( _T(" FontClass = TrueType\n"));
	else if (m_FontClass == FC_ATM)
		TRACE( _T(" FontClass = ATM\n"));
	else 
		TRACE( _T(" FontClass = <unknown>\n"));
}

/********************************************************************************************

>	FTFontMan::FTFontMan() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95				
	Purpose:	Default constructor

********************************************************************************************/

FTFontMan::FTFontMan()
{
}

/********************************************************************************************

	static BOOL TTFontMan::IsOkToCall()

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/9/95
	Returns:	TRUE if the FreeType manager is actually running and can be used on this OpSys
	Purpose:	Determine whether we can make further calls to the TrueType Manager

********************************************************************************************/

BOOL FTFontMan::IsOkToCall()
{
	// Currently its always true as this is the wxOil
	return TRUE;
}

// debugging routine to conveniently output a String_64 string
static void DumpString64User(char* user, TCHAR* msg, const String_64* pString)
{
	TRACEUSER(user, msg);
	TRACEUSER(user, (const TCHAR*)(*pString));  // use TCHAR* conversion operator
}

/********************************************************************************************

>	static BOOL FTFontMan::CacheFontCore(String_64* pFontName, BOOL compatible)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	06/03/06
	Inputs:		pFontName = a pointer to a fontname to find and cache
				compatible = if TRUE, an inexact match can be tolerated, else we want
							 an exact match
	Outputs:	-
	Returns:	TRUE if the font has been cached
				FALSE if not.
	Purpose:	This function attempts to cache a font supported by FreeType. It caches
				the font in the kernels font list, and hence can be accessed via 
				FONTMANAGER-> calls

********************************************************************************************/

BOOL FTFontMan::CacheFontCore(String_64* pFontName, BOOL compatible)
{
	DumpString64User("wuerthne", _T("FTFontMan::CacheNamedFont"), pFontName);
	TRACEUSER("wuerthne", _T("compatible = %d"), compatible);
	
	PangoContext* pPangoContext = GetPangoContext();
   	PangoFontMap* pFontMap = pango_context_get_font_map(pPangoContext);
	PangoFontDescription* pDescription = pango_font_description_new();

	char ASCIIName[64];   // TYPENOTE: Correct - Pango wants a plain char name
	if (!ToASCII((TCHAR*)*pFontName, ASCIIName, 64)) return FALSE;

	// we ask Pango for a font with the given name
	pango_font_description_set_family_static(pDescription, ASCIIName);
	pango_font_description_set_style(pDescription, PANGO_STYLE_NORMAL);
	pango_font_description_set_weight(pDescription, PANGO_WEIGHT_NORMAL);
    pango_font_description_set_size(pDescription, 12 * PANGO_SCALE);

	PangoFont* pFont = pango_font_map_load_font(pFontMap, pPangoContext,
												pDescription);
	pango_font_description_free(pDescription);

	// Pango will always give us some font, no matter whether the font we asked
	// for is installed or not, so check what we really got
	PangoFontDescription *pRealDesc = pango_font_describe(pFont);

	wxString wxName(pango_font_description_get_family(pRealDesc), wxConvUTF8);
	String_64 OurFontName(wxName);
    DumpString64User("wuerthne", _T("Got font"), &OurFontName);
	TRACEUSER("wuerthne", _T("real font description has style = %d, weight = %d"),
			  pango_font_description_get_style(pRealDesc), pango_font_description_get_weight(pRealDesc));
	pango_font_description_free(pRealDesc);
	
	// NB - Apart from the following "if" this routine only fails in case of a technical error
	//      not because the required font is not present. At the moment, we only check that we
	//      got the right font if compatible == FALSE, but we are always called with
	//      compatible == TRUE afterwards, so effectively, we always find a font, in the worst
	//      case some system default font. That means that the clever Panose matching code is
	//      never used because it is only called if both passes (first with compatible == FALSE
	//      then with compatible == TRUE) fail. Removing the "!compatible" check on the other
	//      hand means that we only allow an exact match and hence cannot make use of fontconfig's
	//      font substitution service, most notably using the user's own font substitution rules.

	// check whether the font name is the same as the one we asked for (case-insensitive)
	if (!compatible && !pFontName->IsEmpty() && OurFontName.CompareTo(*pFontName, FALSE) != 0) {
		// we wanted an exact match, but the names do not match, so do not cache the font
		TRACEUSER("wuerthne", _T("Name did not match, so fail"));
		return FALSE;
	}

	// register the font with the kernel
	TRACEUSER("wuerthne", _T("Register font"));
	Application* pApp = GetApplication();
	FontManager* pFontMan = pApp->GetFontManager();
	ENUMLOGFONT OurEnumLogFont;
	OurEnumLogFont.elfLogFont.FaceName = OurFontName;
	FontClass Class = GetFontClass(OurFontName);
	if (Class == FC_TRUETYPE || Class == FC_ATM)
	{
		pFontMan->SetTempFont(Class, &OurFontName, &OurEnumLogFont);  // kernel copies the ENUMLOGFONT structure
		return TRUE;
	}
	return FALSE;
}

/********************************************************************************************

>	static BOOL FTFontMan::CacheNamedFont(String_64* pFontName)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	06/03/06
	Inputs:		pFontName = a pointer to a fontname to find and cache
	Outputs:	-
	Returns:	TRUE if the font has been cached
				FALSE if not.
	Purpose:	This function attempts to cache a font supported by FreeType. It caches
				the font in the kernels font list, and hence can be accessed via 
				FONTMANAGER-> calls

********************************************************************************************/

BOOL FTFontMan::CacheNamedFont(String_64* pFontName)
{
	TRACEUSER("wuerthne", _T("CacheNamedFont %08x"), pFontName);
	if (pFontName == NULL) {
		// we are called with a NULL pointer from the kernel, so pass an empty string instead,
		// which will probably prompt Pango to return its default font
		TRACEUSER("wuerthne", _T("CacheNamedFont called with NULL ptr, using emtpy string instead"));
		String_64 FontName(_T(""));
		return CacheFontCore(&FontName, FALSE);
	}
	else return CacheFontCore(pFontName, FALSE);
}

/********************************************************************************************

>	static BOOL FTFontMan::CacheCompatibleFont(String_64* pFontName)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	06/03/06
	Inputs:		pFontName = a pointer to a fontname to find and cache
	Outputs:	-
	Returns:	TRUE if the font has been cached
				FALSE if not.
	Purpose:	This function attempts to cache a compatible font

********************************************************************************************/

BOOL FTFontMan::CacheCompatibleFont(String_64* pFontName)
{
	return CacheFontCore(pFontName, TRUE);
}

/********************************************************************************************

>	static void TTFontMan::ValidateCache()

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	06/03/06
	Purpose:	This function will attempt to recache all FreeType fonts within the kernels
				font manager cache.

********************************************************************************************/

void FTFontMan::ValidateCache()
{
	PORTNOTETRACE("text", "FTFontMan::ValidateCache - do nothing");
}

// ****************
// Font enumeration
// ****************

/********************************************************************************************

>	FontClass FTFontMan::GetFontClass(String_64& FaceName)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	18/05/06
	Inputs:		FaceName - the name of the font we want to enquire about
	Purpose:	Checks that the font is installed, is scalable and not a virtual font.
				Reports	FC_TRUETYPE or FC_ATM. Reports FC_UNDEFINED if either of the above
				is not true or if the type could not be established.

********************************************************************************************/

FontClass FTFontMan::GetFontClass(String_64& FaceName)
{
	FontClass fc = FC_UNDEFINED;
	PangoFcFont* pPangoFcFont;
	FT_Face pFreeTypeFace;
	if (GetPangoFcFontAndFreeTypeFaceForFaceName(&FaceName, &pPangoFcFont, &pFreeTypeFace))
	{
		// At this stage, we know that the font is scalable, otherwise the above call
		// would have returned an error already. Now, check whether this is a virtual
		// font (e.g., "Monospace") presented to us by fontconfig. We do that by reading
		// back the family name and comparing it to the one we passed.
		PangoFontDescription *pRealDesc = pango_font_describe((PangoFont*)pPangoFcFont);
		wxString wxName(pango_font_description_get_family(pRealDesc), wxConvUTF8);
		String_64 OurFontName(wxName);
		pango_font_description_free(pRealDesc);
		if (OurFontName.CompareTo(FaceName, FALSE) == 0)
		{
			PS_FontInfoRec PSRec;
			if (FT_Get_PS_Font_Info(pFreeTypeFace, &PSRec) != FT_Err_Invalid_Argument)
				fc = FC_ATM;
			else
			{
				if (FT_IS_SFNT(pFreeTypeFace)) fc = FC_TRUETYPE;
				else
				{
					// scalable, but not PS-based and not SFNT-based, so not TrueType either
					// ignore this for the time being, but we can probably support this type
					// of font
				}
			}
		}
		pango_fc_font_unlock_face(pPangoFcFont);
	}
	return fc;
}

// To enumerate fonts using wxWidgets we need to derive from wxFontEnumerator
// and override the OnFacename method
class ClosestFontEnumerator: public wxFontEnumerator
{
public:
	bool OnFacename(const wxString& font);         // TYPENOTE: Correct - overriding wx method
};

// Our ClosestFontEnumerator calls the fixed callback routine in FontMan to report the font


/********************************************************************************************

>	bool ClosestFontEnumerator::OnFacename(const wxString& font)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	19/04/06
	Inputs:		font - the Facename of an enumerated font
	Purpose:	Callback function for font enumeration - passes call on to the kernel Panose
				Panose matcher

********************************************************************************************/
bool ClosestFontEnumerator::OnFacename(const wxString& font)
{
	// we need to pass a ENUMLOGFONT structure to the kernel
	// we can pass pointers to transient structures - the kernel copies the data if it is the
	// best match so far
	// We can only handle names that have less than 64 characters - see MyFontEnumerator::OnFacename
	if (font.length() > 63) return TRUE;
	String_64 OurFontName = font;
	ENUMLOGFONT OurEnumLogFont;
	OurEnumLogFont.elfLogFont.FaceName = font;
	FontManager* pFontMan = GetApplication()->GetFontManager();
	return pFontMan->FindClosestFontFullTry(FTFontMan::GetFontClass(OurFontName),
											&OurEnumLogFont.elfLogFont.FaceName, &OurEnumLogFont);
}

/********************************************************************************************

>	static void FTFontMan::FindClosestFont()

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	19/04/06
	Purpose:	Enumerates all the fonts reporting each to the Panose matcher in FontMan.

********************************************************************************************/

void FTFontMan::FindClosestFont()
{
	// use wxWidgets to enumerate all font families
	TRACEUSER("wuerthne", _T("FTFonMan::FindClosestFont"));
	ClosestFontEnumerator WxEnumerator;
	WxEnumerator.EnumerateFacenames();	
}

// To enumerate fonts using wxWidgets we need to derive from wxFontEnumerator
// and override the OnFacename method
class MyFontEnumerator: public wxFontEnumerator
{
public:
	enum CacheAction { FillCache, UpdateCache };
	MyFontEnumerator(CacheAction action) { m_action = action; }
	bool OnFacename(const wxString& font);         // TYPENOTE: Correct - overriding wx method
private:
	CacheAction m_action;
};

/********************************************************************************************

>	bool MyFontEnumerator::OnFacename(const wxString& font)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	06/03/06
	Inputs:		font - the Facename of an enumerated font
	Purpose:	Callback function for font enumeration - fills/updates the font list cache

********************************************************************************************/

bool MyFontEnumerator::OnFacename(const wxString& font)
{
	// we need to pass an ENUMLOGFONT structure to the kernel
	// TRACEUSER("wuerthne", _T("OnFacename %s"), (const TCHAR*)font);

	// We can only handle names that have less than 64 characters. Longer
	// names are not sensible but there are silly fonts out there that have
	// names exceeding our limit (even though the one font that highlighted
	// this problem seemed to have its copyright message reported as the name
	// which would look silly on the menu anyway). Truncating the name does
	// not make any sense because we would not be able to handle it later on,
	// so the best we can do is to silently ignore the font.
	if (font.length() > 63) return TRUE;
	String_64 OurFontName = font;
	
	ENUMLOGFONT OurEnumLogFont;
	OurEnumLogFont.elfLogFont.FaceName = OurFontName;

	if (m_action == FillCache)
	{
		// first of all, add the font to our cache list - we do that even with fonts that
		// we cannot use, so we can easily see when the available font set has changed
		FTFontMan::AddFontToCache(OurEnumLogFont);
		// TRACEUSER("wuerthne", _T("%s added to font list cache"), (TCHAR*)OurFontName);
	}
	else if (m_action == UpdateCache)
	{
		// check whether the font is in the cache already
		// if it is, update its Referenced field as a side-effect
		if (!FTFontMan::FontIsCached(OurEnumLogFont))
		{
			// not yet cached, so add it now
			FTFontMan::AddFontToCache(OurEnumLogFont);
			TRACEUSER("wuerthne", _T("%s added to font list cache"), (TCHAR*)OurFontName);
		}
	}
	return TRUE;
}

/********************************************************************************************

>	FTFontMan::FontIsCached(ENUMLOGFONT &EnumLogFont)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	19/05/06
	Inputs:		EnumLogFont - the face name wrapped in a LOGFONT/ENUMLOGFONT structure
	Returns:    TRUE if the font is in the cache
	Purpose:	Check whether the given font is in the font list cache
				If it is, update its Referenced field as a side-effect

********************************************************************************************/

BOOL FTFontMan::FontIsCached(ENUMLOGFONT &EnumLogFont)
{
	FTFontCache::iterator it = m_cache->find(EnumLogFont);
	if (it != m_cache->end())
	{
		(*it).second.m_Referenced = TRUE;
		return TRUE;
	}
	return FALSE;
}

/********************************************************************************************

>	FTFontMan::AddFontToCache(ENUMLOGFONT &EnumLogFont)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	19/05/06
	Inputs:		EnumLogFont - the face name wrapped in a LOGFONT/ENUMLOGFONT structure
	Purpose:	Add the given font to the font list cache

********************************************************************************************/

void FTFontMan::AddFontToCache(ENUMLOGFONT &EnumLogFont)
{
	FontClass OurFontClass = FTFontMan::GetFontClass(EnumLogFont.elfLogFont.FaceName);
	FTFontCacheEntry OurEntry(OurFontClass, TRUE);
	(*m_cache)[EnumLogFont] = OurEntry;
}

/********************************************************************************************

>	FTFontMan::UpdateCache()

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	19/05/06
	Inputs:		-
	Purpose:	Update the font list cache to reflect the wxWidgets font enumeration

********************************************************************************************/

void FTFontMan::UpdateCache()
{
	MyFontEnumerator::CacheAction action;
	if (m_cache)
	{
		// we already have a cache, so just update it incrementally
		action = MyFontEnumerator::UpdateCache;
		// we want to purge entries that are no longer installed, so mark all entries as unreferenced
		// the cache update step will mark all the fonts it finds as Referenced
		for (FTFontCache::iterator it = m_cache->begin(); it != m_cache->end(); ++it)
			(*it).second.m_Referenced = FALSE;
	}
	else
	{
		// initial run
		m_cache = new FTFontCache;
		action = MyFontEnumerator::FillCache;
	}
	MyFontEnumerator FontEnumerator(action);
	FontEnumerator.EnumerateFacenames();

	if (action == MyFontEnumerator::UpdateCache) {
		// delete all fonts that have been deinstalled since we last checked
		for (FTFontCache::iterator it = m_cache->begin(); it != m_cache->end();)
		{
			if (!(*it).second.m_Referenced)
			{
				TRACEUSER("wuerthne", _T("%s removed from font list cache"),
						  (const TCHAR*)((*it).first.elfLogFont.FaceName));
				// NB - it++ first increments the iterator, then returns the iterator
				//      to the current element, which is then deleted - this is safe,
				//      as opposed to first deleting and then incrementing
				m_cache->erase(it++);
			}
			else
			{
				++it;		
			}
		}
	}
}

/********************************************************************************************

>	static void FTFontMan::EnumAllFonts(OILEnumFonts* pOilEnumerator)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	06/03/06
	Inputs:		pOilEnumerator = A pointer to an object to call back.
	Purpose:	Gives the kernel a way of enumerating fonts itself

********************************************************************************************/

void FTFontMan::EnumAllFonts(OILEnumFonts* pOilEnumerator)
{
	// use wxWidgets to enumerate all font families
	TRACEUSER("wuerthne", _T("FTFontMan::EnumAllFonts"));

	// we cache the results so we first check whether the cache can still be used
	UpdateCache();

	// now, simply return the information from the cache
	BOOL go_on = TRUE;
	for (FTFontCache::iterator it = m_cache->begin(); go_on && it != m_cache->end(); ++it)
	{
		FontClass OurFontClass = (*it).second.m_Class;
		if (OurFontClass == FC_TRUETYPE || OurFontClass == FC_ATM)
		{
			ENUMLOGFONT EnumLogFont = (*it).first;
			go_on = pOilEnumerator->NewFont(OurFontClass, &EnumLogFont);
		}
	}
	TRACEUSER("wuerthne", _T("FTFontMan::EnumAllFonts done"));
}

/********************************************************************************************

>	FontBase* FTFontMan::CreateNewFont(String_64* pFontName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pFontName = a pointer to a fontname
	Returns:	NULL if no FreeType font structure has been created
				A pointer to a FreeType font structure if successfull.
	Purpose:	This function attempts to create a font instance and will be called by
				the font manager when new fonts are added.

********************************************************************************************/

FTFont* FTFontMan::CreateNewFont(String_64* pFontName, FontClass Class)
{
	DumpString64User("wuerthne", _T("FTFontMan::CreateNewFont"), pFontName);
	FTFont* pFont = new FTFont(Class);
	if (pFont && !pFont->Initialise(pFontName))
	{
		delete pFont;
		return NULL;
	}
	return pFont;
}

/********************************************************************************************

>	static String_64* GetFacenameFromCharDesc(CharDescription &ChDesc)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	06/03/06
	Inputs:		ChDesc - a Kernel character description
	Outputs:	-
	Returns:	The Facename of the referenced font
				NULL if the found could not be found (e.g., not cached - should not happen)
	Purpose:	Extract the name of the font referenced by a Kernel character description

********************************************************************************************/

static String_64* GetFacenameFromCharDesc(CharDescription &ChDesc)
{
	// get LogFont from face handle in char descriptor
	WORD            FaceHandle    = ChDesc.GetTypefaceHandle();
	CachedFontItem* pFontDataItem = FONTMANAGER->GetFont(FaceHandle);
	ERROR2IF(pFontDataItem==NULL,FALSE,"FTFontMan::GetCharOutline could not find cached font");
  	ENUMLOGFONT*    pEnumLogFont  = pFontDataItem->GetEnumLogFont();
	ERROR2IF(pEnumLogFont==NULL,FALSE,"FTFontMan::GetCharOutline could not find cached EnumLogFont");
	DumpString64User("wuerthne", _T("GetFacenameFromCharDesc returning"), &pEnumLogFont->elfLogFont.FaceName);
	return &pEnumLogFont->elfLogFont.FaceName;
}

// convert a possibly unicoded ASCII string into a plain ASCII string

/********************************************************************************************

>	static BOOL ToASCII(TCHAR* src, char* buffer, UINT32 len)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	06/03/06
	Inputs:		src - a pointer to a TCHAR* string
	Outputs:	The converted string in the char* buffer
	Returns:	TRUE if the string was converted OK
				FALSE if not (e.g., buffer overflow, invalid character found)
	Purpose:	Convert a possibly unicoded ASCII string into a plain ASCII string

********************************************************************************************/

static BOOL ToASCII(TCHAR* src, char* buffer, UINT32 len)    // TYPENOTE: correct - we need to use char
{
	char* dest = buffer;
	while(*src >= 32 && len)
	{
		TCHAR b = *src++;
		if (b > 127)
		{
			// outside ASCII range, so fail
			*dest = '\0';
			return FALSE;
		}
		*dest++ = b;
        len--;
	}
    if (!len)
	{
		// buffer overflow, so fail
		dest[-1] = '\0';
		return FALSE;
	}
	else {
		*dest = '\0';
		return TRUE;
	}
}

/********************************************************************************************

>	static PangoFont* GetPangoFontForFaceName(String_64* pFaceName)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	19/04/06
	Inputs:		pFaceName - pointer to a face name
	Outputs:	-
	Returns:	a pointer to the corresponding PangoFont structure
				may return NULL if the font could not be loaded
				the Pango documentation does not state anything about how to or having to
				free the PangoFont object, so we assume it is managed by the FontMap
	Purpose:	return a PangoFont for a particular font face

********************************************************************************************/

static PangoFont* GetPangoFontForFaceName(String_64* pFaceName, BOOL IsBold, BOOL IsItalic)
{
	// DumpString64User("wuerthne", _T("FTFontMan::GetPangoFontForFaceName"), pFaceName);

	char ASCIIFaceName[64]; // TYPENOTE: correct (needed as parameter to Pango)
	if (!ToASCII(*pFaceName, ASCIIFaceName, 64)) return NULL;

	PangoContext* pPangoContext = GetPangoContext();
	ERROR2IF(pPangoContext==NULL,NULL,"FTFontMan::GetPangoFontForCharDesc failed to get PangoContext");
   	PangoFontMap* pFontMap = pango_context_get_font_map(pPangoContext);
	ERROR2IF(pPangoContext==NULL,NULL,"FTFontMan::GetPangoFontForCharDesc failed to get PangoFontMan");

	PangoFontDescription* pDescription = pango_font_description_new();
	pango_font_description_set_family_static(pDescription, ASCIIFaceName);
	pango_font_description_set_style(pDescription, IsItalic ? PANGO_STYLE_ITALIC :  PANGO_STYLE_NORMAL);
	pango_font_description_set_weight(pDescription, IsBold ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL);
    pango_font_description_set_size(pDescription, 12 * PANGO_SCALE);  // arbitrary, we get unscaled outlines later

	PangoFont* pFont = pango_font_map_load_font(pFontMap, pPangoContext, pDescription);
	pango_font_description_free(pDescription);
	if (!PANGO_IS_FONT(pFont)) return NULL;
	return pFont;
}

/********************************************************************************************

>	static PangoFont* GetPangoFontForCharDesc(CharDescription &ChDesc)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	06/03/06
	Inputs:		ChDesc - a Kernel character description
	Outputs:	-
	Returns:	a pointer to the corresponding PangoFont structure
				may return NULL if the font could not be loaded
				the Pango documentation does not state anything about how to or having to
				free the PangoFont object, so we assume it is managed by the FontMap
	Purpose:	return a PangoFont for the font referenced by ChDesc

********************************************************************************************/

static PangoFont* GetPangoFontForCharDesc(CharDescription &ChDesc)
{
	String_64* pFaceName = GetFacenameFromCharDesc(ChDesc);
	return GetPangoFontForFaceName(pFaceName, ChDesc.GetBold(), ChDesc.GetItalic());
}

// Get the application's Pango context - in wxGTK we already have one since wxGTK
// itself uses Pango.

/********************************************************************************************

>	static PangoContext* GetPangoContext()

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	06/03/06
	Inputs:		-
	Outputs:	-
	Returns:	the PangoContext of the application
	Purpose:	Get access to the application's PangoContext - as wxWidgets does it

********************************************************************************************/

static PangoContext* GetPangoContext()
{
	// The magic code to get the Pango context for wxGTK2 and below is copied
	// from wxWidgets/unix/fontenum.cpp
	PangoContext *pPangoContext =
#ifdef __WXGTK20__
		gtk_widget_get_pango_context( wxGetRootWindow() );
#elif defined(__WXGTK__)
		wxTheApp->GetPangoContext();
#else
		#error "Sorry, this source file can only be compiled for wxGTK"
#endif
		return pPangoContext;
}

/********************************************************************************************

>	static INT32 ScaleToDefaultHeight(INT32 coord, INT32 DesignSize)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	06/03/06
	Inputs:		coord - the coordinate to scale
				DesignSize - the design size of the font
	Outputs:	-
	Returns:	the scaled size
	Purpose:	Scale a coordinate according to the font's design size

********************************************************************************************/

static INT32 ScaleToDefaultHeight(INT32 coord, INT32 DesignSize)
{
	return coord * TextManager::GetDefaultHeight() / DesignSize;
}

/********************************************************************************************

>	static BOOL GetPangoFcFontAndFreeTypeFaceForPangoFont(PangoFont* pFont,
													 PangoFcFont** ppPangoFcFont, FT_Face* ppFreeTypeFace)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	19/04/06
	Inputs:		pFont - pointer to a PangoFont
	Outputs:	a pointer to the PangoFcFont is stored in ppPangoFcFont
				a pointer to the FreeType font is stored in ppFreeTypeFace
				When finished with the font and face, the client has to call
				pango_fc_font_unlock_face(*ppPangoFcFont) to free the font
	Returns:	TRUE if the font descriptors have been returned correctly
				FALSE if not.
	Purpose:	Get a PangoFc font and a FreeType face for a Pango font

********************************************************************************************/

static BOOL GetPangoFcFontAndFreeTypeFaceForPangoFont(PangoFont* pFont, PangoFcFont** ppPangoFcFont, FT_Face* ppFreeTypeFace)
{
	if (!pFont) return FALSE;

	// We need to get at the underlying FreeType information, which only works for
	// Pango backends that are based on FreeType. The common factor for those
	// backends is that they are based on FontConfig, so we check whether the font
	// we got is indeed managed by a FontConfig-based backend (Xft or Cairo).
	if (!PANGO_IS_FC_FONT(pFont))
	{
		ERROR2(FALSE, "FTFontMan: font is not a PangoFcFont!");
	}

	// OK, so we can safely cast to PangoFcFont
	PangoFcFont* pFcFont = (PangoFcFont*)pFont;

	// The magic call to get at the underlying FreeType data
	// We must unlock this before returning!
	FT_Face pFreeTypeFace = pango_fc_font_lock_face(pFcFont);
	if (!pFreeTypeFace) {
		pango_fc_font_unlock_face(pFcFont);
		return FALSE;
	}

	// The default charmap is always unicode if present, but we may
	// have a symbol font which we may want to drive in symbol mode.
	// Check whether there is just an Adobe custom or MS Symbol encoding
	// in addition to the synthesized unicode charmap and if so, use that
	// instead.
	FT_CharMap pCustomCharmap = NULL;
	for (INT32 mapnum = 0; mapnum < pFreeTypeFace->num_charmaps; mapnum++)
	{
		FT_CharMap pThisMap = pFreeTypeFace->charmaps[mapnum];
		if (pThisMap->encoding == FT_ENCODING_ADOBE_CUSTOM
			|| pThisMap->encoding == FT_ENCODING_MS_SYMBOL) {
			pCustomCharmap = pThisMap;
			// we go on checking the other encodings
		}
		else if (pThisMap->encoding != FT_ENCODING_UNICODE) {
			// there is an encoding that is neither a custom one
			// nor a Unicode encoding, so this is a language font
			pCustomCharmap = NULL;
			break;
		}
	}
	if (pCustomCharmap) FT_Set_Charmap(pFreeTypeFace, pCustomCharmap);
	
	// We do not support non-scalable fonts - do not report an error, this routine
	// is called during font enumeration and simply returns FALSE in this case
	if (!FT_IS_SCALABLE(pFreeTypeFace))
	{
		pango_fc_font_unlock_face(pFcFont);
		return FALSE;
	}
	*ppPangoFcFont = pFcFont;
	*ppFreeTypeFace = pFreeTypeFace;
	return TRUE;
}

/********************************************************************************************

>	static BOOL GetPangoFcFontAndFreeTypeFaceForCharDesc(CharDescription& ChDesc,
													 PangoFcFont** ppPangoFcFont, FT_Face* ppFreeTypeFace)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	06/03/06
	Inputs:		ChDesc - a Kernel character description
	Outputs:	a pointer to the PangoFcFont is stored in ppPangoFcFont
				a pointer to the FreeType font is stored in ppFreeTypeFace
				When finished with the font and face, the client has to call
				pango_fc_font_unlock_face(*ppPangoFcFont) to free the font
	Returns:	TRUE if the font descriptors have been returned correctly
				FALSE if not.
	Purpose:	Get a PangoFc font and a FreeType face for a char description

********************************************************************************************/

static BOOL GetPangoFcFontAndFreeTypeFaceForCharDesc(CharDescription& ChDesc,
													 PangoFcFont** ppPangoFcFont, FT_Face* ppFreeTypeFace)
{
	// We extract the font description from ChDesc and find a PangoFont for it
	PangoFont* pFont = GetPangoFontForCharDesc(ChDesc);
    return GetPangoFcFontAndFreeTypeFaceForPangoFont(pFont, ppPangoFcFont, ppFreeTypeFace);
}

/********************************************************************************************

>	static BOOL GetPangoFcFontAndFreeTypeFaceForFaceName(String_64* pFaceName,
													 PangoFcFont** ppPangoFcFont, FT_Face* ppFreeTypeFace)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	19/04/06
	Inputs:		pFaceName - pointer to a face name
	Outputs:	a pointer to the PangoFcFont is stored in ppPangoFcFont
				a pointer to the FreeType font is stored in ppFreeTypeFace
				When finished with the font and face, the client has to call
				pango_fc_font_unlock_face(*ppPangoFcFont) to free the font
	Returns:	TRUE if the font descriptors have been returned correctly
				FALSE if not.
	Purpose:	Get a PangoFc font and a FreeType face for a particular font face

********************************************************************************************/

static BOOL GetPangoFcFontAndFreeTypeFaceForFaceName(String_64* pFaceName,
													 PangoFcFont** ppPangoFcFont, FT_Face* ppFreeTypeFace)
{
	PangoFont* pFont = GetPangoFontForFaceName(pFaceName, FALSE, FALSE);
    return GetPangoFcFontAndFreeTypeFaceForPangoFont(pFont, ppPangoFcFont, ppFreeTypeFace);
}

// *********************************
// Text Character Outline Extraction
// *********************************

// the state we have to keep during outline decomposition
typedef struct OutlineDecompositionState {
	BOOL IsFirstMove;        // TRUE if we have not generated a Move command yet
	INT32 DesignSize;        // the DesignSize of the font (for scaling)
	FT_Vector CurrentPoint;  // we need to keep track of the current point for quadratic curve conversion
} DecompState;

// callback functions for FreeType outline decomposition - we need to use return type "int"   // TYPENOTE: Correct
// rather than any of our types to conform to the FreeType interface

// Unfortunately, the FreeType interface was changed for version 2.2 - the FT_Vector parameters
// are now const.
#if FREETYPE_MAJOR >= 2
#if FREETYPE_MINOR >= 2
/* new interface, make parameters const */
#define FREETYPE_CALLBACK_CONST const
#else
#define FREETYPE_CALLBACK_CONST
#endif
#else
#error "XaraLX requires FreeType 2"
#endif

/********************************************************************************************

>	static int AddMoveTo(FREETYPE_CALLBACK_CONST FT_Vector* to, void* user)  // TYPENOTE: Correct

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	28/03/06
	Inputs:		to - target point of move
				user - pointer to our decomposition state
	Outputs:	-
	Returns:	0 for success, non-0 for failure
	Purpose:	Callback to emit a move command

********************************************************************************************/

static int AddMoveTo(FREETYPE_CALLBACK_CONST FT_Vector* to, void* user)  // TYPENOTE: Correct - FreeType callback interface
{
	DecompState* state = (DecompState*)user;
	if (!state->IsFirstMove) {
		// this is the beginning of a subsequent contour (subpath), so close the current one
		// TRACEUSER("wuerthne", _T("calling ClosePath()"));
		OILFontMan::ClosePath();
	}
	POINT p;
	p.x = ScaleToDefaultHeight(to->x, state->DesignSize);
	p.y = ScaleToDefaultHeight(to->y, state->DesignSize);
	// TRACEUSER("wuerthne", _T("calling MoveTo(%d,%d)"), p.x, p.y);
	OILFontMan::AddMoveTo(p);
	state->CurrentPoint = *to;
	state->IsFirstMove = FALSE;
	return 0;
}


/********************************************************************************************

>	static int AddLineTo(FREETYPE_CALLBACK_CONST FT_Vector* to, void* user)   // TYPENOTE: Correct

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	28/03/06
	Inputs:		to - endpoint of line
				user - pointer to our decomposition state
	Outputs:	-
	Returns:	0 for success, non-0 for failure
	Purpose:	Callback to emit a line command

********************************************************************************************/

static int AddLineTo(FREETYPE_CALLBACK_CONST FT_Vector* to, void* user)       // TYPENOTE: Correct - FreeType callback interface
{
	DecompState* state = (DecompState*)user;
	POINT p;
	p.x = ScaleToDefaultHeight(to->x, state->DesignSize);
	p.y = ScaleToDefaultHeight(to->y, state->DesignSize);
	// TRACEUSER("wuerthne", _T("calling LineTo(%d,%d)"), p.x, p.y);
	OILFontMan::AddLineTo(p);
	state->CurrentPoint = *to;
	return 0;
}

/********************************************************************************************

>	static int AddConicTo(FREETYPE_CALLBACK_CONST FT_Vector* control,         // TYPENOTE: Correct
						  FREETYPE_CALLBACK_CONST FT_Vector* to, void* user)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	28/03/06
	Inputs:		control - control point of quadratic Bezier curve
				to - endpoint of curve
				user - pointer to our decomposition state
	Outputs:	-
	Returns:	0 for success, non-0 for failure
	Purpose:	Callback to deal with a quadratic Bezier curve

********************************************************************************************/

static int AddConicTo(FREETYPE_CALLBACK_CONST FT_Vector* control,             // TYPENOTE: Correct - FreeType callback interface
					  FREETYPE_CALLBACK_CONST FT_Vector* to, void* user)
{
	DecompState* state = (DecompState*)user;

	// OK, we got a quadratic curve but we can only handle cubic ones, so convert
	// from quadratic to cubic - fortunately, this is lossless. The following
	// algorithm (with a major error) was found in an article in comp.lang.postscript
	// and, corrected, reads as follows:
	//
	// Let (xb, yb) and (xe, ye) be the beginning and ending coordinates of
	// the Bezier curve, respectively.  Let (xq1, yq1) be the interior control
	// point for the quadratric Bezier curve. Our task is to determine
	// (xc1, yc1) and (xc2, yc), the interior control points of the cubic
	// Bezier segment which matches the quadratic segment.
	// 
	// Solving the algebra led to this conversion:
    //   xc1 = (xb + 2 * xq1) / 3        yc1 = (yb + 2 * yq1) / 3
	//   xc2 = (2 * xq1 + xe) / 3        yc2 = (2 * yq1 + ye) / 3

	POINT p1;
	POINT p2;
	POINT p3;
	p3.x = ScaleToDefaultHeight(to->x, state->DesignSize);
	p3.y = ScaleToDefaultHeight(to->y, state->DesignSize);

	// we need the starting point, which we can find in the current state
	p1.x = ScaleToDefaultHeight((state->CurrentPoint.x + 2 * control->x) / 3, state->DesignSize);
	p1.y = ScaleToDefaultHeight((state->CurrentPoint.y + 2 * control->y) / 3, state->DesignSize);

	p2.x = ScaleToDefaultHeight((2 * control->x + to->x) / 3, state->DesignSize);
	p2.y = ScaleToDefaultHeight((2 * control->y + to->y) / 3, state->DesignSize);
	// TRACEUSER("wuerthne", _T("q calling BezierTo(%d,%d)(%d,%d)(%d,%d)"), p1.x, p1.y, p2.x, p2.y, p3.x ,p3.y);
	OILFontMan::AddBezierTo(p1, p2, p3);
	state->CurrentPoint = *to;
	return 0;
}

/********************************************************************************************

>	static int AddCubicTo(FREETYPE_CALLBACK_CONST FT_Vector* control1,     // TYPENOTE: Correct
						  FREETYPE_CALLBACK_CONST FT_Vector* control2,
						  FREETYPE_CALLBACK_CONST FT_Vector* to, void* user)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	28/03/06
	Inputs:		control1, control2 - control points of cubic Bezier curve
				to - endpoint of curve
				user - pointer to our decomposition state
	Outputs:	-
	Returns:	0 for success, non-0 for failure
	Purpose:	Callback to deal with a cubic Bezier curve

********************************************************************************************/

static int AddCubicTo(FREETYPE_CALLBACK_CONST FT_Vector *control1,          // TYPENOTE: Correct
					  FREETYPE_CALLBACK_CONST FT_Vector *control2,
					  FREETYPE_CALLBACK_CONST FT_Vector* to, void* user)
{
	DecompState* state = (DecompState*)user;
	POINT p1;
	POINT p2;
	POINT p3;
	p1.x = ScaleToDefaultHeight(control1->x, state->DesignSize);
	p1.y = ScaleToDefaultHeight(control1->y, state->DesignSize);
	p2.x = ScaleToDefaultHeight(control2->x, state->DesignSize);
	p2.y = ScaleToDefaultHeight(control2->y, state->DesignSize);
	p3.x = ScaleToDefaultHeight(to->x, state->DesignSize);
	p3.y = ScaleToDefaultHeight(to->y, state->DesignSize);
	// TRACEUSER("wuerthne", _T("c calling BezierTo(%d,%d)(%d,%d)(%d,%d)"), p1.x, p1.y, p2.x, p2.y, p3.x ,p3.y);
	OILFontMan::AddBezierTo(p1, p2, p3);
	state->CurrentPoint = *to;
	return 0;
}

/********************************************************************************************

>	BOOL FTFontMan::GetCharOutline(CharDescription& ChDesc, 
								   DocCoord** ppCoords,
								   PathVerb** ppVerbs,
								   UINT32* pNumCoords,
								   wxDC* pDC)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	06/03/06
	Inputs:		ChDesc	= description of char
				pDC		= (possible) pointer to a DC (for optimisation) defaults to NULL
	Outputs:	ppCoords	= pointer to path coords buffer
				ppVerbs		= pointer to path verbs buffer
				pNumCoords	= number of elements in path
	Returns:	FALSE if unable to generate the character outline
	Purpose:	Get the path associated with a given character

********************************************************************************************/

BOOL FTFontMan::GetCharOutline(CharDescription& ChDesc, 
							   DocCoord** ppCoords,
							   PathVerb** ppVerbs,
							   UINT32* pNumCoords,
							   wxDC* pDC)
{
	TRACEUSER("wuerthne", _T("FTFontMan::GetCharOutline %04x"), ChDesc.GetCharCode());
	// Check some input parameters
	ERROR2IF(ppCoords==NULL,FALSE,"FTFontMan::GetCharOutline ppCoords==NULL");
	ERROR2IF(ppVerbs==NULL,FALSE,"FTFontMan::GetCharOutline ppVerbs==NULL");
	ERROR2IF(pNumCoords==NULL,FALSE,"FTFontMan::GetCharOutline pNumCoords==NULL");

    PangoFcFont* pPangoFcFont;
	FT_Face pFreeTypeFace;
	if (!GetPangoFcFontAndFreeTypeFaceForCharDesc(ChDesc, &pPangoFcFont, &pFreeTypeFace)) return FALSE;

	// get the design size
	INT32 DesignSize = pFreeTypeFace->units_per_EM;
	// TRACEUSER("wuerthne", _T("DesignSize = %d"), DesignSize);

	// load the glyph data for our character into the font's glyph slot
	if (FT_Load_Char(pFreeTypeFace, ChDesc.GetCharCode(), FT_LOAD_NO_SCALE))
	{
		pango_fc_font_unlock_face(pPangoFcFont);
		ERROR2(FALSE, "FTFontMan::GetCharOutline - could not load glyph");
	}

	FT_GlyphSlotRec *pGlyph = pFreeTypeFace->glyph;

	TRACEUSER("wuerthne", _T("found outline, %d contours, %d points"),
			  pGlyph->outline.n_contours, pGlyph->outline.n_points);
	// finally, we have the glyph we want, so transfer it to the outlines cache
	OILFontMan::InitialiseOutlineCache();

	FT_Outline_Funcs funcs = { AddMoveTo, AddLineTo, AddConicTo, AddCubicTo, 0, 0 };
	DecompState state;
	state.DesignSize = DesignSize;
	state.IsFirstMove = TRUE;

	// TRACEUSER("wuerthne", _T("calling Outline_Decompose"));
	if (FT_Outline_Decompose(&pGlyph->outline, &funcs, &state))
	{
		OILFontMan::FinaliseOutlineCache();
		pango_fc_font_unlock_face(pPangoFcFont);
		return FALSE;
	}
	// TRACEUSER("wuerthne", _T("Outline_Decompose success"));

	// close the last contour if there was any output at all
	if (!state.IsFirstMove)
	{
		OILFontMan::ClosePath();
	}
	OILFontMan::FinaliseOutlineCache();
	pango_fc_font_unlock_face(pPangoFcFont);
	(*pNumCoords) = OILFontMan::GetOutlineCache(ppCoords,ppVerbs);
	TRACEUSER("wuerthne", _T("FTFontMan::GetCharOutline Returning %d coords"), *pNumCoords);
	return TRUE;
}

// *******************
// Metrics and kerning
// *******************

/********************************************************************************************

>	OUTLINETEXTMETRIC* FTFontMan::GetOutlineTextMetric(LOGFONT *pLogFont)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	19/04/06
	Inputs:		pLogFont  = font descriptor
	Returns:	an OUTLINETEXTMETRIC structure or NULL if the information cannot be obtained
                (Panose information is only available for TrueType fonts, not for Type-1 fonts)
                The caller is responsible for freeing the structure
	Purpose:	get the Panose structure for a font

********************************************************************************************/

OUTLINETEXTMETRIC* FTFontMan::GetOutlineTextMetric(LOGFONT *pLogFont)
{
	String_64* pFaceName = &pLogFont->FaceName;
	PangoFcFont* pPangoFcFont;
	FT_Face pFreeTypeFace;
	DumpString64User("wuerthne", _T("FTFontMan::GetOutlineTextMetric"), pFaceName);

	// first of all, retrieve the underlying font information
	if (!GetPangoFcFontAndFreeTypeFaceForFaceName(pFaceName, &pPangoFcFont, &pFreeTypeFace)) return NULL;
	// we have successfully retrieved the FreeType information (we need to release it below!)

	// ask FreeType for the Panose information - this is found in the OS/2 font table,
	// so check whether this font has a TrueType OS/2 font table
	TT_OS2* pOS2_Table;
	if ((pOS2_Table = (TT_OS2*)FT_Get_Sfnt_Table(pFreeTypeFace, ft_sfnt_os2)) == NULL   /* error loading */
		|| pOS2_Table->version == 0xffff /* Mac font without OS/2 table */)
	{
		// we could not get the table with the Panose information, either because there
		// was an error when trying to load it or because the font is not a TrueType font,
		// or because it is an old Mac TrueType font without Panose information
		pango_fc_font_unlock_face(pPangoFcFont);
		return NULL;
	}

	// we have got the OS/2 table - it is owned by the face object so we need not free it,
    // it will disappear when we unlock the font face below

	// We need a permanent structure - the pointer may be cached. Otherwise, the kernel
	// will free the structure.
	OUTLINETEXTMETRIC* pMetric = new OUTLINETEXTMETRIC;
	pMetric->otmPanoseNumber = *((struct PANOSE*)&pOS2_Table->panose);
	TRACEUSER("wuerthne", _T("returning valid OUTLINEFONTMETRIC structure"));

	pango_fc_font_unlock_face(pPangoFcFont);
	return pMetric;
}

/********************************************************************************************

>	BOOL FTFontMan::GetAscentDescent(CharDescription& ChDesc, INT32* pAscent, INT32* pDescent)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	09/03/06
	Inputs:		chDesc - a Kernel character description (to identify the font)
	Outputs:	font ascent and descent in pAscent and pDescent
	Returns:	TRUE if the values could be retrieved
				FALSE if not.
	Purpose:	This function returns the ascent and descent of a given font scaled
				to DefaultHeight

********************************************************************************************/

BOOL FTFontMan::GetAscentDescent(CharDescription& ChDesc, INT32* pAscent, INT32* pDescent)
{
    PangoFcFont* pPangoFcFont;
	FT_Face pFreeTypeFace;
	TRACEUSER("wuerthne", _T("GetAscentDescent"));
	if (!GetPangoFcFontAndFreeTypeFaceForCharDesc(ChDesc, &pPangoFcFont, &pFreeTypeFace)) return FALSE;

	// get the design size
	INT32 DesignSize = pFreeTypeFace->units_per_EM;
	*pAscent = ScaleToDefaultHeight(pFreeTypeFace->ascender, DesignSize);
	*pDescent = ScaleToDefaultHeight(-pFreeTypeFace->descender, DesignSize);
	TRACEUSER("wuerthne", _T("returning ascent = %d, descent = %d"), *pAscent, *pDescent);
	pango_fc_font_unlock_face(pPangoFcFont);
	return TRUE;
}

/********************************************************************************************

>	BOOL FTFontMan::GetCharWidth(CharDescription& ChDesc, TCHAR FirstChar, TCHAR LastChar,
							 INT32* pCharWidthsBuf)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	09/03/06
	Inputs:		chDesc - a Kernel character description (to identify the font)
				FirstChar - first character to get the width for
				LastChar - last character
	Outputs:	character widths into pCharWidthsBuf
	Returns:	TRUE if the widths could be retrieved
				FALSE if not.
	Purpose:	This function returns the widths of a range of characters scaled
				to DefaultHeight

********************************************************************************************/

BOOL FTFontMan::GetCharWidth(CharDescription& ChDesc, TCHAR FirstChar, TCHAR LastChar,
							 INT32* pCharWidthsBuf)
{
	TRACEUSER("wuerthne", _T("FTFontMan::GetCharWidth first=%04x last=%04x"), FirstChar, LastChar);
	UINT32 NumChars = LastChar - FirstChar + 1;
    PangoFcFont* pPangoFcFont;
	FT_Face pFreeTypeFace;
	if (!GetPangoFcFontAndFreeTypeFaceForCharDesc(ChDesc, &pPangoFcFont, &pFreeTypeFace)) return FALSE;

	// get the design size
	INT32 DesignSize = pFreeTypeFace->units_per_EM;
	// TRACEUSER("wuerthne", _T("GetCharWidth, DesignSize = %d"), DesignSize);

	for (UINT32 i = 0; i < NumChars; i++) {
		// load the glyph data for our character into the font's glyph slot
		if (FT_Load_Char(pFreeTypeFace, FirstChar + i, FT_LOAD_NO_SCALE | FT_LOAD_LINEAR_DESIGN))
		{
			pango_fc_font_unlock_face(pPangoFcFont);
			ERROR2(FALSE, "FTFontMan::GetCharWidth - could not load glyph");
		}
		FT_GlyphSlotRec *pGlyph = pFreeTypeFace->glyph;
		pCharWidthsBuf[i] = ScaleToDefaultHeight(pGlyph->linearHoriAdvance, DesignSize);
	}
	pango_fc_font_unlock_face(pPangoFcFont);
	return TRUE;
}

/********************************************************************************************

>	BOOL FTFontMan::GetCharsKerning(CharDescription& FontDesc, TCHAR LeftChar, TCHAR RightChar)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	19/04/06
	Inputs:		FontDesc - a Kernel character description (to identify the font)
				LeftChar, RightChar - left and right character
	Outputs:    -
	Returns:	the kerning to be applied between LeftChar and RightChar
	Purpose:	This function returns the kerning between two glyphs scaled to DefaultHeight

********************************************************************************************/

INT32 FTFontMan::GetCharsKerning(CharDescription& FontDesc, TCHAR LeftChar, TCHAR RightChar)
{
    PangoFcFont* pPangoFcFont;
	FT_Face pFreeTypeFace;

	TRACEUSER("wuerthne", _T("GetCharsKerning, %04x %04x"), LeftChar, RightChar);
	if (!GetPangoFcFontAndFreeTypeFaceForCharDesc(FontDesc, &pPangoFcFont, &pFreeTypeFace)) return 0;

	// get the design size
	INT32 DesignSize = pFreeTypeFace->units_per_EM;

	FT_ULong LeftIndex = FT_Get_Char_Index(pFreeTypeFace, LeftChar);
	FT_ULong RightIndex = FT_Get_Char_Index(pFreeTypeFace, RightChar);;

	FT_Vector kerning;
	FT_Error FreeTypeError;
	if ((FreeTypeError = FT_Get_Kerning(pFreeTypeFace, LeftIndex, RightIndex, FT_KERNING_UNSCALED, &kerning)) != 0)
	{
		TRACEUSER("wuerthne", _T("could not get kerning, error = %d"), FreeTypeError);
		return 0;
	}

	TRACEUSER("wuerthne", _T("Got kerning: %d"), kerning.x);
	pango_fc_font_unlock_face(pPangoFcFont);
	return ScaleToDefaultHeight(kerning.x, DesignSize);
}

#endif // __WXGTK__
