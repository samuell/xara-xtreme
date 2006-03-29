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

// Module to deal with font allocation within Camelot.

/*
*/

#include "camtypes.h"

#include <stdlib.h>
#include "fonts.h"
#include "errors.h"
#include "ensure.h"
//#include "resource.h"
#include "camelot.h"
#include "unicdman.h"
#ifndef EXCLUDE_FROM_RALPH
#include "oilruler.h"
#endif

//#include "richard2.h"

// The number of pixels per inch in the x and y direction for the screen display.
SIZE FontFactory::PixelsPerInch = { 1, 1 };

// The font array
CFont *FontFactory::Fonts = NULL;

// Statics to hold registry values for bar fonts
String_32 FontFactory::FontDBSmall("MS Sans Serif");
String_32 FontFactory::FontEFSmall("MS Sans Serif");
INT32 FontFactory::FontDBSmallSize = 7;
INT32 FontFactory::FontEFSmallSize = 7;

BOOL FontFactory::bNoBitmapFonts = TRUE;

#define IS_CHICAGO ((LOBYTE(LOWORD(GetVersion()))) > 3)


/********************************************************************************************

>	BOOL FontFactory::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/05/94
	Returns:	TRUE if the factory initialised ok;
				FALSE if not.
	Purpose:	Initialise the font factory - set up the font handle array and find out
				information about the screen.
	SeeAlso:	FontFactory::Deinit; FontFactory::GetFont; FontFactory::GetCFont

********************************************************************************************/

BOOL FontFactory::Init()
{
	// Sanity check - Make sure nobody has completely screwed up the header!
	ERROR3IF(NUMSTOCKFONTS < 2,
				"I suspect something strange has happened to the StockFont system!");

	// Allocate the font handle array
	TRY
	{
		Fonts = new CFont[NUMSTOCKFONTS];
	}
	CATCH (CMemoryException, e)
	{
		ERROR(_R(IDS_OUT_OF_MEMORY), FALSE);
	}
	END_CATCH

	// Initialise the font handle array
	for (INT32 i = 0; i < NUMSTOCKFONTS; i++)
		Fonts[i].m_hObject = NULL;

	// Find out about the screen display capabilities.
	CDC Screen;
	Screen.CreateIC(TEXT("DISPLAY"), 0, 0, 0);
	PixelsPerInch.cx = Screen.GetDeviceCaps(LOGPIXELSX);
	PixelsPerInch.cy = Screen.GetDeviceCaps(LOGPIXELSY);
	Screen.DeleteDC();

	// Try to create our dodgy bitmap font
	LOGFONT LogFont;
	memset(&LogFont, 0, sizeof(LogFont));
	LogFont.lfCharSet = UnicodeManager::GetFontCharSet();
	LPTCHAR FontName = NULL;
	LogFont.lfCharSet = ANSI_CHARSET;  
	LogFont.lfWeight = FW_NORMAL;
	LogFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	lstrcpy(LogFont.lfFaceName, _T("CCSmall"));
	LogFont.lfHeight = -MulDiv(7, PixelsPerInch.cx, 72);

	// Create the font
	CFont TestFont;
	TestFont.CreateFontIndirect(&LogFont);

	// Get hold of any old DC
	CDC *pDesktopDC = CWnd::GetDesktopWindow()->GetDC();

	// If we got a DC and a font
	if (pDesktopDC && TestFont.m_hObject != NULL)
	{
		CFont * pOldFont = pDesktopDC->SelectObject(&TestFont);		// Select the font into the DC
		char buff[64];
		pDesktopDC->GetTextFace(64, buff);							// Get the typeface name
		pDesktopDC->SelectObject(pOldFont);							// Select the original font back in
		CWnd::GetDesktopWindow()->ReleaseDC(pDesktopDC);			// Release the DC
		if (_tcsncicmp(buff, _T("CCSmall"), 64) == 0)
			bNoBitmapFonts = FALSE;
	}

	if (Camelot.DeclareSection("Fonts", 20))
	{
		Camelot.DeclarePref(NULL, "DialogBarSmall", &FontDBSmall);
		Camelot.DeclarePref(NULL, "EditFieldSmall", &FontEFSmall);

		Camelot.DeclarePref(NULL, "DialogBarSmallSize", &FontDBSmallSize, 1, 50);
		Camelot.DeclarePref(NULL, "EditFieldSmallSize", &FontEFSmallSize, 1, 50);
	}

	return TRUE;
}



/********************************************************************************************

>	void FontFactory::Deinit()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/05/94
	Purpose:	Frees/deletes all logical fonts so far created by the font factory.
	SeeAlso:	FontFactory::Init

********************************************************************************************/

void FontFactory::Deinit()
{
	// Delete any fonts we've created so far.
	delete [] Fonts;
	Fonts = NULL;
}

// UIC 
/********************************************************************************************

>	BOOL FontFactory::InvalidateFont()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/2/96
	Purpose:	Invalidate a font - it will be recreated next time GetCFont is called.

********************************************************************************************/

BOOL FontFactory::InvalidateFont(StockFont Font)
{
  	if ((INT32)Font < 0 || (INT32)Font >= NUMSTOCKFONTS)
	{
		ERROR2RAW("Illegal stock font index in FontFactory::GetCFont");
		return FALSE;
	}

	// Check to see if this font has already been allocated
	Fonts[Font].m_hObject = NULL;

	return TRUE;


}

/********************************************************************************************

>	BOOL FontFactory::CheckStatusLineFontChanged()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/2/96
	Purpose:	Determine whether the system dialog title font has changed.

********************************************************************************************/

BOOL FontFactory::CheckSystemStatusLineFontChanged()
{
	BOOL returnVal = FALSE;
	#if _MFC_VER >= 0x400
			if(IS_CHICAGO)
			{
				// test the font used in gallery title bars
				NONCLIENTMETRICS NCMetrics;
				NCMetrics.cbSize = sizeof(NONCLIENTMETRICS);
				if(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,NULL,&NCMetrics,NULL)==TRUE)
				{

					CFont BBFont; 
					// Create a temporary font.
					BBFont.CreateFontIndirect(&NCMetrics.lfStatusFont);
				   
					if(BBFont!= Fonts[STOCKFONT_BUBBLEHELP])
					{
						InvalidateFont(STOCKFONT_BUBBLEHELP);
						InvalidateFont(STOCKFONT_STATUSBARBOLD);
						InvalidateFont(STOCKFONT_STATUSBAR);
						returnVal= TRUE;
					}

				}

				// test the font used in bubble help

			}
	#endif
	
	
	return returnVal ;
}

/********************************************************************************************

>	BOOL FontFactory::CheckSystemBarFontsChanged()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/2/96
	Purpose:	Determine whether the system dialog title font has changed.

********************************************************************************************/

BOOL FontFactory::CheckSystemBarFontsChanged()
{
	BOOL returnVal = FALSE;
	#if _MFC_VER >= 0x400
			if(IS_CHICAGO)
			{
				// test the font used in gallery title bars
				NONCLIENTMETRICS NCMetrics;
				NCMetrics.cbSize = sizeof(NONCLIENTMETRICS);
				if(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,NULL,&NCMetrics,NULL)==TRUE)
				{
					CFont TBFont; 
					// Create a temporary font.
					TBFont.CreateFontIndirect(&NCMetrics.lfSmCaptionFont);
				   
					if(TBFont!= Fonts[STOCKFONT_DIALOGBARTITLE])
					{
						InvalidateFont(STOCKFONT_DIALOGBARTITLE);
						returnVal= TRUE;
					}

				}

				// test the font used in bubble help

			}
	#endif
	
	
	return returnVal ;
}

/********************************************************************************************

>	CFont *FontFactory::GetCFont(StockFont Font)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/05/94
	Inputs:		Font - the font desired (see StockFont).
	Returns:	Pointer to the CFont object for the required font.  If the required font
				cannot be created, then a pointer to a CFont object for a stock font is 
				returned instead.
	Purpose:	Gets a CFont object for a stock font used in Camelot.  This allows you to 
				get to a particular font quickly and easily.  This font will be 
				automatically cleaned up when Camelot exits, and you should NOT delete this
				font object yourself.
	Errors:		Out of memory; The CreateFont() call fails for some reason.
	SeeAlso:	FontFactory::GetFont; StockFont

********************************************************************************************/

CFont *FontFactory::GetCFont(StockFont Font)
{
#ifndef EXCLUDE_FROM_RALPH
	// Check for bad font arrays
	ERROR3IF(Fonts == NULL, "No font array in FontFactory::GetCFont()");
	if (Fonts == NULL)
		return NULL;

	if ((INT32)Font < 0 || (INT32)Font >= NUMSTOCKFONTS)
	{
		ERROR2RAW("Illegal stock font index in FontFactory::GetCFont\n");
		return(NULL);
	}

	// Check to see if this font has already been allocated
	if (Fonts[Font].m_hObject != NULL)
		// Yes - just return the handle
		return &Fonts[Font];

	// Otherwise, create a LOGFONT structure and get the font.
	LOGFONT LogFont;
	memset(&LogFont, 0, sizeof(LogFont));
	LogFont.lfCharSet = UnicodeManager::GetFontCharSet();
	LPTCHAR FontName = NULL;
	INT32 FontSize = 0;

	switch (Font)
	{
// WEBSTER - markn 15/1/97
// No rulers in Webster
#ifndef WEBSTER
		case STOCKFONT_RULERS:
			if (OILRuler::FontName == NULL)
				OILRuler::FontName = new String_256(_R(IDS_OILRULER_SMALL_FONTS));

			if (OILRuler::FontName != NULL)
			{
				FontSize = OILRuler::FontSize;
				LogFont.lfWeight = FW_NORMAL;
				LogFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
				FontName = (TCHAR*)(*OILRuler::FontName);
			}
			break;
#endif

	 // UIC replaces case STOCKFONT_STATUSBAR: 
	case STOCKFONT_STATUSBAR: 
		{
			BOOL DoneSysFont =FALSE;	
		#if _MFC_VER >= 0x400
			if(IS_CHICAGO)
			{
				NONCLIENTMETRICS NCMetrics;
				NCMetrics.cbSize = sizeof(NONCLIENTMETRICS);
				if(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,NULL,&NCMetrics,NULL)==TRUE)
				{
					LogFont = NCMetrics.lfStatusFont;
					DoneSysFont= TRUE;
				}

			}
		#endif
			if(!DoneSysFont)
			{
				LogFont.lfHeight = 9;
				LogFont.lfWeight = FW_NORMAL;
				LogFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
				FontName = String_64(_R(IDS_FONTS_STATUSBAR)); // "MS Sans Serif";
			}
			break;
		}			

	/*	case STOCKFONT_STATUSBAR:
			LogFont.lfHeight = 9;
			LogFont.lfWeight = FW_NORMAL;
			LogFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
			FontName = String_64(_R(IDS_FONTS_STATUSBAR)); // "MS Sans Serif";
			break;
	
		case STOCKFONT_STATUSBARBOLD:
			LogFont.lfHeight = 9;
			LogFont.lfWeight = FW_BOLD;
			LogFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
			FontName = String_64(_R(IDS_FONTS_STATUSBARBOLD)); // "MS Sans Serif";
			break;
	*/

		// UIC 
		case STOCKFONT_STATUSBARBOLD: 
		{
			BOOL DoneSysFont =FALSE;	
		#if _MFC_VER >= 0x400
			if(IS_CHICAGO)
			{
				NONCLIENTMETRICS NCMetrics;
				NCMetrics.cbSize = sizeof(NONCLIENTMETRICS);
				if(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,NULL,&NCMetrics,NULL)==TRUE)
				{
					LogFont = NCMetrics.lfStatusFont;
					LogFont.lfWeight = FW_BOLD;
					DoneSysFont= TRUE;
				}

			}
		#endif
			if(!DoneSysFont)
			{
				LogFont.lfHeight = 9;
				LogFont.lfWeight = FW_BOLD;
				LogFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
				FontName = String_64(_R(IDS_FONTS_STATUSBARBOLD)); // "MS Sans Serif";
			}
			break;
		}

		case STOCKFONT_SPLASHBOXINFO:
			FontSize = 14;
			LogFont.lfWeight = FW_NORMAL;
			LogFont.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS; 
			FontName = String_64(_R(IDS_FONTS_SPLASHBOXINFO)); // "SmallFonts"
			break;

		case STOCKFONT_DIALOGBARCLIENT:
		case STOCKFONT_RNDRGNFIXEDTEXT:		// Used in kernel-rendered dlgs (SuperGalleries)
			FontSize = 9;
			LogFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
			FontName = String_64(_R(IDS_FONTS_GALLERIES_AND_BAR)); // "MS Sans Serif";
			break;

		case STOCKFONT_DIALOGBARSMALL:
			if (bNoBitmapFonts || UnicodeManager::IsDBCSOS())
			{
				FontSize = FontDBSmallSize;
				LogFont.lfWeight = FW_NORMAL;
				LogFont.lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE;
				FontName = FontDBSmall;
				FontName = "MS Sans Serif";
			}
			else
			{
				FontSize = 7;
				LogFont.lfCharSet = ANSI_CHARSET;  
				LogFont.lfWeight = FW_NORMAL;
				LogFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
				FontName = String_64(_R(IDS_FONTS_DIALOGBARSMALL)); // "CCSMALL";
			}
			break;

		case STOCKFONT_DIALOGBARLARGE:
			FontSize = 9;
			LogFont.lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE;
			FontName = String_64(_R(IDS_FONTS_DIALOGBARLARGE)); // "MS Sans Serif";
		    break;

		case STOCKFONT_EDITFIELDSMALL:
			if (bNoBitmapFonts || UnicodeManager::IsDBCSOS())
			{
				FontSize = FontEFSmallSize;
				LogFont.lfWeight = FW_NORMAL;
				LogFont.lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE;
				FontName = FontEFSmall;
			}
			else
			{
				FontSize = 7;
				LogFont.lfCharSet = ANSI_CHARSET;  
				LogFont.lfWeight = FW_NORMAL;
				LogFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
				FontName = String_64(_R(IDS_FONTS_EDITFIELDSMALL)); // "CCSMALL";
			}
			break;

		case STOCKFONT_EDITFIELDLARGE:
			FontSize = 9;
			if (bNoBitmapFonts || UnicodeManager::IsDBCSOS())
			{
				LogFont.lfWeight = FW_NORMAL;
				LogFont.lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE;
				FontName = "MS Sans Serif";
			}
			else
			{
				LogFont.lfCharSet = ANSI_CHARSET;  
				LogFont.lfWeight = FW_NORMAL;
				LogFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
				FontName = String_64(_R(IDS_FONTS_EDITFIELDLARGE)); // "CCLARGE";
			}
			break;

		case STOCKFONT_GALLERYLIST:
			FontSize = 9;
			LogFont.lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE;
			FontName = String_64(_R(IDS_FONTS_GALLERYLIST)); // "MS Sans Serif";
			break;

		case STOCKFONT_DIALOGBARTITLE:
			FontSize = 7;
			LogFont.lfWeight = FW_NORMAL;
			LogFont.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS; 
			FontName = String_64(_R(IDS_FONTS_DIALOGBARTITLE)); // "SmallFonts";
			break;

		// UIC 
		case STOCKFONT_BUBBLEHELP:	
		{
				BOOL DoneSysFont = FALSE;
			
		#if _MFC_VER >= 0x400
			if(IS_CHICAGO)
			{
				NONCLIENTMETRICS NCMetrics;
				NCMetrics.cbSize = sizeof(NONCLIENTMETRICS);
				if(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,NULL,&NCMetrics,NULL)==TRUE)
				{
					LogFont = NCMetrics.lfStatusFont;
					DoneSysFont= TRUE;
				}

			}
		#endif
			if(!DoneSysFont)
			{
				LogFont.lfHeight = 14;
				LogFont.lfWeight = FW_NORMAL;
				LogFont.lfOutPrecision = OUT_RASTER_PRECIS;
				LogFont.lfClipPrecision = CLIP_CHARACTER_PRECIS;
				LogFont.lfQuality = DRAFT_QUALITY;
				LogFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
				FontName = String_64(_R(IDS_FONTS_BUBBLEHELP)); // "Helvetica";
			}
			break;
		}

		case STOCKFONT_DIALOG:
			if (!GetSystemDialogFont(&LogFont))
				return NULL;
			break;

		default:
			ENSURE(FALSE, "Bad font requested in FontFactory::GetStockFont()");
			return NULL;
	}

	if (FontName)
		lstrcpy(LogFont.lfFaceName, FontName);
	if (FontSize)
		LogFont.lfHeight = -MulDiv(FontSize, PixelsPerInch.cx, 72);

	// Create the font and store it in our array.
	Fonts[Font].CreateFontIndirect(&LogFont);

	// If the font could not be created, use a stock font.
	if (Fonts[Font].m_hObject == NULL)
		Fonts[Font].CreateStockObject(SYSTEM_FONT);

	// Return the handle to the caller.
	return &Fonts[Font];
#else
	return NULL;
#endif
}


/********************************************************************************************

>	HFONT FontFactory::GetFont(StockFont Font)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/05/94
	Inputs:		Font - the font desired (see StockFont).
	Returns:	Handle to the required font.  If the required font cannot be created, then a 
				handle to a stock font is returned instead.
	Purpose:	Gets a handle to a stock font used in Camelot.  This allows you to get a
				handle to a particular font quickly and easily.  This font will be 
				automatically cleaned up when Camelot exits, and you should NOT delete this
				font yourself.
	Errors:		Out of memory; The CreateFont() call fails for some reason.
	SeeAlso:	FontFactory::GetCFont; StockFont

********************************************************************************************/

HFONT FontFactory::GetFont(StockFont Font)
{
	// Just defer the main work to GetCFont().
	CFont *pFont = GetCFont(Font);

	// Did it work?
	if (pFont == NULL)
		// No - return null font handle
		return NULL;

	// Worked ok, so extract font handle and return it.
	return (HFONT) pFont->m_hObject;
}



/********************************************************************************************

>	BOOL FontFactory::SetWindowFont(StockFont Font)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/01/95
	Inputs:		Font - the font desired (see StockFont).
	Returns:	TRUE if no errors
	Purpose:	brute force setting of all gadgets in window to use a stock font
				This should be called after window creation but before the window is shown -
				It does not force a window redraw. 
	Errors:		If window doesn't exist
********************************************************************************************/
BOOL FontFactory::ApplyFontToWindow(HWND ThisWindow , StockFont Font)
{
	if(!(IsWindow(ThisWindow))) 
		ERROR2(FALSE,"Apply Font to a non existent Window");
	
	CFont *pFont = GetCFont(Font);		// Get the font
	if (pFont == NULL)					// If this failed, return quietly
		return(FALSE);
	
	// get the first gadget in the window
	HWND DlgItem;
	DlgItem = ::GetWindow(ThisWindow,GW_CHILD);

	while(DlgItem !=NULL)
	{
		// set it's font
		::SendMessage( DlgItem, WM_SETFONT, (WPARAM)pFont->m_hObject, 0);
		
		// and get the next
		DlgItem = ::GetWindow(DlgItem,GW_HWNDNEXT);
	}
	return TRUE;
}



/********************************************************************************************
>	static BOOL FontFactory::GetSystemDialogFont(LOGFONT* pLogFont)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/96
	Inputs:		see outputs
	Outputs:	pLogFont - descibes the system default font
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Call to read the sustem default font for dialogs (set by the user)
				NOTE: this code was stolen from MFC (see CDialogTemplate::SetSystemFont)
********************************************************************************************/
BOOL FontFactory::GetSystemDialogFont(LOGFONT* pLogFont)
{
	// Salary check
	ERROR2IF(pLogFont == NULL, FALSE, "NULL output parameter");

	// Get a font handle to the default font
	HFONT hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	if (hFont == NULL)
		hFont = (HFONT)::GetStockObject(SYSTEM_FONT);
	ERROR2IF(hFont == NULL, FALSE, "Failed to get default font handle");

	// Get the font description
	INT32 Result = ::GetObject(hFont, sizeof(LOGFONT), pLogFont);
	ERROR2IF(Result == 0, FALSE, "Failed to get default font information");

//	{
//		pszFace = lf.lfFaceName;
//		HDC hDC = ::GetDC(NULL);
//		if (lf.lfHeight < 0)
//			lf.lfHeight = -lf.lfHeight;
//		wDefSize = (WORD)MulDiv(lf.lfHeight, 72, GetDeviceCaps(hDC, LOGPIXELSY));
//		::ReleaseDC(NULL, hDC);
//	}

	return TRUE;
}
