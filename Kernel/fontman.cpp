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
// implementation file for Camelots Font Managler, supposidly

/*
*/

#include "camtypes.h"
// #include "fontbase.h" - included in fontman.h
#include "fontman.h"
#include "errors.h"
#include "txtattr.h"
//#include "richard2.h"

DECLARE_SOURCE( "$Revision$" );

CC_IMPLEMENT_DYNCREATE( CachedFontItem, ListItem )
CC_IMPLEMENT_DYNCREATE( FontManager, CCObject )
CC_IMPLEMENT_DYNCREATE( EnumAllFonts, OILEnumFonts )


// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW     


/*
	THEORY N ALL

	(1) The font manager controls its fonts in a simple list. Each entry contains a
	    font description which knows what kind of font it is.
	(2) All access to fonts should be via the font manager, which in turn asks each
	    font list item.
	(3) For speed and general lack of gobbling memory, the font list is initialised with
	    font names only. If someone actually wants information about a fonts metrics
		or its log font structure (heaven forbid) they call the fontmanagers functions
		to return these items. If the items are null at the time then the info has not
		been cached yet and will immediately get cached (cor cache on demand!). A little
		slower for the first use but nice there after.
	(4) A font name is now no longer enough to uniquely specify a font. The name and class
	    need to be used as you can be using an ATM font called Courier as well as a
		True Type font of the same name. Eeek.
	(5) Any new stuff added should NOT be naughty and follow its own guide lines. The idea
	    is simple. Provide a function in the font manager that the external user can call,
		then implement the same function inside CachedFontItem (ie the font manager usually
		asks the font item if it wants information from it (it does not simply dig the
		data straight out itself!). The font item then asks a virtual function derived
		from FontBase cause the actually font class only knows whats going on at the
		device level.
		That way we might even be able to get risc os fonts working one day. Ha Ha Ha Ha!
	(6) THOU SHALL'T NOT CHANGE SCOPE!
		Anyone found leaping into structures having promoted things from private to public
		will be shot.
	(7) To add a new font controller, simply derive from FontBase and implement the necessary
		functionality, simple as that although currently we do not have our own internal
		font descriptors, we currently rely on the one API structure: ENUMLOGFONT
		The structure OUTLINETEXTMETRIC is cached on demand, since its 'expensive' to
		retrieve. The only time we need to do it for all the fonts should be when the
		new file format needs to do some smart font subsitution.
*/


/********************************************************************************************

>	CachedFontItem::CachedFontItem() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95				
	Purpose:	Default constructor

********************************************************************************************/

CachedFontItem::CachedFontItem()
{
	pFontClass			= NULL;
	Handle				= ILLEGALFHANDLE;
	pEnumLogFont		= NULL;
	pOutlineTextMetric	= NULL;
	FlagIsCorrupt		= FALSE;
	FlagIsReplaced		= FALSE;
}

CachedFontItem::~CachedFontItem()
{
	Delete();
}

/********************************************************************************************
	For all the rest of these functions see their FontManager equivalents
********************************************************************************************/

/********************************************************************************************

>	ENUMLOGFONT* CachedFontItem::GetEnumLogFont()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95
	Inputs:		-
	Returns:	A pointer to the ENUMLOGFONT structure for this font				
	Purpose:	Returns the ENUMLOGFONT structure for this cached font.

********************************************************************************************/

ENUMLOGFONT* CachedFontItem::GetEnumLogFont()
{
	ERROR2IF(pFontClass==NULL, NULL, "structure exists without a FontClass!!!");

	if (pEnumLogFont!=NULL)
		return pEnumLogFont;

	return NULL;
}

/********************************************************************************************

>	OUTLINETEXTMETRIC* CachedFontItem::GetOutlineTextMetric()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/08/96
	Inputs:		-
	Returns:	A pointer to the OUTLINETEXTMETRIC structure for this font, or NULL if
				one doesn't exist (non-truetype fonts).
	Purpose:	Returns the OUTLINETEXTMETRIC structure for this cached font.

********************************************************************************************/

OUTLINETEXTMETRIC* CachedFontItem::GetOutlineTextMetric()
{
	ERROR2IF(pFontClass==NULL, NULL, "this.pFontClass==NULL.");
	ERROR2IF(pEnumLogFont==NULL, NULL, "this.pEnumLogFont==NULL.");

	if (pOutlineTextMetric==NULL)
	{
		// ask the font class for this structure, and cache it
		FontClass Class = GetFontClass();
		pOutlineTextMetric=OILFontMan::GetOutlineTextMetric(Class, &(pEnumLogFont->elfLogFont));
	}

	// return the result.
	return pOutlineTextMetric;
}

/********************************************************************************************

>	String_64* CachedFontItem::GetFontName()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95
	Inputs:		-
	Returns:	A font name
	Purpose:	Get this items name

********************************************************************************************/

String_64* CachedFontItem::GetFontName()
{
	ERROR2IF(pFontClass==NULL, NULL, "structure exists without a FontClass!!!");
	return pFontClass->GetFontName();
}

/********************************************************************************************

>	FontClass CachedFontItem::GetFontClass()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95
	Inputs:		-
	Returns:	A font class
	Purpose:	Get this items class

********************************************************************************************/

FontClass CachedFontItem::GetFontClass()
{
	ERROR3IF(pFontClass==NULL, "structure exists without a FontClass!!!");
	return pFontClass->GetFontClass();
}

/********************************************************************************************

>	void CachedFontItem::DeleteMetrics()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95
	Inputs:		-
	Returns:	-
	Purpose:	Delete this items LOGFONT cached structure

********************************************************************************************/

void CachedFontItem::DeleteMetrics()
{
	if( NULL == pFontClass )
	{
		TRACE( _T("structure exists without a FontClass!!!") );
	}
	
	if (pEnumLogFont!=NULL)
	{
		delete pEnumLogFont;
		pEnumLogFont = NULL;
	}
	
	if (pOutlineTextMetric!=NULL)
	{
		delete pOutlineTextMetric;
		pOutlineTextMetric = NULL;
	}
}

void CachedFontItem::DeleteFontClass()
{
	if (pFontClass!=NULL)
	{
		delete pFontClass;
		pFontClass=NULL;
	}
}

void CachedFontItem::Delete()
{
	DeleteMetrics();	
	DeleteFontClass();
}

/********************************************************************************************

>	BOOL CachedFontItem::IsFullyCached()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95
	Inputs:		-
	Returns:	TRUE if this item is fully cached, FALSE if not
	Purpose:	See if this item has all its cached structures resident

********************************************************************************************/

BOOL CachedFontItem::IsFullyCached()
{
	PORTNOTE("text","CachedFontItem::IsFullyCached - do nothing");
#ifndef DISABLE_TEXT_RENDERING
	TRACEUSER("wuerthne", _T("CachedFontItem::IsFullyCached called") );
	ERROR2IF(pFontClass==NULL, FALSE, "A CachedFontItem structure exists without a FontClass!!!");
	return (pEnumLogFont!=NULL);
#else
	return TRUE;
#endif
}

/********************************************************************************************

>	BOOL CachedFontItem::Compare(String_64* pFontName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95
	Inputs:		pFontName = A pointer to a font name
	Returns:	TRUE if the font name on entry matchs that of this cached font
				FALSE if not
	Purpose:	See if this font has the same name as that passed as a param.

********************************************************************************************/

BOOL CachedFontItem::Compare(String_64 *pFontName)
{
	if( pFontClass == NULL )
	{
		TRACE( _T("A CachedFontItem structure exists without a FontClass!!!") );
		return FALSE;
	}
	
	return pFontClass->Compare( pFontName );
}	


/********************************************************************************************

>	void CachedFontItem::Dump()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95
	Purpose:	Dump the contents of this cache entry out

********************************************************************************************/


void CachedFontItem::Dump()
{
#ifdef _DEBUG
	ERROR3IF(pFontClass==NULL, "A CachedFontItem structure exists without a FontClass!!!");
	TRACE( _T("Font item:\n Handle = %d\n"),Handle);
	if (IsFullyCached())
	{		
		TRACE( _T(" Fully cached\n"));
		OILFontMan::DumpEnumLogFont(pEnumLogFont);
	}
	else
	{
		TRACE( _T(" Not fully cached\n"));
	}
	pFontClass->Dump();
#endif
}


/********************************************************************************************

>	BOOL CachedFontItem::SetCacheData(ENUMLOGFONT FAR* lpelf)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95
	Inputs:		lpelf	= a pointer to the logical font description
	Returns:	TRUE if we manage to cache this data
				FALSE if not (out of memory)				
	Purpose:	This function is called to set the cached data (or metrics) for a particular
				font item. The metrics will have been returned to us from an oil layer
				font manager after a request from us to it to cache them.

********************************************************************************************/

BOOL CachedFontItem::SetCacheData(ENUMLOGFONT FAR* lpelf)
{
	ERROR2IF(lpelf==NULL,FALSE,"CachedFontItem::SetCacheData() given a null ENUMLOGFONT structure");
	ERROR3IF(pFontClass==NULL, "A CachedFontItem structure exists without a FontClass!!!");

	if (pEnumLogFont!=NULL)
	{
		ERROR3("Replacing ENUMLOGFONT structure inside CachedFontItem::SetCacheData()");
		delete pEnumLogFont;
		pEnumLogFont=NULL;
	}

	// try a claim memory for the new structure
	ENUMLOGFONT * MyEnumLogFont = new ENUMLOGFONT;

	if (MyEnumLogFont==NULL)
	{
		return FALSE;
	}
	
	// copy the the contents of the structures given as arguments into the new structures
	*MyEnumLogFont = *lpelf;

	// and point the pointers to these new structures
	pEnumLogFont = MyEnumLogFont;

	return TRUE;
}



// ---------------------- End of CachedFontItem functions -----------------------------------


/********************************************************************************************

>	FontManager::FontManager() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95				
	Purpose:	Constructor for the font manager class

********************************************************************************************/

FontManager::FontManager()
{
	UniqueHandle  = FIRSTFONTHANDLE;
	DefaultHandle = DEFAULTHANDLE;
	TempFontClass = FC_UNDEFINED;
}

FontManager::~FontManager()
{
	TheFontList.DeleteAll();
}

/********************************************************************************************

>	BOOL FontManager::LegalFontClass(INT32 Class, FontClass& RealClass)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95				
	Inputs:		A INT32 describing the font class
	Outputs:	The real font class to use for this numeric value
	Returns:	TRUE if the font class is supported, FALSE if not
	Purpose:	Check whether a numeric value actually indicates a valid class of font
				ie lies in the range of supported font types. This is used generally for
				loading purpose where the font class is loaded as a numeric value.

********************************************************************************************/

BOOL FontManager::LegalFontClass(INT32 Class, FontClass& RealClass)
{
	return OILFontMan::LegalFontClass(Class, RealClass);
}


/********************************************************************************************

>	WORD FontManager::GetNextHandle()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		-
	Outputs:	The next font handle. 
	Returns:	-
	Purpose:	Return a unique handle for the font cache handler
				Handles range from 1..n, 0 is explicitly reserved for the default
				handle, 0x7FFF is defined as an illegal handle.

********************************************************************************************/

WORD FontManager::GetNextHandle()
{
	WORD CurrHandle=UniqueHandle;
	UniqueHandle++;
	return (CurrHandle);
}

/********************************************************************************************

>	BOOL FontManager::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95				
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the fontmanager was able to initialise correctly
				FALSE if not
	Purpose:	Get Camelots Font manager going. We simply cache the default font so
				unknown fonts can be sensibly replaced.
	
********************************************************************************************/

BOOL FontManager::Init()
{
	if (!CacheDefaultFont())	
	{
		ERROR3("FontManager::Init() - Unable to cache the default font");
	}
	return TRUE;
}

/********************************************************************************************

>	CachedFontItem* FontManager::GetFirstFont()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Returns:	A pointer to the first cached font item (NULL if none)

********************************************************************************************/

CachedFontItem* FontManager::GetFirstFont()
{
	return (CachedFontItem*)TheFontList.GetHead();
}

/********************************************************************************************

>	CachedFontItem* FontManager::GetNextFont(CachedFontItem *pItem)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		A pointer to a font item to find the next from.
	Returns:	A pointer to the next cached font item from pItem 
				(NULL if none)

********************************************************************************************/

CachedFontItem* FontManager::GetNextFont(CachedFontItem* pItem)
{
	return (CachedFontItem*)TheFontList.GetNext(pItem);
}



/********************************************************************************************

>	CachedFontItem* FontManager::AddFont(String_64* Name, FontClass fclass, WORD& Handle)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		Name	= a pointer to the font name
				fclass	= a reference to the class of font to add
	Outputs:	Handle = ILLEGALFHANDLE if the font could not be added
				Handle = The font handle if ok
	Returns:	NULL if the font could not be added
				A pointer to the font record
	Purpose:	Creates a new font item for this named font. The font will not be fully
				cached. To cache all the data for the font, call CacheFontData(Handle)

********************************************************************************************/

CachedFontItem* FontManager::AddFont(String_64* Name, FontClass fclass, WORD& retHandle)
{

	CachedFontItem* pItem = new	CachedFontItem;
	if (pItem==NULL)
		return NULL;

	FontBase* pClass = OILFontMan::CreateNewFont(fclass,Name);
	if (pClass==NULL)
	{
		delete pItem;
		pItem=NULL;
		return NULL;
	}

	pItem->pFontClass = pClass;
	pItem->Handle = GetNextHandle();
	TheFontList.AddTail(pItem);

	retHandle = pItem->Handle;

	return pItem;
}

/********************************************************************************************

>	CachedFontItem* FontManager::FindFont(WORD Handle)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		Handle = The font manager font handle
	Outputs:	-
	Returns:	A pointer to the font managers cached font record for this handle
	Purpose:	Find the entry in the font managers font cache which corresponds to this
				handle.

********************************************************************************************/

CachedFontItem* FontManager::FindFont(WORD Handle)
{
	// Check the cache first
	CachedFontItem* pItem = GetFirstFont();
	while (pItem!=NULL)
	{
		if (pItem->Handle == Handle)
			return pItem;
		pItem = GetNextFont(pItem);
	}
	return NULL;
}

/********************************************************************************************

>	CachedFontItem* FontManager::FindFont(String_64* pFontName, FontClass Class=FC_UNDEFINED)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pFontName = a pointer to a font name
				Class	  = describes the type of class (see typedef for details)
							FC_UNDEFINED then ignore the class match and match the first
							item with the same name whatever font class it is
	Outputs:	-
	Returns:	A pointer to the font managers cached font record for this font
				NULL if unable to find the font
	Purpose:	Find the entry in the font managers font list which corresponds to this
				fontname and font class

********************************************************************************************/

CachedFontItem* FontManager::FindFont(String_64* pFontName, FontClass Class)
{
	CachedFontItem* pItem = GetFirstFont();

	if (Class==FC_UNDEFINED)
	{
		while (pItem!=NULL)
		{
			if (pItem->Compare(pFontName))
				return pItem;
			pItem = GetNextFont(pItem);	
		}
	}
	else
	{
		while (pItem!=NULL)
		{
			if (pItem->Compare(pFontName))
			{
				if (pItem->GetFontClass() == Class)
					return pItem;
			}
			pItem = GetNextFont(pItem);	
		}
	}
	return NULL;
}

/********************************************************************************************

>	CachedFontItem* FontManager::GetCachedFont(WORD Handle)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		Handle = The font manager font handle
	Outputs:	-
	Returns:	A pointer to a cached font record or NULL
	Purpose:	This function returns the font associated with the handle passed. If the
				font is not in the cache it will return NULL. It differs from GetFont in
				that it will not attempt to return the DefaultFontItem if it cannot find
				the item specified.

********************************************************************************************/

CachedFontItem* FontManager::GetCachedFont(WORD Handle)
{
	if (Handle==DefaultHandle) return (&DefaultFontItem);
	return FindFont(Handle);
}

/********************************************************************************************

>	WORD FontManager::GetFontHandle(String_64* pFontName, FontClass Class=FC_UNDEFINED)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pFontName = a pointer to a font name
				Class	  = describes the class of font to get the handle of
	Outputs:	-
	Returns:	The handle of the font if the font exists
				The default handle if the font does not exist and Replace is true
				ILLEGALFHANDLE if not
	Purpose:	Get the handle on a cached font (if cached) otherwise return the default
				font handle.

********************************************************************************************/

WORD FontManager::GetFontHandle(String_64* pFontName, FontClass Class)
{
	ERROR2IF(pFontName==NULL, 0, "FontManager::GetFontHandle() called with an illegal font name pointer");
	CachedFontItem* pItem=FindFont(pFontName,Class);
	if (pItem!=NULL)
		return pItem->GetFontHandle();
	
	return DefaultHandle;
}

/********************************************************************************************

>	CachedFontItem* FontManager::GetFont()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to a cached font record or NULL
	Purpose:	Return the precached default font data to use for rendering.
	
********************************************************************************************/

CachedFontItem* FontManager::GetFont()
{
	return &DefaultFontItem;
}

/********************************************************************************************

>	CachedFontItem* FontManager::GetFont(WORD Handle)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		Handle = The font manager font handle
	Outputs:	-
	Returns:	A pointer to a cached font record or NULL
	Purpose:	Return the font data to use for rendering. This function performs a specific
				job in avoiding caching the font data. If the font is already cached then
				fine, it is returned for use. If not, the default font is returned.

********************************************************************************************/

CachedFontItem* FontManager::GetFont(WORD Handle)
{
	CachedFontItem* pItem = FindFont(Handle);
	if ((pItem!=NULL) && (pItem->IsFullyCached()))
		return pItem;
	return &DefaultFontItem;
}

/********************************************************************************************

>	CachedFontItem* FontManager::GetFont(String_64* pFontName, FontClass Class=FC_UNDEFINED)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pFontName = a pointer to a font name
				Class	  = describes the type of class (see typedef for details)
							FC_UNDEFINED then ignore the class match and match the first
							item with the same name whatever font class it is
	Outputs:	-
	Returns:	A pointer to a cached font record or NULL
	Purpose:	Return the font data to use for rendering. This function performs a specific
				job in avoiding caching the font data. If the font is already cached then
				fine, it is returned for use. If not, the default font is returned.

********************************************************************************************/

CachedFontItem* FontManager::GetFont(String_64 *pFontName, FontClass Class)
{
	CachedFontItem* pItem = FindFont(pFontName, Class);
	if ((pItem!=NULL) && (pItem->IsFullyCached()))
		return pItem;
	return &DefaultFontItem;
}

/********************************************************************************************

>	INT32 FontManager::DoesFontExist(String_64* pFontName, 
									BOOL SearchCache,
									BOOL SearchInstalled) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/95
	Inputs:		FontName : Name of font to find
				SearchCache : TRUE then search our internal cached fonts
				SearchInstalled: TRUE then search through the installed set of fonts too
	Outputs:	-
	Returns:	0 if the font cannot be found in the specified search areas
				1 if the font has been installed
				2 if the font has been cached
	Purpose:	First check in the cache if necessary, then through the installed set of
				fonts to find whether the named font actually exists

********************************************************************************************/

INT32 FontManager::DoesFontExist(String_64 *pFontName, BOOL SearchCache, BOOL SearchInstalled)
{
	if (SearchCache)
		if (FindFont(pFontName) != NULL)
			return 2;

	if (SearchInstalled)
		if (IsFontInstalled(pFontName))
			return 1;

	return 0;
}

/********************************************************************************************

>	BOOL FontManager::IsFontValid(WORD Handle)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		Handle = The font manager font handle
	Outputs:	-
	Returns:	TRUE if the font is valid and ready to use
				FALSE if not.
	Purpose:	Check to see if the font described is actually valid, ie all the metrics
				for the font have been cached correctly.

********************************************************************************************/
	
BOOL FontManager::IsFontValid(WORD Handle)
{
	CachedFontItem* pItem = GetCachedFont(Handle);
	return ( (pItem!=NULL) && (pItem->IsFullyCached()) );
}

/********************************************************************************************

>	BOOL FontManager::IsFontReplaced(WORD FontHandle)
	BOOL FontManager::IsFontReplaced(String_64* pFontName, FontClass Class=FC_UNDEFINED)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pFontName = a pointer to a font name
				Class	  = describes the type of class (see typedef for details)
							FC_UNDEFINED then ignore the class match and match the first
							item with the same name whatever font class it is
	Outputs:	-
	Returns:	TRUE if the font has been replaced by the default
				FALSE if not.
	Purpose:	Check to see if the font described has been replaced.

********************************************************************************************/

BOOL FontManager::IsFontReplaced(WORD FontHandle)
{
	BOOL Result = FALSE;

	// check whether its the default font, if it is then obviously it's not been replaced.
	if (IsFontDefault(FontHandle))
	{
		Result = FALSE;
	}
	else
	{
		CachedFontItem *pItem = FindFont(FontHandle);

		if (pItem==NULL)
		{
			Result = TRUE;		// font doesn't exist
		}
		else if (pItem->IsReplaced())
		{
			Result = TRUE;		// IsReplaced flag is set
		}
		else if (!pItem->IsValid())
		{
			Result = TRUE;		// The font isn't valid
		}
	}

	return Result;
}

BOOL FontManager::IsFontReplaced(String_64* pFontName, FontClass Class)
{
	BOOL Result = FALSE;

	// Check whether its the default font, if it is then obviously it's not been replaced.
	if (DefaultFontItem.Compare(pFontName))
	{
		Result = FALSE;
	}
	else
	{
		CachedFontItem *pItem = FindFont(pFontName, Class);

		if (pItem==NULL)
		{
			Result = TRUE;		// font doesn't exist
		}
		else if (pItem->IsReplaced())
		{
			Result = TRUE;		// IsReplaced flag is set
		}
		else if (!pItem->IsValid())
		{
			Result = TRUE;		// The font isn't valid
		}
	}

	return Result;
}

/********************************************************************************************

>	BOOL FontManager::IsFontDefault(WORD Handle)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		Handle = The font manager font handle
	Outputs:	-
	Returns:	TRUE if the font handle is the handle describing the default font
				FALSE if not.
	Purpose:	Check to see if the font handle described is the handle of the default font
				ie the font which is used to replace unknown fonts (fonts not resident on
				the host machine)

********************************************************************************************/

BOOL FontManager::IsFontDefault(WORD Handle)
{
	return (Handle==DefaultHandle);
}

BOOL FontManager::IsFontDefault(CachedFontItem* pItem)
{
	return (pItem == (&DefaultFontItem));
}

/********************************************************************************************

>	CachedFontItem* FontManager::GetFirstFontType(FontClass Class)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		Class = the class of font to retrieve
	Outputs:	-
	Returns:	A pointer to a cached font record or NULL
	Purpose:	Find the entry in the font managers font list which corresponds to the first
				'FontClass' font cached.

********************************************************************************************/

CachedFontItem* FontManager::GetFirstFontType(FontClass Class)
{
	CachedFontItem* pItem = GetFirstFont();
	while (pItem!=NULL)
	{
		if (pItem->GetFontClass() == Class)
			return pItem;
		pItem = GetNextFont(pItem);
	}
	return NULL;
}

/********************************************************************************************

>	ENUMLOGFONT* FontManager::GetEnumLogFont(WORD Handle)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		Handle = The font manager font handle
	Outputs:	-
	Returns:	A pointer to the ENUMLOGFONT structure for this font
				NULL if none
	Purpose:	Return the ENUMLOGFONT structure for this font handle.
				Note the function will automatically substitute the default font if the
				handle provided is invalid or the font it describes is not fully cached.

********************************************************************************************/

ENUMLOGFONT* FontManager::GetEnumLogFont(WORD Handle)
{
	// ok, we need to get the font data for this particular font item	
	CachedFontItem *pItem = GetFont(Handle);
	if (pItem!=NULL)
	{
		return (pItem->GetEnumLogFont());
	}
	else
	{
		return NULL;
	}
}

/********************************************************************************************

>	OUTLINETEXTMETRIC* FontManager::GetOutlineTextMetric(WORD Handle)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/08/96
	Inputs:		Handle = the font manager handle
	Returns:	Pointer to the OUTLINETEXTMETRIC structure for this font, NULL if none
	Purpose:	Return the ENUMLOGFONT structure for this font handle
				Note the function will automatically substitute the default font if the
				handle provided is invalid or the font it describes is not fully cached.

********************************************************************************************/

OUTLINETEXTMETRIC* FontManager::GetOutlineTextMetric(WORD Handle)
{
	// ok, we need to get the font data for this particular font item
	CachedFontItem *pItem = GetFont(Handle);
	if (pItem!=NULL)
	{
		return pItem->GetOutlineTextMetric();
	}
	else
	{
		return NULL;
	}
}

/********************************************************************************************

>	String_64* FontManager::GetFontName(WORD Handle)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		Handle = The font manager font handle
	Outputs:	-
	Returns:	A pointer to the font name associated with this handle
				NULL if the handle is illegal
	Purpose:	Return a pointer to the font name given its font handle

********************************************************************************************/

String_64* FontManager::GetFontName(WORD Handle)
{
	CachedFontItem* pItem = GetCachedFont(Handle);
	if (pItem==NULL)
	{
		ERROR3("FontManager::GetFontName() - given an illegal handle");
		return NULL;
	}
	return pItem->GetFontName();
}


BOOL FontManager::GetFontName(WORD Handle, String_64& OutputString)
{
	CachedFontItem* pItem = GetCachedFont(Handle);
	if (pItem==NULL)
	{
		ERROR3("FontManager::GetFontName() - given an illegal handle");
		return FALSE;
	}
	OutputString = *(pItem->GetFontName());
	return TRUE;
}

/********************************************************************************************

>	FontClass FontManager::GetFontClass(WORD Handle)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		Handle = The font manager font handle
	Outputs:	-
	Returns:	A font class
				FC_UNDEFINED if the handle is unknown
	Purpose:	Return what class of font this handle is. 

********************************************************************************************/

FontClass FontManager::GetFontClass(WORD Handle)
{
	CachedFontItem* pItem = GetCachedFont(Handle);
	if (pItem==NULL)
	{
		ERROR3("FontManager::GetFontClass() - given an illegal handle");
		return FC_UNDEFINED;
	}
	return pItem->GetFontClass();
}
	
/********************************************************************************************

>	WORD FontManager::FindTypeface(String_64 &TypeFacename,
								   FontClass Class)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/08/96
	Inputs:		TypeFaceName	- Typeface name of font to search for
				Class			- font class to search for
	Returns:	Font handle of typeface, or ILLEGALFHANDLE if not found
	Purpose:	Used by the new file format, to check if a particular typeface (and font class)
				is installed and available.
	See Also:	class FontComponent

********************************************************************************************/

WORD FontManager::FindTypeface(String_64 &TypeFaceName, FontClass Class)
{
	WORD FontHandle = ILLEGALFHANDLE;

	CachedFontItem *pItem=FindFont(&TypeFaceName, Class);

	if (pItem!=NULL)
	{
		// font is in the cached font list.
		FontHandle=pItem->Handle;
	}
	// now scan the installed lists for this font, and set the local temp font data
	else if (TempCacheNamedFont(&TypeFaceName, Class, 1))
	{
		AddTempFont(&TypeFaceName, Class, FontHandle);
	}
	// Okay, so we couldn't find the exact font. It may be possible to create the
	// font we are after. ATM for instance allows named font instances to be created
	// from the base multiple master outlines.
	else if (TempCacheNamedFont(&TypeFaceName, Class, 2))
	{
		AddTempFont(&TypeFaceName, Class, FontHandle);
	}
	else
	// completly unsuccessful. :(
	{
		FontHandle=ILLEGALFHANDLE;
	}

	return FontHandle;
}

/********************************************************************************************

>	WORD FontManager::FindClosestFont(const CCPanose &PanoseNumber)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96
	Inputs:		PanoseNumber	- panose number to search against
	Returns:	Font handle of the closest font
	Purpose:	Performs panose matching on the installed fonts

********************************************************************************************/

WORD FontManager::FindClosestFont(const CCPanose &PanoseNumber)
{
	// check, is this a 'simple' panose number with just Weight and Letterform components?
	if (PanoseNumber.GetFamilyType()==PAN_ANY &&
		PanoseNumber.GetSerifStyle()==PAN_ANY &&
		PanoseNumber.GetProportion()==PAN_ANY &&
		PanoseNumber.GetContrast()==PAN_ANY &&
		PanoseNumber.GetStrokeVariation()==PAN_ANY &&
		PanoseNumber.GetArmStyle()==PAN_ANY &&
		PanoseNumber.GetMidline()==PAN_ANY &&
		PanoseNumber.GetXHeight()==PAN_ANY)
	{
		// These 'simple' panose numbers get saved out for old files and ATM fonts. Obviously
		// we can't do any sensible font matching, so just return the default font handle.
		return DEFAULTHANDLE;
	}
	else
	{
		// shove the data we need into class members.
		FindClosestFontHandle = ILLEGALFHANDLE;
		FindClosestPanoseNumber = PanoseNumber;
		FindClosestDistance = 0xffffffff;
		FindClosestTypeFaceName = _T("");
		FindClosestFontClass = FC_UNDEFINED;

		// normalise the panose number.
		FindClosestPanoseNumber.BodgeToNormal();

		// choose whichever panose font matching algorithm you require:
PORTNOTE("text","TrueType/Win specific code removed")
#ifndef EXCLUDE_FROM_XARALX
		// quick - just checks the seven fonts from the Microsoft(tm) Web Font Pack(tm)
		return FindClosestFontQuick(PanoseNumber);
#endif

		// full - checks all the fonts installed on the machine.
		return FindClosestFontFull(PanoseNumber);
	}
} 

PORTNOTE("text","TrueType/Win specific code removed")
#ifndef EXCLUDE_FROM_XARALX
WORD FontManager::FindClosestFontQuick(const CCPanose &PanoseNumber)
{
	WORD MyFontHandle;
	String_64 MyFontName;

	// try "Arial"
	MyFontName = _T("Arial");
	MyFontHandle = CacheNamedFont(&MyFontName, FC_TRUETYPE);
	FindClosestFontQuickTry(MyFontHandle);

	// try "Arial Black"
	MyFontName = _T("Arial Black");
	MyFontHandle = CacheNamedFont(&MyFontName, FC_TRUETYPE);
	FindClosestFontQuickTry(MyFontHandle);

	// try "Comic Sans"
	MyFontName = _T("Comic Sans");
	MyFontHandle = CacheNamedFont(&MyFontName, FC_TRUETYPE);
	FindClosestFontQuickTry(MyFontHandle);

	// try "Courier New"
	MyFontName = _T("Courier New");
	MyFontHandle = CacheNamedFont(&MyFontName, FC_TRUETYPE);
	FindClosestFontQuickTry(MyFontHandle);

	// try "Impact"
	MyFontName = _T("Impact");
	MyFontHandle = CacheNamedFont(&MyFontName, FC_TRUETYPE);
	FindClosestFontQuickTry(MyFontHandle);

	// try "Times New Roman"
	MyFontName = _T("Times New Roman");
	MyFontHandle = CacheNamedFont(&MyFontName, FC_TRUETYPE);
	FindClosestFontQuickTry(MyFontHandle);

	// try "Verdana"
	MyFontName = _T("Verdana");
	MyFontHandle = CacheNamedFont(&MyFontName, FC_TRUETYPE);
	FindClosestFontQuickTry(MyFontHandle);

	return FindClosestFontHandle;
} 


BOOL FontManager::FindClosestFontQuickTry(WORD MyFontHandle)
{
	OUTLINETEXTMETRIC *pOutlineTextMetric = GetOutlineTextMetric(MyFontHandle);

	if (pOutlineTextMetric!=NULL)
	{
		CCPanose MyPanoseNumber = pOutlineTextMetric->otmPanoseNumber;

		MyPanoseNumber.BodgeToNormal();

		UINT32 MyDistance = CCPanose::Distance(FindClosestPanoseNumber, MyPanoseNumber);

		if (MyDistance<FindClosestDistance)
		{
			FindClosestFontHandle = MyFontHandle;
			FindClosestDistance = MyDistance;
		}
	}

	return TRUE;
}
#endif

WORD FontManager::FindClosestFontFull(const CCPanose &PanoseNumber)
{
	WORD MyFontHandle;
	CachedFontItem *pCachedFontItem;

	// this is particularly rubbish code, since we ask the OS for information about each font
	// (the OUTLINETEXTMETRIC structure) and then throw it away if its not needed, rather than
	// caching it for the next time. Of course, if this font manager actually held information
	// about all the fonts on the system, rather than those in use, it would all be much easier.

	// call the OILFontMan to find the closest font.
	OILFontMan::FindClosestFont();

	// We've got the typeface name and font class of the best match, so get the font handle
	pCachedFontItem = FindFont(&FindClosestTypeFaceName, FindClosestFontClass);

	if (pCachedFontItem!=NULL)
	{
		// the font is already cached, so grab the font handle from the CachedFontItem
		MyFontHandle=pCachedFontItem->Handle;
	}
	else
	{
		// the font hasn't been cached yet, so cache it now.
		pCachedFontItem = AddFont(&FindClosestTypeFaceName, FindClosestFontClass, MyFontHandle);
		pCachedFontItem->SetCacheData(&FindClosestEnumLogFont);
	}

	// return with this font handle
	return MyFontHandle;
}

BOOL FontManager::FindClosestFontFullTry(FontClass Class, String_64 *pTypeFaceName, ENUMLOGFONT *pEnumLogFont)
{
	PORTNOTETRACE("text","FontManager::FindClosestFontFullTry - do nothing");
	ERROR2IF(pEnumLogFont==NULL, FALSE, "FindClosestTry called with pEnumLogFont==NULL.");

	OUTLINETEXTMETRIC *pOutlineTextMetric;
	CCPanose MyPanoseNumber;
	UINT32 MyDistance;

	pOutlineTextMetric = OILFontMan::GetOutlineTextMetric(Class, &(pEnumLogFont->elfLogFont));

	if (pOutlineTextMetric!=NULL)
	{
		MyPanoseNumber = (pOutlineTextMetric->otmPanoseNumber);
		MyPanoseNumber.BodgeToNormal();

		MyDistance = CCPanose::Distance(FindClosestPanoseNumber, MyPanoseNumber);

		if (MyDistance<FindClosestDistance)
		{
			FindClosestDistance = MyDistance;
			FindClosestFontClass = Class;
			FindClosestTypeFaceName = *(pTypeFaceName);
			FindClosestEnumLogFont = *(pEnumLogFont);
		}

		free(pOutlineTextMetric);
	}
	
	return TRUE;
}

/********************************************************************************************
>	void FontManager::InvalidateCharMetrics()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/96
	Purpose:	Invalidate char metrics cache
********************************************************************************************/

void FontManager::InvalidateCharMetrics()
{
	FontMetricsCache::InvalidateCharMetrics();
}

/********************************************************************************************
>	BOOL FontManager::GetCharMetrics(CDC* pDC, WCHAR ch, CharDescription& FontDesc, CharMetrics* pCharMetrics);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/96
	Inputs:		pDC      - pointer to DC (rather than creating one each time)
				ch       - the char whose metrics are required
				FontDesc - the font
	Outputs:	pCharMetrics - the metrics
	Returns:	FALSE if fails
	Purpose:	Get the char metrics for the given char and font
				Just dispenses the call to the FontMetricsCache
********************************************************************************************/

BOOL FontManager::GetCharMetrics(CNativeDC* pDC, WCHAR ch, CharDescription& FontDesc, CharMetrics* pCharMetrics)
{
	return FontMetricsCache::GetCharMetrics(pDC, ch, FontDesc, pCharMetrics);
}

/********************************************************************************************
>	MILLIPOINT FontManager::GetCharsKerning(CDC* pDC, WCHAR chLeft, WCHAR chRight, CharDescription& FontDesc);

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/2000
	Inputs:		pDC			- pointer to DC (rather than creating one each time)
				chLeft		- the left char of a kerning pair
				chRight		- the right char of a kerning pair
				FontDesc	- the font
	Returns:	Kern distance between chars or 0 on error or no kern
	Purpose:	Finds the kerning distance between two chars
********************************************************************************************/

MILLIPOINT FontManager::GetCharsKerning(CNativeDC* pDC, WCHAR chLeft, WCHAR chRight, CharDescription& FontDesc)
{
	// TRACEUSER("wuerthne", _T("FontManager::GetCharsKerning called\n") );
	return FontKerningPairsCache::GetCharsKerning(pDC, chLeft, chRight, FontDesc);
}

/********************************************************************************************

>	static BOOL FontManager::GetCharPath(CharDescription& ChDesc,
										 DocCoord** ppCoords,
										 PathVerb** ppVerbs,
										 UINT32* pNumCoords,
										 CDC* pDC=NULL)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		ChDesc	= description of char
				pDC		= (possible) pointer to a DC (for optimisation) defaults to NULL
	Outputs:	ppCoords	= pointer to path coords buffer
				ppVerbs		= pointer to path verbs buffer
				pNumCoords	= number of elements in path
	Returns:	FALSE if unable to return the outline
	Purpose:	Get the path associated with a given char

********************************************************************************************/

BOOL FontManager::GetCharPath(CharDescription& ChDesc,
							  DocCoord** ppCoords,
							  PathVerb** ppVerbs,
							  UINT32* pNumCoords,
							  CNativeDC* pDC)
{
	ERROR2IF(ppCoords==NULL,FALSE,"FontManager::GetCharPath pCoords==NULL");
	ERROR2IF(ppVerbs==NULL,FALSE,"FontManager::GetCharPath ppVerbs==NULL");
	ERROR2IF(pNumCoords==NULL,FALSE,"FontManager::GetCharPath pNumCoords==NULL");

	CachedFontItem* pItem = GetFont(ChDesc.GetTypefaceHandle());
	if (pItem==NULL)
		return FALSE;

	if (pItem->IsCorrupt())
		return FALSE;

	BOOL Success = OILFontMan::GetCharPath(pItem->GetFontClass(), ChDesc, ppCoords, ppVerbs, pNumCoords, pDC);
	if (!Success)
		pItem->SetIsCorrupt(TRUE);

	return Success;
}

/********************************************************************************************

>	BOOL FontManager::CacheDefaultFont()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the default font has been cached
				FALSE if not
	Purpose:	Cache the default font so we can use this as a replacement when unable to
				find strange fonts.

********************************************************************************************/

BOOL FontManager::CacheDefaultFont()
{
	PORTNOTE("text","FontManager::CacheDefaultFont - do nothing");
#ifndef DISABLE_TEXT_RENDERING
	if (DefaultFontItem.pFontClass!=NULL)
	{
		ERROR3("CacheDefaultFont called but a default font is already resident");
		return TRUE;
	}

	// now scan all installed lists an set out local temp font data
	String_64 DefFont(DEFAULTFONTNAME);

	if (!TempCacheNamedFont(&DefFont))
		if (!TempCacheNamedFont(NULL))
			return FALSE;

	FontBase* pClass = OILFontMan::CreateNewFont(TempFontClass,&TempFontName);
	if (pClass==NULL)
		return FALSE;

	DefaultFontItem.pFontClass = pClass;
	DefaultFontItem.Handle = DEFAULTHANDLE;

	if (!DefaultFontItem.SetCacheData(&TempEnumLogFont))
	{
		DefaultFontItem.Delete();
		return FALSE;
	}

	return TRUE;
#else
	return TRUE;
#endif
}

/********************************************************************************************

>	void FontManager::ResetDefaultFont()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Cache the default font so we can use this as a replacement when unable to
				find strange fonts.	The cache is checked first to see if the default font
				is already there.

********************************************************************************************/

void FontManager::ResetDefaultFont()
{
	DefaultFontItem.Delete();
	CacheDefaultFont();
}

/********************************************************************************************

>	WORD FontManager::CacheNamedFont(String_64* pFontName, FontClass Class = FC_UNDEFINED)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pFontName = a pointer to a font name
				Class	  : the class of font to cache
						    FC_UNDEFINED - then try to cache any font by asking each
							font manager in turn.

	Outputs:	-
	Returns:	ILLEGALFHANDLE - if unable to cache the named font
				A unique font handle if cached
	Purpose:	Given a font name try to cache all the data for that font so that we can
				actually us it. The cache will be checked first to make sure the font
				is not already installed.

********************************************************************************************/

WORD FontManager::CacheNamedFont(String_64* pFontName, FontClass Class)
{
	ERROR2IF(pFontName==NULL, ILLEGALFHANDLE, "Bad fontname passed to CacheNamedFont");

	// try to find the font in the cache first
	CachedFontItem *pItem = FindFont(pFontName, Class);
	if (pItem!=NULL)
		return pItem->Handle;
	
	// now scan all installed lists and set our local temp font data
	if (!TempCacheNamedFont(pFontName, Class, 1))
	{
		// ok we couldn't find the exact font. It may be possible
		// to create the font we are after. ATM for instance allows
		// named font instances to be created from their base multiple
		// master outlines.
		TempCacheNamedFont(pFontName, Class, 2);
	}
	
	// add the font that has been returned to us, even if none has.
	// if no font has been installed then we will end up having a font cache
	// object which describes a font as invalid. This is fine as we now know
	// we've done a search for it and it does not exist so we'll use the default
	WORD hndle=ILLEGALFHANDLE;
	AddTempFont(pFontName, Class, hndle);
		
	return (hndle);
}

/********************************************************************************************

>	BOOL FontManager::TempCacheNamedFont(String_64 *pFontName, FontClass Class=FC_UNDEFINED, INT32 Pass=1) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/95				
	Inputs:		pFontName : Name of font to search for
				Class	  : The class of font to cache
						    FC_UNDEFINED - then try to cache any font
				Pass	  : 1 - first pass, try to match the font exactly
						  : 2 - second pass, try to find a compatible font
	Outputs:	-
	Returns:	TRUE if the font has been temporarily installed
	Purpose:	Search through all the installed sets of fonts for the named font and
				place the result in a temp font.

********************************************************************************************/

BOOL FontManager::TempCacheNamedFont(String_64* pFontName, FontClass Class, INT32 Pass)
{
	return (OILFontMan::CacheNamedFont(pFontName, Class, Pass));
}

/********************************************************************************************

>	BOOL FontManager::IsFontInstalled(String_64* pFontName, FontClass Class = FC_UNDEFINED)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/95				
	Inputs:		pFontName : Name of font to search for
				Class	  : The class of font to cache
						    FC_UNDEFINED - then try to cache any font
	Outputs:	-
	Returns:	TRUE if the font has been installed
	Purpose:	Search through all the installed sets of fonts for the named font

********************************************************************************************/

BOOL FontManager::IsFontInstalled(String_64* pFontName, FontClass Class)
{
	return TempCacheNamedFont(pFontName, Class);
}

/********************************************************************************************

>	BOOL FontManager::IsFullyCached(WORD Handle)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		Handle = The font manager font handle
	Outputs:	-
	Returns:	TRUE if the font exists and is cached
				FALSE if not
	Purpose:	Find out if the font is fully cached and ready to use. This will generally
				be true of all fonts added by CacheNamedFont. If the font is not fully
				cached then call CacheFontData(). It will not be if AddFont has been used
				to add the font to the font list.

********************************************************************************************/

BOOL FontManager::IsFullyCached(WORD Handle)
{
	CachedFontItem* pItem = GetCachedFont(Handle);
	if (pItem!=NULL)
		return pItem->IsFullyCached();
	return FALSE;
}

/********************************************************************************************

>	void FontManager::RefreshCache() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/95
	Purpose:	Scans all the cached fonts and checks to see if any are invalid. If a font
				is invalid it is attempted to be made valid.
				This function is generally called when the external worlds fonts have
				changed via install/deinstall.

********************************************************************************************/

void FontManager::RefreshCache()
{
	InvalidateCache();
	OILFontMan::ValidateCache();
	ResetDefaultFont();
}

/********************************************************************************************

	void FontManager::ValidateItem(FontClass Class, String_64 * pFontName, ENUMLOGFONT * lpelf)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/95
	Purpose:	Used as a way of revalidating a cached font. This function is called from
				the various oil level font managers when we've asked them to refresh our
				cache. They simply need to scan their font lists and call this function with
				the new data.

********************************************************************************************/

void FontManager::ValidateItem(FontClass Class, String_64 * pFontName, ENUMLOGFONT * lpelf)
{
	CachedFontItem *pItem = FindFont(pFontName,Class);
	if (pItem)
	{
		pItem->SetCacheData(lpelf);
		pItem->SetIsReplaced(FALSE);
	}
}

/********************************************************************************************

>	void FontManager::InvalidateCache()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/95
	Purpose:	Scans all the cached fonts deletes all cached data. If the entries are not
				recached all visible fonts will be replaced by the default font.

********************************************************************************************/

void FontManager::InvalidateCache()
{
	CachedFontItem *pItem = GetFirstFont();
	while (pItem)
	{
		pItem->DeleteMetrics();
		pItem->SetIsCorrupt(FALSE);
		pItem->SetIsReplaced(TRUE);
		pItem = GetNextFont(pItem);
	}
}

/********************************************************************************************

>	void FontManager::DumpFontCache()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Purpose:	Dubugging facitity - dumps out the contents of our cache.
	
********************************************************************************************/

void FontManager::DumpFontCache()
{
	CachedFontItem *pItem = GetFirstFont();
	while (pItem)
	{
		pItem->Dump();
		pItem = GetNextFont(pItem);
	}
}	

/********************************************************************************************

>	void FontManager::DumpFont(WORD Handle)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Purpose:	Dubugging facitity - dumps out the contents of a font from our cache.
	
********************************************************************************************/

void FontManager::DumpFont(WORD Handle)
{
	CachedFontItem *pItem = FindFont(Handle);
	if (pItem)
		pItem->Dump();
}	

/********************************************************************************************

>	void FontManager::ClearTempFont()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Purpose:	Throws away any font data stored in the temp font.
	
********************************************************************************************/

void FontManager::ClearTempFont()
{
	TempFontClass=FC_UNDEFINED;
	TempFontName.Empty();
}

/********************************************************************************************

>	void FontManager::SetTempFont(FontClass Class, String_64 * pFontName, ENUMLOGFONT * lpelf)
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Purpose:	This function is used by the oil level font managers to set a temporary
				font. There are various other font manager functions which can process the
				temp font.
	
********************************************************************************************/

void FontManager::SetTempFont(FontClass Class, String_64 * pFontName, ENUMLOGFONT * lpelf)
{
	ERROR3IF(lpelf==NULL,"FontManager::SetTempFont() given a null ENUMLOGFONT structure");
	ERROR3IF(Class==FC_UNDEFINED, "FontManager::SetTempFont() given an illegal font class");
	
	TempFontClass=Class;
	TempFontName=(*pFontName);
	TempEnumLogFont=(*lpelf);
}

/********************************************************************************************

>	CachedFontItem* FontManager::AddTempFont(String_64* pFontName, FontClass Class, WORD& hndle)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pFontName = a pointer to the font name
				Class	  = describes the type of class (see typedef for details)
							FC_UNDEFINED then ignore the class match and match the first
							item with the same name whatever font class it is
	Outputs:	Handle	  = ILLEGALFHANDLE if the font could not be added
				Handle	  = The font handle if ok
	Returns:	NULL if the font could not be added
				A pointer to the font record
	Purpose:	Given the temp font data, this function goes and caches that data under
				the provided name and returns the font handle
	
********************************************************************************************/

CachedFontItem* FontManager::AddTempFont(String_64* pFontName, FontClass Class, WORD& hndle)
{
	CachedFontItem* pItem = NULL;
	if (TempFontClass==FC_UNDEFINED)
	{
		if (Class!=FC_UNDEFINED)
			pItem = AddFont(pFontName, Class, hndle);
		else
		{
//			ERROR3("Trying to add a font of an unknown class");
			pItem = AddFont(pFontName, FC_TRUETYPE, hndle);
		}
	}
	else
	{	
		pItem = AddFont(pFontName, TempFontClass, hndle);
		if (pItem)
			pItem->SetCacheData(&TempEnumLogFont);
	}	
	return pItem;
}

/********************************************************************************************

>	BOOL FontManager::TempFontValid()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Returns:	TRUE if the temp font has been set to a valid font
				FALSE if not
	
********************************************************************************************/

BOOL FontManager::TempFontValid()
{
	return (TempFontClass!=FC_UNDEFINED);
}

/********************************************************************************************

>	void FontManager::EncodeFontName(String_64& FontName, String_64& Encoded, INT32 Styles)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/95
	Inputs:		FontName	A reference to a font name to encode
				Encoded		'none constant' string reference to a output string buffer
				Styles		= 1 then try to append 'Bold'
							= 2 then try to append 'Italic'
							= 3 then try to append 'BoldItalic'

	Outputs:	Encoded		contains the string built by this function suitable for output
							to an Illustrator 3.0, 5.0 format file.
	Returns:	-
	Purpose:	Output a font name with bold and italic flags attached. All spaces are
				converted to dashes. The font is first checked for the existance of style
				words bold and italic before new styles are added.

	Notes:		

	FontNames
	---------
							Encodings:
	FaceName				(Bold)						(BoldItalic)
	GaramondE				GaramondE-Bold				GaramondE-BoldItalic
	EuropeExtraBold 		EuropeExtraBold-Bold		EuropeExtraBold-BoldItalic
	European Bold Oblique	European-Bold-Oblique		European-Bold-Oblique
	deGaulleBlack Italic	deGualleBlack-BoldItalic	deGualleBlack-BoldItalic

********************************************************************************************/

void FontManager::EncodeFontName(String_64& FontName, String_64& Encoded, INT32 Styles)
{
	String_64 Copy(FontName);
	String_32 Match;
	
	// Replace all spaces with a hyphen.
 	Copy.SwapChar(' ','-');

	INT32 Length = Copy.Length();

	// Find the last hyphen in the font's name.
	INT32 LastHyphen = Copy.ReverseFind ( '-' );

	// If there is a -XX extension to the name, remove the hyphen.
	if ( LastHyphen == Length - 3 )
	{
		Copy.Remove ( LastHyphen, 1 );
	}

	// Check for bold/italic.
	Match = _R(IDS_FONTMAN_MINUS_BOLD); //"-Bold";
	INT32 Bld = (Copy.Sub(Match,0,0));

	Match = _R(IDS_FONTMAN_MINUS_ITALIC); //"-Italic";
	INT32 Ital = (Copy.Sub(Match,0,0));

	switch (Styles)
	{
		case 1:
		{
			if (Bld<0 && Ital<0)
			{
				Copy += String_16(_R(IDS_FONTMAN_MINUS_BOLD)); // "-Bold";
				break;
			}

			if( ( Bld < 0 ) && ( Ital > -1 ) && ( Length - 7 == Ital ) )
			{
				Copy.Insert( String_32(_R(IDS_FONTMAN_BOLD)), Ital );
			}
			break;
		}

		case 2: 
		{	
			if (Bld<0 && Ital<0)
			{
				Copy += String_16(_R(IDS_FONTMAN_MINUS_ITALIC)); // "-Italic";
				break;
			}
			
			if ((Ital<0) && (Bld>-1) && (Length-5==Bld))
				Copy += String_16(_R(IDS_FONTMAN_ITALIC)); //"Italic";
			break;
		}

		case 3:
		{	
			if (Bld<0 && Ital<0)
			{
				Copy += String_16(_R(IDS_FONTMAN_MINUS_BOLDITALIC)); // "-BoldItalic";
				break;
			}

			// If the font contains the word bold at the end, append italic
			if( ( Ital < 0 ) && ( Bld > -1 ) && ( Length - 5 == Bld ) )
			{
				Copy += String_16(_R(IDS_FONTMAN_ITALIC)); //"Italic";
				break;
			}
									
			// If the font already has italic, insert Bold
			if( ( Bld < 0 ) && ( Ital > -1 ) && ( Length - 7 == Ital ) )
			{
				Copy.Insert( String_32(_R(IDS_FONTMAN_BOLD)), Ital );
			}
			break;

		}
	}

	// Set the encoded output string
	Encoded=Copy;
}

/********************************************************************************************

>	void FontManager::EncodeAndMapFontName ( String_64& FontName,
											 String_64& Encoded,
											 INT32 Styles )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/00
	Inputs:		FontName	- The TrueType font name.
				Encoded		- A string into which the PostScript font name will be written.
				Styles		- Whether the font is bold, italic, or a combination.
	Outputs:	Encoded		- Populated with the encoded font name.
	Returns:	-
	Purpose:	This function first calls EncodeFontName, to generate an approximated
				PostScript font name, before calling a second function to get the PostScript
				font mapping.

********************************************************************************************/

void FontManager::EncodeAndMapFontName ( String_64& FontName,
										 String_64& Encoded,
										 INT32 Styles )
{
	String_64	Mapped;

	// First encode the font name.
	EncodeFontName ( FontName, Encoded, Styles );

	// And then make the mapping.
	if ( Camelot.GetPrefDirect ( _T("EPSFontMapping"), ( TCHAR * ) Encoded, &Mapped ) )
	{
		// There is a PostScript equivalent stored for this font, so use the mapped
		// version as the encoded name.
		Encoded = Mapped;
	}
}

/********************************************************************************************

	void FontManager::GetCompatibleFont(const String_64& EncodedName,
									    String_64& CompatibleFont,
									    INT32& Style)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/95
	Inputs:		EncodedName		the encoded font name to find a compatible font for.
								encoded names are those read direct from the native file.
	Outputs:	CompatibleFont	contains the decoded fontname suitable for font matching
				Style			b0 = 1 then Bold style should be enabled
								b1 = 1 then Italic style should be enabled
	Returns:	
	Purpose:	Find a compatible font to that specified in the encoded font name.
				An encoded name looks something like....

				Times-New-Roman
				Times-New-Roman-Bold
				Times-New-Roman-Italic
				Times-New-Roman-BoldItalic
				ChiversCond-Bold-Oblique

				The algorithm used is based on diminishing returns, ie try to find
				the exact font first, then break the fontname up in various increasingly
				complex ways until a valid substitution is found. This substitution
				can then be used for font caching. Note if the fontmanager fails to find
				any match it will return the encodedname as a last resort. When this
				is cached, its metrics will be NULL and hence be replaced by the default
				font automatically be the font manager.

				extras:
				This function tries to use the font name as specified. If it does not
				exist the function will attempt to use the fontname without styles and
				create styles to fit but only if this new fontname exists. ie it will 
				do the following
			
				FontName					Styles		   NameExists     StylesOutput
		(1)		Times New Roman BoldItalic  			   Yes			  BoldOff / ItalicOff
		(2)		Times New Roman				Bold/Italic	   Yes 			  BoldOn / ItalicOn
		(3)		Times New Roman BoldItalic				   No			  BoldOn / ItalicOn

	Errors:

********************************************************************************************/

void FontManager::GetCompatibleFont(const String_64& EncodedName, String_64& CompatibleFont, INT32& Style)
{
	// first make a copy of the encoded name
	String_64 TryFont(EncodedName);
	
	// now set the return style
	Style=0;

	// Usually the fontnames in the file are saved as /_FontName.Attr-BoldItalic format
	if( TryFont.Sub( String_32( _T("_") ),0 ,0 ) == 0 )
		TryFont.Remove(0,1);

	// remove our inserted - symbols which are encoded as separators on saving
	TryFont.SwapChar('-',' ');

	// Try to find the specified font name, search internal and external caches
	INT32 found = DoesFontExist(&TryFont,TRUE,TRUE);
	if (found>0)
	{
		CompatibleFont=TryFont;
		return;
	}

	// Ok, if we can't find the whole thing, try decoding
	String_64 DecodedName;
	Style = DecodeFontName(EncodedName, DecodedName);

	// find out what styles this font has
	BOOL Bold,Italic;
	Bold=((Style & 1) != 0);
	Italic=((Style & 2) != 0);

	// if both bold and italic found
	if (Bold && Italic)
	{
		String_64 HoldName(TryFont);
		// try to find bold first
		INT32 pos = HoldName.Sub( String_32( _T("Italic") ), 0, 0 );
		HoldName.Remove(pos,6);
		found = DoesFontExist(&HoldName,TRUE,TRUE);
		if (found>0)
		{
			CompatibleFont=HoldName;
			return;
		}		

		// Then try Italic
		HoldName=TryFont;
		pos = HoldName.Sub( String_32( _T("Bold") ), 0, 0 );
		HoldName.Remove(pos,4);
		found = DoesFontExist(&HoldName,TRUE,TRUE);
		if (found>0)
		{
			CompatibleFont=HoldName;
			return;
		}		
	}

	// Does the decoded name exist?, so we can make the rest up in styles?
	found = DoesFontExist(&DecodedName,TRUE,TRUE);
	if (found>0)
	{
		CompatibleFont=DecodedName;
		return;
	}
		
	// otherwise, we have a font with an extension, eg Trinity.Bold
	// (1) Trinity.Bold   - does not exist
	// (2) Trinity        - does not exist
	// now we set the font to (1), which will be replaced by Times-Roman.
	// Now we should also set the font styles too to give us TimesRoman bold

	CompatibleFont=TryFont;
}

/********************************************************************************************

>	INT32 FontManager::DecodeFontName(String_64& IStringRef, String_64& OStringRef)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/95
	Inputs:		IStringRef	the string to decode.
	Outputs:	OStringRef	contains the decoded fontname suitable for font matching
	Returns:	Flags		b0 = 1 then Bold style is enabled
							b1 = 1 then Italic style is enabled
	Purpose:	Given a fontname encoded for EPS files, return the actual font name suitable
				for font matching and its style definintions (if applicable)

********************************************************************************************/

INT32 FontManager::DecodeFontName(const String_64& IStringRef, String_64& OStringRef)
{
	String_32 Match;
	String_64 Temp;
	INT32 pos,Flags;

	OStringRef.Empty();
	Flags=0;
	Temp=IStringRef;

	Match = _R(IDS_FONTMAN_MINUS_BOLDITALIC); // "-BoldItalic";
	pos = Temp.Sub(Match,0,0);
	if (pos>-1)
	{
		Flags |= 3;
		Temp.Left(&OStringRef,pos);	
		Temp=OStringRef;		
	}
	
	Match = _R(IDS_FONTMAN_MINUS_BOLD); // "-Bold";
	pos = Temp.Sub(Match,0,0);
	if (pos>-1)
	{
		Flags |= 1;
		Temp.Left(&OStringRef,pos);	
		Temp=OStringRef;		
	}

	Match = _R(IDS_FONTMAN_MINUS_ITALIC); // "-Italic";
	pos = Temp.Sub(Match,0,0);
	if (pos>-1)
	{
		Flags |= 2;
		Temp.Left(&OStringRef,pos);	
		Temp=OStringRef;		
	}

	// Remove the front _ char
	Match = _T("_");
	if (Temp.Sub(Match,0,0)==0)
		Temp.Remove(0,1);

	Temp.SwapChar('-',' ');
	OStringRef = Temp;
	return Flags;
}

/********************************************************************************************

>	virtual BOOL EnumAllFonts::NewFont(FontClass Class, ENUMLOGFONT FAR* lpelf)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		Class = the class of font returned (ATM,TrueType etc)
				lpelf = a pointer to the font design data
	Returns:	TRUE if the next font should be enumerated
				FALSE if not
	
********************************************************************************************/

BOOL EnumAllFonts::NewFont(FontClass Class, ENUMLOGFONT FAR* lpelf)
{
	// dont enumerate any more please.
	return FALSE;
}
