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
// palman.cpp - a centralised screen palette manager

/*
*/

#include "camtypes.h"

#include "document.h"
#include "docview.h"
#include "grndrgn.h"
//GAT #include "mainfrm.h"
#include "palman.h"
//GAT #include "dlgbar.h"
//GAT #include "ccolbar.h"
#include "rendwnd.h"
#include "fixmem.h"
#include "dibutil.h"	// for identity palette
//GAT #include "bitbutn.h"	// for WM_CAM_PALETTECHANGE
#ifdef RALPH
#include "ralphdoc.h"	// For foreground / background forcing...
#endif

BOOL PaletteManager::m_fPaletteLocked = FALSE;
BOOL PaletteManager::sm_bUseMainPalette = FALSE;
wxPalette PaletteManager::sm_MainPalette;

//#define DEBUG_PALMAN 1

/********************************************************************************************

>	static BOOL PaletteManager::Init(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/96

	Purpose:	Initialises the palette manager

	SeeAlso:	PaletteManager::UpdatePalette

********************************************************************************************/

BOOL PaletteManager::Init(void)
{
#if DEBUG_PALMAN
	TRACE( _T("BOOL PaletteManager::Init(void)\n"));
#endif
	UpdatePalette();

	return(TRUE);
}


/********************************************************************************************

>	static void PaletteManager::LockPalette(BOOL fLocked)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		fLocked			if TRUE, lock the palette, if FALSE, unlock it.

	Purpose:	"Locks" or "unlocks" the palette, preventing or allowing palette changes
				due to window repaints.  Ultra-bodge way of preventing splash-box palette
				flash during workspace restoration.

	Notes:		This bodge has been toned down (Jason). It now does not stop us realizing
				palettes (which is a big big big no-no) but instead forces us to background
				palette realization so that it does not affect the physical screen palette.

	SeeAlso:	PaletteManager::IsRedrawOnPaletteChangeRequired

********************************************************************************************/

void PaletteManager::LockPalette(BOOL fLocked)
{
#if DEBUG_PALMAN
	TRACE( _T("void PaletteManager::LockPalette(BOOL fLocked = %d)\n"), fLocked);
#endif
	m_fPaletteLocked = fLocked;
}



/********************************************************************************************

>	static BOOL CALLBACK PaletteManager::InvalidateChildProc(HWND hWnd, LPARAM lParam)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95

	Inputs:		hWnd - the window to invalidate
				lParam - unused

	Returns:	TRUE (success)

	Purpose:	A callback proc to invalidate child windows if they need to be redrawn
				as a result of a palette change

	Scope:		Static public

********************************************************************************************/


BOOL CALLBACK PaletteManager::InvalidateChildProc(wxWindow* hWnd, LPARAM lParam)
{
/* GAT
#if DEBUG_PALMAN
	TRACE( _T("BOOL CALLBACK PaletteManager::InvalidateChildProc(HWND hWnd = %d, LPARAM lParam = %d)\n"),
		(DWORD)hWnd, (DWORD)lParam);
#endif
	BOOL bNotify = FALSE;
	if (PaletteManager::IsRedrawOnPaletteChangeRequired(hWnd, &bNotify))
	{
		if (bNotify)
			SendMessage(hWnd, WM_CAM_PALETTECHANGE, 0, 0);

		::InvalidateRect(hWnd, NULL, TRUE);
	}
*/
	return(TRUE);
}



/********************************************************************************************

>	static void PaletteManager::RedrawAllPalettedWindows(HWND ParentWindow = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/96

	Inputs:		hWnd - the parent window to invalidate, or NULL to use Camelot's MainFrame

	Purpose:	To redraw the given window (and all children) that are reliant upon 256-
				colour palettes. Usually called with NULL to just redraw the MainFrame.

********************************************************************************************/

void PaletteManager::RedrawAllPalettedWindows(wxWindow* hWnd)
{
/* GAT
#if DEBUG_PALMAN
	TRACE( _T("void PaletteManager::RedrawAllPalettedWindows(HWND hWnd = %d)\n"), (DWORD)hWnd);
#endif
	if (hWnd == NULL)
	{
		CMainFrame *MainFrame = GetMainFrame();
		if (MainFrame != NULL)
			hWnd = MainFrame->m_hWnd;
	}

	if (hWnd != NULL)
	{
		if (IsRedrawOnPaletteChangeRequired(hWnd))
			::InvalidateRect(hWnd, NULL, TRUE);					// And redraw everything if necessary

		EnumChildWindows(hWnd, InvalidateChildProc, 0);			// And redraw all children which need it
	}
*/
}



/********************************************************************************************

>	BOOL PaletteManager::IsRedrawOnPaletteChangeRequired(HWND hWnd, BOOL* pNotifyFirst);

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/95

	Inputs:		hWnd - a window potentially to be redrawn
	Returns:	TRUE if this window should redraw, FALSE if not

	Purpose:	To respond to InvalidateChildProc. Determines if the given window needs
				to be redrawn as a result of a palette change.

	Scope:		Static public

********************************************************************************************/

BOOL PaletteManager::IsRedrawOnPaletteChangeRequired(wxWindow* hWnd, BOOL* pNotifyFirst)
{
	return FALSE;
/* GAT
#if DEBUG_PALMAN
	TRACE( _T("BOOL PaletteManager::IsRedrawOnPaletteChangeRequired(HWND hWnd = %d)\n"), (DWORD)hWnd);
#endif

	if (pNotifyFirst)
		*pNotifyFirst = FALSE;

#ifdef RALPH
	// Nasty Ralph bodge to get the palette working sensibly
	return(TRUE);

#else

	// First see if it is an MFC Window	
	// Well this does some of it
	CWnd* pCWnd = CWnd::FromHandlePermanent(hWnd);

	// don't redraw it then
	// Note we keep its children separately
#ifndef STANDALONE
	if (pCWnd)
	{
		if (pCWnd->IsKindOf(RUNTIME_CLASS(CColourBar)) || // Redraw the colour bar
		    pCWnd->IsKindOf(RUNTIME_CLASS(CRenderWnd)))  // and the view windows
		{
		   return TRUE;
		}

		return FALSE;
	}
#else
	if (pCWnd)
	{
		if (pCWnd->IsKindOf(RUNTIME_CLASS(CRenderWnd)))  // Redraw the view windows
		   return TRUE;

		return FALSE;
	}
#endif

	// OK, it's not an MFC window
	String_256 ClassNameStr;  // The control type

	// Find out the class type of the gadget
	GetClassName(hWnd, (TCHAR*)ClassNameStr, 255);
	if ((ClassNameStr == String_16(TEXT("cc_DialogDraw"))))
		return TRUE;

	if ((ClassNameStr == String_16(TEXT("cc_BitmapButton"))))
	{
		if (pNotifyFirst)
			*pNotifyFirst = TRUE;
		return TRUE;
	}

	if ((ClassNameStr == String_16(TEXT("cc_SmallButton"))))
	{
		if (pNotifyFirst)
			*pNotifyFirst = TRUE;
		return TRUE;
	}

	if ((ClassNameStr == String_16(TEXT("cc_StaticBitmap"))))
	{
		if (pNotifyFirst)
			*pNotifyFirst = TRUE;
		return TRUE;
	}

	return FALSE;

#endif
*/
}	



/********************************************************************************************

>	BOOL PaletteManager::EnsurePalette(HWND hWnd, BOOL ForceRedraw)
>	BOOL PaletteManager::EnsurePalette(CWnd *CWindow, BOOL ForceRedraw)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95

	Inputs:		hWnd - the window for which EnsurePalette was called
				or CWindow the CWnd equivalent
				ForceRedraw - TRUE to force a redraw even when we haven't (seeminlgy) affected
				the palette

	Returns:	TRUE if it chnaged the palette in the course of realising the GDraw palette
				Now returns TRUE if it selects a palette

	Purpose:	To respond to WM_QUERYNEWPALETTE and WM_ONPALETTECHANGED
				Whenever you get one of these messages for youre window, call this method
				if you want to use the normal GDraw palette. The only splitter in this
				area to date is the splashbox window.

	Scope:		Static public

	Notes:		Use ForceRedraw = FALSE for WM_QUERYNEWPALETTE, TRUE for WM_ONPALETTECHANGED

********************************************************************************************/

BOOL PaletteManager::EnsurePalette(wxWindow* hWnd, BOOL ForceRedraw)
{
/* GAT
#if DEBUG_PALMAN
	TRACE( _T("BOOL PaletteManager::EnsurePalette(HWND hWnd = %d, BOOL ForceRedraw = %d)\n"),
		(DWORD)hWnd, ForceRedraw);
#endif

	static PALETTEENTRY OldSysPal[256];
	BOOL ForceBackground = m_fPaletteLocked;

	static BOOL SelectingPalette = FALSE;

	// This code may well look a bit odd but it is necessary to prevent reentrancy problems
	BOOL CheckPalette = !SelectingPalette;

#ifdef RALPH
	if(!RalphDocument::ForegroundPalette)
		ForceBackground = TRUE;
#endif
	
	// This "event" is directed at the application as a whole but the palette we set up is
	// always associated with the Selected DocView and so we'll set Current to Selected...
	if (Document::GetSelected != NULL)
		Document::GetSelected()->SetCurrent();

	if (DocView::GetSelected() != NULL)
		DocView::GetSelected()->SetCurrent();

	if (!UsePalette())		// We don't use a special palette, so do nothing
		return(FALSE);

// This check (to stop us changing the palette when minimised) has been removed (Jason - 26/7/95)
// The reason for this is:
// (a) This is contrary to microsoft coding guidelines,
// (b) If you restore camelot from minimised state, it fails to realise its palette!
// (c) The behaviour is now "correct" and exactly the same as all other apps
//
//	if (MainFrame->IsIconic())			// We are currently in an iconised state, so do nothing
//		return(FALSE);

	SelectingPalette = TRUE;

	HDC hDC = ::GetDC(hWnd);
	
	// We need to check that the palette actully has changed
	// because RealizePalette() will say that 256 colours have changed
	// if you close the help file on NT4/95
	
	PALETTEENTRY NewSysPal[256];

	HPALETTE OldPal = ::SelectPalette(hDC, sm_MainPalette, ForceBackground);
	UINT32 NumColoursChanged = ::RealizePalette(hDC);
	::SelectPalette(hDC, OldPal, TRUE);
	::RealizePalette(hDC);

	GetSystemPaletteEntries(hDC, 0, 256, NewSysPal);

	::ReleaseDC(hWnd, hDC);

	SelectingPalette = FALSE;
	
	if (CheckPalette)
	{
		for (DWORD Index = 0; Index < 256; Index++)
			NewSysPal[Index].peFlags = 0;

		// If the palette has really changed
		if (memcmp(NewSysPal, OldSysPal, sizeof(PALETTEENTRY) * 256) != 0)
		{
//			TRACEUSER( "Gerry", _T("Palette has actually changed\n"));
			// Update the old palette
			memcpy(OldSysPal, NewSysPal, sizeof(PALETTEENTRY) * 256);

//			TRACEUSER( "Gerry", _T("Cols changed %d\n"), NumColoursChanged);

			// Only set the flag if the OS thinks the palette has changed
			if (NumColoursChanged != 0)
				ForceRedraw = TRUE;
		}
//		else
//		{
//			TRACEUSER( "Gerry", _T("Palette not actually changed\n"));
//		}
	}
//	else
//	{
//		TRACEUSER( "Gerry", _T("Not checking palette\n"));
//	}

	if (ForceRedraw)
	{
//		TRACEUSER( "Gerry", _T("Forcing redraw\n"));
		GRenderRegion::GColInit(NULL, TRUE);		// tell GDraw of new palette
		RedrawAllPalettedWindows(hWnd);				// And redraw everything
	}

	// This used to return the commented out line below but was changed
	// to help fix a problem with the help file

//	return(NumColoursChanged != 0);
*/	return(TRUE);
}

/*
BOOL PaletteManager::EnsurePalette(CWnd *CWindow, BOOL ForceRedraw)
{
#if DEBUG_PALMAN
	TRACE( _T("BOOL PaletteManager::EnsurePalette(CWnd *CWindow = %d, BOOL ForceRedraw = %d)\n"),
		(DWORD)CWindow, ForceRedraw);
#endif
	return(EnsurePalette(CWindow->GetSafeHwnd(), ForceRedraw));
}
*/


/********************************************************************************************

>	static CPalette *PaletteManager::StartPaintPalette(CDC *RenderDC, BOOL bForceBackground=FALSE)
>	static HPALETTE PaletteManager::StartPaintPalette(HDC hDC, HPALETTE *pHPal=NULL, BOOL bForceBackground=FALSE);
>	static HPALETTE PaletteManager::StartPaintPalette(HDC hDC, HPALETTE HPal, BOOL bForceBackground=FALSE);

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95
	Inputs:		RenderDC = the DC we're using

				bForceBackground = value to pass to the force background param in SelectPalette
				If you don't understand foreground/background Windows palettes, use the default or ask Jason

				HPal = palette to use (3 only)

	Outputs:	pHPal - if non-null, filled in with the palette which was selected into your DC (2 only)

	Returns:	Pointer to the old palette (or NULL if DC doesn't support palette)
	Purpose:	To select and realise the mainframe GDraw palette before painting
	Scope:		Static public

	SeeAlso:	PaletteManager::StopPaintPalette

********************************************************************************************/
/*
wxPalette* PaletteManager::StartPaintPalette(wxDC* RenderDC, BOOL bForceBackground)
{
#if 1	// GAT
	return NULL;
#else
#if DEBUG_PALMAN
	TRACE( _T("CPalette *PaletteManager::StartPaintPalette(CDC *RenderDC = %d, BOOL bForceBackground = %d)\n"),
		(DWORD)RenderDC, bForceBackground);
#endif
	ERROR2IF (!RenderDC, NULL, "PaletteManager passed NULL RenderRC");

#ifdef RALPH
	if(!RalphDocument::ForegroundPalette)
		bForceBackground = TRUE;
#endif

	if (!(RenderDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE))
		return(NULL);

	if (m_fPaletteLocked)
		bForceBackground = TRUE;

	CPalette * OldPalette = RenderDC->SelectPalette(&sm_MainPalette, bForceBackground);
	RenderDC->RealizePalette();
	
	return OldPalette;
#endif
}
*/

wxPalette* PaletteManager::StartPaintPalette(wxDC* hDC, wxPalette* pHPal, BOOL bForceBackground)
{
#if 1	// GAT
	return NULL;
#else
#if DEBUG_PALMAN
	TRACE( _T("HPALETTE PaletteManager::StartPaintPalette(HDC hDC = %d, HPALETTE *pHPal = %d, BOOL bForceBackground = %d)\n"),
		(DWORD)hDC, (DWORD)pHPal, bForceBackground);
#endif
	ERROR2IF(!hDC, NULL, "PaletteManager passed NULL DC");

#ifdef RALPH
	if(!RalphDocument::ForegroundPalette)
		bForceBackground = TRUE;
#endif

	if (!(::GetDeviceCaps(hDC, RASTERCAPS) & RC_PALETTE))
		return(NULL);

	HPALETTE NewPalette = sm_MainPalette;	// Sneaky HPALETTE operator
	if (pHPal)
		*pHPal = NewPalette;

	if (m_fPaletteLocked)
		bForceBackground = TRUE;

	HPALETTE OldPalette = ::SelectPalette(hDC, NewPalette, bForceBackground);
	::RealizePalette(hDC);

	return OldPalette;
#endif
}

/*
wxPalette* PaletteManager::StartPaintPalette(wxDC* hDC, const wxPalette& HPal, BOOL bForceBackground)
{
#if 1	// GAT
	return NULL;
#else
#if DEBUG_PALMAN
	TRACE( _T("HPALETTE PaletteManager::StartPaintPalette(HDC hDC = %d, HPALETTE HPal = %d, BOOL bForceBackground = %d)\n"),
		(DWORD)hDC, (DWORD)HPal, bForceBackground);
#endif
	ERROR2IF(!hDC, NULL, "PaletteManager passed NULL DC");

#ifdef RALPH
	if(!RalphDocument::ForegroundPalette)
		bForceBackground = TRUE;
#endif

	if (!(::GetDeviceCaps(hDC, RASTERCAPS) & RC_PALETTE))
		return(NULL);

	if (m_fPaletteLocked)
		bForceBackground = TRUE;

	HPALETTE OldPalette = ::SelectPalette(hDC, HPal, bForceBackground);
	::RealizePalette(hDC);

	return OldPalette;
#endif
}
*/


/********************************************************************************************

>	static CPalette *PaletteManager::StopPaintPalette(CDC *RenderDC, CPalette *OldPalette,
														BOOL bForceBackground = TRUE)

>	static HPALETTE PaletteManager::StopPaintPalette(HDC hDC, HPALETTE OldPalette,
														BOOL bForceBackground = TRUE)
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95

	Inputs:		RenderDC/hDC - the DC we're using

				OldPalette - The palette returned to you when you called StartPaintPalette

				bForceBackground = value to pass to the forcebackground param in SelectPalette
				It's not expected that this will ever be anything other than TRUE

	Outputs:	-
	Returns:	Pointer to the old palette (or NULL if DC doesn't support palette)

	Purpose:	To restore the palette after painting, using the palette returned to you
				by StartPaintPalette

	SeeAlso:	PaletteManager::StartPaintPalette

	Scope:		Static public

********************************************************************************************/

wxPalette* PaletteManager::StopPaintPalette(wxDC* RenderDC, wxPalette* OldPalette, BOOL bForceBackground)
{
#if DEBUG_PALMAN
	TRACE( _T("CPalette *PaletteManager::StopPaintPalette(CDC *RenderDC = %d, CPalette *OldPalette = %d, BOOL bForceBackground = %d)\n"),
		(DWORD)RenderDC, (DWORD)OldPalette, bForceBackground);
#endif
	ERROR2IF (!RenderDC, NULL, "PaletteManager passed NULL RenderRC");
// GAT
//	if (!(RenderDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE))
		return(NULL);
/*
	if (m_fPaletteLocked)
		bForceBackground = TRUE;

	CPalette *PrevPalette = RenderDC->SelectPalette(OldPalette, bForceBackground);
	RenderDC->RealizePalette();

	return PrevPalette;
*/
}


/********************************************************************************************

>	static void PaletteManager::UpdatePalette(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Purpose:	Sets up the static main frame palette
				This will be called whenever the palette needs updating
	SeeAlso:	-

********************************************************************************************/

void PaletteManager::UpdatePalette(void)
{
/* GAT
#if DEBUG_PALMAN
	TRACE( _T("void PaletteManager::UpdatePalette(void)\n"));
#endif

	HPALETTE hPal = (HPALETTE) sm_MainPalette.Detach();		// Detach the palette object

	if (hPal != NULL)
	{
		::DeleteObject(hPal);
		hPal = NULL;
	}

	sm_bUseMainPalette = FALSE;

	LPLOGPALETTE lpPal = (LPLOGPALETTE)GRenderRegion::GetRecommendedPalette();

	if (lpPal)
	{
		TRACEUSER( "Gerry", _T("Got a palette in PaletteManager::UpdatePalette()\n"));

		// Gavin's palette has 10 unused entries at the front and 10 at the back, which
		// is dead handy indeed
		hPal = DIBUtil::MakeIdentityPalette( lpPal->palPalEntry, 256 );
		if (hPal)
		{
			sm_bUseMainPalette = sm_MainPalette.Attach(hPal);
		}
	}
*/
}

/********************************************************************************************

>	static void PaletteManager::MakePaletteBrowserCompatible(LPLOGPALETTE pPalette,BOOL AvoidSystemColours)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/97
	Inputs:		pPalette = ptr to 256 entry palette
				AvoidSystemColours = TRUE if you want the system colour palette entries to be lefte alone
									 FALSE if you want the first 216 entries to be the browser palette.

	Purpose:	This inserts all the browser colours into the palette, and marks all other entries
				so that they are not used when creating bitmaps.
				
				An entry is marked as "don't use" by setting the peFlags member to 255
				(e.g. pPalette->palPalEntry[x].peFlags = 255);

  				if AvoidSystemColours is TRUE, the system colour palette entries are marked as "don't use"

				Introduced for Webster.
				Modified heavily for Webster v2 by Markn 20/6/97
	SeeAlso:	-

********************************************************************************************/

void PaletteManager::MakePaletteBrowserCompatible(LPLOGPALETTE pPalette,BOOL AvoidSystemColours)
{
	if (pPalette == NULL)
	{
		ERROR3("Null palette ptr");
		return;
	}

	if (pPalette->palNumEntries != 256)
	{
		ERROR3("Only suitable for 8bpp 256 entry palettes");
		return;
	}

	// Set all entries to be "Don't use"
	INT32 i;
	for (i = 0; i < pPalette->palNumEntries;i++)
	{
		pPalette->palPalEntry[i].peRed   = 0;
		pPalette->palPalEntry[i].peGreen = 0;
		pPalette->palPalEntry[i].peBlue  = 0;
		pPalette->palPalEntry[i].peFlags  = 255;
	}

	// If system colours entries are to be avoided, start from the first non-system colour entry (index value 11)
	if (AvoidSystemColours)
		i = 11;	// 11 in case the bitmap is transparent, in which case index 10 will hold the transparent colour
	else
		i = 1;	// 1 in case the bitmap is transparent, in which case index 0 will hold the transparent colour

	// Fill it with the browser colours
	PaletteManager::FillWithBrowserColours(pPalette,i);

	// Debug sanity check
	#ifdef _DEBUG
	TRACE( _T("Checking 216 colours\n"));
	if (!ContainsAllBrowserColours(pPalette))
		ERROR3("Do what! Some browser colours are missing!");
	#endif
}

/********************************************************************************************

>	static void PaletteManager::FillWithBrowserColours(LPLOGPALETTE pPalette,INT32 StartIndex)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/97
	Inputs:		pPalette = ptr palette
				StartIndex = the index to start filling the entries from
	Returns:	-
	Purpose:	Stuffs all the browser colours into the palette provided
				Also markes each peFlags entry to 0 (i.e. so Gavin will use it).
	SeeAlso:	-

********************************************************************************************/

void PaletteManager::FillWithBrowserColours(LPLOGPALETTE pPalette,INT32 StartIndex)
{
	ERROR3IF(pPalette == NULL,"NULL Entry param");
	if (pPalette == NULL)
		return;

	INT32 i = StartIndex;

	// Generate the entire browser palette
	for (INT32 r=0;r <= 255;r += 51)
	{
		for (INT32 g=0;g <=255;g += 51)
		{
			for (INT32 b=0;b <=255;b += 51)
			{
				if (i < pPalette->palNumEntries)	// Extra safety check
				{
					pPalette->palPalEntry[i].peRed   = r;
					pPalette->palPalEntry[i].peGreen = g;
					pPalette->palPalEntry[i].peBlue  = b;
					pPalette->palPalEntry[i].peFlags = 0;	// Mark as "Use this colour if you want"
				}

				i++;	// Next entry please
			}
		}
	}
}

/********************************************************************************************

>	static void PaletteManager::ContainsAllBrowserColours(LPLOGPALETTE pPalette)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/97
	Inputs:		pPalette = ptr palette
	Returns:	TRUE if palette contains all the 216 browser palette entries
				FALSE otherwise
	Purpose:	A test to see if the palette contains all the 216 browser palette entries.
	SeeAlso:	-

********************************************************************************************/

BOOL PaletteManager::ContainsAllBrowserColours(LPLOGPALETTE pPalette)
{
	for (INT32 r=0;r <= 255;r += 51)
	{
		BYTE ra = r;
		for (INT32 g=0;g <=255;g += 51)
		{
			BYTE ga = g;
			for (INT32 b=0;b <=255;b += 51)
			{
				BYTE ba = b;
				BOOL found = FALSE;
				for (INT32 i=0;i<pPalette->palNumEntries;i++)
				{
					// if peFlags != 255, then it hasn't been marked as "Don't use", so check it
					if (pPalette->palPalEntry[i].peFlags != 255)
					{
						found = pPalette->palPalEntry[i].peRed   == ra &&
								pPalette->palPalEntry[i].peGreen == ga &&
								pPalette->palPalEntry[i].peBlue  == ba;
						if ( found )
							break;
					}
				}
				if (!found)
					return FALSE;
			}
		}
	}
	return TRUE;
}


/********************************************************************************************

>	static LPLOGPALETTE PaletteManager::CreateBrowserPalette(LPLOGPALETTE pPalette,BOOL AvoidSystemColours)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/97
	Inputs:		AvoidSystemColours = TRUE if you want the system colour palette entries to be left alone.
									 FALSE if you want the first 216 entries to be the browser palette.

	Returns:	ptr to a palette, or NULL if it fails
	Purpose:	Creates a palette of browser colours

				To delete the palette, use CCFree().
	SeeAlso:	-

********************************************************************************************/

LPLOGPALETTE PaletteManager::CreateBrowserPalette(BOOL AvoidSystemColours)
{
	LPLOGPALETTE pPalette = NULL;
	const size_t TotalPal = sizeof(LOGPALETTE) + ( sizeof(PALETTEENTRY) * 256 );
	pPalette = (LPLOGPALETTE)CCMalloc( TotalPal );
	if (pPalette!= NULL)
	{
		pPalette->palVersion = 0x300;
		pPalette->palNumEntries = 256;
		MakePaletteBrowserCompatible(pPalette,AvoidSystemColours);
	}

	return pPalette;
}

/********************************************************************************************

>	static void PaletteManager::FindNearestColour(const ColourRGBT& Colour, LPLOGPALETTE pPalette, ColourRGBT* const pResult)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/97
	Inputs:		Colour		= Original colour definition
				pPalette	= look-up palette
				pResult		= place to put closest colour definition

	Returns:	pResult contains a colour from the palette that's closest to the original
	Purpose:	Finds the closest colour in the given palette.

				The closest colour is the one that is closest in terms of distance within the RGB colour
				cube.
	SeeAlso:	-

********************************************************************************************/

void PaletteManager::FindNearestColour(const ColourRGBT& Colour, LPLOGPALETTE pPalette, ColourRGBT* const pResult)
{
	ERROR3IF(pResult == NULL,"NULL pResult pointer");
	if (pResult == NULL)
		return;

	// Convert original colour into an RGB palette entry
	PALETTEENTRY PalColour;
	PalColour.peRed   = BYTE(Colour.Red.MakeDouble()*255);
	PalColour.peGreen = BYTE(Colour.Green.MakeDouble()*255);
	PalColour.peBlue  = BYTE(Colour.Blue.MakeDouble()*255);

	// Find the closest palette entry
	PALETTEENTRY ClosestPalColour = FindNearestColour(PalColour,pPalette);

	// Convert back to a ColourRGBT
	pResult->Red   = FIXED24(double(ClosestPalColour.peRed)/255.0);
	pResult->Green = FIXED24(double(ClosestPalColour.peGreen)/255.0);
	pResult->Blue  = FIXED24(double(ClosestPalColour.peBlue)/255.0);
}

/********************************************************************************************

>	static PALETTEENTRY PaletteManager::FindNearestColour(const PALETTEENTRY& PalColour, LPLOGPALETTE pPalette)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/97
	Inputs:		PalColour	= Original colour definition
				pPalette	= look-up palette

	Returns:	A colour from the palette that's closest to the original
	Purpose:	Finds the closest colour in the given palette.

				The closest colour is the one that is closest in terms of distance within the RGB colour
				cube.
	SeeAlso:	-

********************************************************************************************/

PALETTEENTRY PaletteManager::FindNearestColour(const PALETTEENTRY& PalColour, LPLOGPALETTE pPalette)
{
	PALETTEENTRY Dummy = {0,0,0,0};
	ERROR2IF(pPalette == NULL,Dummy,"NULL Palette ptr");
	ERROR2IF(pPalette->palNumEntries < 1,Dummy,"No Palette entries");

	double SmallestDistance = 0.0;
	BOOL FirstDistanceCalc = TRUE;
	INT32 ClosestColourIndex = 0;

	// look at all the colours in the palette
	for (INT32 i=0;i<pPalette->palNumEntries;i++)
	{
		// If the peFlags is 0, then it's not been marked as "ignore", so look at it
		if (pPalette->palPalEntry[i].peFlags == 0)
		{
			// Get the distance between the colour and this palette entry
			double d = FindColourDistanceSquared(&PalColour, &(pPalette->palPalEntry[i]));

			if (FirstDistanceCalc)
			{
				// First time around, so 'd' must be closest yet
				FirstDistanceCalc = FALSE;
				SmallestDistance = d;
				ClosestColourIndex = i;
			}
			else if (d < SmallestDistance)
			{
				// Smallest distance so far?  if so remember the distance & index
				SmallestDistance = d;
				ClosestColourIndex = i;
			}
		}
	}

	// Return the closest palette entry
	return (pPalette->palPalEntry[ClosestColourIndex]);
}

//  This function is identical to FindNearestColour, except that it returns an index
//  rather than a colour.
INT32 PaletteManager::FindIndexOfNearestColour(const PALETTEENTRY& PalColour, LPLOGPALETTE pPalette)
{
//	PALETTEENTRY Dummy = {0,0,0,0};

	double SmallestDistance = 0.0;
	BOOL FirstDistanceCalc = TRUE;
	INT32 ClosestColourIndex = 0;

	// look at all the colours in the palette
	for (INT32 i=0;i<pPalette->palNumEntries;i++)
	{
		// If the peFlags is 0, then it's not been marked as "ignore", so look at it
		if (pPalette->palPalEntry[i].peFlags == 0)
		{
			// Get the distance between the colour and this palette entry
			double d = FindColourDistanceSquared(&PalColour, &(pPalette->palPalEntry[i]));

			if (FirstDistanceCalc)
			{
				// First time around, so 'd' must be closest yet
				FirstDistanceCalc = FALSE;
				SmallestDistance = d;
				ClosestColourIndex = i;
			}
			else if (d < SmallestDistance)
			{
				// Smallest distance so far?  if so remember the distance & index
				SmallestDistance = d;
				ClosestColourIndex = i;
			}
		}
	}

	// Return the closest palette entry
	return ClosestColourIndex;
}

/********************************************************************************************

>	static double PaletteManager::FindColourDistanceSquared(const PALETTEENTRY* pPal1,const PALETTEENTRY* pPal2)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/97
	Inputs:		pPal1 = Colour 1
				pPal2 = Colour 2

	Returns:	The distance between the two colours with the RGB cube, squared 
	Purpose:	Finds the distance between two colours
	SeeAlso:	-

********************************************************************************************/

double PaletteManager::FindColourDistanceSquared(const PALETTEENTRY* pPal1,const PALETTEENTRY* pPal2)
{
	ERROR2IF(pPal1 == NULL || pPal2 == NULL,0.0,"NULL pal entry pointer"); 

	double dr = double(pPal1->peRed   - pPal2->peRed  );
	double dg = double(pPal1->peGreen - pPal2->peGreen);
	double db = double(pPal1->peBlue  - pPal2->peBlue );

	return dr*dr+dg*dg+db*db ;
}

/********************************************************************************************

>	static void PaletteManager::FindNearestBrowserColour(const ColourRGBT& Colour,ColourRGBT* const pResult)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/97
	Inputs:		Colour		= Original colour definition
				pResult		= place to put closest colour definition

	Returns:	pResult contains a colour from the browser palette that's closest to the original
	Purpose:	Finds the closest colour in the browser palette.

				The closest colour is the one that is closest in terms of distance within the RGB colour
				cube.
	SeeAlso:	-

********************************************************************************************/

void PaletteManager::FindNearestBrowserColour(const ColourRGBT& Colour,ColourRGBT* const pResult)
{
	ERROR3IF(pResult == NULL,"NULL pResult pointer");
	if (pResult == NULL)
		return;

	LPLOGPALETTE pBrowserPalette = PaletteManager::CreateBrowserPalette(FALSE);
	if (pBrowserPalette != NULL)
	{
		FindNearestColour(Colour,pBrowserPalette,pResult);
		CCFree(pBrowserPalette);
	}
}

/********************************************************************************************

>	static void PaletteManager::SnapToPalette(LPLOGPALETTE pPalette, const LPLOGPALETTE pPaletteToSnapTo)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/97
	Inputs:		pPalette		 = ptr to palette that will be snapped
				pPaletteToSnapTo = ptr to palette to snap to

	Returns:	-
	Purpose:	This snaps each colour in pPalette to the closest definition in pPaletteToSnapTo

  				The closest colour is the one that is closest in terms of distance within the RGB colour
				cube.

	SeeAlso:	-

********************************************************************************************/

void PaletteManager::SnapToPalette(LPLOGPALETTE pPalette, const LPLOGPALETTE pPaletteToSnapTo)
{
	ERROR3IF(pPalette == NULL || pPaletteToSnapTo == NULL,"NULL pal pointer"); 
	if (pPalette == NULL || pPaletteToSnapTo == NULL)
		return;

	// look at all the colours in the palette
	for (INT32 i=0;i<pPalette->palNumEntries;i++)
	{
		// Get the next pal entry
		PALETTEENTRY PalEntry = pPalette->palPalEntry[i];

		// Find the closest pal entry in the palette to snap to
		PALETTEENTRY NearestPalEntry = FindNearestColour(PalEntry, pPaletteToSnapTo);

		// Set the pal entry to the nearest colour
		pPalette->palPalEntry[i] = NearestPalEntry;
	}
}


/********************************************************************************************

>	static void PaletteManager::SnapToBrowserPalette(LPLOGPALETTE pPalette)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/97
	Inputs:		pPalette		 = ptr to palette that will be snapped

	Returns:	-
	Purpose:	This snaps each colour in pPalette to the closest definition in browser palette

  				The closest colour is the one that is closest in terms of distance within the RGB colour
				cube.

	SeeAlso:	-

********************************************************************************************/

void PaletteManager::SnapToBrowserPalette(LPLOGPALETTE pPalette)
{
	ERROR3IF(pPalette == NULL,"NULL pPalette pointer");
	if (pPalette == NULL)
		return;

	LPLOGPALETTE pBrowserPalette = PaletteManager::CreateBrowserPalette(FALSE);
	if (pBrowserPalette != NULL)
	{
		SnapToPalette(pPalette,pBrowserPalette);
		CCFree(pBrowserPalette);
	}
}

/********************************************************************************************

>	static void PaletteManager::PokePaletteEntry(LPLOGPALETTE Palette, INT32 * index,
												 BYTE red, BYTE green, BYTE blue, BYTE Flags = 0)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/97
	Inputs:		Palette		palette to write to.
				index		address of variable holding palette entry to write to
				red			red value to enter into palette entry 
				green		green value to enter into palette entry 
				blue		blue value to enter into palette entry
	Outputs:	index incremented by one 
	Returns:	-
	Purpose:	Puts the required value	of rgb into the specified palette entry.
	SeeAlso:	OutputDIB::PokePaletteEntry;

********************************************************************************************/
void PaletteManager::PokePaletteEntry(LPLOGPALETTE Palette, INT32 * index,
									  BYTE red, BYTE green, BYTE blue, BYTE Flags)
{
	ERROR3IF(Palette == NULL || index == NULL,"PaletteManager::PokePaletteEntry bad params");
	if (Palette == NULL || index == NULL)
		return;

	Palette->palPalEntry[*index].peRed = red;	
	Palette->palPalEntry[*index].peGreen = green;	
	Palette->palPalEntry[*index].peBlue = blue;	
	Palette->palPalEntry[*index].peFlags = 0;
	// increment the counter and return it
	(*index) ++;	
}	

/********************************************************************************************

>	static LPLOGPALETTE PaletteManager::Get4bppBrowserPalette(UINT32 ReservedColours)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> Humprhys
	Created:	15/8/97
	Inputs:		ReservedColours		number of reserved colours in this palette e.g. transparent colour
	Returns:	LOGPALETTE containing the required palette or NULL
	Purpose:	This produces a browser compatible 4bpp Palette.

********************************************************************************************/

LPLOGPALETTE PaletteManager::Get4bppBrowserPalette(UINT32 ReservedColours)
{
	ERROR3IF(ReservedColours > 15,"Bad number of reserved colours");

	LPLOGPALETTE pPalette = DIBUtil::AllocateLogPalette(16);
	if (pPalette == NULL)
		return NULL;

	INT32 i = 0;
	// Gavin uses a fixed 16 colour palette
	// Usually we use a half intensity value, instead we need to snap it to either
	// 40% (102) or 60% (153) to be browser compatible
	// Define a constant so its easy to change this snapped half intensity value
	const BYTE Shiv = 153;
	// We shall just use a browser snapped version of this 
	PokePaletteEntry(pPalette, &i, 0x00, 0x00, 0x00); // black
	PokePaletteEntry(pPalette, &i, Shiv, 0x00, 0x00); // brown
	PokePaletteEntry(pPalette, &i, 0x00, Shiv, 0x00); // Half green
	PokePaletteEntry(pPalette, &i, Shiv, Shiv, 0x00); // Half browny (red/green)
	PokePaletteEntry(pPalette, &i, 0x00, 0x00, Shiv); // Dark blue
	PokePaletteEntry(pPalette, &i, Shiv, 0x00, Shiv); // Mauve
	PokePaletteEntry(pPalette, &i, 0x00, Shiv, Shiv); // greeny/blue
	PokePaletteEntry(pPalette, &i, Shiv, Shiv, Shiv); // mid grey

	PokePaletteEntry(pPalette, &i, 0xcc, 0xcc, 0xcc); // light grey from 196 to 204
	PokePaletteEntry(pPalette, &i, 0xff, 0x00, 0x00); // red
	PokePaletteEntry(pPalette, &i, 0x00, 0xff, 0x00); // green
	PokePaletteEntry(pPalette, &i, 0xff, 0xff, 0x00); // yellow
	PokePaletteEntry(pPalette, &i, 0x00, 0x00, 0xff); // blue
	PokePaletteEntry(pPalette, &i, 0xff, 0x00, 0xff); // magenta
	PokePaletteEntry(pPalette, &i, 0x00, 0xff, 0xff); // cyan
	PokePaletteEntry(pPalette, &i, 0xff, 0xff, 0xff); // white

/*	if (ReservedColours > 0)
	{
		// Used to pick index 10 to be transparent but now we pick the light grey
		i = 8;
		PokePaletteEntry(pPalette, &i, 0xFF, 0xFF, 0xFF, 0xFF); // don't use white

		// Warn if we try to pick more than 1 reserved colour
		ERROR3IF(ReservedColours > 1,"Get4bppBrowserPalette We can only cope with 1 reserved colour");
	} */

	return pPalette;
}

/********************************************************************************************

>	static INT32 PaletteManager::FindFirstDontUseColourInPalette(LPLOGPALETTE pPalette)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> Humprhys
	Created:	15/8/97
	Inputs:		pPalette		palette to check
	Returns:	index of the transparent colour or -1
	Purpose:	This finds the first colour marked as don't use in the specified palette.
				This colour should correspond to the transparent colour.

********************************************************************************************/

INT32 PaletteManager::FindFirstDontUseColourInPalette(LPLOGPALETTE pPalette)
{
	ERROR2IF(pPalette == NULL,-1,"FindFirstDontUseColourInPalette Bad palette");

	UINT32 colours = pPalette->palNumEntries; 
	INT32 DontUseColour = -1; 
	PALETTEENTRY * pPaletteEntry = pPalette->palPalEntry;
	for (UINT32 i = 0; (i < colours) && (DontUseColour == -1); i++)
	{
		if (pPaletteEntry[i].peFlags == 0xFF)
			DontUseColour = i;
	}
	
	return DontUseColour;
}
