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
// implementation file for 'OILfontman' font manager which supports all
// other forms of derived font managers (currently ATM and TrueType)

/*
*/

#include "camtypes.h"
#ifdef __WXGTK__
#include "ftfonts.h"
#endif
#include "textfuns.h"
#include "fontman.h"		// includes fontbase.h
#include "errors.h"
#include "doccoord.h"
#include "pathtype.h"
#include "txtattr.h"
#include "app.h"

#include "fontimpl.h"       // private definitions
//#include "mike.h"

DECLARE_SOURCE( "$Revision$" );

CC_IMPLEMENT_DYNCREATE( FontBase, CCObject )
CC_IMPLEMENT_DYNCREATE( OILFontMan, CCObject )
CC_IMPLEMENT_DYNCREATE( OILEnumFonts, CCObject )
CC_IMPLEMENT_MEMDUMP( FontMetricsCacheEntry, CC_CLASS_MEMDUMP );
CC_IMPLEMENT_MEMDUMP( FontMetricsCache, CC_CLASS_MEMDUMP );
CC_IMPLEMENT_MEMDUMP( FontKerningPairsCacheEntry, CC_CLASS_MEMDUMP );
CC_IMPLEMENT_MEMDUMP( FontKerningPairsCache, CC_CLASS_MEMDUMP );
CC_IMPLEMENT_MEMDUMP( CharOutlineCache, CC_CLASS_MEMDUMP );

#define new CAM_DEBUG_NEW

// a macro to ignore a parameter without causing a warning
#define IGNOREPARAM(x) x = x

/////////////////////////////////////////
// Some outline cache constants
UINT32 CharOutlineCache::CacheSize = 0;
DocCoord CharOutlineCache::LastMoveTo;
DocCoord CharOutlineCache::CacheCoords[OILFONTLIMIT];
PathVerb CharOutlineCache::CacheVerbs[OILFONTLIMIT];

// define some font metrics/kerning pairs cache statics
FontMetricsCacheEntry		FontMetricsCache::mpFontMetricsData[FontMetricsCache::NUMENTRIES];
FontKerningPairsCacheEntry	FontKerningPairsCache::m_FontKerningPairsCacheData[FontKerningPairsCache::NUMENTRIES];


/********************************************************************************************

>	OILFontMan::OILFontMan() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95				
	Purpose:	Default constructor

********************************************************************************************/

OILFontMan::OILFontMan()
{
}

/********************************************************************************************

>	BOOL OILFontMan::LegalFontClass(INT32 Class, FontClass& RealClass)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95				
	Inputs:		A INT32 describing the font class
	Outputs:	RealClass - holds the real FontClass typedef of the numeric class
	Returns:	TRUE if the font class is supported, FALSE if not
	Purpose:	Check whether a numeric value actually indicates a valid class of font
				ie lies in the range of supported font types. This is used generally for
				loading purpose where the font class is loaded as a numeric value.
				The class given will be converted to RealClass on exit.

********************************************************************************************/

BOOL OILFontMan::LegalFontClass(INT32 Class, FontClass& RealClass)
{
	BOOL ok = ((Class>((INT32)FC_UNDEFINED)) && (Class<((INT32)FC_ILLEGAL)));
	(ok) ? (RealClass=(FontClass)(Class)) : (RealClass=FC_UNDEFINED);
	return ok;
}



/********************************************************************************************

>	BOOL OILFontMan::IsOkToCall(FontClass Class)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95				
				Class  : The class of font
	Outputs:	TRUE if the font class is ok to make further calls to
				FALSE if not.
	Purpose:	Check whether this particular font class manager is up and running.
	
********************************************************************************************/

BOOL OILFontMan::IsOkToCall(FontClass Class)
{
	switch (Class)
	{
		case FC_ATM:
PORTNOTE("text","ATM never OK")
#ifndef EXCLUDE_FROM_XARALX
			return ATMFontMan::IsOkToCall();
#else
			return FALSE;
#endif
			break;
		case FC_TRUETYPE:
#ifndef EXCLUDE_FROM_XARALX
			return TTFontMan::IsOkToCall();
#else
			return FALSE;
#endif
			break;
		case FC_FREETYPE:
#ifdef __WXGTK__
			return FTFontMan::IsOkToCall();
#else
			return FALSE;
#endif
			break;
		default:
			break;
	}
	ERROR3("Unknown font class passed to OILFontMan::IsOkToCall()");
	return FALSE;
}


/********************************************************************************************

>	static BOOL OILFontMan::CacheNamedFont(String_64* pFontName, FontClass Class=FC_UNDEFINED, INT32 Pass=1)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pFontName = a pointer to a fontname to find and cache
				Class	  : The class of font to cache
						    FC_UNDEFINED - then try to cache any font
				Pass	  : 1 - first pass, try to match the font exactly
						  : 2 - second pass, try to find a compatible font
	Outputs:	-
	Returns:	TRUE if the font has been cached
				FALSE if not.
	Purpose:	This function attempts to cache a font of some variety. It asks each
				fontmanager in turn to attempt to cache the font. So for something like
				'Symbol' a true type font will be cached as this font manager is asked
				first

********************************************************************************************/

BOOL OILFontMan::CacheNamedFont(String_64* pFontName, FontClass Class, INT32 Pass)
{
	// Sticks the font in a temporary cache
	switch (Pass)
	{
		// PASS1 - try to cache an exact match
		case 1:
		{
			switch (Class)
			{
				case FC_UNDEFINED:
				{
#ifdef __WXGTK__
					if (FTFontMan::CacheNamedFont(pFontName))
						return TRUE;
#endif
PORTNOTE("text","We do not use TTFontMan in wxOil")
#ifndef EXCLUDE_FROM_XARALX
					if (TTFontMan::CacheNamedFont(pFontName))
						return TRUE;
#endif
PORTNOTE("text","Never cache ATM font")
#ifndef EXCLUDE_FROM_XARALX
					return ATMFontMan::CacheNamedFont(pFontName);
#else
					return FALSE;
#endif					
					break;
				}

				case FC_TRUETYPE:
				{
PORTNOTE("text","We do not use TTFontMan in wxOil")
#ifndef EXCLUDE_FROM_XARALX
					return TTFontMan::CacheNamedFont(pFontName);
#else
					return FALSE;
#endif
					break;
				}

				case FC_ATM:
				{
PORTNOTE("text","Never cache ATM font")
#ifndef EXCLUDE_FROM_XARALX
					return ATMFontMan::CacheNamedFont(pFontName);
#else
					return FALSE;
#endif					
					break;
				}

				case FC_FREETYPE:
				{
#ifdef __WXGTK__
					return FTFontMan::CacheNamedFont(pFontName);
#else
					return FALSE;
#endif
					break;
				}

				default:
					ERROR3("Unknown font class passed to OILFontMan::CacheNamedFont()");
			}
			break;
		}

		// PASS2 - try to create a compatible font as a replacement
		case 2:
		{
			switch (Class)
			{
				case FC_UNDEFINED:
				{
#ifdef __WXGTK__
					if (FTFontMan::CacheCompatibleFont(pFontName))
						return TRUE;
#endif

PORTNOTE("text","We do not use TTFontMan in wxOil")
#ifndef EXCLUDE_FROM_XARALX
					if (TTFontMan::CacheCompatibleFont(pFontName))
						return TRUE;
#endif
PORTNOTE("text","Never cache ATM font")
#ifndef EXCLUDE_FROM_XARALX
					return ATMFontMan::CacheCompatibleFont(pFontName);
#else
					return FALSE;
#endif					
					break;
				}

				case FC_TRUETYPE:
				{
PORTNOTE("text","We do not use TTFontMan in wxOil")
#ifndef EXCLUDE_FROM_XARALX
					return TTFontMan::CacheCompatibleFont(pFontName);
#else
					return FALSE;
#endif
					break;
				}

				case FC_ATM:
				{
PORTNOTE("text","Never cache ATM font")
#ifndef EXCLUDE_FROM_XARALX
					return ATMFontMan::CacheCompatibleFont(pFontName);
#else
					return FALSE;
#endif					
				}

				case FC_FREETYPE:
				{
#ifdef __WXGTK__
					return FTFontMan::CacheCompatibleFont(pFontName);
#else
					return FALSE;
#endif
					break;
				}

				default:
					ERROR3("Unknown font class passed to OILFontMan::CacheNamedFont()");

			}
			break;
		}
	
		default:
			ERROR3("Unknown pass selector passed to OILFontMan::CacheNamedFont()");
	}

	return FALSE;
}

/********************************************************************************************

>	static void OILFontMan::ValidateCache()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Purpose:	This function attempts to recache all fonts of all varieties

********************************************************************************************/

void OILFontMan::ValidateCache()
{
#ifdef __WXGTK__
	FTFontMan::ValidateCache();
#endif
PORTNOTE("text","We do not use TTFontMan in wxOil")
#ifndef EXCLUDE_FROM_XARALX
	TTFontMan::ValidateCache();
#endif
PORTNOTE("text","ATM deactivated")
#ifndef EXCLUDE_FROM_XARALX
	ATMFontMan::ValidateCache();
#endif
}

/********************************************************************************************

>	static void OILFontMan::FindClosestFont()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96
	Inputs:		-
	Returns:	-
	Purpose:	Calls each of the font managers, when searching for a match to a panose number

********************************************************************************************/

void OILFontMan::FindClosestFont()
{
#ifdef __WXGTK__
	FTFontMan::FindClosestFont();
#endif
PORTNOTE("text","We do not use TTFontMan in wxOil")
#ifndef EXCLUDE_FROM_XARALX
	TTFontMan::FindClosestFont();
#endif
PORTNOTE("text","ATM deactivated")
#ifndef EXCLUDE_FROM_XARALX
	ATMFontMan::FindClosestFont();
#endif
}

/********************************************************************************************

>	FontBase* OILFontMan::CreateNewFont(FontClass Class, String_64* pFontName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		Class = the type of font item to create
				pFontName = the name of the font to put in the created item
	Retruns:	A pointer to a font item (dependent on the class on entry)
	Purpose:	This function attempts to create a font instance and will be called by
				the font manager when new fonts are added.

********************************************************************************************/

FontBase* OILFontMan::CreateNewFont(FontClass Class, String_64* pFontName)
{
	switch (Class)
	{
		case FC_FREETYPE:
#ifdef __WXGTK__
		case FC_UNDEFINED:
			return FTFontMan::CreateNewFont(pFontName);
#else
			return NULL;
#endif
			break;
		case FC_TRUETYPE:
PORTNOTE("text","We do not use TTFontMan in wxOil")
#ifndef EXCLUDE_FROM_XARALX
			return TTFontMan::CreateNewFont(pFontName);
#else
			return NULL;
#endif
			break;
		case FC_ATM:
PORTNOTE("text","ATM deactivated")
#ifndef EXCLUDE_FROM_XARALX
			return ATMFontMan::CreateNewFont(pFontName);
#else
			return NULL;
#endif
			break;
		default:
			break;
	}
	ERROR3("OILFontMan::CreateNewFont() - Unknown font class");
	return NULL;
}

/********************************************************************************************

>	OUTLINETEXTMETRIC *OILFontMan::GetOutlineTextMetric(FontClass Class, LOGFONT *pLogFont)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/08/96
	Inputs:		Class		- class of font
				pLogFont	- LOGFONT structure describing the font to retrieve the
							  OUTLINETEXTMETRIC from
	Returns:	Pointer to the OUTLINETEXTMETRIC structure for a font, or NULL if one
				doesn't exist.
	Purpose:	Returns the OUTLINETEXTMETRIC structure for a font.

********************************************************************************************/

OUTLINETEXTMETRIC *OILFontMan::GetOutlineTextMetric(FontClass Class, LOGFONT *pLogFont)
{
	switch (Class)
	{
		case FC_FREETYPE:
#ifdef __WXGTK__
			return FTFontMan::GetOutlineTextMetric(pLogFont);
#else
			return NULL;
#endif
			break;
		case FC_TRUETYPE:
PORTNOTE("text","TT deactivated")
#ifndef EXCLUDE_FROM_XARALX
			return TTFontMan::GetOutlineTextMetric(pLogFont);
#else
			return NULL;
#endif
			break;
		case FC_ATM:
PORTNOTE("text","ATM deactivated")
#ifndef EXCLUDE_FROM_XARALX
			return ATMFontMan::GetOutlineTextMetric(pLogFont);
#else
			return NULL;
#endif
			break;
		default:
			break;
	}
	ERROR3("OILFontMan::CreateNewFont() - Unknown font class");
	return NULL;
}

/********************************************************************************************
>	OILFontMan::InvalidateCharMetrics()

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	20/04/2006
	Purpose:	Facade routine for FontMetricsCache::InvalidateCharMetrics

********************************************************************************************/
void OILFontMan::InvalidateCharMetrics()
{
	FontMetricsCache::InvalidateCharMetrics();
}

/********************************************************************************************
>	BOOL GetCharMetrics(wxDC* pDC, WCHAR ch, CharDescription& FontDesc, CharMetrics* pCharMetrics)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	20/04/2006
	Purpose:	Facade routine for FontMetricsCache::GetCharMetrics

********************************************************************************************/
BOOL OILFontMan::GetCharMetrics(wxDC* pDC, WCHAR ch, CharDescription& FontDesc, CharMetrics* pCharMetrics)
{
	return FontMetricsCache::GetCharMetrics(pDC, ch, FontDesc, pCharMetrics);
}

/********************************************************************************************
>	MILLIPOINT GetCharsKerning(wxDC* pDC, WCHAR chLeft, WCHAR chRight, CharDescription& FontDesc)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	20/04/2006
	Purpose:	Facade routine for FontKerningPairsCache::GetCharsKerning

********************************************************************************************/
MILLIPOINT OILFontMan::GetCharsKerning(wxDC* pDC, WCHAR chLeft, WCHAR chRight, CharDescription& FontDesc)
{
	return FontKerningPairsCache::GetCharsKerning(pDC, chLeft, chRight, FontDesc);
}

/********************************************************************************************

>	static UINT32 OILFontMan::GetOutlineCache(DocCoord** pCoords, PathVerb** pVerbs);

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		-
	Outputs;	pCoords, a pointer to the coordinate cache
				pVerbs, a pointer to the verb cache
	Returns:	The number of elements in the path cache.
	Purpose:	The function returns pointers to a statically allocated cache consiting of
				a verb and coordinate array. The cache is generally used by the font manager
				gen outline routines to temporarily store outlines of characters.

********************************************************************************************/

UINT32 OILFontMan::GetOutlineCache(DocCoord** pCoords, PathVerb** pVerbs)
{
	if (pCoords!=NULL)
		(*pCoords)=(CharOutlineCache::CacheCoords);
	if (pVerbs!=NULL)
		(*pVerbs)=(CharOutlineCache::CacheVerbs);
	return (CharOutlineCache::CacheSize);
}

/********************************************************************************************

>	static void OILFontMan::InitialiseOutlineCache()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/95
	Inputs:		-
	returns:	-
	Purpose:	Resets the OILFontMan CharOutlineCache to zero. Must be called before
				any calls to AddMoveTo, AddLineTo, AddBezierTo, ClosePath.

********************************************************************************************/

void OILFontMan::InitialiseOutlineCache()
{
	CharOutlineCache::CacheSize=0;
	CharOutlineCache::LastMoveTo.x = 0x7FFFFFFF;
}

/********************************************************************************************

>	BOOL OILFontMan::AddMoveTo(POINT& p0)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		p0 = a coordinate
	Returns:    TRUE if the coordinate was added to the outline cache
				FALSE if not
	Purpose:	Adds a new moveto element and point to the character outline cache at
				the insert position.

********************************************************************************************/

BOOL OILFontMan::AddMoveTo(POINT& p0)
{
	if ((CharOutlineCache::CacheSize)<OILFONTLIMIT)
	{
		CharOutlineCache::CacheCoords[CharOutlineCache::CacheSize].x = p0.x;
		CharOutlineCache::CacheCoords[CharOutlineCache::CacheSize].y = p0.y;
		CharOutlineCache::CacheVerbs[CharOutlineCache::CacheSize] = PT_MOVETO;
		CharOutlineCache::CacheSize++;
		CharOutlineCache::LastMoveTo.x = p0.x;
		CharOutlineCache::LastMoveTo.y = p0.y;
		return TRUE;
	}
	ERROR3("CharOutlineCache overflow");
	return FALSE;
}

BOOL OILFontMan::AddMoveTo(DocCoord& p0)
{
	if ((CharOutlineCache::CacheSize)<OILFONTLIMIT)
	{
		CharOutlineCache::CacheCoords[CharOutlineCache::CacheSize] = p0;
		CharOutlineCache::CacheVerbs[CharOutlineCache::CacheSize] = PT_MOVETO;
		CharOutlineCache::CacheSize++;
		CharOutlineCache::LastMoveTo = p0;
		return TRUE;
	}
	ERROR3("CharOutlineCache overflow");
	return FALSE;
}

/********************************************************************************************

>	BOOL OILFontMan::AddLineTo(POINT& p0)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		p0 = a coordinate
	Returns:    TRUE if the coordinate was added to the outline cache
				FALSE if not
	Purpose:	Adds a new lineto element and point to the character outline cache at
				the insert position.

********************************************************************************************/

BOOL OILFontMan::AddLineTo(POINT& p0)
{
	if ((CharOutlineCache::CacheSize)<OILFONTLIMIT)
	{
		CharOutlineCache::CacheCoords[CharOutlineCache::CacheSize].x = p0.x;
		CharOutlineCache::CacheCoords[CharOutlineCache::CacheSize].y = p0.y;
		CharOutlineCache::CacheVerbs[CharOutlineCache::CacheSize] = PT_LINETO;
		CharOutlineCache::CacheSize++;
		return TRUE;
	}
	ERROR3("CharOutlineCache overflow");
	return FALSE;
}

BOOL OILFontMan::AddLineTo(DocCoord& p0)
{
	if ((CharOutlineCache::CacheSize)<OILFONTLIMIT)
	{
		CharOutlineCache::CacheCoords[CharOutlineCache::CacheSize] = p0;
		CharOutlineCache::CacheVerbs[CharOutlineCache::CacheSize] = PT_LINETO;
		CharOutlineCache::CacheSize++;
		return TRUE;
	}
	ERROR3("CharOutlineCache overflow");
	return FALSE;
}

/********************************************************************************************

>	BOOL OILFontMan::AddBezierTo(POINT& p0, POINT& p1, POINT& p2)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		p0,p1,p2 = a set of coordinates
	Returns:    TRUE if the coordinates were added to the outline cache
				FALSE if not
	Purpose:	Adds a new bezierto element to the character outline cache at the insert
				position.

********************************************************************************************/

BOOL OILFontMan::AddBezierTo(POINT& p0, POINT& p1, POINT& p2)
{
	if ((CharOutlineCache::CacheSize)<(OILFONTLIMIT-3))
	{
		CharOutlineCache::CacheCoords[CharOutlineCache::CacheSize].x = p0.x;
		CharOutlineCache::CacheCoords[CharOutlineCache::CacheSize].y = p0.y;
		CharOutlineCache::CacheVerbs[CharOutlineCache::CacheSize] = PT_BEZIERTO;
		CharOutlineCache::CacheSize++;
		CharOutlineCache::CacheCoords[CharOutlineCache::CacheSize].x = p1.x;
		CharOutlineCache::CacheCoords[CharOutlineCache::CacheSize].y = p1.y;
		CharOutlineCache::CacheVerbs[CharOutlineCache::CacheSize] = PT_BEZIERTO;
		CharOutlineCache::CacheSize++;
		CharOutlineCache::CacheCoords[CharOutlineCache::CacheSize].x = p2.x;
		CharOutlineCache::CacheCoords[CharOutlineCache::CacheSize].y = p2.y;
		CharOutlineCache::CacheVerbs[CharOutlineCache::CacheSize] = PT_BEZIERTO;
		CharOutlineCache::CacheSize++;
		return TRUE;
	}
	ERROR3("CharOutlineCache overflow");
	return FALSE;
}

BOOL OILFontMan::AddBezierTo(DocCoord& p0, DocCoord& p1, DocCoord& p2)
{
	if ((CharOutlineCache::CacheSize)<(OILFONTLIMIT-3))
	{
		CharOutlineCache::CacheCoords[CharOutlineCache::CacheSize] = p0;
		CharOutlineCache::CacheVerbs[CharOutlineCache::CacheSize] = PT_BEZIERTO;
		CharOutlineCache::CacheSize++;
		CharOutlineCache::CacheCoords[CharOutlineCache::CacheSize] = p1;
		CharOutlineCache::CacheVerbs[CharOutlineCache::CacheSize] = PT_BEZIERTO;
		CharOutlineCache::CacheSize++;
		CharOutlineCache::CacheCoords[CharOutlineCache::CacheSize] = p2;
		CharOutlineCache::CacheVerbs[CharOutlineCache::CacheSize] = PT_BEZIERTO;
		CharOutlineCache::CacheSize++;
		return TRUE;
	}
	ERROR3("CharOutlineCache overflow");
	return FALSE;
}

/********************************************************************************************

>	BOOL OILFontMan::ClosePath()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		-
	Retruns:	TRUE if the current path has been closed
				FALSE if there is no valid path in the cache
	Purpose:	To close the cached path

********************************************************************************************/

BOOL OILFontMan::ClosePath()
{
	if ((CharOutlineCache::CacheSize)<1)
		return FALSE;

	// Just check the initialised value of LastMoveto for sanity
	if (CharOutlineCache::LastMoveTo.x == 0x7FFFFFFF)
	{
		ERROR3("No moveto in outline cache");
		return FALSE;
	}
	
	// find the last coordinate visited
	DocCoord LastCoord = CharOutlineCache::CacheCoords[CharOutlineCache::CacheSize-1];

	// if last coord does not close the path then add an extra element
	if (LastCoord != CharOutlineCache::LastMoveTo)
		if (!AddLineTo(CharOutlineCache::LastMoveTo))
			return FALSE;

	// finally close the element and record its position
	CharOutlineCache::CacheVerbs[CharOutlineCache::CacheSize-1] |= PT_CLOSEFIGURE;

	return TRUE;
}

/********************************************************************************************

>	static void OILFontMan::FinaliseOutlineCache()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/95
	Inputs:		-
	returns:	-
	Purpose:	Tidies what is left by a series of calls to AddMoveTo, AddLineTo, AddCurveTo
				to make sure the path format is correct for our internal processors.

********************************************************************************************/

void OILFontMan::FinaliseOutlineCache()
{
}

/********************************************************************************************

>	static BOOL OILFontMan::GetCharPath(CharDescription& ChDesc,
										DocCoord** ppCoords,
										PathVerb** ppVerbs,
										UINT32* pNumCoords,
										wxDC* pDC=NULL)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		ChDesc	= description of char
				pDC		= (possible) pointer to a DC (for optimisation) defaults to NULL
	Outputs:	ppCoords	= pointer to path coords buffer
				ppVerbs		= pointer to path verbs buffer
				pNumCoords	= number of elements in path
	Returns:	FALSE if fails
	Purpose:	Get the path associated with a given char

********************************************************************************************/

BOOL OILFontMan::GetCharPath(FontClass fclass,
							 CharDescription& ChDesc,
							 DocCoord** ppCoords,
							 PathVerb** ppVerbs,
							 UINT32* pNumCoords,
							 wxDC *pDC)
{
	TRACEUSER("wuerthne",_T("OILFontMan::GetCharPath"));
	BOOL Success=FALSE;
	switch (fclass)
	{
		case FC_TRUETYPE:
PORTNOTE("text","TrueType deactivated")
#ifndef EXCLUDE_FROM_XARALX
			Success = TextManager::GetTTCharPath(ChDesc, ppCoords, ppVerbs, pNumCoords, pDC);
#endif
			ERROR1IF(Success==FALSE, FALSE, _R(IDE_FONTMAN_NOTTOUTLINE));
			break;

		case FC_ATM:
PORTNOTE("text","ATM deactivated")
#ifndef EXCLUDE_FROM_XARALX
			Success = ATMFontMan::GetCharOutline(ChDesc, ppCoords, ppVerbs, pNumCoords, pDC);
#endif
			ERROR1IF(Success==FALSE, FALSE, _R(IDE_FONTMAN_NOATMOUTLINE));
			break;

		case FC_FREETYPE:
#ifdef __WXGTK__
			Success = FTFontMan::GetCharOutline(ChDesc, ppCoords, ppVerbs, pNumCoords, pDC);
#else
			Success = FALSE;
#endif
			ERROR1IF(Success==FALSE, FALSE, _R(IDE_FONTMAN_NOFTOUTLINE));
			break;
		default:
			break;
	}
	ERROR3IF(Success==FALSE,"Unknown font class in OILFontMan::GetCharPath");
	return Success;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// FontMetricsCacheEntry

/********************************************************************************************
>	FontMetricsCacheEntry::FontMetricsCacheEntry()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/96
	Purpose:	Default constructor

********************************************************************************************/

FontMetricsCacheEntry::FontMetricsCacheEntry()
{
	for (INT32 i=0; i<NUMCHARS; i++)
		pCharWidths[i] = 0;

	FontEmWidth = 0;
	FontAscent  = 0;
	FontDescent = 0;
	CharDescription emptyCharDesc(0, 0, 0, 0);
	FontDesc    = emptyCharDesc;
}


/********************************************************************************************
>	BOOL FontMetricsCacheEntry::CacheFontMetrics(wxDC* pDC, CharDescription FontDesc,
												MILLIPOINT DefaultHeight, INT32 DesignSize)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/96
	Inputs:		pDC           - DC with design size font selected
				FontDesc      - descriptor of font which is being cached
				DefaultHeight - default height of char (ie size of font for which char widths are cached)
				DesignSize    - size of font selected in DC (in Logical units - pixels)
	Returns:	FALSE if fails
	Purpose:	Refill the font cache entry
********************************************************************************************/

BOOL FontMetricsCacheEntry::CacheFontMetrics( wxDC* pDC, CharDescription FontDesc,
												MILLIPOINT DefaultHeight, INT32 DesignSize )
{
#ifdef __WXGTK__
	INT32 Ascent;
	INT32 Descent;
#endif

	// these values are ignored - the DesignSize needs to be read from the font, so the
	// scaling needs to be done by FTFontMan itself
	IGNOREPARAM(DefaultHeight);
	IGNOREPARAM(DesignSize);

#ifdef __WXGTK__
	// TRACEUSER("wuerthne", _T("CacheFontMetrics DesignSize = %d"), DesignSize);
	if (FTFontMan::GetAscentDescent(FontDesc, &Ascent, &Descent) == FALSE) return FALSE;
	// scaling to DefaultHeight was done in GetAscentDescent already
	SetFontAscent( Ascent );
	SetFontDescent( -Descent );

	// get char widths and convert form design size in pixels to default height in millipoints
	static INT32 pTempCharWidthBuf[NUMCHARS];
	if (FTFontMan::GetCharWidth(FontDesc, FIRSTCHAR, LASTCHAR, pTempCharWidthBuf)==FALSE)
		return FALSE;
	// scaling to DefaultHeight was done in GetCharWidth already
	for (INT32 i=0; i<NUMCHARS; i++)
		pCharWidths[i] = pTempCharWidthBuf[i];

	// if 'em' char in cache, get it from the cache else calculate its width separately
	if (CharInCacheRange(FONTEMCHAR))
		SetFontEmWidth( GetCharWidthFromCache(FONTEMCHAR) );
	else
	{
		ERROR3("FontMetricsCache::GetCharMetrics() - 'FONTEMCHAR' not in cache!");
		INT32 TempCharWidth = 0;
		if (FTFontMan::GetCharWidth(FontDesc, FONTEMCHAR, FONTEMCHAR, &TempCharWidth)==FALSE)
			return FALSE;
		// scaling to DefaultHeight was done in GetCharWidth already
		SetFontEmWidth( TempCharWidth );
	}

	// update cache tag
	SetFontDesc(FontDesc);
#else // !wxGTK
	return FALSE;
#endif

#ifndef EXCLUDE_FROM_XARALX
	// debug test
	CheckCharWidthsSameAsABCWidths(pDC,FontDesc);
#endif
	return TRUE;
}


/********************************************************************************************
>	static void FontMetricsCacheEntry::CheckCharWidthsSameAsABCWidths(wxDC* pDC, CharDescription FontDesc)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/96
	Inputs:		pDC      - 
				FontDesc - 
	Purpose:	Test to see if ::GetCharWidth() with design size font is as accurate as ::GetCharABCWidths()
********************************************************************************************/

void FontMetricsCacheEntry::CheckCharWidthsSameAsABCWidths(wxDC* pDC, CharDescription FontDesc)
{
#ifdef _DEBUG
#ifndef EXCLUDE_FROM_XARALX
	// ensure that the char widths are the same as the sum of the ABC widths

	// do nothing if its an ATM font (or an error)
	CachedFontItem* pItem = FONTMANAGER->GetFont(FontDesc.GetTypefaceHandle());
	if (pItem->GetFontClass()==FC_ATM || pItem==NULL || pItem->IsCorrupt())
		return;

	INT32 pTempCharWidthBuf[NUMCHARS];
	ABC pTempCharABCBuf[NUMCHARS];
	BOOL    ok = TextManager::GetCharWidth(    pDC, FIRSTCHAR, LASTCHAR, pTempCharWidthBuf);
	if (ok) ok = TextManager::GetCharABCWidths(pDC, FIRSTCHAR, LASTCHAR, pTempCharABCBuf);
	ERROR3IF(!ok,"FontMetricsCache::CheckCharWidthsSameAsABCWidths() - TextManager::GetCharWidth()/GetCharABCWidths() failed!\n");

	for (INT32 i=0; i<NUMCHARS; i++)
	{
		ABC* pABC     = &pTempCharABCBuf[i];
		INT32 SumABC    = pABC->abcA+pABC->abcB+pABC->abcC;
		INT32 CharWidth = pTempCharWidthBuf[i];
		ERROR3IF_PF(SumABC!=CharWidth,("FontMetricsCache::CheckCharWidthsSameAsABCWidths() - sum of ABCwidths (%d) differs from CharWidths (%d)",SumABC,CharWidth));
	}
#endif

//#ifdef _DEBUG
#if 0
	// test accuracy of old and new methods of calculating metrics

	INT32 DesignSizeInPixels = TextManager::GetDesignSize(pDC);
	INT32 DefaultSizeInMP    = TextManager::GetDefaultHeight();
	INT32 dpi                = pDC->GetDeviceCaps(LOGPIXELSY);
	for (MILLIPOINT FontSizeInMP=4000; FontSizeInMP<=72000; FontSizeInMP+=4000)
	{
		double SumNewMetrics      = 0;
		double SumOldMetrics      = 0;
		double SumAccurateMetrics = 0;
		for (WCHAR ch=0; ch<256; ch++)
		{
			INT32 DesignSizeMetricInPixels = 500;
			BOOL ok = TextManager::GetCharWidth(pDC,ch,ch,&DesignSizeMetricInPixels);
			ERROR3IF(!ok,"FontMetricsCache::CheckCharWidthsSameAsABCWidths() - TextManager::GetCharWidth() failed!\n");

			// new calc
			MILLIPOINT InchMetricInMP                  = MulDiv(DesignSizeMetricInPixels, DefaultSizeInMP, DesignSizeInPixels);
			FIXED16    InchMetricToFontSizeMetricRatio = Div32By32(FontSizeInMP, DefaultSizeInMP);
			MILLIPOINT NewFontSizeMetricInMP           = InchMetricInMP * InchMetricToFontSizeMetricRatio;
			SumNewMetrics += NewFontSizeMetricInMP;

			// old calc
			MILLIPOINT DesignSizeMetricInMP                  = MulDiv(DesignSizeMetricInPixels, 72000, dpi);
			MILLIPOINT DesignSizeInMP                        = MulDiv(DesignSizeInPixels, 72000, dpi);
			FIXED16    DesignSizeMetricToFontSizeMetricRatio = Div32By32(FontSizeInMP, DesignSizeInMP);
			MILLIPOINT OldFontSizeMetricInMP                 = DesignSizeMetricInMP * DesignSizeMetricToFontSizeMetricRatio;
			SumOldMetrics += OldFontSizeMetricInMP;

			// accurate calc
			double AccurateFontSizeMetricInMP = (double) DesignSizeMetricInPixels / DesignSizeInPixels * FontSizeInMP;
			SumAccurateMetrics += AccurateFontSizeMetricInMP;
		}
		TRACEUSER( "Ed", _T("Sum of CharWidths (%5dMP): new=%8.0f (e=%6.2f), old=%8.0f (e=%6.2f), accrutate=%8.2f\n"), FontSizeInMP, SumNewMetrics, SumNewMetrics-SumAccurateMetrics, SumOldMetrics, SumOldMetrics-SumAccurateMetrics, SumAccurateMetrics);
	}
#endif // EXCLUDE_FROM_XARALX
#endif // _DEBUG
}


/////////////////////////////////////////////////////////////////////////////////////////////
// FontMetricsCache

/********************************************************************************************
>	static BOOL FontMetricsCache::GetCharMetrics(wxDC* pDC, WCHAR ch, CharDescription& FontDesc,
													CharMetrics* pCharMetrics)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/96
	Inputs:		pDC			= a pointer to a device context
				ch 			= a character code
	 			FontDesc	= a font description
	Outputs:	pCharMetrics = metrics of char requested
	Returns:	FALSE if fails
	Purpose:	Get the metrics of a given char for the font selected in the DC,
	Note:		Uses a single entry cache for speed
********************************************************************************************/

BOOL FontMetricsCache::GetCharMetrics(wxDC* pDC, WCHAR ch, CharDescription& FontDesc, CharMetrics* pCharMetrics)
{
	// TRACEUSER("wuerthne", _T("FontMetricsCache::GetCharMetrics %04x"), ch);
	IGNOREPARAM(pDC);
	ERROR2IF(pCharMetrics==NULL,FALSE,"FontMetricsCache::GetCharMetrics() - pCharMetrics==NULL");
	ERROR2IF(FontDesc.GetCharCode()!=FONTEMCHAR,FALSE,"FontMetricsCache::GetCharMetrics() - FontDesc char should be 'FONTEMCHAR'");

	// find if font is in cache (and if so which entry)
	INT32 CacheEntry = 0;
	while (CacheEntry<NUMENTRIES && mpFontMetricsData[CacheEntry].GetFontDesc()!=FontDesc)
		CacheEntry +=1;

	// if font not in cache, recache it, or if char not in cached range, get it explicitly
	MILLIPOINT CharWidth  = 0;
	BOOL CharInCacheRange = FontMetricsCacheEntry::CharInCacheRange(ch);
	if (CacheEntry>=NUMENTRIES || !CharInCacheRange)
	{
		// get design size of font, and default heigh
		INT32 DesignSize    = TextManager::GetDesignSize(pDC);  // ignores pDC
		INT32 DefaultHeight = TextManager::GetDefaultHeight();

		CachedFontItem* pItem = FONTMANAGER->GetFont(FontDesc.GetTypefaceHandle());
		if (pItem==NULL || pItem->IsCorrupt())
			return FALSE;

		// get a LogFont, create a font and select it into the DC
		LOGFONT	CharLogFont;
		if (TextManager::GetLogFontFromCharDescriptor(pDC, FontDesc, &CharLogFont, DesignSize)==FALSE)
			return FALSE;

		// if font not in cache, cache its metrics throwing out a random entry
		if (CacheEntry>=NUMENTRIES)
		{
			CacheEntry = rand() % NUMENTRIES;
			if (mpFontMetricsData[CacheEntry].CacheFontMetrics(pDC, FontDesc, DefaultHeight, DesignSize) == FALSE)
				return FALSE;
		}

		// if char not in cache, find it's width separately (unless it is FONTEMCHAR - eg for kern code, blank line)
		if (!CharInCacheRange)
		{
			if (ch==FONTEMCHAR)
				CharWidth = mpFontMetricsData[CacheEntry].GetFontEmWidth();
			else
			{
#ifdef __WXGTK__
				// FTFontMan returns the scaled character width already
				if (FTFontMan::GetCharWidth(FontDesc, ch, ch, &CharWidth)==FALSE)
#endif
					return FALSE;
			}
		}
	}

	if (CharInCacheRange)
		CharWidth = mpFontMetricsData[CacheEntry].GetCharWidthFromCache(ch);

	pCharMetrics->CharWidth   = CharWidth;
	pCharMetrics->FontAscent  = mpFontMetricsData[CacheEntry].GetFontAscent();
	pCharMetrics->FontEmWidth = mpFontMetricsData[CacheEntry].GetFontEmWidth();
	pCharMetrics->FontDescent = mpFontMetricsData[CacheEntry].GetFontDescent();
	return TRUE;
}


/********************************************************************************************
>	static void FontMetricsCache::InvalidateCharMetrics()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/96
	Purpose:	Invalidate the FontMetricsCache
********************************************************************************************/

void FontMetricsCache::InvalidateCharMetrics()
{
	CharDescription emptyCharDesc(0, 0, 0, 0);
	for (INT32 i=0; i<NUMENTRIES; i++)
		mpFontMetricsData[i].SetFontDesc( emptyCharDesc );
}

/////////////////////////////////////////////////////////////////////////////////////////////
// FontKerningPairsCache

/********************************************************************************************
>	static MILLIPOINT FontKerningPairsCache::GetCharsKerning(wxDC* pDC, WCHAR chLeft,
													WCHAR chRight, CharDescription& FontDesc);

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/20/2000
	Inputs:		pDC			= a pointer to a device context
				chLeft		= a character code of the left char of a kerning pair
				chRight		= a character code of the right char of a kerning pair
	 			FontDesc	= a font description
	Returns:	Kern in millipoints (or zero if no kern or error)
	Purpose:	Get the kern between two chars of a font
	Note:		The kern pairs are cached (see FontKerningPairsCacheEntry::GetCharsKerning)
********************************************************************************************/

MILLIPOINT FontKerningPairsCache::GetCharsKerning(wxDC* pDC, WCHAR chLeft, WCHAR chRight,
												  CharDescription& FontDesc)
{
	// TRACEUSER("wuerthne", _T("GetCharsKerning %04x %04x"), chLeft, chRight);
	ERROR2IF(pDC==NULL, 0, "FontKerningPairsCache::GetCharsKerning() - pDC==NULL");
	ERROR2IF(FontDesc.GetCharCode() != FONTEMCHAR, FALSE,
		"FontKerningPairsCache::GetCharsKerning() - FontDesc char should be 'FONTEMCHAR'");
#ifndef DISABLE_TEXT_RENDERING
	// find out whether the font is in the cache (and if so which entry)
	INT32 CacheEntry = 0;
	while (CacheEntry < NUMENTRIES && m_FontKerningPairsCacheData[CacheEntry].GetFontDesc() != FontDesc)
		CacheEntry++;

	if (CacheEntry >= NUMENTRIES)
	{
		// TRACEUSER("wuerthne", _T("did not find cache entry, so allocate new one"));
		// the font is not in the cache, so throw out a random cache entry and prepare
		// the cache entry so we can start caching the kerning data for the new font
		CacheEntry = rand() % NUMENTRIES;
		m_FontKerningPairsCacheData[CacheEntry].Reinitialise(FontDesc);
	}
	return m_FontKerningPairsCacheData[CacheEntry].GetCharsKerning(chLeft, chRight);
#else
	return 0;
#endif
}

#ifdef _DEBUG
/********************************************************************************************

>	void FontKerningPairsCache::Dump()

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/2000
	Purpose:	Dumps the class's data

********************************************************************************************/
void FontKerningPairsCache::Dump()
{
	TRACE( _T(">>> Font kerning data start >>>\n"));
	for (INT32 i=0; i<NUMENTRIES; ++i)
	{
		TRACE( _T("Entry %d:\n"), i);
		m_FontKerningPairsCacheData[i].Dump();
	}
	TRACE( _T("<<< Font kerning data end <<<\n"));
}
#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////////////////////
// FontKerningPairsCacheEntry

/********************************************************************************************
>	FontKerningPairsCacheEntry::FontKerningPairsCacheEntry()

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	20/04/2006
	Purpose:	Default constructor

********************************************************************************************/
FontKerningPairsCacheEntry::FontKerningPairsCacheEntry()
{
	CharDescription emptyCharDesc(0, 0, 0, 0);
	FontDesc = emptyCharDesc;
	m_pPairsCacheMap = NULL;
}

/********************************************************************************************
>	FontKerningPairsCacheEntry::FontKerningPairsCacheEntry()

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	20/04/2006
	Purpose:	Destructor

********************************************************************************************/
FontKerningPairsCacheEntry::~FontKerningPairsCacheEntry()
{
	if (m_pPairsCacheMap) delete m_pPairsCacheMap;
}

/********************************************************************************************
>	FontKerningPairsCacheEntry::Reinitialise(CharDescription& NewFontDesc)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	20/04/2006
	Inputs:		NewFontDesc = a font descriptor specifying the font to which this cache
							  entry should refer from now on
	Purpose:	Clears the cache entry and initialises it for a new font

********************************************************************************************/
void FontKerningPairsCacheEntry::Reinitialise(CharDescription& NewFontDesc)
{
	FontDesc = NewFontDesc;
	if (m_pPairsCacheMap) delete m_pPairsCacheMap;
	m_pPairsCacheMap = new std::map<UINT32,INT32>;
	// we do not check whether the new has succeeded - in case it has not,
	// we will not fall over later but caching will be disabled for this entry
}

/********************************************************************************************
>	FontKerningPairsCacheEntry::GetCharsKerning(WCHAR chLeft, WCHAR chRight)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	20/04/2006
	Inputs:		chLeft		= a character code of the left char of a kerning pair
				chRight		= a character code of the right char of a kerning pair
	Returns:	Kern in millipoints (or zero if no kern or error)
	Purpose:	Gets the kerning for a character pair (and caches it if not cached yet)
	Note:		The cache uses an STL map, which means logarithmic access time. Maybe
				a hash_map could do better.

********************************************************************************************/
MILLIPOINT FontKerningPairsCacheEntry::GetCharsKerning(WCHAR chLeft, WCHAR chRight)
{
	UINT32 key;
	// we cache the kerning for characters in the range 0x0 - 0xFFFF only
	// we also need the map set up - if it could not be allocated, we simply bypass the cache
	if (chLeft > 0x10000 || chRight > 0x10000 || m_pPairsCacheMap == NULL)
	{
		// TRACEUSER("wuerthne", _T("Bypass cache"));
#ifdef __WXGTK__
		return FTFontMan::GetCharsKerning(FontDesc, chLeft, chRight);
#else
		return 0;
#endif
	}

	key = (chLeft << 16) | chRight;
	std::map<UINT32,INT32>::iterator it = m_pPairsCacheMap->find(key);
	if (it != m_pPairsCacheMap->end()) {
		// we found a cached entry
		// TRACEUSER("wuerthne", _T("kern pair found"));
		return it->second;
	}
	else {
		// we did not find an entry, so get the kerning from the underlying font system
#ifdef __WXGTK__
		INT32 kerning = FTFontMan::GetCharsKerning(FontDesc, chLeft, chRight);
#else
		INT32 kerning = 0;
#endif
		// and cache it
		// TRACEUSER("wuerthne", _T("cache kern pair"));
		(*m_pPairsCacheMap)[key] = kerning;
		return kerning;
	}
}

#if _DEBUG

/********************************************************************************************

>	void FontKerningPairsCacheEntry::Dump()

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	21/04/2006
	Purpose:	Dumps the class data

********************************************************************************************/
void FontKerningPairsCacheEntry::Dump()
{
	// dump the map
	if (m_pPairsCacheMap != NULL) {
		for (std::map<UINT32,INT32>::iterator it = m_pPairsCacheMap->begin(); it != m_pPairsCacheMap->end(); ++it)
		{
			UINT32 key = (*it).first;
			TRACE(_T("cached kern pair %04x/%04x = %d"), key >> 16, key & 0xffff, (*it).second);
		}
	}
}

#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////////////////////
// some debug stuff


#if _DEBUG

/********************************************************************************************

>	static void CharOutlineCache::Dump()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/95
	Purpose:	Dump out the contents of the character outline cache

********************************************************************************************/

void CharOutlineCache::Dump()
{
	UINT32 i=0;

	TRACE( _T("CHAROUTLINECACHE DUMP\n"));

	while (i < CacheSize)
	{
		switch ((CacheVerbs[i]) & ~PT_CLOSEFIGURE)
		{
			case PT_MOVETO:
				TRACE( _T("MoveTo "));
				TRACE( _T("(%d,"),CacheCoords[i].x);
				TRACE( _T("%d)\n"),CacheCoords[i].y);
				i++;
				break;
			case PT_LINETO:
				TRACE( _T("LineTo "));
				TRACE( _T("(%d,"),CacheCoords[i].x);
				TRACE( _T("%d)\n"),CacheCoords[i].y);
				i++;
				break;
			case PT_BEZIERTO:
				TRACE( _T("CurveTo "));
				TRACE( _T("(%d,"),CacheCoords[i].x);
				TRACE( _T("%d) "),CacheCoords[i].y);
				TRACE( _T("(%d,"),CacheCoords[i+1].x);
				TRACE( _T("%d) "),CacheCoords[i+1].y);
				TRACE( _T("(%d,"),CacheCoords[i+2].x);
				TRACE( _T("%d)\n"),CacheCoords[i+2].y);
				i+=3;
				break;
		}
	}	
}

#endif

#if _DEBUG

/********************************************************************************************

>	void OILFontMan::DumpEnumLogFont(ENUMLOGFONT* lpelf)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/95
	Purpose:	Dump out the contents of a log font.

********************************************************************************************/

void OILFontMan::DumpEnumLogFont(ENUMLOGFONT* lpelf)
{
	TRACE( _T("DumpELogFont at %ld\n"),lpelf);
	if (lpelf!=NULL)
	{
		TRACE( _T(" ---------------- \n"));
		TRACE( _T("Enum Log font structure:\n"));
#ifndef EXCLUDE_FROM_XARALX
		TRACE( _T(" Full name = %s\n"), lpelf->elfFullName);
		TRACE( _T(" Style     = %s\n"), lpelf->elfStyle);
#endif
		DumpLogFont(&(lpelf->elfLogFont));
		TRACE( _T(" ---------------- \n"));
	}
}

void OILFontMan::DumpLogFont(LOGFONT* lplf)
{
	if (lplf!=NULL)
	{
		TRACE( _T("Log font structure:\n"));
#ifndef EXCLUDE_FROM_XARALX
		TRACE( _T(" Height 			= %d\n"), lplf->lfHeight);
		TRACE( _T(" Width			= %d\n"), lplf->lfWidth);
		TRACE( _T(" Escapement		= %d\n"), lplf->lfEscapement);
		TRACE( _T(" Orientation 	= %d\n"), lplf->lfOrientation);
		TRACE( _T(" Weight			= %d\n"), lplf->lfWeight);
		TRACE( _T(" Italic			= %d\n"), lplf->lfItalic);
		TRACE( _T(" Underline		= %d\n"), lplf->lfUnderline);
		TRACE( _T(" StrikeOut		= %d\n"), lplf->lfStrikeOut);
		TRACE( _T(" CharSet			= %d\n"), lplf->lfCharSet);
		TRACE( _T(" OutPrecision	= %d\n"), lplf->lfOutPrecision);
		TRACE( _T(" ClipPrecision	= %d\n"), lplf->lfClipPrecision);
		TRACE( _T(" Quality			= %d\n"), lplf->lfQuality);
		TRACE( _T(" PitchAndFamily	= %d\n"), lplf->lfPitchAndFamily);
		TRACE( _T(" FaceName		= %s\n"), lplf->lfFaceName);
#endif
	}
}

#endif

// ------------------------ End of OilFontMan functions -------------------------------------


/********************************************************************************************

>	FontBase::FontBase()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95				
	Purpose:	Default constructor

********************************************************************************************/

FontBase::FontBase()
{
}


/********************************************************************************************

>	BOOL FontBase::Initialise(String_64 *pFontName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95				
	Inputs:		pFontName = A pointer to a font name
	Purpose:	Initialises the OILFont font class

********************************************************************************************/

BOOL FontBase::Initialise(String_64 *pFontName)
{
	TheFontName = *pFontName;
	return TRUE;
}


/********************************************************************************************

>	virtual void FontBase::Delete()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95
	Inputs:		-
	Returns:	-				
	Purpose:	Deletes the contents of a OILFont font class

********************************************************************************************/

void FontBase::Delete()
{
	ERROR3("Error Error, FontBase::Delete() called - bad derivation / v-table corrupted");
}


/********************************************************************************************

>	virtual BOOL FontBase::Compare(String_64* pFontName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95
	Inputs:		pFontName = A pointer to a font name
				Class	  = The font class
	Returns:	TRUE if the font name on entry matchs that of this cached font
	Purpose:	Compare one font item name with an other

********************************************************************************************/

BOOL FontBase::Compare(String_64* pFontName)
{
	return ((TheFontName.CompareTo(*pFontName)) == 0);
}



/********************************************************************************************

>	virtual void FontBase::Dump()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/09/95
	Inputs:		-
	Returns:	-
	Purpose:	-

********************************************************************************************/

void FontBase::Dump()
{
	TRACE( _T(" Font name = %s\n"),((TCHAR*)TheFontName));
}


// ------------------------ End of FontBase functions -------------------------------------

/********************************************************************************************

>	void EnumFonts()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Purpose:	This function allows kernel routines to enumerate all the installed fonts
				in the system. It is currently used to build menus

********************************************************************************************/

OILEnumFonts::OILEnumFonts()
{
}

void OILEnumFonts::Execute()
{
PORTNOTE("text","We do not use TTFontMan in wxOil")
#ifndef EXCLUDE_FROM_XARALX
	TTFontMan::EnumAllFonts(this);
#endif
PORTNOTE("text","We do not use ATMFontMan in wxOil")
#ifndef EXCLUDE_FROM_XARALX
	ATMFontMan::EnumAllFonts(this);
#endif
#ifdef __WXGTK__
	FTFontMan::EnumAllFonts(this);
#endif
	// Add any further OIL Level font managers here
}

BOOL OILEnumFonts::NewFont(FontClass Class, ENUMLOGFONT *lpelf)
{
	return FALSE;
}
