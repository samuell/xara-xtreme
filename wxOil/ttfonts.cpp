// $Id: ttfonts.cpp 662 2006-03-14 21:31:49Z alex $
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
// implementation file for TrueType Font manager winoil inclusion

/*
*/

#include "camtypes.h"
#include "ttfonts.h"
#include "fontman.h"
#include "app.h"
#include "textfuns.h"
#include "oilpanse.h"

DECLARE_SOURCE( "$Revision: 662 $" );

CC_IMPLEMENT_DYNCREATE( TTFontMan, CCObject )
CC_IMPLEMENT_DYNCREATE( TTFont, FontBase )

#define new CAM_DEBUG_NEW     

/********************************************************************************************

>	TTFont::TTFont()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95				
	Purpose:	Default constructor

********************************************************************************************/

TTFont::TTFont()
{
}

/********************************************************************************************

>	TTFont::~TTFont()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95				
	Purpose:	destructor for the TTFont class

********************************************************************************************/

TTFont::~TTFont()
{
}

/********************************************************************************************

>	void TTFont::Dump()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95
	Purpose:	Dump the contents of this cache entry out

********************************************************************************************/

void TTFont::Dump()
{
	FontBase::Dump();
	TRACE( _T(" FontClass = TrueType\n"));
}

/********************************************************************************************

>	TTFontMan::TTFontMan() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95				
	Purpose:	Default constructor

********************************************************************************************/

TTFontMan::TTFontMan()
{
}

/********************************************************************************************

	static BOOL TTFontMan::IsOkToCall()

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/9/95
	Returns:	TRUE if the TrueType mangler is actually running and can be used on this OpSys
	Purpose:	Determine whether we can make further calls to the TrueType Manager

********************************************************************************************/

BOOL TTFontMan::IsOkToCall()
{
	// Currently its always true as this is the winoil
	return TRUE;
}

/********************************************************************************************

>	static BOOL TTFontMan::CacheNamedFont(String_64* pFontName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pFontName = a pointer to a fontname to find and cache
	Outputs:	-
	Returns:	TRUE if the font has been cached
				FALSE if not.
	Purpose:	This function attempts to cache a font of the true type variety. It caches
				the font in the kernels font list, and hence can be accessed via 
				FONTMANAGER-> calls

********************************************************************************************/

BOOL TTFontMan::CacheNamedFont(String_64* pFontName)
{
	CDC Screen;
	if (Screen.CreateIC(TEXT("DISPLAY"), 0, 0, 0))
	{
		Application* pApp = GetApplication();
		FontManager* pFontMan = pApp->GetFontManager();
		ERROR2IF(pFontMan == NULL,FALSE,"NULL FontManager ptr");
		pFontMan->ClearTempFont();
		EnumFontFamilies(Screen.m_hDC,
						 NULL,
						 (FONTENUMPROC) TTFontMan_CallBackCacheNamedFont,
						 (LPARAM)pFontName);
		Screen.DeleteDC();
		return pFontMan->TempFontValid();
	}
	return FALSE;
}

/********************************************************************************************

>	static BOOL TTFontMan::CacheCompatibleFont(String_64* pFontName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pFontName = a pointer to a fontname to find and cache
	Outputs:	-
	Returns:	-
	Purpose:	This function attempts to cache a font of the ATM variety

********************************************************************************************/

BOOL TTFontMan::CacheCompatibleFont(String_64* pFontName)
{
	return FALSE;
}

/********************************************************************************************

>	static void TTFontMan::ValidateCache()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Purpose:	This function will attempt to recache all true type fonts within the kernels
				font manager cache.

********************************************************************************************/

void TTFontMan::ValidateCache()
{
	CDC Screen;
	if (Screen.CreateIC(TEXT("DISPLAY"), 0, 0, 0))
	{
		EnumFontFamilies(Screen.m_hDC,
						 NULL,
						 (FONTENUMPROC) TTFontMan_CallBackValidateFont,
						 NULL);
		Screen.DeleteDC();
	}
}

/********************************************************************************************

>	static void TTFontMan::FindClosestFont()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96
	Purpose:	Enumerates all the fonts, looking for a match to a panose number

********************************************************************************************/

void TTFontMan::FindClosestFont()
{
	CDC Screen;
	if (Screen.CreateIC(TEXT("DISPLAY"), 0, 0, 0))
	{
		EnumFontFamilies(Screen.m_hDC,
						 NULL,
						 (FONTENUMPROC) TTFontMan_CallBackFindClosestFont,
						 NULL);
		Screen.DeleteDC();
	}
}

/********************************************************************************************

>	TTFont* TTFontMan::CreateNewFont(String_64* pFontName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pFontName = a pointer to a fontname
	Returns:	NULL if no true type font structure has been created
				A pointer to a true type font structure if successfull.
	Purpose:	This function attempts to create a font instance and will be called by
				the font manager when new fonts are added.

********************************************************************************************/

TTFont* TTFontMan::CreateNewFont(String_64* pFontName)
{
	TTFont *pFont = new TTFont;
	if (pFont==NULL)
		return NULL;
	if (!pFont->Initialise(pFontName))
	{
		delete pFont;
		return NULL;
	}
	return pFont;
}

/********************************************************************************************

>	OUTLINETEXTMETRIC *TTFontMan::GetOutlineTextMetric(LOGFONT *pLogFont)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/08/96
	Inputs:		pLogFont	- pointer to the LOGFONT describing the font in question
	Returns:	pointer to an OUTLINETEXTMETRIC structure, or NULL if none exists.
	Purpose:	Retrieves the OUTLINETEXTMETRIC structure for a TrueType font

	The caller is responsible for deleting this after use.

********************************************************************************************/

OUTLINETEXTMETRIC *TTFontMan::GetOutlineTextMetric(LOGFONT *pLogFont)
{
	ERROR2IF(pLogFont==NULL, FALSE, "Parameter pLogFont==NULL.");

	// watch closely now - it gets fairly involved down here

	OUTLINETEXTMETRIC *pOutlineTextMetric = NULL;
	INT32 Value;

	CDC DisplayDC;
	if (DisplayDC.CreateIC(TEXT("DISPLAY"), 0, 0, 0))
	{
		CFont *pNewCFont = new CFont;

		if (pNewCFont->CreateFontIndirect(pLogFont) != NULL)
		{
			//		**	**													**   **
			//		 ** **		must remember to select the old CFont		 ** **
			//		  ***		back in again when we've					  ***
			//		 ** **		finished using this new CFont!				 ** **
			//		**   **													**   **
			
			CFont *pOldCFont = DisplayDC.SelectObject(pNewCFont);
			
			if (pOldCFont != NULL)
			{
				TEXTMETRIC MyTextMetric;

				if (DisplayDC.GetTextMetrics(&MyTextMetric) != 0)
				{
					if (MyTextMetric.tmPitchAndFamily & TMPF_TRUETYPE == TMPF_TRUETYPE)
					{
						// its a TrueType font, so get the OUTLINETEXTMETRIC structure and run

						Value = DisplayDC.GetOutlineTextMetrics(NULL, NULL);

						// claim a block of memory for the OUTLINETEXTMETRIC class
						pOutlineTextMetric = (OUTLINETEXTMETRIC *) malloc(Value);

						// now get the OUTLINETEXTMETRIC structure itself
						Value = DisplayDC.GetOutlineTextMetrics(Value, pOutlineTextMetric);

						if (Value==FALSE)
						{
							// failed to get the outline text metrics, so free the memory we claimed
							free(pOutlineTextMetric);
							pOutlineTextMetric=NULL;
						}
					}
					else
					{
						// not a truetype font, so do nothing.
					}
				}
				else
				{
					DisplayDC.SelectObject(pOldCFont);
					DisplayDC.DeleteDC();
					delete pNewCFont;
					ERROR2(NULL, "Unable to retrieve TEXTMETRIC structure from DisplayDC.")
				}

				//		**	**													**   **
				//		 ** **		select the old CFont back into the			 ** **
				//		  ***		DC now we've finished using the 			  ***
				//		 ** **		new CFont!									 ** **
				//		**   **													**   **

				DisplayDC.SelectObject(pOldCFont);
			}
			else
			{
				delete pNewCFont;
				DisplayDC.DeleteDC();
				ERROR2(NULL, "Unable to select font into IC.");
			}
		}
		else
		{
			delete pNewCFont;
			DisplayDC.DeleteDC();
			ERROR2(NULL, "Unable to create font.");
		}
		delete pNewCFont;
		DisplayDC.DeleteDC();
	}
	else
	{
		ERROR2(NULL, "Unable to create an IC.");
	}

	return pOutlineTextMetric;
}

/********************************************************************************************

>	static void TTFontMan::EnumAllFonts(OILEnumFonts* pClass)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pClass = A pointer to an object to call back.
	Purpose:	Gives the kernel a way of enumerating fonts itself

********************************************************************************************/

void TTFontMan::EnumAllFonts(OILEnumFonts* pClass)
{
	CDC Screen;
	if (Screen.CreateIC(TEXT("DISPLAY"), 0, 0, 0))
	{
		EnumFontFamilies(Screen.m_hDC,
						 NULL,
						 (FONTENUMPROC) TTFontMan_CallBackDispatchFont,
						 (LPARAM)pClass);
		Screen.DeleteDC();
	}
}

/********************************************************************************************

>	INT32 APIENTRY TTFontMan_CallBackCacheNamedFont(
	ENUMLOGFONT FAR*  lpelf,		// address of logical-font data 
    NEWTEXTMETRIC FAR*  lpntm,		// address of physical-font data 
    INT32 FontType,					// type of font 
    LPARAM lParam 					// address of application-defined data  

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Purpose:	The call back function providing all font data

********************************************************************************************/

INT32 APIENTRY TTFontMan_CallBackCacheNamedFont(
	ENUMLOGFONT FAR*  lpelf,		// address of logical-font data 
    NEWTEXTMETRIC FAR*  lpntm,		// address of physical-font data 
    INT32 FontType,					// type of font 
    LPARAM lParam 					// address of application-defined data  
	)
{
	if (FontType & TRUETYPE_FONTTYPE)
	{
		// find the font name
		String_64 FontName(lpelf->elfLogFont.lfFaceName);

		if (lParam==NULL || (_tcsncicmp(FontName, *((String_64 * )lParam), 64) == 0))
		{
			FontManager* pFontMan = GetApplication()->GetFontManager();
			pFontMan->SetTempFont(FC_TRUETYPE, &FontName, lpelf);
			return FALSE;
		}
	}
    return TRUE;
}

/********************************************************************************************

>	INT32 APIENTRY TTFontMan_CallBackValidateFont(
	ENUMLOGFONT FAR*  lpelf,		// address of logical-font data 
    NEWTEXTMETRIC FAR*  lpntm,		// address of physical-font data 
    INT32 FontType,					// type of font 
    LPARAM lParam 					// address of application-defined data  

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Purpose:	The call back function providing all font data

********************************************************************************************/

INT32 APIENTRY TTFontMan_CallBackValidateFont(
	ENUMLOGFONT FAR*  lpelf,		// address of logical-font data 
    NEWTEXTMETRIC FAR*  lpntm,		// address of physical-font data 
    INT32 FontType,					// type of font 
    LPARAM lParam 					// address of application-defined data  
	)
{
    UNREFERENCED_PARAMETER (lpntm);

	if (FontType & TRUETYPE_FONTTYPE)
	{
		// find the font name
		String_64 FontName(lpelf->elfLogFont.lfFaceName);

		FontManager* pFontMan = GetApplication()->GetFontManager();
		pFontMan->ValidateItem(FC_TRUETYPE, &FontName, lpelf);
	}
    return TRUE;
}

/********************************************************************************************

	INT32 APIENTRY TTFontMan_CallBackDispatchFont(
	ENUMLOGFONT FAR*  lpelf,		// address of logical-font data 
    NEWTEXTMETRIC FAR*  lpntm,		// address of physical-font data 
    INT32 FontType,					// type of font 
    LPARAM lParam 					// address of application-defined data  

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Purpose:	The call back function providing all font data

********************************************************************************************/

INT32 APIENTRY TTFontMan_CallBackDispatchFont(
	ENUMLOGFONT FAR*  lpelf,		// address of logical-font data 
    NEWTEXTMETRIC FAR*  lpntm,		// address of physical-font data 
    INT32 FontType,					// type of font 
    LPARAM lParam 					// address of application-defined data  
	)
{
    UNREFERENCED_PARAMETER (lpntm);

	if ((FontType & TRUETYPE_FONTTYPE) && lpelf->elfLogFont.lfFaceName[0] != TEXT('@'))
	{
		return ((OILEnumFonts*) lParam)->NewFont(FC_TRUETYPE, lpelf);
	}

	return TRUE;
}

/********************************************************************************************

	INT32 APIENTRY TTFontMan_CallBackFindClosestFont(
	ENUMLOGFONT FAR*  lpelf,		// address of logical-font data 
    NEWTEXTMETRIC FAR*  lpntm,		// address of physical-font data 
    INT32 FontType,					// type of font 
    LPARAM lParam 					// address of application-defined data  

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96
	Purpose:	The call back function for finding the closest font

********************************************************************************************/

INT32 APIENTRY TTFontMan_CallBackFindClosestFont(
	ENUMLOGFONT FAR*  lpelf,		// address of logical-font data 
    NEWTEXTMETRIC FAR*  lpntm,		// address of physical-font data 
    INT32 FontType,					// type of font 
    LPARAM lParam 					// address of application-defined data  
	)
{
    UNREFERENCED_PARAMETER (lpntm);

	if (FontType & TRUETYPE_FONTTYPE)
	{
		// find the font name
		String_64 FontName(lpelf->elfLogFont.lfFaceName);

		FontManager* pFontMan = GetApplication()->GetFontManager();
		return pFontMan->FindClosestFontFullTry(FC_TRUETYPE, &FontName, lpelf);
	}
	
    return TRUE;
}
