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

/*
*/

// GRNDRGN.CPP
//
// Created: 29/6/93 by Will
//

#include "camtypes.h"
// camconfig.h must be included immediately after camtypes.h
#include "camconfig.h"

//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]

// Includes (alphabetically ordered for convenience)
//#include "andy.h"
//#include "app.h"
//#include "attrmgr.h"
#include "bitmpinf.h"
#include "camelot.h"
//GAT #include "camvw.h"
#include "camview.h"
//#include "ccfile.h"
#include "colormgr.h"
//#include "colplate.h"
//#include "csrstack.h"

#include "devcolor.h"
//#include "dibconv.h"			// GAT - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "dibutil.h"
//#include "document.h"
//#include "ensure.h"
//#include "errors.h"
#include "f16spec.h"			// for IsMathCoprocInstalled
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//GAT #include "fonts.h"				// For FixedSystem font from the FontFactory
//#include "fracfill.h"
#include "fuzzclip.h"

#include "gbrush.h"				// GAT
//#include "gconsts.h"
//#include "gdimagic.h"
#include "XaDraw.h"				// naughty
//GAT #include "gdrawasm.h"
#include "gdrawcon.h"
#include "gradtbl.h"
//GAT #include "grndbmp.h"
//GAT #include "grndbrsh.h"			// so we don't init offscreen rendering for GRenderBrush

#include "cstroke.h"			// Gavins new GenStroke Functions! MRH 19/5/00

//#if REAL_DDB
//#include "grndddb.h"
//#endif

//#include "grnddib.h"
#include "grndprnt.h"
#include "grndrgn.h"
//GAT #include "grndwing.h"
//#include "gversion.h"

//#include "mainfrm.h"
#include "moldpers.h"
//#include "moldshap.h"
//#include "nodemold.h"
//#include "ops.h"
#include "osrndrgn.h"
#include "palman.h"
//#include "paper.h"
//#include "paths.h"
//#include "pathutil.h"
#include "princomp.h"
#include "printctl.h"

#include "qualattr.h"
//#include "speedtst.h"
//#include "view.h"
#include "oilbitmap.h"
//#include "ralphdoc.h"
#include "fillramp.h"
#include "bubbleid.h"
#include "grndclik.h"
//#include "mario.h"
#include <math.h>

// Offscreen rendering system - Ilan
#include "offattr.h"
#include "offscrn.h"
//#include "capturemanager.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "clipattr.h"	// so we know what a ClipRegionAttribute is.

#include "maskedrr.h"
#include "prdlgctl.h"

#include "camprofile.h"
#include "fillattr2.h"

DECLARE_SOURCE("$Revision$");


// Declare all the classes in this file and ask for memory tracking
CC_IMPLEMENT_DYNAMIC( GRenderRegion, RenderRegion )
CC_IMPLEMENT_DYNCREATE(OpGDraw, Operation)

#define new CAM_DEBUG_NEW

// Release version of VC++ 2 does not have all the GDI constants that the old one
// did, so we add the one we need here.
#ifndef DIB_PAL_INDICES
#define DIB_PAL_INDICES (2)
#endif

// Define these symbols if you want extra debugging help
#ifdef _DEBUG
//	#define DEBUG_ATTACH_CAPTURED_BMP
//	#define DEBUG_ATTACH_RENDERSTATE_BMP
//	#define DEBUG_ATTACH_TCHANGE_BMP
#endif


BOOL GRenderRegion::WantDoGDraw = TRUE;				// TRUE if user wants it
UINT32 GRenderRegion::WantGDrawDepth;					// desired depth of off-screen bitmap
//BOOL GRenderRegion::WantNoPalette;					// TRUE if palette switching not wanted
//UINT32 GRenderRegion::WantBlitMode;					// 0=auto, 1=Streth, 2=SetDIB, 3=BitBlt, 4=BadDDB
//BOOL GRenderRegion::WantHighColourDirect;			// TRUE if deep sprite plots directly
//BOOL GRenderRegion::WantDeepGDraw;					// TRUE for 32-bit DIB plotting to screen
//BOOL GRenderRegion::WantWinG;						// TRUE for WinG rendering
INT32  GRenderRegion::WhichTransparency = 0;			// type of transparency

BOOL GRenderRegion::CanDoGDraw;						// TRUE if it is physically permitted
//BOOL GRenderRegion::CanDoPalIndices;				// TRUE if GDI does DIB_PAL_INDICES
//BOOL GRenderRegion::CanSetPalette;					// TRUE if screen has palette support
//BOOL GRenderRegion::CanDoDeepDIBs; 					// TRUE if understands 16- and 32-bpp DIBs

BitmapConvertHint GRenderRegion::ScreenHinting;		// used to hint bitmap conversions

INT32 GRenderRegion::ScreenDepth;						// in bits/pixel
INT32 GRenderRegion::ScreenWidth;						// in pixels
INT32 GRenderRegion::ScreenHeight;					// in pixels
INT32 GRenderRegion::LogPixelsX;
INT32 GRenderRegion::LogPixelsY;

GDrawContext *GRenderRegion::pRealGD = NULL;		// ptr to GDraw we want to use

GDrawContext *GRenderRegion::GD = NULL;				// ptr to GDraw we want to use

GDrawContext *GRenderRegion::ErrorDiffContext = NULL;	// ptr to GDraw we want to use for error diffusion

LPRGBQUAD GRenderRegion::Fixed256Palette;			// NULL if not fixed palette (256 cols only)

//BOOL GRenderRegion::RealFPU;						// TRUE if real, FALSE if emulated

//INT32 GRenderRegion::LastPaletteFlag = -1;			// Last value passed to GDraw_SelectPalette

INT32 GRenderRegion::ViewDither = 2;					// Dither style used for views
INT32  GRenderRegion::WhichDither = 2;				// which dither mode for 32->8 conversions

RealLogPalette GRenderRegion::ErrorDiffPalette = 	// ptr to GDraw's default palette so we do not
{													// have to call GDrawContext::SelectPalette (with its
	0x300,											// possibly slow context switch)
	256
};

static RealLogPalette StandardPalette = 
{
	0x300,
	256
};

static BYTE NoTransTable[256];						// default translation table

static BOOL ClippedPathIsValid;

enum BlitModeType
{
	BLITMODE_STRETCH,
	BLITMODE_SETDIBS,
	BLITMODE_BITBLT,
	BLITMODE_BADDDB
} BlitMode;

// TODOG: Nasty local preference...
static INT32 LargeGradTables = 0;
	  
// setting this to non-zero controls fresh bitmap initialisation
#define	FILL_BITMAP	0


/********************************************************************************************

	Preference:	ClickTranspLimit
	Section:	Screen
	Range:		0 to 255
	Purpose:	Determine how transparent rendered pixels may be before click detection
				will ignore them.
				E.g. 230 means pixels must be 90% transparent before they will be ignored

********************************************************************************************/

UINT32 GRenderRegion::ClickTranspLimit = 255;


/********************************************************************************************

	Preference:	HighQualityAA
	Section:	Rendering
	Range:		FALSE - normal Anti-aliasing
				TRUE - High quality Anti-aliasing
	Purpose:	Determine whether to use GDraw's high quality anti-aliasing mode

********************************************************************************************/

BOOL GRenderRegion::HighQualityAA = FALSE;

/********************************************************************************************

	Preference:	BitmapConversion
	Section:	Rendering
	Range:		0 - no bitmap conversion (show printer colours will render wrong)
				1 - fast conversion (visible banding in deep bitmaps and cached stuff)
				2 - medium conversion (best setting for general use)
				3 - slow conversion (not yet implemented, most accurate but slow)
	Purpose:	Specifies which bitmap conversion algorithm is used by GDraw when rendering 
				bitmaps in show printer colours mode.

********************************************************************************************/

INT32 GRenderRegion::BitmapConversion = 2;


/********************************************************************************************

	Preference:	RealFPU
	Section:	Screen
	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Range:		0 or 1
	Purpose:	To tell GDraw whether there is a real FPU in the machine, or whether it
				is emulated. It uses less FPU and more integer when this is 0, giving better
				performance on SX machines. Default value to suit hardware.

********************************************************************************************/




/********************************************************************************************

	Preference:	UseGdraw
	Section:	Debugflags
	Range:		TRUE or FALSE
	Purpose:	Allows the user to determine whether gdraw is used or not. Default to on.

********************************************************************************************/

/********************************************************************************************

	Preference:	GDrawDepth
	Section:	Debugflags
	Range:		0,1,4,8,16,32 (24 is NOT valid)
	Purpose:	Allows the user to determine the depth of the off-screen bitmap used in
				GDraw rendering. Defaults to 0 which means 'same as screen'.

********************************************************************************************/

/********************************************************************************************

	Preference:	ShowTransparency
	Section:	Debugflags
	Range:		TRUE or FALSE
	Purpose:	Allows the user to determine whether 32-bit DIBs are used or not, to control
				transparency. Will vanish at some point when this can be determined
				algorithmically. GDrawDepth must be zero for this to work.

********************************************************************************************/

/********************************************************************************************

	Preference:	TransparencyType
	Section:	Debugflags
	Range:		0=none, 1=reflective, 2=subtractive, 3=additive
	Purpose:	Allows the user to determine type of transpanrency used.

********************************************************************************************/

/********************************************************************************************

	Preference:	TryWinG
	Section:	Debugflags
	Range:		TRUE or FALSE
	Purpose:	Allows the user to use WinG (if available).

********************************************************************************************/

/********************************************************************************************

	Preference:	DontUsePalette
	Section:	Debugflags
	Range:		0 or 1
	Purpose:	Allows the user to stop changing the Windows palette. Defaults to 0 which
				means will use palette if available. Setting to 1 allows debugging of code
				as if it was a fixed-palete device (although such devices usually misbehave
				in the GetSystemPaletteUse call among others so the emulation isn't perfect).

********************************************************************************************/

/********************************************************************************************

	Preference:	BlitMode
	Section:	Debugflags
	Range:		0=auto, 1=StretchDIB, 2=SetDibsToDevice, 3=BitBlt
	Purpose:	Specified which blit method to use, defaults to 0 which is 1 for Win32s,
				2 for Windows NT. One day might be able to work out best method by trying
				out the	video driver. Value of 3 is iffy.

********************************************************************************************/

/********************************************************************************************

	Preference:	HighColourDirect
	Section:	Debugflags
	Range:		0=no, 1=yes
	Purpose:	Makes high-colour bitmaps get plotted directly, instead of being converted
				into 256 colours first. Is very much slower than conversion. Defaults to 0.

********************************************************************************************/


/********************************************************************************************

	Preference:	WhichDither
	Section:	Debugflags
	Range:		0-n (not range checked).
	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Purpose:	Determines which of Gavin's dither algorithms are used when converting
				high colour bitmaps down to lower colour ones. Defaults to 2 (greyscale
				corrected), 1=error diffused, 0=ordered dither

********************************************************************************************/





/////////////////////////////////////////////
// in order to help debug this code, we keep a history of calls so we can see what has
// happened. For use only in desperate debug situations.
// 0 means no, 1 means remember, 2 means dump as we go

#define	REMEMBER_GDRAW	0

#if REMEMBER_GDRAW

static struct RemRR
{
	GRenderRegion *RR;
	char *Where;
} RememberList[256];										// crude circular list

static INT32 RememberWhere = 0;

static void DoRememberMe(GRenderRegion *lpG, char *name)
{
	if (
		RememberWhere &&
		(RememberList[ RememberWhere-1 ].Where == name) &&	// pointer comparison is OK
		(RememberList[ RememberWhere-1 ].RR == lpG)
	   )
		// same as last time so ignore
		return;

	RememberList[ RememberWhere ].Where = name;
	RememberList[ RememberWhere ].RR = lpG;

	RememberWhere++;
	if (RememberWhere == 256)
		RememberWhere = 0;										// wrap past the end

	#if REMEMBER_GDRAW==2
	TRACE( _T("%08lx %s\n"), lpG, name );
	#endif
}

static void DoRememberDump(GRenderRegion *lpG)
{
	for (INT32 i=0; i<RememberWhere; i++)
		TRACE( _T("%08lx %s\n"), RememberList[i].RR, RememberList[i].Where);		
}

#define	RememberMe(name)	DoRememberMe(this, name)
#define	RememberDump()		DoRememberDump(this)

#else
// stub the function calls out
#define	RememberMe(name)
#define	RememberDump()

#endif

/********************************************************************************************

>	BOOL GRenderRegion::Init( BOOL bFirstTime )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/93
	Inputs:		bFirstTime is TRUE if called during startup, or FALSE if called subsequently
				(e.g. when screen mode changes).
	Outputs:	-
	Returns:	TRUE if worked OK
	Purpose:	Determine possibility of using gdraw routines and determine various
				characterisitics of the screen.
	Errors:		-
	Scope:		Static

********************************************************************************************/

BOOL GRenderRegion::Init( BOOL bFirstTime )
{
	BOOL GDrawImpossible = FALSE;

	// Scary critical section stuff
//	CriticalSection::Init();	// Not required for CDraw
	
	if (bFirstTime)
	{
		CanDoGDraw = FALSE;

		if (Camelot.DeclareSection( _T("DebugFlags"), 20))
		{
			Camelot.DeclarePref( NULL, _T("UseGdraw"), &WantDoGDraw, FALSE, TRUE );
			Camelot.DeclarePref( NULL, _T("GDrawDepth"), &WantGDrawDepth, 0, 32 );
//			Camelot.DeclarePref( NULL, "DontUsePalette", &WantNoPalette, FALSE, TRUE );
//			Camelot.DeclarePref( NULL, "BlitMode", &WantBlitMode, 0, 4 );
//			Camelot.DeclarePref( NULL, "HighColourDirect", &WantHighColourDirect, 0, 1 );
//			Camelot.DeclarePref( NULL, "TryWinG", &WantWinG, FALSE, TRUE );
			Camelot.DeclarePref( NULL, _T("WhichDither"), &WhichDither, 0, 20 );
//			Camelot.DeclarePref( NULL, "LargeGradTables", &LargeGradTables, 0, 1 );
// TransparencyType removed by Gerry (23/8/96) cos its rampant
//			Camelot.DeclarePref( NULL, "TransparencyType", &WhichTransparency, 0, 10 );
//			Camelot.DeclarePref( NULL, "ResizeRegions", &ResizeRegions, FALSE, TRUE );
		}

		if (Camelot.DeclareSection( _T("Screen"), 10))
		{
			Camelot.DeclarePref( NULL, _T("ClickTranspLimit"), &ClickTranspLimit, 0, 255 );
			// defaults to hardware value
//			RealFPU = IsMathCoprocInstalled();
//			Camelot.DeclarePref( NULL, "RealFPU", &RealFPU, FALSE, TRUE );
			Camelot.DeclarePref(NULL, _T("ViewDither"), &ViewDither, 0, 4);
		}

		if (Camelot.DeclareSection(_T("Rendering"), 20))
		{
			Camelot.DeclarePref( _T("Rendering"), _T("HighQualityAA"), &HighQualityAA, FALSE, TRUE );
			Camelot.DeclarePref( _T("Rendering"), _T("BitmapConversion"), &BitmapConversion, 0, 3 );
		}

		if (pRealGD == NULL)
			pRealGD = new GDrawAsm();						// get permanent one

		// fill in default translation tables
		for (INT32 i=0; i<256; i++)
			NoTransTable[i] = i;
	}

	// Initialise the palette manager
//	PaletteManager::Init();
	
	// Initialise the DIBConvert classes (don't care if it fails)
	DIBConvert::Init();

	::wxDisplaySize(&ScreenWidth,&ScreenHeight);
	ScreenDepth = ::wxDisplayDepth();
	wxScreenDC dc;
	wxSize size = OSRenderRegion::GetFixedDCPPI(dc);
PORTNOTE("other","Can't handle different DPIs, using X")
	LogPixelsX = size.x;
	LogPixelsY = size.y; //size.y;
	ERROR3IF(LogPixelsX != LogPixelsY, "Luke says non-square pixels are not supported");

//	CanSetPalette = FALSE;

	// on startup lets make sure the GDraw version is OK. Must be done before any GDraw calls
	if (bFirstTime)
	{
		if (!pRealGD->Init())							// checks version etc
		{
			GDrawImpossible = TRUE;
			CanDoGDraw = FALSE;
			Error::SetError( _R(IDW_BADGDRAW), NULL, 0 );
			InformWarning();							// tell the user his DLL is wrong
			Error::ClearError();						// else we won't start up
		}
		GD = pRealGD;
	}
/*
	switch ( ScreenDepth )
	{
	case  4 : ScreenHinting = CONVHINT_SCREEN4; break;
	case  8 : ScreenHinting = CONVHINT_SCREEN8; break;
	case 16 : ScreenHinting = CONVHINT_FINAL16; break;
	case 24 : ScreenHinting = CONVHINT_FINAL24; break;
	default : ScreenHinting = CONVHINT_NONE;
	}
*/
	// we can set a palette if we are on a 256-colour screen and it says that we can
PORTNOTE("other","GRenderRegion::Init - Removed palette code")
#ifndef EXCLUDE_FROM_XARALX
	if (
		ScreenDepth==8 &&				// 256 cols only
		!WantNoPalette					// and user hasn't forbidden it
	   )
	{
		if (GetDeviceCaps( hdc, RASTERCAPS ) & RC_PALETTE )
			CanSetPalette = TRUE;
	}
#endif

	ScreenHinting = CONVHINT_NONE;		// safe default value
	if ( ScreenDepth<=8 )
		ScreenHinting = DIBUtil::CalcConvertHint(ScreenDepth,NULL);
	else
	{
		// work out bitmap hinting by opening tiny window in top left of screen
		// It is TOPMOST so that, with luck, nothing will obscure it
	//	wxWindow tempWindow(AfxGetApp().GetTopWindow(),-1,wxPoint(0,0),wxSize(0,0),wxNO_BORDER);
	//	tempWindow.Show();
	//	tempWindow.Raise();
	//	wxClientDC tempDC(&tempWindow);
	//	ScreenHinting = DIBUtil::CalcConvertHint(ScreenDepth,&tempDC);
		wxColour colour;
		dc.GetPixel(0,0,&colour);
		ScreenHinting = DIBUtil::CalcConvertHint(ScreenDepth,&dc);
		dc.SetPen(wxPen(colour));
		dc.DrawPoint(0,0);
	}

	if (
		!GDrawImpossible &&
		(
			ScreenDepth==1 ||
			ScreenDepth==4 ||
			ScreenDepth==8 ||									// only certain depths allowed
			ScreenDepth==16 ||
			ScreenDepth==24 ||
			ScreenDepth==32
		)
	   )
	{
		CanDoGDraw = TRUE;
/*
		// DIB_PAL_INDICES is not available on Win32s or Chicago
		// or on 256 colour fixed palette devices
		if (IsWin32s())
			CanDoPalIndices = FALSE;
		else
			CanDoPalIndices = TRUE;
*/
		EnsurePalette(0);
		// set the stack limit to 100k. Assumes stacks go backwards
		GD->SetStackSize(100*1024);

		if (ErrorDiffContext == NULL)
		{
			ErrorDiffContext = new GDrawAsm();			// get permanent one

			if (ErrorDiffContext)
				ErrorDiffContext->Init();
		}

/*		UpdateErrorDiffPalette();

		if (
			(ScreenDepth==8) &&
			!CanSetPalette
		   )
		{
			// if we're a 256 colour fixed palette device then we make up a default RGB table
			// and use that
			Fixed256Palette = (LPRGBQUAD) CCMalloc( sizeof(RGBQUAD) * 256 );

			const LOGPALETTE* lpPal = GD->SelectPalette( 0 );

			if (Fixed256Palette && lpPal)
			{
				for (INT32 i=0; i<256; i++)
				{
					Fixed256Palette[i].rgbRed   = lpPal->palPalEntry[i].peRed;
					Fixed256Palette[i].rgbGreen = lpPal->palPalEntry[i].peGreen;
					Fixed256Palette[i].rgbBlue  = lpPal->palPalEntry[i].peBlue;
					Fixed256Palette[i].rgbReserved = 0;
				}
			}

			// DIB_PAL_INDICES makes no sense on these devices
			CanDoPalIndices = FALSE;
		}
*/
	}
/*
	switch (WantBlitMode)
	{
		case 4:
			#if REAL_DDB
			// user wants badDDBs, only allow if possible
			if (GRenderDDB::CanDoBadDDBs())
			{
				BlitMode = BLITMODE_BADDDB;
				break;
			}
			#endif
			// else fall through to default mode
			nobreak;
		case 0:
			// automatic setting using faster blits on non-NT platforms
			if (IsWin32NT())
				BlitMode = BLITMODE_SETDIBS;
			else
				BlitMode = ( (ScreenDepth==4) || (ScreenDepth==8) ) ? BLITMODE_BITBLT : BLITMODE_STRETCH;
			break;
		case 1:
			BlitMode = BLITMODE_STRETCH;
			break;
		case 2:
			BlitMode = BLITMODE_SETDIBS;
			break;
		case 3:
			// use DDBs on 16- and 256-colour modes only
			BlitMode = ( (ScreenDepth==4) || (ScreenDepth==8) ) ? BLITMODE_BITBLT : BLITMODE_STRETCH;
			break;
		default:
			BlitMode = BLITMODE_STRETCH;					// safest default value
			break;
	}
*/
	// only NT can do BI_BITFIELD, Win32s and Chicago cannot.
	// Actually NT isn't guaranteed to support that any more
	//CanBiBitfield = IsWin32NT() ? TRUE : FALSE;

	// only NT can do 16- and 32-bit DIBs, Win32s cannot. Chicago might?
//	CanDoDeepDIBs = IsWin32NT() ? TRUE : FALSE;
// Changed by Will, on 14/7/95, as requested by Jason.
// Apparently the PlotDeepDIB routine doesn't dither correctly.
//	CanDoDeepDIBs = FALSE;

//	#if REAL_DDB && WIN16
//	GRenderDDB::CanDoBadDDBs();
//	#endif

	// init WinG if required. Saves memory if not wanted
//	if (WantWinG)
//		GRenderWinG::Init( bFirstTime );

	if (!Operation::RegisterOpDescriptor(
						0,
						_R(IDS_GDRAW),
						CC_RUNTIME_CLASS(OpGDraw),
						OPTOKEN_GDRAW,
						OpGDraw::GetState,
						0,	// help ID
						_R(IDBBL_GDRAWOP),
						0	// bitmap ID
						))
		return FALSE; 

	if (!GBrush::InitGBrush( bFirstTime ))
		return FALSE;

	INT32 Flatness = INT32(MILLIPOINTS_PER_INCH/96 / 2);
	GD->SetFlatness( Flatness );

	return TRUE;
}

/********************************************************************************************

>	void GRenderRegion::DeInit()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Call once during termination to perform any necessary cleanup.
	Errors:		-
	Scope:		Static

********************************************************************************************/

void GRenderRegion::DeInit()
{
/*	if (Fixed256Palette != NULL)
	{
		CCFree(Fixed256Palette);
		Fixed256Palette = NULL;
	}
*/
	ERROR3IF(GD != pRealGD, "GDraw context not restored in GRenderRegion::DeInit");
	
	GD = NULL;

	if (pRealGD)
	{
		delete pRealGD;
		pRealGD = NULL;
	}

	// Clean up the DIBConvert classes
	DIBConvert::DeInit();

	if (ErrorDiffContext != NULL)
	{
		delete ErrorDiffContext;
		ErrorDiffContext = NULL;
	}

//	GRenderWinG::Deinit();

	// Scary critical section stuff
//	CriticalSection::DeInit();	// Not required for CDraw
	
	// Make sure GBrush doesn't leave any memory leaks lying around
	GBrush::DeinitGBrush();
}

// set this to 1 to leave a border around all GDraw plotting areas
#define	DEBUG_BORDER	0

/********************************************************************************************

>	GRenderRegion::GRenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale,
									UINT32 Depth, double dpi)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93
	Inputs:		ClipRect is a DocRect defining the invalid rectangle to be rendered.
				ConvertMatrix is a Matrix for converting Doc coords to OS coords.
				ViewScale is the scale factor of the view, used to calculate how much to
				flatten paths. Depth is bitmap depth required. dpi is the pixels-per-inch
				which can be zero for screen resolution.
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for a GRenderRegion.
				INT32 dpi changed to double dpi (12/12/95) to improve the range of values allowed
				at the < 1000dpi settings that we will be using.
	Errors:		-
	SeeAlso:    -

********************************************************************************************/

GRenderRegion::GRenderRegion(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale,
								UINT32 Depth, double dpi)
	: RenderRegion(ClipRegion, ConvertMatrix, ViewScale)
{
	pBitmapInfo	= NULL;
	pBits		= NULL;
#if USE_wxBITMAP
	pBitmap		= NULL;
	pBitmapData	= NULL;
#endif

//	BitmapSize = 0;
	
	// If specified dpi is zero then use the calculated screen dpi otherwise use the specified value
	if (dpi == 0.0)
		PixelsPerInch = (double)LogPixelsX;
	else
		PixelsPerInch = dpi;
	ERROR3IF(PixelsPerInch < 4.0,"GRenderRegion PixelsPerInch < 4 dpi, is this good?");

	hPalette = NULL;
	hPrevPalette = NULL;
	uBitmapDepth = Depth;
//GAT	FractalBitmap = NULL;
	UseSolidColours = FALSE;
	LocalBitmap = TRUE;								// always local bitmap currently
	m_bEnableConversion = TRUE;
	dScaledPixelWidth = 0.0;

	if (LocalBitmap)
	{
		WRect = wxRect(0,0,0,0);

		// this ensure triggers up on bitmap export, so it is removed
		//ENSURE( (WRect.left>=0) && (WRect.top>=0), "neg wrect");

		#if DEBUG_BORDER
		// kludge the rectangle slightly
		WRect.left += 2; WRect.width -= 4;
		WRect.top += 2; WRect.height -= 4;
		#endif
	}
	else
		ENSURE(FALSE, "Non local bitmap error");

	// BG time slice should be 0.5 of a second due to blit overhead
	Timeslice = 500;

	// Set default dither type
	DitherStyle8Bit = WhichDither;

	m_DoCompression = FALSE;

	m_bForceBitmapSmoothing = FALSE;
	m_ForcePrintingCaps = FALSE;

	// Set the render caps up
	GetRenderRegionCaps(&Caps);

	// Ilan
	IsWrapped = FALSE;

	// Default to not forcing Init (allocation will force it anyway)
	ForceInitBmpBits = FALSE;

	RememberMe("Constructor");
}


/********************************************************************************************

>	GRenderRegion::~GRenderRegion()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default Destructor for GRenderRegion Class. Frees up memory.
	Errors:		-
	SeeAlso:	- GRenderRegion::FreeOffscreenState()

********************************************************************************************/

GRenderRegion::~GRenderRegion()
{
#ifdef RALPH
#ifdef _DEBUG
	if(GetCurrentThreadId() == RalphDocument::GetImportingThreadID())
	{
		TRACE( _T("Whoop Whoop... GRenderRegion::~GRenderRegion called from load thread\n"));
		AfxDebugBreak();
	}
#endif
#endif

	if (RenderFlags.Rendering)
	{
		TRACE( _T("StopRender() was not called before destructor\n") );
		StopRender();
	}

//GAT	if (FractalBitmap)
//		delete FractalBitmap;
	
	// manual removal of the contents of our clip-maps.
	FreeStoredClipRegions();

	RememberMe("Destructor");
}


// Call this inplace of FreeLPBits in any derived classes!!!!
// Used in deconstructors of derived classes and in GetNextBand(), ResetRegion(), ResizeRegion()
void GRenderRegion::FreeOffscreenState()
{
	// This function gets called when render regions deconstruct
	// It frees memory allocate to offscreen bitmaps as wella s the inital
	// bitmap.
	// NB this gets called before the attribute context stack is unwound (this happens
	// when ~RenderRegion calls CleanUpBeforeDestruct(RenderRegion *pRegion))

	// This function frees handles the freeing of all memory allocted to bitmaps. Includes:-
	// 1) offscreen bmp memory	- alloc'd by the Offscreen rendering system, NOT from limited mem
	// 2) initial bmp memory	- alloc'd by virtual GetLPBits() in a derived class specific fashion
	//							- (ie either from limited mem manager or normal heap)
	//
	// It also restores the initial rendering states' wrect, cliprect, and gmatrix variables

	Capture* pCapture = NULL;
	while (GetTopCapture())
	{
		pCapture = GetTopCapture();
		m_CaptureStack.Pop();

		// If we've reached the master capture
		// reset the rendering variables in case this RenderRegion survives for a while
		if (pCapture->IsMaster())
			SetRenderToCapture(pCapture, FALSE, FALSE);
		else
			pCapture->FreeDIB();

		delete pCapture;
	}

#if USE_wxBITMAP
	FreeWxBitmap() ;
#else
	if ( pBitmapInfo && pBits )
	{
		FreeLPBits( pBitmapInfo, pBits );
		pBitmapInfo = NULL;
		pBits = NULL;
	}
#endif
}

BOOL GRenderRegion::CopyRenderInfo( const RenderRegion &Other)
{
	RememberMe("CopyRenderInfo");

	if (!RenderRegion::CopyRenderInfo( Other ))
		return FALSE;

	ENSURE( Other.IsKindOf( CC_RUNTIME_CLASS(GRenderRegion) ) , "CopyRenderOther wrong class" );

	// safe to cast now
	const GRenderRegion *GOther = (GRenderRegion*)&Other;

	// copy most of the member variables
	LocalBitmap = GOther->LocalBitmap;
	if (LocalBitmap)
	{
		// leave WRect alone
		ScreenRect = wxRect(0,0,0,0);
	}
	else
	{
		// (hmm, untested dubious code)
		WRect = GOther->WRect;
		ScreenRect = GOther->ScreenRect;
	}

	PixelsPerInch = GOther->PixelsPerInch;
	dScaledPixelWidth = GOther->dScaledPixelWidth;
	hPalette = GOther->hPalette;
	hPrevPalette = NULL;

	return TRUE;
}

/********************************************************************************************
>	BOOL GRenderRegion::()
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/00
	Purpose:	Get rid of offscreen state stack so we can restart from scratch
	Errors:		-
	SeeAlso:	GRenderDIB::ResizeRegion()
********************************************************************************************/
void GRenderRegion::ResetRegion(DocRect &NewClipRect)
{
#ifdef _DEBUG
//	if(CurrentRenderState)
//		TRACEUSER( "Ilan", _T("RR (0x%x) being reset\n   Context = %s (0x%x)\n   Offscreen stack size = %d\n"), (DWORD)this, CurrentRenderState->Name(),(DWORD)CurrentRenderState, m_OffscreenStateStack.Size());
//		TRACEUSER( "Ilan", _T("RR (0x%x) being reset\n   Context = %s (0x%x)\n   Offscreen stack size = %d\n"), (DWORD)this, CurrentRenderState->Name(),(DWORD)CurrentRenderState, m_CaptureStack.Size());
#endif

	// base class version (called below) needs initial CurrentClipRect to be correct.
	// FreeOffscreenState restores RR state as well as freeing offscreen bitmap memory and stacked states
	FreeOffscreenState();

	RenderRegion::ResetRegion(NewClipRect);
}

// NB if you override this ensure that you set lpBitmapInfo->bmiHeader.biCompression
// as it must be correct when GDrawAsm::SetBitmap() is called
void GRenderRegion::InitBmpBits()
{
	ENSURE(pBits,"Call to InitBmpBits with null bits pointer!");
	if(!pBits)
		return;

	LPBITMAPINFOHEADER bh = &pBitmapInfo->bmiHeader;
	BYTE* pSetBits = (BYTE*)pBits;

	BOOL UseInternalFormat = FALSE;
	if(m_DoCompression)
	{
		bh->biCompression=0x80000001;		// needs to be set correctly for GetDrawContext()->SetBitmap()
		UseInternalFormat = TRUE;
	}

	// for offscreen rendering system which doesn't set m_DoCompression properly
	if(bh->biCompression==0x80000001)
		UseInternalFormat = TRUE;

	// ensure all bitmap memory is initialised (ie don't use the 
	// bogus info from the bmp header dimensions)
	// NB this assumes that the bmp buff lpBits was allocated using AllocDIB (usually via GetLPBits() fn)
	UINT32 BmpSize = GetDIBBitsSize( bh );
	UINT32 Index = 0;

	if(bh->biBitCount==32)
	{
		// 32bpp
		if(UseInternalFormat)
		{
//			TRACEUSER( "Gerry", _T("Initialising GRR to transparent black\n"));
			while(Index < BmpSize)
			{
				// initial values for using internal alpha channelled format
				// fully transparent black bacground

				pSetBits[Index++] = 0x00;
				pSetBits[Index++] = 0x00;
				pSetBits[Index++] = 0x00;
				pSetBits[Index++] = 0xFF;
			}
		}
		else
		{
//			TRACEUSER( "Gerry", _T("Initialising GRR to opaque white\n"));
			while(Index < BmpSize)
			{
				// fully opaque white background
				pSetBits[Index++] = 0xFF;
				pSetBits[Index++] = 0xFF;
				pSetBits[Index++] = 0xFF;
				pSetBits[Index++] = 0x00;
			}
		}
	}
	else
	{
#if FILL_BITMAP
		// this is taken from the previous bmp initialisation code (ie from old StartRender())
		memset( pBits, FILL_BITMAP+1, BmpSize );
#else
		// this is what I would expect it to be (ie white background)
		memset( pBits, 0xFF, BmpSize );
#endif
	}
}

/********************************************************************************************

>	BOOL GRenderRegion::StartRender()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/93
	Purpose:	Initialises an GRenderRegion for rendering.
				Sets up a bitmap and gmatrix ready for gdraw rendering.
	Errors:		-
	SeeAlso:	OSRenderRegion::StartRender()
********************************************************************************************/
BOOL GRenderRegion::StartRender()
{
	// Ilan
	// The first time StartRender get's called it allocs memory for bitmaps
	// However, subsequent calls are made to StartRender during background redrawing, where
	// rendering pauses partway through to service other messages, and then resumes later
	// In order to pause, StopRender is called to store a pointer to the last node rendered
	// and, in the case where we are not busy rendering offscreen, blit the partly completed
	// bitmap to screen
	// "AllocatedNewBmp" allows us to distinguish between initial calls to StartRender (where
	// GetLPBits is called to alloc new bitmaps), and 'restart' calls which don't need to alloc
	// any new bmps.
	// In the former case the bitmap must be initialised
	// In the later case the bitmap must only be re-initialised if m_bDoCompression is set
	// as in this case inplace bmp conversion will have occured in the process of transfering 
	// the bitmap to the screen (ie during the last StopRender() call)
	BOOL AllocatedNewBmp = FALSE;

	RememberMe("StartRender");

	// Call base class first
	if (!RenderRegion::StartRender())
		return FALSE;
//	RenderMatrix.Dump();

	// Check that Initialise hasn't been called already
	ENSURE(RenderFlags.Rendering == FALSE, "Initialise called whilst already rendering");
	
	// JCF: added this check for retail builds as otherwise this ENSURE goes off after a
	// Crash Me Render.
	if (RenderFlags.Rendering != false) return FALSE;
	
	TRACEUSER("Gavin",_T("GRenderRegion::StartRender - RenderFlags.Rendering = TRUE;\n"));
	RenderFlags.Rendering = TRUE;

	// lets get the Rect into screen co-ords so we can use our big screen bitmap
	// Use a version of OSRenderRegion::DocRectToWin that uses the actual dpi we have stored
	// in PixelsPerInch
	//RECT Rect = OSRenderRegion::DocRectToWin( RenderMatrix, CurrentClipRect, PixelsPerInch );
	// Use a virtual function so that we can now override it in some special case.
	// At present, we use this to try and fix pixel problems on bitmap export (GRenderBitmap)
	// The e and f components of RenderMatrix may be changed by the call, but only in the bitmap export case
	//
	// NB PixelsPerInch always set to 96.0 so never accurate unless at 100% zoom?!
	wxRect Rect;
	if (GetMasterCapture())
		Rect = CalculateWinRect(GetMasterCapture()->CaptureMatrix, GetMasterCapture()->CaptureRect, GetMasterCapture()->dPixelsPerInch);
	else
		Rect = CalculateWinRect(RenderMatrix, CurrentClipRect, PixelsPerInch);

	if (LocalBitmap)
	{
		ScreenRect = wxRect(0,0,0,0);
		// if WRect has changed at all, we're in trouble
		const INT32 NewWidth  =  Rect.width;
		const INT32 OldWidth  = WRect.width;
		const INT32 NewHeight =  Rect.height;
		const INT32 OldHeight = WRect.height;

		// Its changed size and/or position, so we'll try and re-size the bitmap
		// If this fails, then we'll just re-allocate
		// NB GetNextBand() always frees the bitmap memory structs and sets them to NULL
		// GRenderDIB::ResizeRegion sets up new bitmap, WRect and CurrentClipRect so doesn't need this
		// code
		// What would use this code??
		// Code used by ExportRender(GRenderOptPalette* pPalRegion) when rendering in strips
		if ( pBits!=NULL && pBitmapInfo!=NULL && (NewWidth!=OldWidth || NewHeight!=OldHeight) )
		{
			// Make sure we are not after an empty one
			if (NewWidth == 0 || NewHeight == 0)
				return FALSE;

			// Sanity check
			ERROR3IF((NewWidth < 0 || NewHeight < 0),"GRenderRegion::StartRender got a bad rectangle (Width/Height < 0)\n");

			ResetRegion(RegionRect);
			if (!RenderRegion::StartRender())
				return FALSE;

			// Get a new bitmap to replace the last one
#if USE_wxBITMAP
			pBitmapInfo = AllocWxBitmap(NewWidth,NewHeight,uBitmapDepth);
#else
			pBitmapInfo = GetLPBits(NewWidth, NewHeight, uBitmapDepth, &pBits);
#endif
			if (pBitmapInfo==NULL)
			{
				TRACE( _T("Growing GG failed\n"));
				return FALSE;
			}
			AllocatedNewBmp = TRUE;
			GetMasterCapture()->SetDIB(pBitmapInfo, pBits);
		}

		// set new rectangle size
		WRect = Rect;
	}
	else
	{
		WRect = Rect;
		ScreenRect = WRect;
		if (RenderView != NULL)
		{
			CCamView *RenderWindow = RenderView->GetConnectionToOilView();
			if ( RenderWindow!=NULL && RenderWindow->GetFrame()!=NULL )
			{
//				RenderWindow->ClientToScreen( &ScreenRect );
				wxPoint tl = ScreenRect.GetTopLeft();
				wxPoint br = ScreenRect.GetBottomRight();
				RenderWindow->GetFrame()->ClientToScreen(tl);
				RenderWindow->GetFrame()->ClientToScreen(br);
				ScreenRect = wxRect(tl,br);
			}
		}
		else
			ERROR2(FALSE, "No render view in GRenderRegion::StartRender()");

	}

	// Create the default, master capture...
	Capture* pNewMasterCapture = NULL;

	if (m_CaptureStack.Empty())
	{
		ERROR2IF(pBitmapInfo!=NULL, FALSE, "How has this region got a bitmap when the capture stack is empty?");

		pNewMasterCapture = new Capture(this, CAPTUREINFO(ctNESTABLE, cfMASTER | cfPIXWIDTHSCALE), RenderMatrix, CurrentClipRect, PixelsPerInch, CurrentColContext);
		if (pNewMasterCapture==NULL) return FALSE;

		m_CaptureStack.Push(pNewMasterCapture);
	}

#ifdef _DEBUG
	else
	{
		// Check capture stack sanity!
		Capture* pDebugCapture = GetTopCaptureBitmap();
		if (pDebugCapture)
		{
			ERROR3IF(pDebugCapture->CaptureRect != CurrentClipRect, "Wha?");
//			ERROR3IF(!(pDebugCapture->CaptureMatrix == RenderMatrix), "Wha?");
			ERROR3IF(pDebugCapture->dPixelsPerInch != PixelsPerInch, "Wha?");
		}
	}
#endif

	if (pBitmapInfo==NULL)
	{
		ERROR2IF(GetMasterCapture()->dPixelsPerInch!=GetPixelsPerInch(), FALSE, "Master capture has different dpi than master bitmap");

		// Get a bitmap
#if USE_wxBITMAP
		pBitmapInfo = AllocWxBitmap(WRect.width, WRect.height, uBitmapDepth);
#else
		pBitmapInfo = GetLPBits(WRect.width, WRect.height, uBitmapDepth, &pBits);
#endif
		if (pBitmapInfo==NULL)
		{
			ERROR3("Allocating GRenderRegion bitmap failed\n");
			return FALSE;
		}

		AllocatedNewBmp = TRUE;
		GetMasterCapture()->SetDIB(pBitmapInfo, pBits);
	}
#ifdef _DEBUG
	else
	{
		// Check capture stack sanity!
		ERROR3IF(GetTopCaptureBitmap()==NULL, "There must be at least ONE bitmap capture!");
	}
#endif

	// Do the middle bit
	if (!StartRenderMiddle())
		return FALSE;

	// Determine the correct hinting value for the current output mode, and call gavin to
	// set up his offscreen bitmap.
//	DWORD GavinHint = DIBUtil::GetGavinBlitFormat(ScreenDepth, uBitmapDepth, ScreenHinting);

	// If we have just created the master capture then we should initialise the bitmap
	if (AllocatedNewBmp || ForceInitBmpBits)
	{
		InitBmpBits();
	}

	// Make sure we don't Init again until something forces it 
	ForceInitBmpBits = FALSE;

	// Gerry has re-written this bit so it works properly
	// It no longer tries to select a palette when the RenderDC doesn't exist or
	// doesn't support a palette
	if (RenderDC != NULL)
	{
/* GAT	if (ScreenDepth == 8)
		{
			// use frame windows palette if desired
			if (PaletteManager::UsePalette())
				hPalette = *(PaletteManager::GetPalette());
		}
*/
		if (hPalette != NULL)
//			hPrevPalette = PaletteManager::StartPaintPalette(RenderDC->m_hDC);
			hPrevPalette = PaletteManager::StartPaintPalette(RenderDC);

		if (uBitmapDepth == 8)
		{
			// If we are not error diffusing then set the dither style
			if ((DitherStyle8Bit != 0) && (DitherStyle8Bit != 3))
				GetDrawContext()->SetDitherStyle((DitherStyle) DitherStyle8Bit);		// Stupid prototype
		}
/* GAT	if (ScreenDepth == 8)
		{
			// Whether we set a palette or not, we need this call on all 8-bit screens
			GRenderRegion::GColInit(RenderDC->m_hDC);
		}
*/	}

	// Start rendering into the current Capture...
	// (And always apply the clip region...)
	// PROBLEM:	Clipping information is lost when background rendering kicks in.
	//
	// SOLUTION:	(Re)apply the current clipping region whenever the render region
	//				starts up (again).		  vvvv
	SetRenderToCapture(GetTopCaptureBitmap(), TRUE);

	ENSURE(pBitmapInfo && pBits, "No bitmap to render into");

//	RenderMatrix.Dump();

	return TRUE;
}

/********************************************************************************************

>	void GRenderRegion::AllocWxBitmap( UINT32 Width, UINT32 Height, UINT32 Depth )

	Author:		Gavin_Theobald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/2005
	Inputs:		Width of bitmap
				Height of bitmap
				Depth of Bitmap
	Outputs:	
	Returns:	Pointer to BITMAPINFO structure. Returns NULL on failure.
	Purpose:	To create bitmap from wxBitmap structure.
	SeeAlso:	

********************************************************************************************/

#if USE_wxBITMAP

LPBITMAPINFO GRenderRegion::AllocWxBitmap( UINT32 Width, UINT32 Height, UINT32 Depth )
{
	pBitmap = new wxBitmap(Width,Height,Depth);
	pBitmapData = new wxAlphaPixelData(*pBitmap);

	uLineSize = DIBUtil::ScanlineSize(Width,Depth);
	INT32 size = uLineSize*Height;
	if ( size==0 )
		size = 4;										// in case of zero w or h

	size_t extras;
	INT32 used_cols = 0;
	switch (Depth)
	{
	case  1: extras =   2; used_cols =   2; break;
	case  4: extras =  16; used_cols =  16; break;
	case  8: extras = 256; used_cols = 256; break;
	case 16: extras =   3;                  break;
	case 24: extras =   0;                  break;
	case 32: extras =   3;                  break;
	default:
		ENSURE(FALSE, "Bad bitmap depth");
		return NULL;
	}

	LPBITMAPINFO pInfo;
	pInfo = (LPBITMAPINFO)CCMalloc(UINT32(sizeof(BITMAPINFO)+extras*sizeof(COLORREF)));
	if ( pInfo==NULL )
	{
		// Free the memory from the appropriate place
		delete pBitmapData;
		delete pBitmap;
		pBitmapData = NULL;
		pBitmap		= NULL;
		return NULL;
	}

	pBits = (BYTE*)pBitmap->GetRawData(*pBitmapData,Depth);
	if ( pBitmapData->GetRowStride()<0 )
		pBits += pBitmapData->GetRowStride()*(Height-1);
	
	pInfo->bmiHeader.biSize				= sizeof(BITMAPINFOHEADER);
	pInfo->bmiHeader.biWidth			= Width;
	pInfo->bmiHeader.biHeight			= Height;
	pInfo->bmiHeader.biPlanes			= 1;
	pInfo->bmiHeader.biBitCount			= Depth;
	pInfo->bmiHeader.biCompression 		= BI_RGB;
	pInfo->bmiHeader.biXPelsPerMeter 	= 3780;				// Default to 96 dpi
	pInfo->bmiHeader.biYPelsPerMeter 	= 3780;
	pInfo->bmiHeader.biClrUsed			= used_cols;
	pInfo->bmiHeader.biClrImportant		= 0;
	pInfo->bmiHeader.biSizeImage 		= size;

	return pInfo;
}

#endif

/********************************************************************************************

>	void GRenderRegion::FreeWxBitmap()

	Author:		Gavin_Theobald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/2005
	Inputs:		Width of bitmap
				Height of bitmap
				Depth of Bitmap
	Outputs:	
	Returns:	
	Purpose:	To free bitmap
	SeeAlso:	

********************************************************************************************/

#if USE_wxBITMAP

void GRenderRegion::FreeWxBitmap()
{
	if ( pBits )
	{
		pBitmap->UngetRawData(*pBitmapData);
		delete pBitmap;
		delete pBitmapData;
		CCFree(pBitmapInfo);
		pBitmap		= NULL;
		pBits		= NULL;
		pBitmapData	= NULL;
		pBitmapInfo	= NULL;
	}
}

#endif

/********************************************************************************************

>	virtual WinRect GRenderRegion::CalculateWinRect( const Matrix& RenderMatrix, const DocRect& docrect,
													 const double dpi)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/96
	Inputs:		DocRect is a rectangle on document co-ords.
				dpi is the resolution of the device we are rendering to
	Outputs:	RenderMatrix is the rendering matrix, may have e anf f components changed by the call
	Returns:	Object containing the new rectangle coordinates.     			
	Purpose:	To convert a rectangle in Doc coordinates to a rectangle in Win coordinates
				taking account of the destination dpi rather than assuming screen dpi.
				Virtual so that it can be overriden by different render regions if so required.
	SeeAlso:	OSRenderRegion::DocRectToWin;

********************************************************************************************/

WinRect GRenderRegion::CalculateWinRect( Matrix& RenderMatrix, const DocRect& docrect,
										 const double dpi)
{
	// lets get the Rect into screen co-ords so we can use our big screen bitmap
	// Use a version of OSRenderRegion::DocRectToWin that uses the actual dpi we have stored
	// in PixelsPerInch
	// Ilan
	// I need this to use the supplied rectangle - docrect.
	// Looks like previous version was a typo so I am changing it here.
	// Karim 07/07/2000
	// I'm now changing this to actually _use_ the dpi param in its calculations.
	// I've checked, and I'm 99.99% certain this doesn't screw up any existing code.
	return OSRenderRegion::DocRectToWin( RenderMatrix, docrect, dpi );
//	return OSRenderRegion::DocRectToWin( RenderMatrix, docrect, PixelsPerInch );
//	return OSRenderRegion::DocRectToWin( RenderMatrix, CurrentClipRect, PixelsPerInch );
}

WinRect GRenderRegion::CalculateWinRect( const DocRect& docrect )
{
	return CalculateWinRect(RenderMatrix, docrect, PixelsPerInch );
}

/********************************************************************************************

>	BOOL GRenderRegion::StartRenderAfter(GMATRIX *GMat)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/94
	Inputs:		Pointer to a Gavin matrix which can be modified.
	Outputs:	-
	Returns:	TRUE if worked, FALSE is failed.
	Purpose:	A virtual fn in this class which can be overridden if more work on a GMatrix
				is required. Default version returns TRUE.

********************************************************************************************/

BOOL GRenderRegion::StartRenderAfter( GMATRIX *GMat )
{
	return TRUE;
}

/********************************************************************************************

>	BOOL GRenderRegion::StartRenderMiddle()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if worked, FALSE is failed.
	Purpose:	A virtual fn in this class which can be overridden if more work on a lpBits
				is required. Default version returns TRUE.

********************************************************************************************/

BOOL GRenderRegion::StartRenderMiddle()
{
	return TRUE;
}

/********************************************************************************************

>	BOOL GRenderRegion::StopRender()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/93
	Inputs:		RenderState is a Node* pointing at the current rendering node in the tree
	Outputs:	-
	Returns:	TRUE if something has been rendered since StartRender was called.
	Purpose:	Stops the rendering of a GRenderRegion, saving it's current renderstate so
				that rendering can continue where it left off, later on.
				Copies the gdraw internal bitmap to its final resting place (e.g. the screen).
				Also tidies up windows' palette.

	Errors:		-
	SeeAlso:	OSRenderRegion::StopRender

********************************************************************************************/

BOOL GRenderRegion::StopRender()
{
	RememberMe("StopRender");

	// Remember this now before it gets cleared by SetClean
	// or set by the debug code below
	BOOL bHaveRendered = HaveRenderedSomething;

#if 0
	if (HaveRenderedSomething)
	{
		TRACE( _T("GRR#  DebugClipRect = (%d, %d) - (%d, %d)\n"), CurrentClipRect.lo.x, CurrentClipRect.lo.y, CurrentClipRect.hi.x, CurrentClipRect.hi.y);

		DocRect TempRect(CurrentClipRect);
		TempRect.Inflate(-GetScaledPixelWidth());
		TRACE( _T("GRR#  Filling = (%d, %d) - (%d, %d)\n"), TempRect.lo.x, TempRect.lo.y, TempRect.hi.x, TempRect.hi.y);
		Path ClipPath;
		ClipPath.Initialise();
		ClipPath.CreatePathFromDocRect(&TempRect);

		SaveContext();
		SetLineColour(COLOUR_TRANS);
		SetFillColour(COLOUR_BLUE);
		DrawPath(&ClipPath);
		RestoreContext();
	}
#endif
#if 0
	{
		SaveContext();

		SetLineColour(COLOUR_TRANS);
		SetFillColour(COLOUR_BLUE);

		DocRect SmallRect(750, 750, 1500, 1500);
		Path ThePath;
		ThePath.Initialise();
		ThePath.CreatePathFromDocRect(&SmallRect);

		DrawPath(&ThePath);
		ThePath.Translate(50, 1500);
		DrawPath(&ThePath);
		ThePath.Translate(50, 1500);
		DrawPath(&ThePath);
		ThePath.Translate(50, 1500);
		DrawPath(&ThePath);
		ThePath.Translate(50, 1500);
		DrawPath(&ThePath);
		ThePath.Translate(50, 1500);
		DrawPath(&ThePath);
		ThePath.Translate(50, 1500);
		DrawPath(&ThePath);
		ThePath.Translate(50, 1500);
		DrawPath(&ThePath);
		ThePath.Translate(50, 1500);
		DrawPath(&ThePath);
		ThePath.Translate(50, 1500);
		DrawPath(&ThePath);
		ThePath.Translate(50, 1500);
		DrawPath(&ThePath);
		ThePath.Translate(50, 1500);
		DrawPath(&ThePath);
		ThePath.Translate(50, 1500);
		DrawPath(&ThePath);
		ThePath.Translate(50, 1500);
		DrawPath(&ThePath);
		ThePath.Translate(50, 1500);
		DrawPath(&ThePath);
		ThePath.Translate(50, 1500);
		DrawPath(&ThePath);

		RestoreContext();
	}
#endif
	// Check that Initialise was called
//	ENSURE(RenderFlags.Rendering, "DeInitialise called before Initialise");
	TRACEUSER("Gavin",_T("GRenderRegion::StopRender - RenderFlags.Rendering = FALSE;\n"));
	RenderFlags.Rendering = FALSE;

	// Don't Blit if we are partway through rendering offscreen. The offscreen bitmap must only
	// be blitted once it is completed. Offscreen system takes care of resetting the rect which
	// GDraw has rendered to (ie ChangedBBox)
	if (pBitmapInfo)
	{
		if (MasterCaptureIsCurrent() && !TopCaptureIsDirect())
		{
			// if we rendered something then blit, else we don't. This is handy for not
			// double-blitting paper areas. Could take it even further to only blit those areas
			// of the bitmap which have been written to.

			if (HaveRenderedSomething)
			{
				// Show the current rendering state on screen
				// No, just needs a simple blit to screen...
				// This is the commonest, fast case
				DisplayBits();

				// We have updated the screen, so now we're 'clean'.
				SetClean(TRUE, FALSE);
			}
		}
		else
		{
			// We were rendering into Capture bitmaps when this Stop request
			// occurred so, to show current progress, we must blit the contents
			// of the capture stack into a copy of the master bitmap and
			// put that on the screen...
			//
			// Note that we CANNOT alter the master bitmap itself because when
			// the captures terminate properly they will again be blitted into
			// the master bitmap and so the master bitmap must not contain any
			// earlier version of the captures otherwise transparency values
			// would be multiplied.
			//
			if (HaveRenderedSomething)
			{
				// Make a temporary bitmap from the current state of things and display that
				DisplayCurrentState();

				// We have updated the screen, so now we're 'clean'.
				SetClean(TRUE, FALSE);
			}
		}
	}
	else
	{
		if(!IsWrapped)
			TRACE( _T("GRenderRegion::StopRender called with no bitmap\n"));
	}

	if (hPrevPalette)
		PaletteManager::StopPaintPalette(RenderDC, hPrevPalette);

	// Clean out the Fuzzy Rectangles
	InnerRect.MakeEmpty();
	OuterRect.MakeEmpty();

	// Karim 04/05/2000
	// free all of our cached clipping regions.
	FreeStoredClipRegions();

	// Reset dither style to ordered dither on 8bpp screens
	if (ScreenDepth == 8)
		GetDrawContext()->SetDitherStyle(DITHER_GREY_ORDERED);

	return bHaveRendered;
}




/********************************************************************************************

>	virtual void GRenderRegion::DisplayCurrentState()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/07/2005
	Purpose:	Make a temporary bitmap, copy current GRenderRegion bitmap into it
				then render other offscreen state into it before blitting it to screen
	Notes:		This routine does a lot of bitmap copying and blitting so it is much
				slower than just blitting the current bitmap to screen!

********************************************************************************************/

void GRenderRegion::DisplayCurrentState()
{
	// Create a temporary capture that is a copy of the master capture
	// (and get a copy of the bitmap)
	Capture* pTempCapture = new Capture(GetMasterCapture(), TRUE);

	if (pTempCapture)
	{
		// Point rendering at the temp capture bitmap
		SetRenderToCapture(pTempCapture);

		// Render the current capture state into the temp bitmap
		RenderCurrentCaptureState();

#ifdef DEBUG_ATTACH_RENDERSTATE_BMP
{
// Create a Kernel bitmap from the bmp data.
WinBitmap* wBitmap			= new WinBitmap(pTempCapture->lpBitmapInfo, pTempCapture->lpBits);
KernelBitmap* OffscreenBitmap	= new KernelBitmap(wBitmap,TRUE);
OffscreenBitmap->AttachDebugCopyToCurrentDocument("Render State Bitmap");
wBitmap->BMBytes = ((WinBitmap*)OILBitmap::Default)->BMBytes;
delete OffscreenBitmap;
}
#endif

		// Show the temp bitmap to the user
		DisplayBits(pTempCapture->lpBitmapInfo, pTempCapture->lpBits);

		// Reset rendering back to normal
		SetRenderToCapture(GetTopCaptureBitmap());

		// Release the temp bitmap and delete the temp capture
		pTempCapture->FreeDIB();
		delete pTempCapture;
		pTempCapture = NULL;
	}
	else
	{
		// Failed to craete temp bitmap
		// So just blit what we can...
		DisplayBits();
	}
}
				
				
				
/********************************************************************************************

>	virtual void GRenderRegion::SetClean(BOOL bResetChangedBounds, BOOL FillWhite)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/06/94
	Purpose:	Informs the GRenderRegion that it should consider it's bitmap image to
				be clean, i.e. it's an accurate representation of what is on screen
				already, so there is no need to blit it.
				This is used in conjunction with the PaperRenderRegion to avoid
				unnecessary blits.
	SeeAlso:	RenderRegion::SetClean; PaperRenderRegion

********************************************************************************************/

void GRenderRegion::SetClean(BOOL bResetChangedBounds, BOOL FillWhite)
{
	// Reset our rendering flag...
	HaveRenderedSomething = FALSE;
	if (bResetChangedBounds)
		GetDrawContext()->ClearChangedBBox();
}



/********************************************************************************************

>	void GRenderRegion::SetImmediateRender(BOOL SetVal)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/99
	Inputs:		Value to set
	Outputs:	-
	Returns:	-
	Purpose:	sets the immediate render flag to the value specified
********************************************************************************************/
// Moved to header file so that it is inlined automatically
//void GRenderRegion::SetImmediateRender(BOOL SetVal)
//{
//	RenderFlags.bImmediateRender = SetVal;
//}



/********************************************************************************************

>	BOOL GRenderRegion::StrokePath( const DocCoord *Coords, const PathVerb *Verbs, 
									UINT32 Count, BOOL DrawArrows = FALSE )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/94
	Inputs:		Coords: list of points to draw
				Verbs: list of path verbs
				Count: number of point
				DrawArrows: whether or not it should attempt to draw arrows on open
							subpaths.
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Sits on top of GDraw_StrokePath that calculates Joins, Caps & Width then
				renders via GDraw.
				Does dashed lines too now.
				Also renders ArrowHeads in needed.
	Errors:		-
	Scope:		Private

********************************************************************************************/

BOOL GRenderRegion::StrokePath( const DocCoord *Coords, const PathVerb *Verbs, UINT32 Count,
								const DocCoord *UnclippedCoords, const PathVerb *UnclippedVerbs, 
								UINT32 UnclippedCount,
								BOOL DrawArrows )
{
	CapStyles Caps;
	JoinStyles Join;

	switch (RR_STARTCAP())
	{
		case LineCapRound:
			Caps = CAPS_ROUND;
			break;
		case LineCapSquare:
			Caps = CAPS_SQUARE;
			break;
		case LineCapButt:
			Caps = CAPS_BUTT;
			break;
		default:
			ENSURE(FALSE, "Bad startcap");
			Caps = CAPS_ROUND;
			break;
	}
	switch (RR_JOINTYPE())
	{
		case BevelledJoin:
			Join = JOIN_BEVEL;
			break;
		case MitreJoin:
			Join = JOIN_MITER;
			break;
		case RoundJoin:
			Join = JOIN_ROUND;
			break;
		default:
			ENSURE(FALSE, "bad jointype");
			Join = JOIN_ROUND;
			break;
	}

	BOOL StrokeOK = TRUE;

	// DMc updates so that arrow heads are drawn on the line not after it
	// 25/2/99
	// make a copy of the coord array
	DocCoord * pCopyCoords = NULL;
	if (UnclippedCount != 0)
	{
		pCopyCoords = (DocCoord *)(CCMalloc(sizeof(DocCoord) * UnclippedCount));

		for (UINT32 i = 0; i < UnclippedCount; i++)
		{
			pCopyCoords[i].x = UnclippedCoords[i].x;
			pCopyCoords[i].y = UnclippedCoords[i].y;
		}
	}

	BOOL bRenderedArrows = FALSE;

	// DMc update for arrow heads being drawn on the line not after it
	// 25/2/99
	// Pass the Unclipped coords to the arrow rendering routine.
	if (DrawArrows && pCopyCoords)
	{
		bRenderedArrows = DrawPathArrowHeads((DocCoord*)pCopyCoords, (PathVerb*)UnclippedVerbs, UnclippedCount);
	}

	// We won't stroke anything, if the path has been
	// fuzzy clipped so much, that there are no points left
	if (Count > 1 && pCopyCoords)
	{
		MILLIPOINT Width = RR_LINEWIDTH();

		if (RenderFlags.HitDetect)
		{
			// rendering to off-screen bitmaps means minimum 3-pixel wide lines
			if (Width < (ScaledPixelWidth*1))
				Width = ScaledPixelWidth*1;
		}
		else if (RRQuality.GetQuality() == Quality::QualityGuideLayer)
		{
			SetDashPattern(SD_DASH1);
			DocColour* pCol = AttrQuality::GetColour();
			if (pCol != NULL)
				SetGCol(*pCol);				
			else
			{
				DocColour	colorRed(COLOUR_RED);
				SetGCol( colorRed );
			}
			
			Width = ScaledPixelWidth;
			DrawArrows = FALSE;
		}
		else if (RRQuality.GetLineQuality() < Quality::FullLine)
		{
			// do 0-width lines if line quality is low
			Width = 0;
			DrawArrows = FALSE;
		}
		else if (RRQuality.GetAntialiasQuality() < Quality::FullAntialias)
		{
			// if not anti-aliasing then check for lines thinner than one pixel as Gavin doesn't
			// render them very usefully
			if (Width < ScaledPixelWidth)
				Width = 0;
		}

		DashType* pDashRec = NULL;
		DashType GavinDash;

		if ( Width > 0 && 
			(RRQuality.GetLineQuality() >= Quality::FullLine) && 
			 RR_DASHPATTERN().Elements > 0)
		{
			INT32 Length = RR_DASHPATTERN().Elements;

			// Gavin Dashes are Max of 8 elements
			if (Length > 8) Length = 8;


			// If the flag is set, then we need to scale the dash elements
			// according to the linewidth
			BOOL DoScale = RR_DASHPATTERN().ScaleWithLineWidth;

			FIXED16 Scale = DoScale ? (double(Width) / double(RR_DASHPATTERN().LineWidth)) : 1;

			GavinDash.Length = Length;
			GavinDash.Offset = LongMulFixed16(RR_DASHPATTERN().DashStart, Scale);

			for (INT32 el = 0; el < Length; el++)
			{
				// Copy each element into the Gavin Dash, scaling as we go
				GavinDash.Array[el] = LongMulFixed16(RR_DASHPATTERN().ElementData[el], Scale);
			}

			pDashRec = &GavinDash;
		}

		// DMc update 25/2/99
		// previously :
		// StrokeOK = GetDrawContext()->StrokePath( (POINT*)Coords, Verbs, Count, FALSE,
		// 						Width, Caps, Join, pDashRec );

		if (bRenderedArrows)
		{
			StrokeOK = GetDrawContext()->StrokePath( (POINT*)pCopyCoords, UnclippedVerbs, UnclippedCount, FALSE,
									Width, Caps, Join, pDashRec );
		}
		else
		{
			StrokeOK = GetDrawContext()->StrokePath( (POINT*)Coords, Verbs, Count, FALSE,
									Width, Caps, Join, pDashRec );
		}
	}

	if (pCopyCoords)
	{
		CCFree(pCopyCoords);
		pCopyCoords = NULL;
	}

	if (bRenderedArrows)
		return TRUE;

	return StrokeOK;
}




/********************************************************************************************

>	void GRenderRegion::DrawPathToOutputDevice(Path* PathToDraw)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/93
	Inputs:		PathToDraw is a pointer to a Path object to render
	Outputs:	-
	Returns:	-
	Purpose:	Renders a path object using gavins routines
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void GRenderRegion::DrawPathToOutputDevice(Path* PathToDraw, PathShape)
{
	// If we are not drawing complex shapes and this shape is, then return
//	if ((!RenderComplexShapes) && (TestForComplexShape(&Caps)))
//		return;

//	TRACEUSER( "Gerry", _T("GRenderRegion::DrawPathToOutputDevice\n"));

	DWORD Style = 0;
	TranspGradTable TranspTable;

	// Something to render later
	DocCoord* 	Coords;
	PathVerb* 	Verbs;
	UINT32 		NumCoords;
	DocCoord* 	UnclippedCoords;
	PathVerb* 	UnclippedVerbs;
	UINT32 		UnclippedNumCoords;

	BOOL ShouldDeleteCoords = FALSE;

	// Keep the unclipped path coords for arrow rendering
	UnclippedCoords 	= PathToDraw->GetCoordArray();
	UnclippedVerbs  	= PathToDraw->GetVerbArray();
	UnclippedNumCoords 	= PathToDraw->GetNumCoords();

	ClippedPathIsValid = TRUE;

	// setup the fuzzy clipping rects.
	SetupFuzzyClipRects();

	// try and clip the path before rendering it, for bigger zoom values
	if (TryToFuzzyClip(PathToDraw, &Coords, &Verbs, &NumCoords))
	{
		// This may have clipped out all the points (or left just a moveto),
		// and in this case, it may seem pointless to continue.
		// but we will carry on, so that ArrowHeads get a chance to render
		// using the unclipped path coords.  
		// This shouldn't be a problem, so long as anything that uses the clipped
		// coords, checks to make sure there are enough (more than 1).

		if (NumCoords < 2)
		{
			ClippedPathIsValid = FALSE;
//			OutputDebugString("Clipped path is too small\n");
		}
		else
		{
//			char Str[256];
//			_stprintf(Str, "Path clipped. Count=%d, Coords@%x, Verbs@%x\n",NumCoords,Coords,Verbs);
//			OutputDebugString(Str);
/*
			for (INT32 i=0; i<(NumCoords-1); i++)
			{
				if (Verbs[i] == PT_BEZIERTO)
				{
					if (Coords[i] == Coords[i+3])
					{
						ClippedPathIsValid = FALSE;
	//					OutputDebugString("Clipped path zero length element, Ignoring it.\n");
						NumCoords = 0;
						break;				
					}	
					
					i += 2;
				}
				else
				{
					if (Coords[i] == Coords[i+1])
					{
						ClippedPathIsValid = FALSE;
	//					OutputDebugString("Clipped path zero length element, Ignoring it.\n");
						NumCoords = 0;
						break;				
					}	
				}
			}
*/
		}

		// Yes, we clipped the paths alright
		ShouldDeleteCoords = TRUE;
	}
	else
	{
		// No Clipping here matey
		Coords = PathToDraw->GetCoordArray();
		Verbs  = PathToDraw->GetVerbArray();
		NumCoords = PathToDraw->GetNumCoords();
	}

	enum Quality::Fill FillQuality = RRQuality.GetFillQuality();

	RememberMe("DrawPath");
	HaveRenderedSomething = TRUE;

	if ( PathToDraw->IsFilled )
	{
		BOOL ExtendedFill = FALSE;
		FillGeometryAttribute *pFillProvider 
			= (FillGeometryAttribute *) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;

		if ( 
			(pFillProvider->GetRuntimeClass() != CC_RUNTIME_CLASS(FlatFillAttribute)) &&
			(FillQuality >= Quality::Graduated) 
		   )
			ExtendedFill = TRUE;

		if ( 
			(pFillProvider->GetRuntimeClass() == CC_RUNTIME_CLASS(BitmapFillAttribute)) &&
			(FillQuality >= Quality::Solid) 
		   )
			ExtendedFill = TRUE;

		if ( 
			(pFillProvider->GetRuntimeClass() == CC_RUNTIME_CLASS(FractalFillAttribute)) &&
			(FillQuality >= Quality::Solid) 
		   )
			ExtendedFill = TRUE;

		if (ExtendedFill)
		{
			// VeryMono forces non-extended fill
			if (RenderFlags.VeryMono)
			{
				// Up until now, we always rendered everything solid black when
				// in 'VeryMono' mode, but now we have 'masked' bitmaps, we
				// need to render them (will all non-masked pixels plotted black)
				if (pFillProvider->GetRuntimeClass() == CC_RUNTIME_CLASS(BitmapFillAttribute))
				{
					// Check for a masked bitmap ....
					KernelBitmap* pBitmap = pFillProvider->GetBitmap();
					INT32 Index = 0;

					if (pBitmap && (pBitmap->GetTransparencyIndex(&Index) || pBitmap->GetBPP()>=32))
					{
						// It's a masked bitmap, so we'll need to plot all
						// the non-masked pixels as black
						if (!pFillProvider->RenderFill(this, PathToDraw))
							ExtendedFill = FALSE;							// fail safe
					}
					else
					{
						ExtendedFill = FALSE;	// Not a masked bitmap
					}
				}
				else
				{
					ExtendedFill = FALSE;		// Not a bitmap at all
				}
			}
			else if (!pFillProvider->RenderFill(this, PathToDraw))
			{
				ExtendedFill = FALSE;										// fail safe
			}
		}

	   	if ( (!ExtendedFill) && 
			 (FillQuality >= Quality::Solid)
		   )
		{
			DocColour FlatColour = RR_FILLCOLOUR();

			if (RenderFlags.VeryMono && 
				pFillProvider->GetRuntimeClass() == CC_RUNTIME_CLASS(BitmapFillAttribute))
			{
				// Bitmap fills will return 'No Colour' as their flat colour,
				// but for 'VeryMono' rendering they should be treated as black.
				FlatColour = COLOUR_BLACK;
			}

			if (SetFillGCol(FlatColour))
			{
				if (!RenderFlags.VeryMono && GetTransparencyFill(&TranspTable, &Style))
				{
					DocColour Col = RR_FILLCOLOUR();
					COLORREF rgb = ConvertColourToTransScreenWord(CurrentColContext, &Col);
					GetDrawContext()->SetTransparency(rgb, (TransparencyEnum)(Style>>8));
				}

				if (NumCoords > 1)
				{
					GetDrawContext()->FillPath( (POINT*)Coords, Verbs, NumCoords,
									(RR_WINDINGRULE()==EvenOddWinding) ? 0 : 1);
				}
			}
		}
	}

	if ( PathToDraw->IsStroked )
	{
		// Should we try and draw some ArrowHeads ?
		BOOL DrawArrows = (RRQuality.GetLineQuality() >= Quality::FullLine) &&
						  !(RR_STARTARROW().IsNullArrow() && RR_ENDARROW().IsNullArrow()) &&
						  ArrowRec::DoesPathNeedArrowHeads(UnclippedCoords, UnclippedVerbs, UnclippedNumCoords);

		if ((FillQuality <= Quality::Bitmaps))
		{
			// Just do everything in black outlines...
			DocColour	colorBlk(COLOUR_BLACK);
			if ( SetGCol( colorBlk ) )
				StrokePath( Coords, Verbs, NumCoords, 
							UnclippedCoords, UnclippedVerbs, UnclippedNumCoords, DrawArrows );
		}
		else if (RenderFlags.VeryMono)
		{
			// Just do everything in black outlines...
			DocColour	colorBlk(COLOUR_BLACK);
			if ( !(RR_STROKECOLOUR().IsTransparent()) && SetGCol( colorBlk ) )
				StrokePath( Coords, Verbs, NumCoords, 
							UnclippedCoords, UnclippedVerbs, UnclippedNumCoords, DrawArrows );
		}
		else if ( SetGCol( RR_STROKECOLOUR() ) )
		{
				StrokePath( Coords, Verbs, NumCoords, 
							UnclippedCoords, UnclippedVerbs, UnclippedNumCoords, DrawArrows );
		}
	}

	// We drew a clipped path, so get rid of the extra clipped coords
	if (ShouldDeleteCoords)
	{
		delete Coords;
		delete Verbs;
	}
}



/********************************************************************************************

>	virtual void GRenderRegion::SetClipRegion(ClipRegionAttribute* pClipAttr, BOOL Temp)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26 April 2000

	Inputs:		pClipAttr		the ClipRegionAttribute to set & push on the attr-stack.
				Temp			whether the attr is temporary or not - ie if you created
								it on the heap, do you want it automatically deleted when
								it goes out of scope.

	Outputs:	The attribute stack gets this attr pushed onto it, and the current
				clip region is set accordingly.

	Purpose:	Set the current ClipRegion for this RenderRegion.
				pClipAttr contains a path, and all future rendering will occur *through*
				that clipping path, after it is clipped to any pre-existing clipping region.

	See Also:	RestoreClipRegion(), ClipRegionAttribute.

********************************************************************************************/
void GRenderRegion::SetClipRegion(ClipRegionAttribute* pClipAttr, BOOL Temp)
{
	RenderRegion::SetClipRegion(pClipAttr, Temp);
	ApplyCurrentClipRegion();
}



/********************************************************************************************

>	virtual void GRenderRegion::RestoreClipRegion(ClipRegionAttribute* pClipAttr, BOOL Temp)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26 April 2000
	Inputs:		pClipAttr		the ClipRegionAttribute to restore over the current
								attribute.
				Temp			whether the attr is temporary or not - ie if you created
								it on the heap, do you want it automatically deleted when
								it goes out of scope.

	Outputs:	The attribute stack gets its top ClipRegionAttribute deleted & replaced
				with this one, with the RR's future clipping-region set accordingly.

	Purpose:	Set the current ClipRegion for this RenderRegion.
				pClipAttr contains a path, and all future rendering will occur *through*
				that clipping path, after it is clipped to any pre-existing clipping region.

	See Also:	SetClipRegion(), ClipRegionAttribute.

********************************************************************************************/
void GRenderRegion::RestoreClipRegion(ClipRegionAttribute* pClipAttr, BOOL Temp)
{
	// Karim 26/02/2001
	// Once a clip-region has been restored, we no longer need to cache its associated
	// clipping region, so we'll free it up here.
	// We only freeing up the old clip-region if the incoming attr is different
	// from the outgoing attr, and if we can successfully remove old clip-region
	// from our cache.

//	REGION*					pClipRegion			= NULL;
	ClipRegionAttribute*	pOutGoingClipAttr	= RR_CLIPREGION();
	ClipRegionAttribute*	pInComingClipAttr	= NULL;

	// Actually restore the incoming over the outgoing attribute in our attr-context.
	RenderRegion::RestoreClipRegion(pClipAttr, Temp);

	// Free up outgoing clip-region, now it's unneeded.
	pInComingClipAttr = RR_CLIPREGION();
	if (pInComingClipAttr != pOutGoingClipAttr &&
		pOutGoingClipAttr != NULL)
	{
//		if (m_ClipRegionMap.Lookup((void*)pOutGoingClipAttr, ((void*&)pClipRegion)))
//		{
//			if (m_ClipRegionMap.RemoveKey((void*)pOutGoingClipAttr))
//			{
//				if (pClipRegion != NULL)
//					delete [] ((BYTE *)pClipRegion);
//			}
//		}
		// Normally GetTopCaptureBitmap must return a valid pointer
		// However RestoreClipRegion can be called when RenderRegions are being shut down
		Capture* pBitCapture = GetTopCaptureBitmap();
		if (pBitCapture)
			pBitCapture->RemoveCachedClipRegion(pOutGoingClipAttr);
	}

	// Finally, adopt the new clip-attr as our clipping region.
	ApplyCurrentClipRegion();
}



/********************************************************************************************

>	BOOL GRenderRegion::ApplyCurrentClipRegion() const

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14 April 2000
	Inputs:		
	Outputs:	GDraw should have its clipping region and rectangle set so that rendering
				only occurs within a region specified by the clipping path which the current
				ClipRegionAttribute is carrying.
	Returns:	TRUE if successful,
				FALSE otherwise.
	Purpose:	Have a look at the current ClipRegionAttribute and make sure
				we set our clipping region accordingly. Details within the method.
	Errors:		ERROR3 in DEBUG if we have no valid ptr to a ClipRegionAttribute.
	See also:	

********************************************************************************************/
BOOL GRenderRegion::ApplyCurrentClipRegion()
{
//	return TRUE;

	// note that we don't bother with any clipping unless we're actually doing something
	// useful, eg rendering.
	if (!RenderFlags.Rendering)
		return FALSE;

	// This is what we do...
	//
	//	1.	We keep a hash table containing clip regions, with an entry for each unique
	//		ClipRegionAttribute, so that we do the time-consuming (I assume!) make-region
	//		work as little as possible.
	//	2.	If we haven't seen a ClipRegionAttribute before, then we make a region from/for
	//		it and stick the association in the table for future look-up
	//	3.	We apply the clip region appropriate to the current attribute.
	//	4.	We do all the above for device clip rectangles too, which need to be re-applied
	//		whenever a new clip region is applied.

	// remember the current clip region, or make a new one for the current clipattr.
	BOOL			fSuccess	= FALSE;
	const REGION*	pClipRegion	= NULL;
	ClipRegionAttribute* pClipAttr = RR_CLIPREGION();

	ERROR3IF(pClipAttr == NULL, "Doh! NULL ClipRegionAttribute in render region!");

	// if we don't have a clip-region, do a DeviceClipRectangle() with an 'infinite' rect.
	// this will reset GDraw's clip-rectangle to the size of its bitmap, and also
	// reset its clip-region to empty.
	if (pClipAttr->GetClipPath() == NULL)
	{
		RECT BigRect;
		BigRect.bottom	= BigRect.left	= INT32_MIN;
		BigRect.top		= BigRect.right	= INT32_MAX;
		fSuccess = GetDrawContext()->DeviceClipRectangle(&BigRect);
	}

	// try to look up a clip-region for this clip-attr; failing that, create a new one
	// and store it.
	else
	{
//		if (!m_ClipRegionMap.Lookup((void*)pClipAttr, ((void*&)pClipRegion)))
//		{
//			pClipRegion	= MakeClipRegionFromClipAttr(pClipAttr);
//			m_ClipRegionMap.SetAt((void*)pClipAttr, ((void*&)pClipRegion));
//		}
		pClipRegion = GetTopCaptureBitmap()->GetCachedClipRegion(pClipAttr);
		if (pClipRegion==NULL)
		{
			pClipRegion	= MakeClipRegionFromClipAttr(pClipAttr);
			GetTopCaptureBitmap()->SetCachedClipRegion(pClipAttr, pClipRegion);
		}

		fSuccess = GetDrawContext()->ClipRegion(pClipRegion);
	}

	return fSuccess;
}



/********************************************************************************************

>	BOOL GRenderRegion::RenderGradFillPath(Path *PathToDraw, GradFillAttribute *Fill )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/94
	Inputs:		PathToDraw is a pointer to a Path object to render. Fill is the grad fill
				required.
	Outputs:	-
	Returns:	TRUE if understood fill type & rendered it, FALSE if not
	Purpose:	Fills a grad filled path (linear or radial)
	Errors:		-

	Notes:		Added support for square, 3 and 4 colour fills (Gerry 12/8/96)

********************************************************************************************/

BOOL GRenderRegion::RenderGradFillPath(Path *PathToDraw, GradFillAttribute *Fill)
{
	DWORD Style = 0;
	TranspGradTable TranspTable;

	BOOL SpecialFill = FALSE;

	CCRuntimeClass *FillType = Fill->GetRuntimeClass();


	if (FillType == CC_RUNTIME_CLASS(RadialFillAttribute))
		Style = 1;
	else if (FillType == CC_RUNTIME_CLASS(LinearFillAttribute))
		Style = 0;
	else if (FillType == CC_RUNTIME_CLASS(ConicalFillAttribute))
		Style = 2;
	else if (FillType == CC_RUNTIME_CLASS(SquareFillAttribute))
		Style = 3;
	else if (FillType == CC_RUNTIME_CLASS(ThreeColFillAttribute))
		SpecialFill = TRUE;
	else if (FillType == CC_RUNTIME_CLASS(FourColFillAttribute))
		SpecialFill = TRUE;
	else
	{
		ERROR3("Unknown fill type");
		return FALSE;								// unknown fill type
	}

	if (uBitmapDepth == 32)
	{
		// InitTransparencyFill will setup the transparency fill if there is one
		if (!GetTransparencyFill(&TranspTable, &Style))
			Style |= (WhichTransparency<<8);		// Set the fill type of the transparency
	}

	CProfileBiasGain DiagramMapper = Fill->GetProfile ();

	// --- Convert ArtWorks style attribute into Gavin-style
	POINT A,B,C,D;

	DocCoord* StartPoint 	= Fill->GetStartPoint();
	DocCoord* EndPoint  	= Fill->GetEndPoint();
	DocCoord* EndPoint2 	= Fill->GetEndPoint2();
	DocCoord* EndPoint3 	= Fill->GetEndPoint3();

	A.x = (*StartPoint).x;	A.y = (*StartPoint).y;
	B.x = (*EndPoint).x;	B.y = (*EndPoint).y;

	if (EndPoint2 != NULL)
    {
    	C.x = (*EndPoint2).x;	
    	C.y = (*EndPoint2).y;
	}

	if (FillType==CC_RUNTIME_CLASS(LinearFillAttribute))
	{
		A.x = (*StartPoint).x;	A.y = (*StartPoint).y;
    	B.x = (*EndPoint2).x;   B.y = (*EndPoint2).y;
		C.x = (*EndPoint).x;	C.y = (*EndPoint).y;
	}
	else if (FillType==CC_RUNTIME_CLASS(ConicalFillAttribute))
	{
		B.x = A.x - (B.x - A.x);
		B.y = A.y - (B.y - A.y);
	}

	BOOL Perspective = FALSE;

	// Is it a perspectivised fill ?
	if (Fill->IsPerspective())
	{
		Perspective = TRUE;

		POINT PGram[4];

		// Setup the Perspective control points
		if (FillType==CC_RUNTIME_CLASS(LinearFillAttribute))
		{
			PGram[0].x = (*StartPoint).x;	PGram[0].y = (*StartPoint).y;
			PGram[1].x = (*EndPoint2).x;	PGram[1].y = (*EndPoint2).y;
			PGram[2].x = (*EndPoint3).x;	PGram[2].y = (*EndPoint3).y;
			PGram[3].x = (*EndPoint).x;		PGram[3].y = (*EndPoint).y;
		}
		else
		{
			PGram[0].x = A.x;				PGram[0].y = A.y;
			PGram[1].x = B.x;				PGram[1].y = B.y;
			PGram[2].x = (*EndPoint3).x;	PGram[2].y = (*EndPoint3).y;
			PGram[3].x = (*EndPoint2).x;	PGram[3].y = (*EndPoint2).y;
		}

		// Are the control points sensible ?

		// Gerry removed the pixel size test cos it makes small repeating fills go spappy
		
		if (MouldPerspective::WillBeValid(PGram))
		{
			// We're going to plot a perspective fill,
			// so copy the perspective control points
			A.x = PGram[0].x;	A.y = PGram[0].y;
			B.x = PGram[1].x;	B.y = PGram[1].y;
			C.x = PGram[2].x;	C.y = PGram[2].y;
			D.x = PGram[3].x;	D.y = PGram[3].y;
		}
		else
		{
			// The mould is bad...
			Perspective = FALSE;
		}
	}

	// This needs to be allocated at the same level as the FillPath call below...
	GradTable Table(FALSE);

	INT32 Tiling = ((FillMappingAttribute*)CurrentAttrs[ATTR_FILLMAPPING].pAttr)->Repeat;

	if (SpecialFill)
	{
		// The fill is a fabby new one

		if ((FillType != CC_RUNTIME_CLASS(ThreeColFillAttribute)) &&
			(FillType != CC_RUNTIME_CLASS(FourColFillAttribute)))
		{
			ERROR3("Unknown new fill type");
			return FALSE;								// unknown fill type
		}

		// Clear the bottom byte of the style cos it means something new
		Style = Style & 0xFFFFFF00;

		// If the transparency type is one of the unimplemented ones then set to zero
		if ((Style & 0xFF00) < 0x400)
			Style = (Style & 0xFF0000);
		
		// We wont do anything scary with the colours (separation/correction)
		// as this only renders to a GDraw bitmap

		PColourRGBT Result;

		CurrentColContext->ConvertColour(Fill->GetStartColour(), (ColourPacked *) &Result);
		COLORREF StartColour = RGB(Result.Red, Result.Green, Result.Blue);

		CurrentColContext->ConvertColour(Fill->GetEndColour(), (ColourPacked *) &Result);
		COLORREF EndColour = RGB(Result.Red, Result.Green, Result.Blue);

		CurrentColContext->ConvertColour(Fill->GetEndColour2(), (ColourPacked *) &Result);
		COLORREF EndColour2 = RGB(Result.Red, Result.Green, Result.Blue);

		if (FillType == CC_RUNTIME_CLASS(FourColFillAttribute))
			CurrentColContext->ConvertColour(Fill->GetEndColour3(), (ColourPacked *) &Result);
		COLORREF EndColour3 = RGB(Result.Red, Result.Green, Result.Blue);

		/*CProfileBiasGain DefaultBiasGain;

		if (!(DiagramMapper == DefaultBiasGain))
		{
			Table.BuildTable(Fill->Colour, Fill->EndColour, GetRenderView(), GetFillEffect(), DiagramMapper, LargeGradTables);
			GetDrawContext()->SetGraduation( Style, Table.GetTable(), &A, &B, &C );
		}*/

		if (FillType == CC_RUNTIME_CLASS(ThreeColFillAttribute))
		{			
			// Style & 0xFF = 0 (simple with better colours outside the mesh)
			// Style & 0xFF = 2 (tiled)

			if (Tiling == 1)	// If it is a simple fill
				Style |= 0;		// set the style to 0
			else
				Style |= 2;		// otherwise set it to 2

			//CProfileBiasGain DefaultBiasGain;			// default
														// DiagramMapper is the applied biasgain

			//if (DiagramMapper == DefaultBiasGain)		// if the applied biasgain is the default biasgain
			//{
				if (Perspective)
					GetDrawContext()->Set3WayGraduation4(Style, StartColour, EndColour, EndColour2,
											&A, &B, &C, &D);
				else
					GetDrawContext()->Set3WayGraduation(Style, StartColour, EndColour, EndColour2,
											&A, &B, &C);
			//}
			//else
			//{
			//	Table.BuildTable(Fill->Colour, Fill->EndColour, GetRenderView(), GetFillEffect(), DiagramMapper, LargeGradTables);
			//	GetDrawContext()->Set3WayGraduation(Style, StartColour, EndColour, EndColour2,
//											&A, &B, &C);
			//}
		}
		else
		{
			// Style & 0xFF = 1 (for better colours outside the mesh)
			// Style & 0xFF = 2 (tiled)

			if (Tiling == 1)	// If it is a simple fill
				Style |= 1;		// set the style to 1
			else
				Style |= 2;		// otherwise set it to 2

			// The colours in the following lines are correct!!!
			if (Perspective)
				GetDrawContext()->Set4WayGraduation4(Style, StartColour, EndColour, EndColour3, EndColour2,
										&A, &B, &C, &D);
			else
				GetDrawContext()->Set4WayGraduation(Style, StartColour, EndColour, EndColour3, EndColour2,
										&A, &B, &C);
		}
	}
	else
	{
// Mark Howitt, 6/10/97. Enable the repeating fill type operation if selected
#ifdef NEW_FEATURES
		if(Tiling == 4) Style |= 0x80;		// Check to see if repeating, if so set it
#endif

		// CGS ....
		// We now need to decide exactly who is going to build the table!
		// If the fill has the default biasgain, then we will continue to build
		// this in the standard fashion (i.e.  let GDraw build it).
		// If however it does NOT, then we will build the table ourselves (with respect to
		// the applied biasgain) ....
		
		CProfileBiasGain DefaultBiasGain;			// default
													// DiagramMapper is the applied biasgain

		if (DiagramMapper == DefaultBiasGain)		// if the applied biasgain is the default biasgain
		{
			// get on and use standard rendering stuff ....

			ColourRamp *pColourRamp = Fill->GetColourRamp();
			if (pColourRamp)
			{
				if (Tiling != 4)
				{
					Table.BuildTable(Fill->Colour, Fill->EndColour, pColourRamp, GetRenderView(), GetFillEffect(), LargeGradTables);
				}
				else
				{
					Table.BuildHighQualityRepeatTable(Fill->Colour, Fill->EndColour, pColourRamp, GetRenderView(), GetFillEffect());
				}
			}
			else
			{
				if (Tiling != 4)
				{
					Table.BuildTable(Fill->Colour, Fill->EndColour, GetRenderView(), GetFillEffect(), LargeGradTables);
				}
				else
				{
					Table.BuildHighQualityRepeatTable(Fill->Colour, Fill->EndColour, NULL, GetRenderView(), GetFillEffect());
				}
			}

			if (Perspective)
				GetDrawContext()->SetGraduation4( Style, Table.GetTable(), &A, &B, &C, &D );
			else
				GetDrawContext()->SetGraduation( Style, Table.GetTable(), &A, &B, &C );
		}
		else
		{
			// use our new rendering stuff that takes account of profiles ....

			if (Tiling != 4)
			{
				Table.BuildTable(Fill->Colour, Fill->EndColour, GetRenderView(), GetFillEffect(), DiagramMapper, LargeGradTables);
				GetDrawContext()->SetGraduation( Style, Table.GetTable(), &A, &B, &C );
			}
			else
			{
				Table.BuildHighQualityRepeatTable(Fill->Colour, Fill->EndColour, GetRenderView(), GetFillEffect(), DiagramMapper);
				GetDrawContext()->SetGraduation( Style, Table.GetTable(), &A, &B, &C );
			}
			GetDrawContext()->SetGraduation( Style, Table.GetTable(), &A, &B, &C );
		}
	}

	// Now it is time to Draw the path, so we will Fuzzy Clip it just in case it is too big
	DocCoord* Coords;
	PathVerb* Verbs;
	UINT32 TotalCoords;
	BOOL ShouldDeleteCoords = FALSE;

	// try and clip the path before rendering it, for bigger zoom values
	if (TryToFuzzyClip(PathToDraw, &Coords, &Verbs, &TotalCoords))
	{
		// Yes, we clipped the paths alright
		ShouldDeleteCoords = TRUE;
	}
	else
	{
		// No Clipping here matey
		Coords = PathToDraw->GetCoordArray();
		Verbs  = PathToDraw->GetVerbArray();
		TotalCoords = PathToDraw->GetNumCoords();
	}

	// Draw the actual path
	if (TotalCoords > 1)
		GetDrawContext()->FillPath( (POINT*)Coords, Verbs, TotalCoords, (RR_WINDINGRULE()==EvenOddWinding) ? 0 : 1 );

	// Clean up if we need to
	if (ShouldDeleteCoords)
	{
		delete Coords;
		delete Verbs;
	}

	#if 0
	if (0)
	{
		// debug code - plot the points
		GetDrawContext()->SetColour( RGB(0,0,0) );
		/*DocCoord*/ POINT Path[5];
		BYTE Verbs[] = { PT_MOVETO, PT_LINETO, PT_MOVETO, PT_LINETO, PT_LINETO };
		/*DocCoord*/POINT D;
		D.x = (A.x+B.x)/2;
		D.y = (A.y+B.y)/2;

		Path[0] = A;
		Path[1] = B;
		Path[2] = A;
		Path[3] = C;
		Path[4] = D;
		GetDrawContext()->StrokePath( (LPPOINT)Path, Verbs, sizeof(Verbs), FALSE, 0, CAPS_ROUND, JOIN_ROUND, NULL );
	}
	#endif
	return TRUE;
}




/////////////////////////////////////////////////////////////////////////////////////////////
//
//								Offscreen rendering system
//
/////////////////////////////////////////////////////////////////////////////////////////////

INT32 PixelsPerMeterAtCurrentViewScale( FIXED16 Scale)
{
	double	PixPerMetre =	PIXELS_PER_INCH * INCHES_PER_METRE;
			PixPerMetre *=	Scale.MakeDouble();

	return (INT32) (PixPerMetre + 0.5);
}



/********************************************************************************************
>	virtual void GRenderRegion::SetOffscreen(OffscreenAttrValue* pAttr)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/2000
	Inputs:		pAttr	the offscreen attribute (eg FeatherAttrValue) to use.

				** Following only used by GRenderRegion version **
				info	pointer to bmp info struct which we will setup.
				bits	pointer to bitmap bits which we'll setup.

	Purpose:	Create a new bitmap into which, the subtree rooted at this attributes parent,
				will be rendered. Then call base version to push the current OffscreenAttr
				onto the context stack.

	SeeAlso:	GRenderRegion::RestoreOffscreen; GRenderRegion::
********************************************************************************************/
void GRenderRegion::SetOffscreen(OffscreenAttrValue* pAttr)
{
	// Karim 23/07/2000
	// Mustn't have feathering overhead for hit-testing, so
	// don't bother doing anything if we're a hit-test RR.
	if (IsHitDetect())
		return;

	if (m_ForcePrintingCaps)
		return;

	// Save pointer to OffscreenAttr, and move current OffscreenAttr to the attr context stack
	RenderRegion::SetOffscreen(pAttr);
}
/*void GRenderRegion::SetOffscreen(OffscreenAttrValue* pAttr)
{
//	TRACEUSER( "Gerry", _T("GRenderRegion::SetOffscreen (0x%08x)\n"), pAttr);

	// Karim 23/07/2000
	// Mustn't have feathering overhead for hit-testing, so
	// don't bother doing anything if we're a hit-test RR.
	if (IsHitDetect())
		return;

	if (m_ForcePrintingCaps)
		return;

	// Karim 23/07/2000
	// Don't bother with feathering at all if our quality setting is too low.
	// Note: this test should probably be made in a Feather override of a Node vfn.
	if (RRQuality.GetFillQuality() < Quality::Graduated)
		return;

	//---------------------------------------------
	// Karim 20/07/2000
	// Moved from OffscreenAttrValue::Render(), to avoid checks for render-region types
	// in the feathering code.
	// The following code works out and sets the offscreen attr's
	// offscreen bounding box.

	// Invalid area = intersection of node's and render-region's bounding rects.
	//
	// Notes:	1.	Inflate the node's bounding rect slightly, so it _completely_
	//				encloses the node (sometimes it doesn't quite).
	//			2.	Inflate the RR's clip-rect slightly, so that we don't get
	//				anti-aliasing edge artifacts in the final feather-bmp render.
	//			3.	If printing, then use the GRR's inner rect instead of its clipping
	//				rect, to fix stripey printing of feathers.
	//				This BODGE is necessary because for some reason CurrentClipRect
	//				does not cover a GRR's whole bitmap when printing !?!?
	MILLIPOINT LargePixelWidth = (MILLIPOINT)ceil(GetScaledPixelWidthDouble());
	DocRect NodeBB	= pAttr->GetLinkedNode()->GetBoundingRect();
	NodeBB.Inflate( LargePixelWidth );

	DocRect GRegionBB = CurrentClipRect;
// Removed by Phil 2/6/2004
// Seems to be bogus...
//		if (IsPrinting())
//			GRegionBB = InnerRect;
//		else
		GRegionBB.Inflate( 2 * LargePixelWidth );

	if (!NodeBB.IsIntersectedWith(GRegionBB))
		return;

	// calculate their intersection.
	DocRect drOffBB	= NodeBB.Intersection(GRegionBB);

	// Ensure that we have minimum pixel dimensions
	// Expand cliprect if more pixels required
	// NB expanding by GRR->ScaledPixelWidth will ensure we increase bmp size 
	// by exactly the no. of pixels required
	UINT32 RequiredWidth, RequiredHeight;
	pAttr->GetMinimumOffscreenBmpDimensions(&RequiredWidth,&RequiredHeight);
	WinRect Rect = CalculateWinRect( drOffBB );
	UINT32 ProposedWidth	= Rect.right - Rect.left;
	UINT32 ProposedHeight	= Rect.bottom - Rect.top;
	MILLIPOINT ScaledPixelWidth = GetScaledPixelWidth();

	// We have to check if the render matrix involves rotation as we have to
	// expand the DocRect in the other direction
	// First get the components of the matrix
	FIXED16 comps[4];
	RenderMatrix.GetComponents(comps, NULL);
	// It is rotated if a = d = 0 and b = -c
	BOOL bRotated = (comps[0] == FIXED16(0) && comps[3] == FIXED16(0) && comps[1] == -comps[2]);
	if(ProposedWidth < (INT32)RequiredWidth)
	{
		// make the bitmap wider
		// For odd pixel differences, widen rightwards by extra pixel
		MILLIPOINT ExtraPixelWidth = (MILLIPOINT)(RequiredWidth - ProposedWidth);
		MILLIPOINT Decrement = (ExtraPixelWidth / 2) * ScaledPixelWidth;
		MILLIPOINT Increment = Decrement + ((ExtraPixelWidth % 2) * ScaledPixelWidth);

		// if there is a rotation involved we have to increase the height of the DocRect
		// otherwise we increase the width
		if (bRotated)
		{
			drOffBB.loy -= Decrement;
			drOffBB.hiy += Increment;
		}
		else
		{
			drOffBB.lox -= Decrement;
			drOffBB.hix += Increment;
		}
	}

	if (ProposedHeight < (INT32)RequiredHeight)
	{
		// make the bitmap taller
		// For odd pixel differences, stretch upwards by extra pixel
		MILLIPOINT ExtraPixelHeight = (MILLIPOINT)(RequiredHeight - ProposedHeight);
		MILLIPOINT Decrement = (ExtraPixelHeight / 2) * ScaledPixelWidth;
		MILLIPOINT Increment = Decrement + ((ExtraPixelHeight % 2) * ScaledPixelWidth);

		// if there is a rotation involved we have to increase the width of the DocRect
		// otherwise we increase the height
		if (bRotated)
		{
			drOffBB.lox -= Decrement;
			drOffBB.hix += Increment;
		}
		else
		{
			drOffBB.loy -= Decrement;
			drOffBB.hiy += Increment;
		}
	}

	pAttr->SetOffscreenBoundingRect(drOffBB);

	//---------------------------------------------
	// Now startup the offscreen capture...
	// 
	// If a non-mix attribute is already in scope we must not start capturing transparently
	// because transparent bitmaps can't render non-mix transparencies correctly
	// (See SetTranspFillGeometry and ChangeCapture)
	//
	TranspFillAttribute* pTransAttr = (TranspFillAttribute *) GetCurrentAttribute(ATTR_TRANSPFILLGEOMETRY);
	UINT32 ttype = pTransAttr->GetTranspType();
	BOOL bStartTransparently = (ttype==TT_NoTranspType || ttype==TT_Mix);

	// Also, scan down into the tree and see whether this node has any non-mix that would
	// cause us to revert to non-transparent capture
	// Because it's (probably) quicker to scan the tree than to revert during rendering...
	// Urgh.
	NodeRenderableInk* pLinkedNode = pAttr->GetLinkedNode()->IsAnObject() ? (NodeRenderableInk*)pAttr->GetLinkedNode() : NULL;
	bStartTransparently = bStartTransparently && !(pLinkedNode && pLinkedNode->ContainsNonMixTransparency(this));

	BOOL bOK = StartCapture(pAttr,
							drOffBB,
							CAPTUREINFO(ctNESTABLE, cfNONE + cfDEBUG),		// Don't allow collapse to master bitmap, CollapseToMaster=0
							bStartTransparently,
							!bStartTransparently
							);
	if (!bOK)
		return;

	// Save pointer to OffscreenAttr, and move current OffscreenAttr to the attr context stack
	RenderRegion::SetOffscreen(pAttr);
}*/



/*
	GRenderRegions can be deleted or reset midway through rendering offscreen

	Requirement: ensure all bitmap memory is freed, as well as cleaning up the offscreen state stack

	Problems:	1) Initial bitmap memory needs to be freed by the class that allocated it
				2) GRenderRegion bmp pointers may be pointing to offscreen bitmap memory when
				   the classes deconstructor is called
				3) FreeLPBits is a pure virtual function so can't be called from ~GRenderRegion
				4) Normally stack memory freed by GRenderRegion::RestoreOffscreen()
				   and bitmap memory freed by OffscreenAttrValue::GoingOutOfScope
				   as RenderRegion attribute stack is pushed and popped (ie as attributes
				   render and go out of scope)
				5) On deconstruction, attributes can only call the RenderRegion 'Restore-" virtual
				   functions, not the GRenderRegion overrides.

	Solution:
				1) BmpMemory and offscreen stack memory must be freed in one go (GRenderRegion::FreeOffscreenState())
				2) This must be called from the deconstructor of the derived render region which is
				   being deleted
				OR
				   the ResetRegion function
				3) The attributes must be notified that their offscreen bitmap pointers are invalid
				   and must not attempt to do anything with that memory
				4) The GRenderRegion stack freeing function (GRenderRegion::RestoreOffscreen())
				   must check to see if the stack is empty and default to RenderRegion::RestoreOffscreen()
				   if it is


	// pAttr is the offscreen attribute which is going to be restored.

*/

void GRenderRegion::RestoreOffscreen(OffscreenAttrValue* pAttr)
{
	if (m_ForcePrintingCaps)
		return;

	// call our base class, to update the offscreen attr stack.
	// this will also trigger a call to the current attr's OffscreenRenderingCompleted() method.
	RenderRegion::RestoreOffscreen(pAttr);
}
/*void GRenderRegion::RestoreOffscreen(OffscreenAttrValue* pAttr)
{
	if (m_ForcePrintingCaps)
		return;

	// End this capture:
	// Don't render captured bitmap back into render region - owner will do that after processing
	// Release bitmap here - if this region is being reset
	LPBITMAPINFO lpCapturedBitmapInfo = NULL;
	LPBYTE lpCapturedBits = NULL;
	DocRect CaptureRect = CurrentOffscreenAttr->GetOffscreenBoundingRect();	// Initialise to size of feathered object
															// so that StopCapture can return sensible 24BPP bitmap if needed
	Capture* pTopCapture = GetTopCapture();
	ENSURE(pTopCapture && pTopCapture->GetOwner()==CurrentOffscreenAttr, "RestoreOffscreen attr is not owned by top capture\n");
	if (pTopCapture && pTopCapture->GetOwner()==CurrentOffscreenAttr)
	{
		StopCapture(CurrentOffscreenAttr, FALSE, IsBeingReset, &lpCapturedBitmapInfo, &lpCapturedBits, &CaptureRect);
	}

	if (!IsBeingReset)
	{
		// Normal restore in process of traversing the tree
		// So tell the offscreen attribute to handle the captured bitmap as it sees fit...
		CurrentOffscreenAttr->OffscreenRenderingCompleted(this, lpCapturedBitmapInfo, lpCapturedBits, CaptureRect);
	}

	// call our base class, to update the offscreen attr stack.
	// this will also trigger a call to the current attr's OffscreenRenderingCompleted() method.
	RenderRegion::RestoreOffscreen(pAttr);
}*/


// This function is used by OffscreenAttrValue::GoingOutOfScope to render a bitmap
// with its convolution applied as a transparency.
// If we run out of memory we will be unable to allocate memory to create the
// bitmap mask so this function handles a NULL BitmapTranspFillAttribute as input.
BOOL GRenderRegion::RenderBitmapWithTransparency(
												 Path *pPath,
												 BitmapFillAttribute* pFill,
												 BitmapTranspFillAttribute* pTranspFill
												)
{
/*
 *	Karim 08/11/2000
 *	This commented out code does the job using more abstract instructions.
 *
	ERROR2IF(pFill == NULL || pPath == NULL, FALSE,
			"GRenderRegion::RenderBitmapWithTransparency; NULL input params!");

	AttrTranspFillMappingLinear	TranspFillMapping;
	AttrFillMappingLinear		FillMapping;

	SaveContext();

	SetLineColour(COLOUR_NONE);

	FillMapping.Value.Repeat = 1;
	FillMapping.Render(this);

	pFill->Render(this);

	if (pTranspFill != NULL)
	{
		TranspFillMapping.Value.Repeat = 1;
		TranspFillMapping.Render(this);

		pTranspFill->Render(this);
	}

	DrawPath(pPath);

	RestoreContext();

	return TRUE;
*/


	// Store a ptr to the current fill-mapping attr, replacing it with a single-tile mapping.
	FillMappingAttribute* pCurrentFillMapAttr = (FillMappingAttribute*)CurrentAttrs[ATTR_FILLMAPPING].pAttr;

	FillMappingAttribute FillMap;
	FillMap.Repeat = 1;

	CurrentAttrs[ATTR_FILLMAPPING].pAttr = (AttributeValue*)&FillMap;


	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// Transparency fill:-
	// 1) store existing current transp attributes (so that they can be restored afterwards)
	// 2) create required transp fill mapping
	// 3) set current transp attrs to the new transparency attrs

	TranspFillAttribute* pCurrentTranspAttr = NULL;
	TranspFillMappingAttribute* pCurrentTranspMapAttr = NULL;
	TranspFillMappingAttribute TranspMap;
	TranspMap.Repeat = 1;

	if (pTranspFill != NULL)
	{
		pCurrentTranspAttr = RR_FILLTRANSP();
		pCurrentTranspMapAttr = (TranspFillMappingAttribute*)CurrentAttrs[ATTR_TRANSPFILLMAPPING].pAttr;

		CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr = pTranspFill;
		CurrentAttrs[ATTR_TRANSPFILLMAPPING].pAttr = (AttributeValue*)&TranspMap;
	}

	BOOL res = RenderBitmapFill(pPath, pFill);

	// Restore pre-existing current attrs
	CurrentAttrs[ATTR_FILLMAPPING].pAttr = pCurrentFillMapAttr;
	if (pTranspFill != NULL)
	{
		CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr = pCurrentTranspAttr;
		CurrentAttrs[ATTR_TRANSPFILLMAPPING].pAttr = pCurrentTranspMapAttr;
	}

	return res;
/**/
}

/////////////////////////////////////////////////////////////////////////////////////////////



// Andy Hills, 23-10-00:
// Helper functions for RenderBitmapFill and SetBitmapTransparencyFill



/********************************************************************************************

>	BOOL GRenderRegion::IsScaledUp(FillGeometryAttribute *Fill)

	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/10/00
	Inputs:		Fill	the bmp fill to test
	Outputs:	-
	Returns:	TRUE if the bitmap is scaled up (i.e. zoomed into / enlarged)
	Purpose:	Helper function for RenderBitmapFill and SetBitmapTransparencyFill.
				Used to work out whether or not to smooth bitmap fills/transparencies.
	Errors:		-

********************************************************************************************/

BOOL GRenderRegion::IsScaledUp(FillGeometryAttribute *Fill)
{
	// validate *Fill
	BOOL bIsBitmapFill = (	(Fill->IsKindOf( CC_RUNTIME_CLASS(BitmapFillAttribute) ))
						||	(Fill->IsKindOf( CC_RUNTIME_CLASS(TranspFillAttribute) )));

	ERROR3IF( (!bIsBitmapFill), "GRenderRegion::IsScaledUp - Fill should be a bmp fill or a transp bmp fill" );
	if( !bIsBitmapFill ) return FALSE;


	// calculate the desired exactly shrunk bitmap size from the handles on the old bitmap
	DocCoord* pStartpoint	= Fill->GetStartPoint();
	DocCoord* pEndpoint		= Fill->GetEndPoint();
	DocCoord* pEndpoint2	= Fill->GetEndPoint2();

	// Karim 03/08/2000
	UINT32 NewWidth	= (UINT32)(0.5 + pStartpoint->Distance(*pEndpoint)  / GetScaledPixelWidth());
	UINT32 NewHeight	= (UINT32)(0.5 + pStartpoint->Distance(*pEndpoint2) / GetScaledPixelWidth());

	return (NewWidth > Fill->GetBitmap()->GetWidth() && NewHeight > Fill->GetBitmap()->GetHeight());
}


/********************************************************************************************

>	BOOL GRenderRegion::IsAt100Percent(FillGeometryAttribute *Fill)

	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/10/00
	Inputs:		Fill	the bmp fill to test
	Outputs:	-
	Returns:	TRUE if the bitmap is at 100% (i.e. not zoomed in or resized)
	Purpose:	Helper function for RenderBitmapFill and SetBitmapTransparencyFill.
				Used to work out whether or not to smooth bitmap fills/transparencies.
	Errors:		-

********************************************************************************************/

BOOL GRenderRegion::IsAt100Percent(FillGeometryAttribute *Fill)
{
	// validate *Fill
	BOOL bIsBitmapFill = (	(Fill->IsKindOf( CC_RUNTIME_CLASS(BitmapFillAttribute) ))
						||	(Fill->IsKindOf( CC_RUNTIME_CLASS(TranspFillAttribute) )));

	ERROR3IF( (!bIsBitmapFill), "GRenderRegion::IsAt100Percent - Fill should be a bmp fill or a transp bmp fill" );
	if( !bIsBitmapFill ) return FALSE;


	// calculate the desired exactly shrunk bitmap size from the handles on the old bitmap
	DocCoord* pStartpoint	= Fill->GetStartPoint();
	DocCoord* pEndpoint		= Fill->GetEndPoint();
	DocCoord* pEndpoint2	= Fill->GetEndPoint2();

	// Karim 03/08/2000
	UINT32 NewWidth	= (UINT32)(0.5 + pStartpoint->Distance(*pEndpoint)  / GetScaledPixelWidth());
	UINT32 NewHeight	= (UINT32)(0.5 + pStartpoint->Distance(*pEndpoint2) / GetScaledPixelWidth());

	return (NewWidth == Fill->GetBitmap()->GetWidth() && NewHeight == Fill->GetBitmap()->GetHeight());
}


/********************************************************************************************

>	BOOL GRenderRegion::IsDistorted(FillGeometryAttribute *Fill)

	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/10/00
	Inputs:		Fill	the bmp fill to test
	Outputs:	-
	Returns:	TRUE if the bitmap is distorted (rotated or skewed).
	Purpose:	Helper function for RenderBitmapFill and SetBitmapTransparencyFill.
				Used to work out whether or not to smooth bitmap fills/transparencies.
	Errors:		-

********************************************************************************************/

BOOL GRenderRegion::IsDistorted(FillGeometryAttribute *Fill)
{
	// validate *Fill
	BOOL bIsBitmapFill = (	(Fill->IsKindOf( CC_RUNTIME_CLASS(BitmapFillAttribute) ))
						||	(Fill->IsKindOf( CC_RUNTIME_CLASS(TranspFillAttribute) )));

	ERROR3IF( (!bIsBitmapFill), "GRenderRegion::IsDistorted - Fill should be a bmp fill or a transp bmp fill" );
	if( !bIsBitmapFill ) return FALSE;


	// get fill handles
	DocCoord* pStartpoint	= Fill->GetStartPoint();
	DocCoord* pEndpoint		= Fill->GetEndPoint();
	DocCoord* pEndpoint2	= Fill->GetEndPoint2();

	// Is the bmp rotated/skewed?
	// If so, it is deemed 'distorted', and will be smoothed, even at 100%.
	// N.B. We will ignore rotation through 90/180/270 degrees,
	// or vertical/horizontal flipping.
	// i.e. we will deem the bmp to be distorted if either of the
	// endpoints do not line up with the startpoint.
	// 
	// e.g. these are all non-distorted fills:
	// 
	// B       A--->B  C<---A
	// ^       |            |
	// |       |            |
	// |       V            V
	// A--->C  C            B
	// 
	// startpoint A(x0,y0), endpoints B(x1,y1), C(x2,y2)

	ERROR3IF(GetScaledPixelWidth()==0, "Scaled pixel width is 0 in IsDistorted");
	if (GetScaledPixelWidth()==0)
		return TRUE;
	
	ERROR3IF(GetScaledPixelWidth()==0, "It's all gone wrong!");
	if (GetScaledPixelWidth()==0) return TRUE;						// Fail safe

	UINT32 x0 = UINT32(0.5 + pStartpoint->x / GetScaledPixelWidth());
	UINT32 y0 = UINT32(0.5 + pStartpoint->y / GetScaledPixelWidth());
	UINT32 x1 = UINT32(0.5 + pEndpoint  ->x / GetScaledPixelWidth());
	UINT32 y1 = UINT32(0.5 + pEndpoint  ->y / GetScaledPixelWidth());
	UINT32 x2 = UINT32(0.5 + pEndpoint2 ->x / GetScaledPixelWidth());
	UINT32 y2 = UINT32(0.5 + pEndpoint2 ->y / GetScaledPixelWidth());

	//TRACEUSER( "Andy", _T("distorted=%d (%ld,%ld),(%ld,%ld),(%ld,%ld)\n"),
	//			(!(((x0==x1) && (y0==y2)) || ((x0==x2) && (y0==y1)))),
	//			x0,y0,x1,y1,x2,y2);

	return !((x0==x1 && y0==y2) || (x0==x2 && y0==y1));
}


/********************************************************************************************
>	BOOL GRenderRegion::NeedToSmooth(FillGeometryAttribute *Fill,BOOL bAlreadyScaled)

	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/10/00
	Inputs:		Fill			the bmp fill to test
				bAlreadyScaled	TRUE if the bmp has already been scaled to the correct size
								(using the super-smooth scaling algorithm)
	Outputs:	-
	Returns:	TRUE if the bitmap needs to be smoothed.
				This depends on various factors - e.g.
				* is it at 100%?
				* if not, has it already been scaled to 100%?
				* is it rotated/skewed?
				* is interpolation enabled?
				* are we forcing interpolation? (e.g. we're rendering the shadow of a bitmap)
				* is document-wide bitmap smoothing (options dialogue) enabled?
	Purpose:	Helper function for RenderBitmapFill and SetBitmapTransparencyFill.
				Used to work out whether or not to smooth bitmap fills/transparencies.
	Errors:		-

********************************************************************************************/

BOOL GRenderRegion::NeedToSmooth(FillGeometryAttribute *Fill,BOOL bAlreadyScaled)
{
	// Karim 16/10/2000
	// The shadow routines need a way to enforce bitmap smoothing for all
	// anti-aliased rendering.

	if (IsForcingBitmapSmoothing())
	{
		return TRUE;
	}

	//TRACEUSER( "Andy", _T("antialias=%d\n"),(RRQuality.GetAntialiasQuality() >= Quality::FullAntialias));
	
	// check that smoothing is enabled, and detail=max, and document bitmap smoothing is enabled
	if (Fill==NULL ||
		BitmapFillAttribute::m_doBitmapSmoothing == FALSE ||
		RRQuality.GetAntialiasQuality() < Quality::FullAntialias ||
		(Document::GetCurrent() && !(Document::GetCurrent())->GetBitmapSmoothing()) )
	{
		return FALSE;
	}

	// Andy 19/10/2000
	BOOL bNot100Percent	= ! (IsAt100Percent(Fill) || bAlreadyScaled);
	BOOL bNotScaledUp	= ! IsScaledUp(Fill);
	BOOL bIsDistorted = IsDistorted(Fill);

	// do the smoothing if the bitmap is not at 100%
	// if it is at 100% sub-pixel movements will be snapped to pixel alignment
	if( Fill->GetBitmap()->GetInterpolation() )
	{
		// 'smooth when scaled up' is on --> interpolate if zoom isn't 100%
		return (bNot100Percent || bIsDistorted);
	}
	else
	{
		// 'smooth when scaled up' is off --> don't interpolate if zoomed in (horizontally AND vertically)
		return ((bNot100Percent && bNotScaledUp) || ((!bNot100Percent) && bIsDistorted));
	}
}


/********************************************************************************************
>	BOOL GRenderRegion::SetSmoothingFlags(FillGeometryAttribute *Fill)

	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/00
	Inputs:		Fill			the bmp fill for which to set the flags
	Outputs:	-
	Returns:	TRUE on success.
	Purpose:	Helper function for RenderBitmapFill and SetBitmapTransparencyFill.
				Sets the smoothing flags using SetBitmapSmoothingFlag and
				SetTileFilteringFlag.
				These flags determine the quality of subsequent SetBitmapFill / 
				SetTransparentBitmapFill operations.
	Errors:		-

********************************************************************************************/

BOOL GRenderRegion::SetSmoothingFlags(FillGeometryAttribute *Fill)
{
	// if we're printing or exporting, maximise quality
	BOOL bNeedMaxQuality = (IsUsingSmoothedBitmaps() || IsPrinting());

	// Andy Hills, 24-10-00:
	// Moved the SetExtraSmoothFlag logic to here:
	// the flag needs to be set before we SetBitmapFill.
	BOOL SetExtraSmoothFlag   = FALSE;
	BOOL SetTileFilteringFlag = FALSE;

	if( !bNeedMaxQuality )
	{
		// Normal document rendering --> use normal bmp rendering functions.
		// The bitmap may require smoothing (e.g. if it's being rotated/skewed).
		
		// to smooth or not to smooth? NeedToSmooth answers the question.
		SetExtraSmoothFlag = NeedToSmooth( Fill, FALSE /* we haven't scaled the bitmap down */ );
	}
	else
	{
		// Create bitmap dialogue / printing
		// --> Set tile filtering flag (i.e. maximum quality scaling/rotation algorithm)
		SetTileFilteringFlag = NeedToSmooth( Fill, FALSE );
	}

	// Get default values from current active capture
	Capture* pCapture = GetTopCapture();
	if (pCapture)
	{
		if (!SetExtraSmoothFlag) SetExtraSmoothFlag = pCapture->info.caFlags.BitmapSmoothing;
		if (!SetTileFilteringFlag) SetTileFilteringFlag = pCapture->info.caFlags.TileFiltering;
	}

	GetDrawContext()->SetTileSmoothingFlag(SetExtraSmoothFlag);
	GetDrawContext()->SetTileFilteringFlag(SetTileFilteringFlag);

	return TRUE;
}


/********************************************************************************************

>	BOOL GRenderRegion::RenderBitmapFill(Path *PathToDraw, BitmapFillAttribute *Fill )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Inputs:		PathToDraw is a pointer to a Path object to render. Fill is the bitmap fill
				required.
	Outputs:	-
	Returns:	TRUE if understood fill type & rendered it, FALSE if not
	Purpose:	Fills a shape with a bitmap
	Errors:		-

********************************************************************************************/

BOOL GRenderRegion::RenderBitmapFill(Path *PathToDraw, BitmapFillAttribute* Fill)
{
	if ( Fill==NULL || Fill->GetBitmap()==NULL || Fill->GetBitmap()->ActualBitmap==NULL)
		return FALSE;									// if no bitmap

	ENSURE( Fill->GetBitmap()->ActualBitmap->IsKindOf( CC_RUNTIME_CLASS( CWxBitmap ) ),
																"Strange bitmapfill");

	INT32 bpp = Fill->GetBitmap()->GetBPP();
	CWxBitmap *WinBM = NULL;
	CWxBitmap *OrigWinBM = NULL;
//	BYTE *pGreyTable = NULL;
//	BOOL bContoned = FALSE;
//	BOOL bAlpha = FALSE;
/*
	if (Fill->GetStartColour()!=NULL && Fill->GetEndColour()!=NULL)
	{
		// The bitmap is contoned
		bContoned = TRUE;

//		if (bpp == 32 && Fill->GetBitmap()->IsTransparent())
//			bAlpha = TRUE;

		if ( bpp<=8 )
		{
			// Not alpha so we need to use the greyscale version
			WinBM = (CWxBitmap*)Fill->GetBitmap()->GetGreyscaleVersion();
			if (WinBM)
				bpp = WinBM->GetBPP();
			else
			{
				// Aha, we're gunna do some clever palette jiggery pokery
				pGreyTable = Fill->GetBitmap()->ActualBitmap->GetGreyscaleTable();
			}
		}
	}	

	if (WinBM == NULL)
	{
*/		// Use the normal bitmap
		WinBM = (CWxBitmap*)Fill->GetBitmap()->ActualBitmap;
//	}

	if (WinBM == NULL || WinBM->BMInfo == NULL  || WinBM->BMBytes == NULL)
		return FALSE;

	// convert ArtWorks style attribute into Gavin-style
	POINT PGram[4];
	PGram[0].x = Fill->StartPoint.x; PGram[0].y = Fill->StartPoint.y;
	PGram[1].x = Fill->EndPoint.x;	 PGram[1].y = Fill->EndPoint.y;

	BOOL Perspective = Fill->IsPerspective();
	if ( Perspective )
	{
		// Gerry removed the pixel size test cos it makes small repeating fills go spappy
		PGram[2].x = Fill->EndPoint3.x;	PGram[2].y = Fill->EndPoint3.y;
		PGram[3].x = Fill->EndPoint2.x;	PGram[3].y = Fill->EndPoint2.y;

		if ( !MouldPerspective::WillBeValid(PGram) )
		{
			Perspective = FALSE;
			TRACEUSER( "Will", _T("Bitmap Perspective points are Invalid !!\n"));
		}
	}
	if ( !Perspective )
	{
		PGram[2].x = Fill->EndPoint2.x;
		PGram[2].y = Fill->EndPoint2.y;
	}		

	// get the colour we use to fill around the edges from the current solid fill colour
	COLORREF DefaultColour = 0xFFFFFFFF;

	DWORD Style = 0;
	TranspGradTable TranspTable;

	if (uBitmapDepth==32)
	{
		// Check for a Transparency Fill
		GetTransparencyFill(&TranspTable, &Style);
	}
 	else
		Style = WhichTransparency<<8;

	INT32 Tiling = ((FillMappingAttribute*)CurrentAttrs[ATTR_FILLMAPPING].pAttr)->Repeat;
	Style |= Tiling;

	// --- Colour-correct and/or Contone the bitmap as necessary
	RGBQUAD *Palette = NULL;

	// Do the colour correction. This may produce a new pointer in Palette or BitmapBits,
	// which we should CCFree() when we are done with it - see the end of this function
/*	ColourCorrectBitmap(Fill, WinBM->BMInfo, &Palette);

	if ( bContoned && bpp>8 )
		Style |= 0x80 ;

	// Now see if we need to muck around with the palette for the contoning
	if (pGreyTable != NULL)
	{
//		ERROR3IF(bpp != 8, "Greytable should only be here when rendering an 8bpp bitmap");
		RGBQUAD *OldPalette = Palette;

		// Create a new palette
		Palette = (RGBQUAD *) CCMalloc(sizeof(RGBQUAD)<<bpp);
		if (Palette == NULL)
		{
			ERROR3("No memory for palette");
			return FALSE;
		}
		
		// Copy the entries from the contone palette into the new one,
		// using the Grey table as a guide
		for (INT32 i=0; i<1<<bpp; i++)
			Palette[i] = OldPalette[pGreyTable[i]];

		if (OldPalette != WinBM->BMInfo->bmiColors)
			CCFree(OldPalette);			// Don't need the contone palette any more
	}
*/
	// Check for transparent bitmap plotting
	BYTE *BitmapBits = WinBM->BMBytes;

	if (bpp <= 8)
	{
		INT32 NumCols = 1<<bpp;
		// Search for a transparent colour setting the Style flags if necessary...
		for (INT32 i=0; i<NumCols; i++)
		{
			if (WinBM->BMInfo->bmiColors[i].rgbReserved == 0xFF)
			{
				Style |= 0x008000;

				if (RenderFlags.VeryMono)
				{
					// We want to render everything as black,
					// so we'll use a temporary palette, with all
					// the colours set to black
					if (Palette == NULL)	// If no temporary palette allocated, get one
						Palette = (RGBQUAD *) CCMalloc(256 * sizeof(RGBQUAD));

					if (Palette != NULL)
					{
						// Set the whole palette to black
						memset(Palette, 0, 256*sizeof(RGBQUAD));
						// And set the transparent colour to transparent white
						Palette[i].rgbRed   	= 0xFF;
						Palette[i].rgbGreen 	= 0xFF;
						Palette[i].rgbBlue  	= 0xFF;
						Palette[i].rgbReserved 	= 0xFF;
					}
				}
				break;
			}
		}
	}

	// If we didn't create a temporary palette, then we'll use the original bitmap
	if ( Palette==NULL && bpp<=8 && !Fill->IsKindOf(CC_RUNTIME_CLASS(FractalFillAttribute)) &&
									!Fill->IsKindOf(CC_RUNTIME_CLASS(  NoiseFillAttribute)) )
		Palette = WinBM->BMInfo->bmiColors;

	LPBYTE pTranspTable = NULL;
	if ( uBitmapDepth==32 && bpp==32 )
	{
		// If rendering a non-paletted bitmap, then gavin needs the
		// transparency table to be passed in
		pTranspTable = (LPBYTE) TranspTable.GetTable();			// this cast is OK as the structure
																// returned is an array of bytes
		Style |= 0x004000;	// MarkH 15/7/99 Need to OR the Style with 0x004000 to say that
							// we want to use the alpha channel!!
	}

	BOOL bDoBitmapFill = TRUE;

	BOOL bClearBitmapConv = FALSE;
	BYTE* pSepTables = NULL;
//	BGR *pCyanSepTable = NULL;
//	BGR *pMagentaSepTable = NULL;
//	BGR *pYellowSepTable = NULL;
//	BGR *pBlackSepTable = NULL;
//	BYTE *pUnderColourRemovalTable = NULL;
//	BYTE *pBlackGenerationTable = NULL;
	CWxBitmap* pNewBitmap = NULL;
	
	// --- Add Separation Style bits as approriate to the current colour separation mode
	// Currently, the only "separation" option we use is composite print preview
	// SepStyle for the SetBitmapFill functions are always in the MS byte of Style
	if (bpp > 8)	// Only needed for deep bitmaps
	{
		// If we've got a valid colour plate and it is a composite preview
		if (CurrentColContext->GetColourPlate() != NULL &&
			!CurrentColContext->GetColourPlate()->IsDisabled())
		{
			
			if (CurrentColContext->GetColourPlate()->GetType() == COLOURPLATE_COMPOSITE)
			{
				// Find the colour manager (if there is one), and ask it for the 15-bit RGB
				// printer colour matching lookup table
PORTNOTE("cms", "DisabledXaraCMS")
#ifndef EXCLUDE_FROM_XARALX
				XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();
				if (lpCMSMan != NULL)
				{
					DWORD *MatchTable = lpCMSMan->GetPaperViewTable();
					ERROR3IF(MatchTable == NULL, "Can't create/find colour matching table");
					if (MatchTable != NULL)
					{
						GetDrawContext()->SetBitmapConversionTable((BGR *)MatchTable);
						bClearBitmapConv = TRUE;
						// Bits 3 and 4 of top byte of Style specifies the conversion algorithm
						// Make sure it only affects the 2 bits
						Style |= ((BitmapConversion & 3) << 27);
						
						// Make a copy of the bitmap
//						pNewBitmap = (WinBitmap*)WinBM->MakeCompositeCopy(RenderView);
//						OrigWinBM = WinBM;					// Save original bitmap pointer
//						WinBM = pNewBitmap;					// Use this bitmap instead
//						BitmapBits = WinBM->BMBytes;		// And make sure we update this
					}
				}
#endif
			}
			else if (CurrentColContext->GetColourPlate()->GetType() == COLOURPLATE_SPOT)
			{
				GetDrawContext()->SetColour(0xFFFFFF);
				bDoBitmapFill = FALSE;
			}
			else if (CurrentColContext->GetColourPlate()->GetType() != COLOURPLATE_NONE)
			{
				pSepTables = (BYTE *) CCMalloc(5 * 256 * sizeof(BYTE));
				if (pSepTables != NULL)
				{
					String_256 PrintProfile;
PORTNOTE("cms", "DisabledXaraCMS")
#ifndef EXCLUDE_FROM_XARALX
					XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();
					if (lpCMSMan)
						lpCMSMan->GetPrinterProfile(&PrintProfile);
#endif
					ColourContextCMYK *cc = new ColourContextCMYK(RenderView, &PrintProfile);
					if (cc->GetProfileTables(pSepTables))
					{
						// Make a copy of the bitmap
						pNewBitmap = (CWxBitmap*)WinBM->MakeSeparatedCopy(RenderView->GetColourPlate(), pSepTables);
						OrigWinBM = WinBM;					// Save original bitmap pointer
						WinBM = pNewBitmap;					// Use this bitmap instead
						BitmapBits = WinBM->BMBytes;		// And make sure we update this
					}
					delete cc;
				}
			}
		}
	}

	BOOL Result = FALSE; // function working correctly

	if (bDoBitmapFill)
	{
		//---------------------------------------------------------------------------------------
		// Setup contoning
		//---------------------------------------------------------------------------------------

		if ( Fill->GetStartColour()!=NULL && Fill->GetEndColour()!=NULL )
		{
			DocColour* pCS = Fill->GetStartColour();
			DocColour* pCE = Fill->GetEndColour  ();
			INT32 sr,sg,sb;
			INT32 er,eg,eb;
			pCS->GetRGBValue(&sr,&sg,&sb);
			pCE->GetRGBValue(&er,&eg,&eb);
			GetDrawContext()->SetContone(GetFillEffect()+1,RGB(sr,sg,sb),RGB(er,eg,eb));
		}
		else
			GetDrawContext()->SetContone(0);

		GetDrawContext()->SetBias(3,Fill->GetProfile().GetBias()) ;
		GetDrawContext()->SetGain(3,Fill->GetProfile().GetGain()) ;
		GetDrawContext()->SetOutputRange(3,0x00,0xFF) ;

		//---------------------------------------------------------------------------------------
		// Smoothing decisions
		//---------------------------------------------------------------------------------------

		SetSmoothingFlags(Fill);

		//---------------------------------------------------------------------------------------
		// SetBitmapFill here
		//---------------------------------------------------------------------------------------

		//TRACEUSER( "Andy", _T("SetBitmapFill2 (scale and/or rotate)\n"));

		if (Perspective)	// Is a Perspective applied ?
		{
			// then plot it perspectivised
			Result = GetDrawContext()->SetPerspectiveBitmapFill(
							&(WinBM->BMInfo->bmiHeader),
							BitmapBits,
							Style,
							PGram,
							DefaultColour,
							Palette,
							NULL, NULL, NULL, pTranspTable,
							0
							);
		}
		else
		{
			// just plot it normally
			Result = GetDrawContext()->SetBitmapFill(
							&(WinBM->BMInfo->bmiHeader),
							BitmapBits,
							Style,
							PGram,
							DefaultColour,
							Palette,
							NULL, NULL, NULL, pTranspTable,
							0
							);
		}
	}

	// Now it is time to Draw the path, so we will Fuzzy Clip it just in case it is too big
	DocCoord* Coords;
	PathVerb* Verbs;
	UINT32 TotalCoords;
	BOOL ShouldDeleteCoords = FALSE;

	// try and clip the path before rendering it, for bigger zoom values
	if (TryToFuzzyClip(PathToDraw, &Coords, &Verbs, &TotalCoords))
	{
		// Yes, we clipped the paths alright
		ShouldDeleteCoords = TRUE;
	}
	else
	{
		// No Clipping here matey
		Coords = PathToDraw->GetCoordArray();
		Verbs  = PathToDraw->GetVerbArray();
		TotalCoords = PathToDraw->GetNumCoords();
	}

	// Draw the actual path
	if (TotalCoords > 1)
		GetDrawContext()->FillPath( (POINT*)Coords, Verbs, TotalCoords, (RR_WINDINGRULE()==EvenOddWinding) ? 0 : 1 );

PORTNOTE( "other", "m_pTempTransparencyBMPBits seems never to be used, removed by GAT" )
#ifndef EXCLUDE_FROM_XARALX
	// Free the memory of the transparency bitmap if this was allocated temporarly
	// by either Get/SetTransparency
	// from using the bitmap scaling function
	if (m_pTempTransparencyBMPBits)
	{
		CCFree (m_pTempTransparencyBMPBits);
		m_pTempTransparencyBMPBits = NULL;
	}
#endif

	// Clean up if we need to
	if (ShouldDeleteCoords)
	{
		delete Coords;
		delete Verbs;
	}

	if (pNewBitmap)
	{
		WinBM = OrigWinBM;
		delete pNewBitmap;
	}
/*
	if (pAlphaContoneBmp)
	{
		if (WinBM == pAlphaContoneBmp)
			WinBM = pBeforeAlphaContoneBmp;
		delete pAlphaContoneBmp;
	}
*/
	// Free any memory used for colour-corrected bitmap palettes.
	// If this pointer doesn't point at the original palette, then it has
	// been temporarily allocated by ColourCorrectBitmap, above.
	if (Palette != WinBM->BMInfo->bmiColors)
		CCFree(Palette);

	if (bClearBitmapConv)
		GetDrawContext()->SetBitmapConversionTable(NULL);

	if (pSepTables)
	{
		GetDrawContext()->SetSeparationTables();	// Defaults to setting everything to NULL
		CCFree(pSepTables);
	}
/*
	if (pCyanSepTable)
		CCFree(pCyanSepTable);
	if (pMagentaSepTable)
		CCFree(pMagentaSepTable);
	if (pYellowSepTable)
		CCFree(pYellowSepTable);
	if (pBlackSepTable)
		CCFree(pBlackSepTable);
	if (pUnderColourRemovalTable)
		CCFree(pUnderColourRemovalTable);
	if (pBlackGenerationTable)
		CCFree(pBlackGenerationTable);
*/
	// Reset smoothing flags
	SetSmoothingFlags(NULL);

	return TRUE;
}



/********************************************************************************************

>	BOOL GRenderRegion::GetTransparencyFill(TranspGradTable* pTranspTable, DWORD* Style)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/94
	Inputs:		pTranspTable is a pointer to a Transparency Table to fill in.  This table must
				still exist at the time of rendering, as Gavin just keeps a pointer to it.
				So be careful with those Local Variables !!
				Style is a Pointer to the Fill style for us to update.
	Outputs:	Style is updated to include Transparency Style.
	Returns:	TRUE, if a Transparent fill has been set.
				FALSE, means it will use the Tranparency in the Colour Fill.
	Purpose:	Initialises a Transparency fill.
	Scope:		Protected

	Notes:		Added support for square, 3 and 4 point transparency (Gerry 12/8/96)

********************************************************************************************/

BOOL GRenderRegion::GetTransparencyFill(TranspGradTable* pTranspTable, DWORD* Style)
{
	if (uBitmapDepth != 32)
		return FALSE;		// Check transparency rendering is on

	COLORREF StartTransp;
	COLORREF EndTransp;
	DWORD TransStyle = 0;
	BOOL SpecialFill = FALSE;

	// Get the current Transparency Fill Geometry
	TranspFillAttribute* TranspFill = RR_FILLTRANSP();

//	StartTransp = ConvertColourToTransScreenWord(CurrentColContext, &TranspFill->Colour);
//	EndTransp = ConvertColourToTransScreenWord(CurrentColContext, &TranspFill->EndColour);

	// Get the transparency from the Fill
//	StartTransp = (StartTransp 	& 0xFF000000) >> 24;
//	EndTransp 	= (EndTransp 	& 0xFF000000) >> 24;

	StartTransp = 0;

	if (TranspFill->GetStartTransp() != NULL)
    	StartTransp = *TranspFill->GetStartTransp();

	UINT32 tType = TranspFill->GetTranspType();

	// Get the Transparency Fill Type
	CCRuntimeClass* FillType = TranspFill->GetRuntimeClass();

	// If it's a flat fill, then we don't need to do any
	// graduated stuff.  So Just update the Style and Return
	if (FillType == CC_RUNTIME_CLASS(FlatTranspFillAttribute))
	{
		if (tType == TT_Mix && StartTransp == 0)
			return FALSE;		// Use Colour Fill's Transparency

		*Style |= (MapTranspTypeToGDraw(tType, FALSE)<<8);
		*Style |= (StartTransp<<16);

		return TRUE;
	}
	else if (FillType == CC_RUNTIME_CLASS(ThreeColTranspFillAttribute))
	{
		SpecialFill = TRUE;
	}
	else if (FillType == CC_RUNTIME_CLASS(FourColTranspFillAttribute))
	{
		SpecialFill = TRUE;
	}
	else if (TranspFill->IsKindOf(CC_RUNTIME_CLASS(BitmapTranspFillAttribute)))
	{
//		TRACEUSER( "Will", _T("Bitmap Transparency, Start=%d, End=%d\n"),StartTransp,EndTransp);

		// If it is a bitmap (or fractal) transparency then get the other function to set it up
		return SetBitmapTransparencyFill((TranspFillAttribute*)TranspFill, Style, pTranspTable);
	}

	EndTransp = 255;

	if (TranspFill->GetEndTransp() != NULL)
    	EndTransp = *TranspFill->GetEndTransp();

	// It must be a Graduated Transparency !!
	// That's the spirit !! None of these boring 'Flat' things.
	// You want a nice Elliptical Transparency fading out at the edges.

	// Make the Transparency Table if required

	// CGS ....
	// We now need to decide exactly who is going to build the table!
	// If the transparency fill has the default biasgain, then we will continue to build
	// this in the standard fashion (i.e.  let GDraw build it).
	// If however it does NOT, then we will build the table ourselves (with respect to
	// the applied biasgain) ....

	CProfileBiasGain DefaultBiasGain;								// default profile
	CProfileBiasGain DiagramMapper = TranspFill->GetProfile ();		// applied profile

	INT32 Tiling = ((FillMappingAttribute*)CurrentAttrs[ATTR_TRANSPFILLMAPPING].pAttr)->Repeat;

	// make some optimisation decisions ....
	
	if (DiagramMapper == DefaultBiasGain)			// applied profile is the default profile
	{
		if (!SpecialFill)
		{
			TransparencyRamp *pTranspRamp = TranspFill->GetTranspRamp();

			if (Tiling != 4)
			{
				pTranspTable->BuildTable(StartTransp, EndTransp, pTranspRamp);
			}
			else
			{
				pTranspTable->BuildHighQualityRepeatTable(StartTransp, EndTransp, pTranspRamp);
			}
		}
	}
	else
	{
		if (!SpecialFill)
		{
			if (Tiling != 4)
			{
				pTranspTable->BuildTable(StartTransp, EndTransp, DiagramMapper);
			}
			else
			{
				pTranspTable->BuildHighQualityRepeatTable(StartTransp, EndTransp, DiagramMapper);
			}
		}
	}

//	if (IsUserName("Will")) TRACE( _T("Grad Transparency, Start=%d, End=%d\n"),StartTransp,EndTransp);

	// Find the control points of the Transparency Fill
	POINT TA,TB,TC,TD;

	DocCoord* StartPoint 	= TranspFill->GetStartPoint();
	DocCoord* EndPoint  	= TranspFill->GetEndPoint();
	DocCoord* EndPoint2 	= TranspFill->GetEndPoint2();
	DocCoord* EndPoint3 	= TranspFill->GetEndPoint3();

	if (StartPoint != NULL)
    {
    	TA.x = (*StartPoint).x;	
    	TA.y = (*StartPoint).y;
	}

	if (EndPoint != NULL)
    {
    	TB.x = (*EndPoint).x;	
    	TB.y = (*EndPoint).y;
	}

	if (EndPoint2 != NULL)
    {
    	TC.x = (*EndPoint2).x;	
    	TC.y = (*EndPoint2).y;
	}

	// Convert the Fill Type into a Gavin Fill Type
	if (FillType == CC_RUNTIME_CLASS(RadialTranspFillAttribute))
	{
		TransStyle = 1;		// Elliptical Transparency
	}
	else if (FillType == CC_RUNTIME_CLASS(LinearTranspFillAttribute))
	{
		TransStyle = 0;		// Linear Transparency

		TA.x = (*StartPoint).x;	TA.y = (*StartPoint).y;
    	TB.x = (*EndPoint2).x;	TB.y = (*EndPoint2).y;
		TC.x = (*EndPoint).x;	TC.y = (*EndPoint).y;
	}
	else if (FillType == CC_RUNTIME_CLASS(ConicalTranspFillAttribute))
	{
		TransStyle = 2;		// Conical Transparency

		TB.x = TA.x - (TB.x - TA.x);
		TB.y = TA.y - (TB.y - TA.y);
	}
	else if (FillType == CC_RUNTIME_CLASS(SquareTranspFillAttribute))
	{
		TransStyle = 3;		// Square Transparency
	}


	BOOL Perspective = FALSE;

	// Is it a perspectivised fill ?
	if (TranspFill->IsPerspective())
	{
		Perspective = TRUE;

		POINT PGram[4];

		// Setup the Perspective control points
		if (FillType==CC_RUNTIME_CLASS(LinearTranspFillAttribute))
		{
			PGram[0].x = (*StartPoint).x;	PGram[0].y = (*StartPoint).y;
			PGram[1].x = (*EndPoint2).x;	PGram[1].y = (*EndPoint2).y;
			PGram[2].x = (*EndPoint3).x;	PGram[2].y = (*EndPoint3).y;
			PGram[3].x = (*EndPoint).x;		PGram[3].y = (*EndPoint).y;
		}
		else
		{
			PGram[0].x = TA.x;				PGram[0].y = TA.y;
			PGram[1].x = TB.x;				PGram[1].y = TB.y;
			PGram[2].x = (*EndPoint3).x;	PGram[2].y = (*EndPoint3).y;
			PGram[3].x = (*EndPoint2).x;	PGram[3].y = (*EndPoint2).y;
		}

		// Check the mould is valid
		// Gerry removed the pixel size test cos it makes small repeating fills go spappy
		if (MouldPerspective::WillBeValid(PGram))
		{
			// We're going to plot a perspective fill,
			// so copy the perspective control points
			TA.x = PGram[0].x;	TA.y = PGram[0].y;
			TB.x = PGram[1].x;	TB.y = PGram[1].y;
			TC.x = PGram[2].x;	TC.y = PGram[2].y;
			TD.x = PGram[3].x;	TD.y = PGram[3].y;
		}
		else 
		{
			// The mould is bad...
			Perspective = FALSE;
		}
	}

	if (SpecialFill)
	{
		// The fill is a fabby new one

		if ((FillType != CC_RUNTIME_CLASS(ThreeColTranspFillAttribute)) &&
			(FillType != CC_RUNTIME_CLASS(FourColTranspFillAttribute)))
		{
			ERROR3("Unknown new fill type");
			return FALSE;								// unknown fill type
		}

		BYTE StartTransp = *TranspFill->GetStartTransp();
		BYTE EndTransp = *TranspFill->GetEndTransp();
		BYTE EndTransp2 = *TranspFill->GetEndTransp2();

		BYTE EndTransp3 = 255;
		if (TranspFill->GetEndTransp3() != NULL)
    		EndTransp3 = *TranspFill->GetEndTransp3();

		DWORD OurStyle = 0;

		if (FillType == CC_RUNTIME_CLASS(ThreeColTranspFillAttribute))
		{			
			// Style & 0xFF = 0 (simple with better colours outside the mesh)
			// Style & 0xFF = 2 (tiled)

			if (Tiling == 1)
				OurStyle |= 0;
			else
				OurStyle |= 2;

			if (Perspective)
				GetDrawContext()->SetTransparent3WayGraduation4(OurStyle, StartTransp, EndTransp, EndTransp2,
													&TA, &TB, &TC, &TD);
			else
				GetDrawContext()->SetTransparent3WayGraduation(OurStyle, StartTransp, EndTransp, EndTransp2,
													&TA, &TB, &TC);
		}
		else
		{
			// Style & 0xFF = 1 (for better colours outside the mesh)

			if (Tiling == 1)
				OurStyle |= 1;
			else
				OurStyle |= 2;

			// The transparency values in the following lines are correct!!!
			if (Perspective)
				GetDrawContext()->SetTransparent4WayGraduation4(OurStyle, StartTransp, EndTransp, EndTransp3, EndTransp2,
													&TA, &TB, &TC, &TD);
			else
				GetDrawContext()->SetTransparent4WayGraduation(OurStyle, StartTransp, EndTransp, EndTransp3, EndTransp2,
													&TA, &TB, &TC);
		}
	}
	else
	{
//Mark Howitt, 14/10/97. If we have selected the new transparent repeating grad fills, set repeating.
#ifdef NEW_FEATURES
		if(Tiling == 4)
			TransStyle |= 0x10000;
#endif	
// Set the Current Transparency to be a graduated Transparency
		if (Perspective)
			GetDrawContext()->SetTransparentGraduation4( TransStyle, pTranspTable, &TA, &TB, &TC, &TD );
		else
			GetDrawContext()->SetTransparentGraduation( TransStyle, pTranspTable, &TA, &TB, &TC );
	}

	// Set the fill style to be a graduated transparency
	// MarkH 2/8/99 - Check to see which type we`re using and alter the style accordingly!
	*Style |= (MapTranspTypeToGDraw(tType, TRUE)<<8);
	return TRUE;
}

/********************************************************************************************

>	BOOL GRenderRegion::SetBitmapTransparencyFill(TranspFillAttribute* Fill, DWORD* Style,
											  TranspGradTable* pTranspTable)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/94
	Returns:	TRUE, if transparency was set ok.
				FALSE, if failed to set the bitmap transparency.
	Purpose:	Sets up a bitmap transparency fill.
	Scope:		Protected

********************************************************************************************/

BOOL GRenderRegion::SetBitmapTransparencyFill(TranspFillAttribute* Fill, DWORD* Style,
											  TranspGradTable* pTranspTable)
{
	if (Fill->GetBitmap()->ActualBitmap == NULL)
		return FALSE;									// if no bitmap

	ENSURE( Fill->GetBitmap()->ActualBitmap->IsKindOf( CC_RUNTIME_CLASS( CWxBitmap ) ),
																"Strange bitmapfill");

	UINT32 bpp = Fill->GetBitmap()->GetBPP();
	CWxBitmap *WinBM = (CWxBitmap*)Fill->GetBitmap()->ActualBitmap;

/*	CWxBitmap *WinBM = NULL;

	BYTE *pGreyTable = NULL;
	if ( bpp<=8 )
	{
		WinBM = (CWxBitmap*)Fill->GetBitmap()->GetGreyscaleVersion();
		if (WinBM)
			bpp = WinBM->GetBPP();
		else
		{
			// Aha, we're gunna do some clever palette jiggery pokery
			pGreyTable = Fill->GetBitmap()->ActualBitmap->GetGreyscaleTable();
		}
	}

	if (WinBM == NULL)
	{
		// Use the normal bitmap
		WinBM = (CWxBitmap*)Fill->GetBitmap()->ActualBitmap;
	}
*/
	if (WinBM == NULL || WinBM->BMInfo == NULL  || WinBM->BMBytes == NULL)
		return FALSE;

	INT32 StartTransp = 0;
	INT32 EndTransp = 0;
	if (Fill->GetStartTransp() != NULL)
    	StartTransp = *Fill->GetStartTransp();
	if (Fill->GetEndTransp() != NULL)
    	EndTransp = *Fill->GetEndTransp();
/*
	TransparencyRamp *pTranspRamp=Fill->GetTranspRamp();
	// Note:  BuildBitmapTable is optimised internally depending upon the profile ....
	pTranspTable->BuildBitmapTable(StartTransp, EndTransp, pTranspRamp, Fill->GetProfile ());

	BYTE* pRamp = (LPBYTE)(pTranspTable->GetTable());
	// Now see if we need to muck around with the palette for the contoning
	BYTE aRamp[0x100];
	if ( pGreyTable )
	{
		// Copy the entries from the old ramp into the new one,
		// using the Grey table as a guide
		for (INT32 i=0; i<1<<bpp; i++)
			aRamp[i] = pRamp[pGreyTable[i]];
		pRamp = aRamp;
	}
*/

	BYTE aRamp[0x100];
	BYTE* pRamp = NULL;
	if ( bpp<=8 && !Fill->IsKindOf(CC_RUNTIME_CLASS(FractalTranspFillAttribute)) &&
				   !Fill->IsKindOf(CC_RUNTIME_CLASS(  NoiseTranspFillAttribute)) )
	{
		RGBQUAD* pPalette = WinBM->BMInfo->bmiColors;
		if ( pPalette )
		{
			for (INT32 i=0; i<1<<bpp; i++)
				aRamp[i] = (pPalette[i].rgbRed  *0x4D4D4D+
							pPalette[i].rgbGreen*0x979797+
							pPalette[i].rgbBlue *0x1C1C1C) >> 24;
			pRamp = aRamp;
		}
	}

	// convert ArtWorks style attribute into Gavin-style
	POINT PGram[4];
	PGram[0].x = (*Fill->GetStartPoint()).x; 
	PGram[0].y = (*Fill->GetStartPoint()).y;
	PGram[1].x = (*Fill->GetEndPoint  ()).x;
	PGram[1].y = (*Fill->GetEndPoint  ()).y;

	BOOL Perspective = Fill->IsPerspective();
	if ( Perspective )
	{
		// Gerry removed the pixel size test cos it makes small repeating fills go spappy
		PGram[2].x = (*Fill->GetEndPoint3()).x;
		PGram[2].y = (*Fill->GetEndPoint3()).y;
		PGram[3].x = (*Fill->GetEndPoint2()).x;
		PGram[3].y = (*Fill->GetEndPoint2()).y;

		if ( !MouldPerspective::WillBeValid(PGram) )
		{
			Perspective = FALSE;
			TRACEUSER( "Will", _T("Bitmap Perspective points are Invalid !!\n"));
		}
	}
	if ( !Perspective )
	{
		PGram[2].x = (*Fill->GetEndPoint2()).x;
		PGram[2].y = (*Fill->GetEndPoint2()).y;
	}		

	// get the colour we use to fill around the edges from the current solid fill colour
	COLORREF DefaultColour;

//	DefaultColour = ConvertColourToTransScreenWord(CurrentColContext, &RR_FILLCOLOUR() );
	DefaultColour = 0xFFFFFFFF;

	UINT32 tType = Fill->GetTranspType();
	DWORD TranspStyle = tType<<8;

//	INT32 Tiling = Fill->GetTesselation();
	INT32 Tiling = 
		((TranspFillMappingAttribute*)CurrentAttrs[ATTR_TRANSPFILLMAPPING].pAttr)->Repeat;
	TranspStyle |= Tiling;

	// MarkH 2/8/99 - Check to see which type we`re using and alter the style accordingly!
	*Style |= (MapTranspTypeToGDraw(tType, TRUE)<<8);

	// If the bitmap has an alpha channel, then use this for the transparency channel.
	if ( bpp==32 && Fill->GetBitmap()->IsTransparent() )
		TranspStyle |= 0x8000;

	GetDrawContext()->SetBias(3,Fill->GetProfile().GetBias()) ;
	GetDrawContext()->SetGain(3,Fill->GetProfile().GetGain()) ;
	GetDrawContext()->SetOutputRange(3,StartTransp,EndTransp) ;

	//---------------------------------------------------------------------------------------
	// Smoothing decisions
	//---------------------------------------------------------------------------------------

	SetSmoothingFlags(Fill);

	if (Perspective)	// Is a Perspective applied ?
	{
		GetDrawContext()->SetPerspectiveTransparentBitmapFill(
							&(WinBM->BMInfo->bmiHeader),
							WinBM->BMBytes,
							TranspStyle,
							PGram,
							DefaultColour,
							pRamp,
							0
							);
	}
	else
	{
		GetDrawContext()->SetTransparentBitmapFill(
							&(WinBM->BMInfo->bmiHeader),
							WinBM->BMBytes,
							TranspStyle,
							PGram,
							DefaultColour,
							pRamp,
							0
							);
	}

	// Reset smoothing flags
	SetSmoothingFlags(NULL);

	return TRUE;
}

/********************************************************************************************

>	BOOL GRenderRegion::SetFractalFill(ColourFillAttribute* Fill, DWORD* Style)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/94
	Returns:	TRUE, if fill  was set ok.
				FALSE, if failed to set the fill.
	Purpose:	Sets up a fractal fill.
	Scope:		Protected

********************************************************************************************/

BOOL GRenderRegion::SetFractalFill(ColourFillAttribute* Fill, DWORD* Style)
{
	return FALSE;
}

/********************************************************************************************

>	BOOL GRenderRegion::SetFractalTransparencyFill(TranspFillAttribute* Fill, DWORD* Style)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/94
	Returns:	TRUE, if transparency was set ok.
				FALSE, if failed to set the fractal transparency.
	Purpose:	Sets up a fractal transparency fill.
	Scope:		Protected

********************************************************************************************/

BOOL GRenderRegion::SetFractalTransparencyFill(TranspFillAttribute* Fill, DWORD* Style)
{
/*
	BeginBusyCursor();

	FractalTranspFillAttribute* FractFill = (FractalTranspFillAttribute*)Fill;

	// First make a fractal from the variables stored in the fill.
	PlasmaFractalFill TheFractal(FractFill->Seed, TRUE, 0, 
									FractFill->Graininess, FractFill->Gravity);

	// Get the fill control points
	DocCoord Start = *Fill->GetStartPoint();
	DocCoord End   = *Fill->GetEndPoint();
	DocCoord End2  = *Fill->GetEndPoint2();

	//if (IsUserName("Will")) TRACE( _T("Start = %d,%d\n"),Start.x, Start.y);
	//if (IsUserName("Will")) TRACE( _T("End   = %d,%d\n"),End.x, End.y);
	//if (IsUserName("Will")) TRACE( _T("End2  = %d,%d\n"),End2.x, End2.y);

	// Find the size of a pixel.  Note: This assumes pixels are square at the moment.
	MILLIPOINT PixelWidth = GetScaledPixelWidth();

	// Calculate the size of the fractal (in Pixels) from the control points
   	INT32 FracPixWidth 	= INT32(Start.Distance(End) * 2 / PixelWidth);
   	INT32 FracPixHeight 	= INT32(Start.Distance(End2)* 2 / PixelWidth);

	// The 'actual' dimensions of a fractal must be square and a power of 2.
	// 'GetDimension' finds an appropriate value for this dimension.
	// It will be the next power of 2 higher than Width or Height (whichever is biggest).
	INT32 FracPixDim 	= TheFractal.GetDimension(FracPixWidth, FracPixHeight);

	//if (IsUserName("Will")) TRACE( _T("Fractal Pixel Width = %d, Height = %d\n"),FracPixWidth,FracPixHeight);
	//if (IsUserName("Will")) TRACE( _T("Fractal Pixel Dim   = %d\n"),FracPixDim);

	// Because the 'actual'	fractal is always square we have to scale it to fit into
	// our no-square render region.
	double FracXScale = double(FracPixDim) / double(FracPixWidth);
	double FracYScale = double(FracPixDim) / double(FracPixHeight);
	//if (IsUserName("Will")) TRACE( _T("X Scale = %f\n"),FracXScale);
	//if (IsUserName("Will")) TRACE( _T("Y Scale = %f\n"),FracYScale);

	// Get the 'display' fractal size in document units (MILLIPOINTS)
   	INT32 FracDocWidth  = FracPixWidth  * PixelWidth;
   	INT32 FracDocHeight = FracPixHeight * PixelWidth;
	// and find the origin.
	DocCoord FractalOrigin = DocCoord(Start.x - FracDocWidth/2, Start.y - FracDocHeight/2);

	//if (IsUserName("Will")) TRACE( _T("Fract Origin = %d,%d\n"),FractalOrigin.x, FractalOrigin.y);
	//if (IsUserName("Will")) TRACE( _T("Fract Width  = %d\n"),FracDocWidth);
	//if (IsUserName("Will")) TRACE( _T("Fract Height = %d\n"),FracDocHeight);

	// Make a rectangle describing the unscaled fractal bounds.
	DocRect FractRect = DocRect(FractalOrigin, FractalOrigin);
	FractRect.IncludePoint(DocCoord(FractalOrigin.x + FracDocWidth, 
									FractalOrigin.y + FracDocHeight));

	//if (IsUserName("Will")) TRACE( _T("Fractal Rect = %d,%d, %d,%d\n"),FractRect.lox, FractRect.loy,
	//																FractRect.hix, FractRect.hiy);

	// Get the bounds of the Render Region.
	DocRect RRRect = GetClipRect();

	//if (IsUserName("Will")) TRACE( _T("RR Rect      = %d,%d, %d,%d\n"),RRRect.lox, RRRect.loy,
	//																RRRect.hix, RRRect.hiy);

	// Clip the render region rect to the fractal rect.
	DocRect RRClipRect;
	if (RRRect.IsIntersectedWith(FractRect))
		RRClipRect = RRRect.Intersection(FractRect);
	else
		RRClipRect = DocRect(0,0,0,0);

	//if (IsUserName("Will")) TRACE( _T("RR Clip Rect = %d,%d, %d,%d\n"),RRClipRect.lox, RRClipRect.loy,
	//																RRClipRect.hix, RRClipRect.hiy);

	// Find the offset of the clipped render region rect from the fractal origin.
	INT32 dx = (RRClipRect.lox - FractalOrigin.x) / PixelWidth;
	INT32 dy = (RRClipRect.loy - FractalOrigin.y) / PixelWidth;

	// Find the size of the bitmap, by scaling the clipped render region rect.
	INT32 BitmapPixWidth		= INT32((RRClipRect.Width()  / PixelWidth) * FracXScale);
	INT32 BitmapPixHeight	= INT32((RRClipRect.Height() / PixelWidth) * FracYScale);

	//if (IsUserName("Will")) TRACE( _T("Bitmap Width  = %d\n"),BitmapPixWidth);
	//if (IsUserName("Will")) TRACE( _T("Bitmap Height = %d\n"),BitmapPixHeight);
	//if (IsUserName("Will")) TRACE( _T("Bitmap Origin = %d,%d\n"),dx,dy);

	// The width and height to actually plot the bitmap.
	INT32 BitmapPlotWidth 	= (RRClipRect.Width());
	INT32 BitmapPlotHeight 	= (RRClipRect.Height());

	// Calculate the Dpi of the bitmap.
	INT32 Dpi = 72000/PixelWidth;
	//if (IsUserName("Will")) TRACE( _T("Dpi = %d\n"),Dpi);

	// Create the Bitmap !!
	FractalBitmap = new KernelBitmap(BitmapPixWidth, BitmapPixHeight, 8, Dpi);

	CWxBitmap *WinBM = (CWxBitmap*)FractalBitmap->ActualBitmap;

	// Bodge !!
	// Initialise the Bitmap so that areas that aren't plotted into come out white

	// Render the Fractal into the bitmap
 	TheFractal.DoFill(FractalBitmap, FracPixDim, INT32(dx*FracXScale), INT32(dy*FracYScale));

	// convert ArtWorks style attribute into Gavin-style
	POINT PGram[3];

	PGram[0].x = RRClipRect.lox;					PGram[0].y = RRClipRect.loy;
	PGram[1].x = RRClipRect.lox + BitmapPlotWidth;	PGram[1].y = RRClipRect.loy;
	PGram[2].x = RRClipRect.lox;					PGram[2].y = RRClipRect.loy + BitmapPlotHeight;

	// get the colour we use to fill around the edges from the current solid fill colour
	COLORREF DefaultColour;

	DefaultColour = ConvertColourToTransScreenWord(CurrentColContext, &RR_FILLCOLOUR() );

	DWORD TranspStyle = 0;

	UINT32 TranspType = Fill->GetTranspType();

	TranspStyle  = (TranspType)<<8;
//	TranspStyle |= WhichTiling;
	TranspStyle |= 2;

	*Style |= (TranspType + 6)<<8;

	// and finally ... plot the bitmat
	BOOL Result = GetDrawContext()->SetTransparentBitmapFill(
							&(WinBM->BMInfo->bmiHeader),
							WinBM->BMBytes,
							TranspStyle,
							PGram,
							DefaultColour,
							(LPBYTE*)pTranspTable,
							0
							);

	EndBusyCursor();
*/
	return TRUE;
}




/********************************************************************************************

>	void GRenderRegion::DrawRect(DocRect *RectToRender)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/93
	Inputs:		RectToRender is a pointer to a rectangle to render
	Outputs:	-
	Returns:	-
	Purpose:	Renders a rectangle using gavins routines. Rectangles are not anti-aliased.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void GRenderRegion::DrawRect(DocRect *RectToRender)
{
	// If we are not drawing complex shapes and this shape is, then return
	if ((!RenderComplexShapes) && (TestForComplexShape(&Caps)))
		return;

	// If the rect given does not intersect the clip rect, then do not bother
	// Draw rect is only really used to draw the paper etc, so this check is not
	// generally performed in situ, which is why I do it here
	if (!RectToRender->IsIntersectedWith(CurrentClipRect))
		return;

	// OK, it intersects, but we should try to clip it with the inner rect
	DocRect DocDrawRect = RectToRender->Intersection(InnerRect);
	RECT	DrawRect;
	DrawRect.left	= DocDrawRect.lo.x;
	DrawRect.right	= DocDrawRect.hi.x;
	DrawRect.top	= DocDrawRect.hi.y;
	DrawRect.bottom = DocDrawRect.lo.y;

	RememberMe("DrawRect");
	HaveRenderedSomething = TRUE;

	// Render the rectangle
	if (SetFillGCol(RR_FILLCOLOUR()))
		GetDrawContext()->FillRectangle(&DrawRect);

	// And maybe stroke the outline of the rect if we need to
	if (SetGCol(RR_STROKECOLOUR()))
	{
		// build up a path so we can Fuzzy Clip and render it
		static BYTE  Verbs[4] = {PT_MOVETO, PT_LINETO, PT_LINETO, PT_LINETO | PT_CLOSEFIGURE};
		DocCoord Coords[4];

		// Fill in the coords to be going on with
		Coords[0].x = DocDrawRect.lo.x;
		Coords[0].y = DocDrawRect.lo.y;

		Coords[1].x = DocDrawRect.lo.x;
		Coords[1].y = DocDrawRect.hi.y;

		Coords[2].x = DocDrawRect.hi.x;
		Coords[2].y = DocDrawRect.hi.y;

		Coords[3].x = DocDrawRect.hi.x;
		Coords[3].y = DocDrawRect.lo.y;

		// draw a line around it
		StrokePath(Coords, Verbs, 4,
				   Coords, Verbs, 4, FALSE);
	}
}

/********************************************************************************************

>	void GRenderRegion::DrawDragRect(DocRect *RectToRender)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/93
	Inputs:		RectToRender is a pointer to a rectangle to render in EOR type mode.
	Outputs:	-
	Returns:	-
	Purpose:	Does NOTHING AT ALL as gdraw cannot do xor plotting
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void GRenderRegion::DrawDragRect(DocRect *)
{
	ENSURE(FALSE, "Gdraw cannot do DrawDragRect");
}

/********************************************************************************************

>	void GRenderRegion::DrawBlob(DocCoord p, BlobType type)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/93
	Inputs:		A point and a blob type
	Outputs:	-
	Returns:	-
	Purpose:	Does NOTHING AT ALL as gdraw cannot do xor plotting
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void GRenderRegion::DrawBlob(DocCoord p, BlobType type)
{
	// class GRenderClickColour needs to render the paper (for custom colour picker control)
	// BUT we also need to avoid firing off the ENSURE !!!!
	
	if (this->IsKindOf (CC_RUNTIME_CLASS (GRenderClick/*Colour*/)))
	{
		return;
	}

	ENSURE( FALSE, "GDraw cannot do blobs");

	DocRect MyBlob;

	OSRenderRegion::GetBlobRect( ScaleFactor, p, type, &MyBlob);
	DrawRect( &MyBlob );
}

/********************************************************************************************

>	void GRenderRegion::DrawLine(const DocCoord &StartPoint, const DocCoord &EndPoint)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/93
	Inputs:		Two points to render a line between
	Outputs:	-
	Returns:	-
	Purpose:	Renders a line using gdraw.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void GRenderRegion::DrawLine(const DocCoord &StartPoint, const DocCoord &EndPoint)
{
	// If we are not drawing complex shapes and this shape is, then return
	if ((!RenderComplexShapes) && (TestForComplexShape(&Caps)))
		return;

	RememberMe("DrawLine");
	HaveRenderedSomething = TRUE;

	// Set the line colour
	if (!SetGCol( RR_STROKECOLOUR() ))
		return;

	// Build a path for gavin to render
	static PathVerb Verbs[2] = {PT_MOVETO, PT_LINETO};
	DocCoord Coords[2];
	DocCoord ClippedCoords[2];
	PathVerb ClippedVerbs[2];

	Coords[0] = StartPoint;
	Coords[1] = EndPoint;

	// Actually clip the path to the rectangles
	size_t Worked = FuzzyClip( (PPOINT)Coords, Verbs, 2, FALSE, (RECT*)(&InnerRect), (RECT*)(&OuterRect),
							(PPOINT)(ClippedCoords), ClippedVerbs, 2);

	// Render it
	if (Worked==(size_t)-1)
	{
		StrokePath( Coords, Verbs, 2,
		 			Coords, Verbs, 2, TRUE);
	}
	else
	{
		StrokePath( ClippedCoords, ClippedVerbs, (UINT32)Worked, 
					Coords, Verbs, 2, TRUE);
	}
}

/********************************************************************************************

>	void GRenderRegion::DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Inputs:		Point - The Coord specifying the bottom left of the Bitmap.
				pBitmap, the KernelBitmap to plot.
	Purpose:	Draws an un-scaled Bitmap.
				Use this for drawing Icons for dialogue boxes etc.
				The Bitmap can be any size, but will be plotted unscaled.
				Note that 'Point' specifies where the bottom left of the bitmap will be.

********************************************************************************************/

void GRenderRegion::DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap)
{
	if (pBitmap == NULL)
		return;

	ENSURE( pBitmap->ActualBitmap->IsKindOf( CC_RUNTIME_CLASS( CWxBitmap ) ),
																"Strange bitmapfill");
	CWxBitmap *WinBM = (CWxBitmap*)pBitmap->ActualBitmap;

	// Check the Bitmap has some data in it
	if (
		(WinBM->BMInfo==NULL) ||
		(WinBM->BMBytes==NULL)
	   )
		return;

	INT32 PixelSize = CalcPixelWidth();

	BitmapInfo Info;
	BOOL GotInfo = pBitmap->ActualBitmap->GetInfo( &Info );

	// Get the Bitmap width and height in Millipoints
	INT32 Width  = GotInfo ? Info.PixelWidth  * PixelSize : 16 * PixelSize;
	INT32 Height = GotInfo ? Info.PixelHeight * PixelSize : 16 * PixelSize;
		
	// Setup the Gavin control points
	POINT PGram[3];
	PGram[0].x = Point.x;				PGram[0].y = Point.y;
	PGram[1].x = Point.x + Width;		PGram[1].y = Point.y;
	PGram[2].x = Point.x;				PGram[2].y = Point.y + Height;


	// --- Colour-correct and/or Contone the bitmap as necessary
	RGBQUAD *Palette = NULL;

	// Do the colour correction. This may produce a new pointer in Palette,
	// which we should CCFree() when we are done with it - see the end of this function
	ColourCorrectBitmap(NULL, WinBM->BMInfo, &Palette);

	// If it didn't create a new palette for us, then we'll use the original one
	if (Palette == NULL)
		Palette = WinBM->BMInfo->bmiColors;

	DWORD Style = 1;		// GDraw bitmap-plotting style flagword
	BOOL bClearBitmapConv = FALSE;

	// --- Add Separation Style bits as approriate to the current colour separation mode
	// Currently, the only "separation" option we use is composite print preview
	// SepStyle for the SetBitmapFill functions are always in the MS byte of Style

	if (WinBM->GetBPP() > 8)	// Only needed for deep bitmaps
	{
		// If we've got a valid colour plate and it is a composite preview
		if (CurrentColContext->GetColourPlate() != NULL &&
			!CurrentColContext->GetColourPlate()->IsDisabled() &&
			CurrentColContext->GetColourPlate()->GetType() == COLOURPLATE_COMPOSITE)
		{
			// Find the colour manager (if there is one), and ask it for the 15-bit RGB
			// printer colour matching lookup table
PORTNOTE("cms", "DisabledXaraCMS")
#ifndef EXCLUDE_FROM_XARALX
			XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();
			if (lpCMSMan != NULL)
			{
				DWORD *MatchTable = lpCMSMan->GetPaperViewTable();
				ERROR3IF(MatchTable == NULL, "Can't create/find colour matching table");
				if (MatchTable != NULL)
				{
					GetDrawContext()->SetBitmapConversionTable((BGR *)MatchTable);
					bClearBitmapConv = TRUE;
					// Bits 3 and 4 of top byte of Style specifies the conversion algorithm
					// Make sure it only affects the 2 bits
					Style |= (BitmapConversion & 3) << 27;
				}
			}
#endif
		}
	}

	BYTE *BitmapBits = WinBM->BMBytes;

	// Plot the Bitmap
	GetDrawContext()->PlotBitmap(	&(WinBM->BMInfo->bmiHeader),
					BitmapBits,
					Style,
					PGram,
					Palette,
					NULL, NULL, NULL,
					NoTransTable
					);

	// Free any memory used for colour-corrected bitmap palettes.
	// If this pointer doesn't point at the original palette, then it has
	// been temporarily allocated by ColourCorrectBitmap, above.
	if (Palette != WinBM->BMInfo->bmiColors)
		CCFree(Palette);

	if (bClearBitmapConv)
	{
		GetDrawContext()->SetBitmapConversionTable(NULL);
	}
}



/********************************************************************************************

>	void GRenderRegion::DrawBitmap(const DocRect& rect, KernelBitmap* pBitmap)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Inputs:		Point - The Coord specifying the bottom left of the Bitmap.
				pBitmap, the KernelBitmap to plot.
	Purpose:	Draws an un-scaled Bitmap.
				Use this for drawing Icons for dialogue boxes etc.
				The Bitmap can be any size, but will be plotted unscaled.
				Note that 'Point' specifies where the bottom left of the bitmap will be.

********************************************************************************************/

void GRenderRegion::DrawBitmap(const DocRect& rect, KernelBitmap* pBitmap)
{
	if (pBitmap == NULL)
		return;

	ENSURE( pBitmap->ActualBitmap->IsKindOf( CC_RUNTIME_CLASS( CWxBitmap ) ),
																"Strange bitmapfill");
	CWxBitmap *WinBM = (CWxBitmap*)pBitmap->ActualBitmap;

	// Check the Bitmap has some data in it
	if (
		(WinBM->BMInfo==NULL) ||
		(WinBM->BMBytes==NULL)
	   )
		return;

//	INT32 PixelSize = CalcPixelWidth();

	// Setup the Gavin control points
	POINT PGram[3];
	PGram[0].x = rect.lo.x;	PGram[0].y = rect.lo.y;
	PGram[1].x = rect.hi.x;	PGram[1].y = rect.lo.y;
	PGram[2].x = rect.lo.x;	PGram[2].y = rect.hi.y;


	// --- Colour-correct and/or Contone the bitmap as necessary
	RGBQUAD *Palette = NULL;

	// Do the colour correction. This may produce a new pointer in Palette,
	// which we should CCFree() when we are done with it - see the end of this function
	ColourCorrectBitmap(NULL, WinBM->BMInfo, &Palette);

	// If it didn't create a new palette for us, then we'll use the original one
	if (Palette == NULL)
		Palette = WinBM->BMInfo->bmiColors;

	DWORD Style = 1;		// GDraw bitmap-plotting style flagword
	BOOL bClearBitmapConv = FALSE;

	// --- Add Separation Style bits as approriate to the current colour separation mode
	// Currently, the only "separation" option we use is composite print preview
	// SepStyle for the SetBitmapFill functions are always in the MS byte of Style
	if (WinBM->GetBPP() > 8)	// Only needed for deep bitmaps
	{
		// If we've got a valid colour plate and it is a composite preview
		if (CurrentColContext->GetColourPlate() != NULL &&
			!CurrentColContext->GetColourPlate()->IsDisabled() &&
			CurrentColContext->GetColourPlate()->GetType() == COLOURPLATE_COMPOSITE)
		{
			// Find the colour manager (if there is one), and ask it for the 15-bit RGB
			// printer colour matching lookup table
PORTNOTE("cms", "DisabledXaraCMS")
#ifndef EXCLUDE_FROM_XARALX
			XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();
			if (lpCMSMan != NULL)
			{
				DWORD *MatchTable = lpCMSMan->GetPaperViewTable();
				ERROR3IF(MatchTable == NULL, "Can't create/find colour matching table");
				if (MatchTable != NULL)
				{
					GetDrawContext()->SetBitmapConversionTable((BGR *)MatchTable);
					bClearBitmapConv = TRUE;
					// Bits 3 and 4 of top byte of Style specifies the conversion algorithm
					// Make sure it only affects the 2 bits
					Style |= (BitmapConversion & 3) << 27;
				}
			}
#endif
		}
	}
	BYTE *BitmapBits = WinBM->BMBytes;

	// Plot the Bitmap
	GetDrawContext()->PlotBitmap(	&(WinBM->BMInfo->bmiHeader),
					BitmapBits,
					Style,
					PGram,
					Palette,
					NULL, NULL, NULL,
					NoTransTable
					);

	// Free any memory used for colour-corrected bitmap palettes.
	// If this pointer doesn't point at the original palette, then it has
	// been temporarily allocated by ColourCorrectBitmap, above.
	if (Palette != WinBM->BMInfo->bmiColors)
		CCFree(Palette);

	if (bClearBitmapConv)
	{
		GetDrawContext()->SetBitmapConversionTable(NULL);
	}
}



/********************************************************************************************

>	void GRenderRegion::DrawBitmap(const DocCoord &Point, UINT32 BitmapID, UINT32 ToolID = NULL)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Inputs:		Point - The Coord specifying the bottom left of the Bitmap.
				BitmapID, the resource ID of the bitmap to plot.
   				ToolID, the optional ToolID of the tool containing the Bitmap resource.
	Purpose:	Draws an un-scaled Bitmap.
				Use this for drawing Icons for dialogue boxes etc.
				The Bitmap can be any size, but will be plotted unscaled.
				Note that 'Point' specifies where the bottom left of the bitmap will be.

********************************************************************************************/

void GRenderRegion::DrawBitmap(const DocCoord &Point, UINT32 BitmapID, UINT32 ToolID)
{
	ERROR3IF( 0 != ToolID, "GRenderRegion::DrawBitmap can't cope with Tool resources yet" );

	KernelBitmap* pBitmap;

	// Make a new Oil Bitmap
	OILBitmap* pOilBmp = OILBitmap::Create();
	if (pOilBmp == NULL)
		return;

	// and copy the data from the resources
	pOilBmp->LoadBitmap(BitmapID);

	// Make a KernelBitmap and attach the Oil Bitmap
	pBitmap = new KernelBitmap(pOilBmp, TRUE);	 	// TRUE = temp bitmap
	if (pBitmap == NULL)
		return;

	// Plot the Bitmap
	DrawBitmap(Point, pBitmap);

	// Delete it (will also delete the Oil Bitmap)
	delete pBitmap;
}


/********************************************************************************************

>	virtual void GRenderRegion::DrawFixedSystemText(StringBase *TheText,
														DocRect &BoundsRect)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/4/95
	Inputs:		TheText - The string to render
				BoundsRect - Determines where the text is to be plotted. The text will also
				be clipped within this bounding rectangle if it exceeds the available space.

	Purpose:	To draw simple text, using the default host-operating-system font.
				The size and style of this font are decided by the host OS (or oil code)
				and cannot be set in any way. To determine how much space is needed to
				display a string with this method, see the SeeAlso.

	Notes:		This method is intended solely for the use of kernel-rendered dialogues,
				such as SuperGalleries, which need to display simple text information. It
				should not be confused with document rendering of arbitrary text paths etc.

				Currently, the text is drawn in a default manner (left justified and centered
				vertically; one line of text only (no word-wrap onto subsequent lines), etc)
				Do not use special characters such as tab/newline -their effect is undefined

				NOTE: this GRenderRegion version of the function plots with BkMode OPAQUE.

	SeeAlso:	GRenderRegion::GetFixedSystemTextSize;
				GRenderRegion::SetFixedSystemTextColours

********************************************************************************************/

void GRenderRegion::DrawFixedSystemText(StringBase *TheText, DocRect &BoundsRect, UINT32 uFormat)
{
	wxString Text = (wxString)(TCHAR *)(*TheText);

	wxFont SaveFont=RenderDC->GetFont();

	wxFont FixedFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	FixedFont.SetPointSize(8);
	RenderDC->SetFont(FixedFont);

	wxDC * pDC = RenderDC;
	wxSize DPI = OSRenderRegion::GetFixedDCPPI(*pDC);
//	INT32 XDPI = DPI.GetWidth();
//	INT32 YDPI = DPI.GetHeight();
//	INT32 LineHeight = RenderDC->GetCharHeight();

	wxCoord w, h;
	RenderDC->GetTextExtent(Text, &w, &h);

	RenderDC->SetFont(SaveFont);

	INT32 PixelWidth = CalcScaledPixelWidth();
	// Check for divide by zeros. Shouldn't happen but very bad when it happens on galleries
	ERROR3IF(PixelWidth == 0, "GRenderRegion::DrawFixedSystemText PixelWidth = 0");
	if (PixelWidth == 0)
		PixelWidth = 1;

	// Now make a windows (pixels) rectangle from the Bounding DocRect (millipoints)
	INT32 bw = BoundsRect.Width()/PixelWidth;
	INT32 bh = BoundsRect.Height()/PixelWidth;

#if 0
	// Clip the windows rect so that is no bigger than the required rectangle
	if (w < bw) 
		bw = w;

	if (h < bh)
		bh = h;

	if ((bh<=0) || (bw<=0))		// Still a valid rectangle?
		return;
#endif

	// Create a memory DC
	wxBitmap bitmap(w, h);
	wxMemoryDC MemDC;
	MemDC.SelectObject(bitmap);
	MemDC.SetFont(FixedFont);

	MemDC.SetBackgroundMode(wxSOLID);

	// Copy the Text colours from the RenderDC into the memory DC
	MemDC.SetTextForeground(RenderDC->GetTextForeground());
	MemDC.SetTextBackground(RenderDC->GetTextBackground());

	// Draw the text into the memory DC
	MemDC.DrawText((TCHAR *) (*TheText), 0, 0);

	KernelBitmap* pBitmap;

	// Make a new Oil Bitmap
	OILBitmap* pOilBmp = OILBitmap::Create();
	if (pOilBmp == NULL)
		return;

	// and copy the data from the resources
	((CWxBitmap *)pOilBmp)->CreateFromwxBitmap(&bitmap);

	// Make a KernelBitmap and attach the Oil Bitmap
	pBitmap = new KernelBitmap(pOilBmp, TRUE);	 	// TRUE = temp bitmap
	if (pBitmap == NULL)
		return;

	// Calculate the orgin needed to centre the bitmap vertically
	INT32 Height = h * PixelWidth;

	DocCoord BottomLeft;
	BottomLeft.x = BoundsRect.lo.x;
	BottomLeft.y = (BoundsRect.lo.y + BoundsRect.Height()/2) - Height/2;

	GetDrawContext()->SetDefaultBitmapParameters();
	// Get gavin to plot the kernel bitmap
	DrawBitmap(BottomLeft, pBitmap);

	// Delete the Kernel Bitmap
	delete pBitmap;

}



/********************************************************************************************

>	virtual void GRenderRegion::SetFixedSystemTextColours(DocColour *TextCol,
														 DocColour *Background)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/4/95
	Inputs:		TextCol - The foreground (text) colour (may be NULL)
				Background - The background colour (may be NULL)

	Purpose:	To set the text and background colour(s) for any FixedSystem text rendered
				in this render region in the future (within the current rendering pass).

	Notes:		If either of the colours is passed in as a NULL pointer, that colour will
				not be set, and will remain at the previous setting.

				Note that these values are passed directly to the host OS at the point
				of calling, so you must call this method every time the colour is changed
				(i.e. don't rely on just changing the DocColours that you passed in
				originally, as they aren't remembered in any way). Note also that if you
				call DrawFixedSystemText without a prior call to this method, the colours
				used are undefined.

				The colour chosen may not exactly match the colour requested - under
				Windows, for example, the nearest solid (non-dithered) colour to that
				requested will be used. Thus, it is best to stick to 'simple' colour
				schemes.

				This method may or may not affect some other rendering processes (for
				example, under Windows, plotting of bitmaps may be affected by the
				settings used here...)

	SeeAlso:	GRenderRegion::DrawFixedSystemText

********************************************************************************************/

void GRenderRegion::SetFixedSystemTextColours(DocColour *TextCol, DocColour *Background)
{
	if (TextCol != NULL)
	{
		COLORREF clr = ConvertColourToScreenWord(CurrentColContext, TextCol);
		RenderDC->SetTextForeground(clr);
	}

	if (Background != NULL)
	{
		COLORREF clr = ConvertColourToScreenWord(CurrentColContext, Background);
		RenderDC->SetTextBackground(clr);
	}
}



/********************************************************************************************

>	virtual void GRenderRegion::GetFixedSystemTextSize(StringBase *TheText,
														DocRect *BoundsRect)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/4/95

	Inputs:		TheText - The string to obtain the rendered size from

	Outputs:	BoundsRect - Returned with the size of the rectangle needed to display
				said text string. This rect always has one corner at (0,0)

	Returns:	-

	Purpose:	To determine how much room is needed to plot a bit of text with
				GRenderRegion::DrawFixedSystemText

	Notes:		If for any reason the call fails, an origin based rectangle with zero
				height and width will be returned.
				
	SeeAlso:	GRenderRegion::DrawFixedSystemText

********************************************************************************************/

void GRenderRegion::GetFixedSystemTextSize(StringBase *TheText, DocRect *BoundsRect, double* atDpi)
{
	ERROR3IF(TheText == NULL, "GRenderRegion::GetFixedSystemTextSize given a null text pointer");
	ERROR3IF(BoundsRect == NULL, "GRenderRegion::GetFixedSystemTextSize given a null bounds rect pointer");
	if(TheText == NULL || BoundsRect == NULL)
		return;


	wxString Text = (wxString)(TCHAR *)(*TheText);

	wxFont SaveFont=RenderDC->GetFont();

	wxFont FixedFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	FixedFont.SetPointSize(8);
	RenderDC->SetFont(FixedFont);

	wxDC * pDC = RenderDC;
	wxSize DPI = OSRenderRegion::GetFixedDCPPI(*pDC);
	INT32 XDPI = DPI.GetWidth();
	INT32 YDPI = DPI.GetHeight();
	INT32 LineHeight = RenderDC->GetCharHeight();

	wxCoord w, h;
	RenderDC->GetTextExtent(Text, &w, &h);

	RenderDC->SetFont(SaveFont);

	// For some reason, Rect.bottom and Rect.top seem to be incorrect, so we have
	// to use the returned LineHeight value

	*BoundsRect = DocRect(0, 0,
						  (INT32)(((double)(w) * IN_MP_VAL) / XDPI),
						  (INT32)(((double)LineHeight * IN_MP_VAL) / YDPI) );

}

/********************************************************************************************

>	BOOL GRenderRegion::DrawArrowHead(ArrowRec &ArrowToDraw,
									  DocCoord &Centre, DocCoord &Direction)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Inputs:		An ArrowRec defining the Arrowhead, the Centre point of the Arrow and a 
				point defining the direction of the Arrow.
	Purpose:	Draw an Arrow head on the start or end of a line.
	SeeAlso:	-

********************************************************************************************/

BOOL GRenderRegion::DrawArrowHead(ArrowRec &ArrowToDraw,
								  DocCoord &Centre, DocCoord &Direction)
{
	// Call base class version for now.
	return RenderRegion::DrawArrowHead(ArrowToDraw, Centre, Direction);
}

/********************************************************************************************

>	GMATRIX GRenderRegion::SetGavinMatrix(GMATRIX* GMatrix)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		The new GMATRIX to use.
	Returns: 	The old GMATRIX that was being used.
	Purpose:	Updates the GMATRIX used for rendering.
	SeeAlso:	GRenderRegion::MakeGavinMatrix

********************************************************************************************/

GMATRIX GRenderRegion::SetGavinMatrix(GMATRIX* GMat)
{
	// Get the current GMATRIX being used
	GMATRIX OldMatrix = CurrentGMatrix;

	// See if anyone wants to adjust the matrix
	if (!StartRenderAfter( GMat ))
		return OldMatrix;

	// Now update the current Matrix.

	// Why am I assigning each element individually ?
	// Don't ask me.  Ask the compiler why it can't
	// handle 'CurrentGMatrix = *GMat'
	CurrentGMatrix.AX = GMat->AX;
	CurrentGMatrix.AY = GMat->AY;
	CurrentGMatrix.BX = GMat->BX;
	CurrentGMatrix.BY = GMat->BY;
	CurrentGMatrix.CX = GMat->CX;
	CurrentGMatrix.CY = GMat->CY;

	// Set the new Matrix
	GetDrawContext()->SetMatrix( GMat );

	return OldMatrix;
}




/********************************************************************************************

>	virtual GMATRIX GRenderRegion::MakeGavinMatrix(Matrix NewRenderMatrix, DocRect ClipRect, double dPixelsPerInch, BOOL bMasterCapture)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com> (from Will (from Phil code))
	Created:	??/??/2004
	Inputs:		The Kernel Matrix and cliprect to use to create the Gavin Matrix.
	Returns: 	TRUE if the GMatrix was created ok.
	Purpose:	Make a Gavin Matrix out of a Kernel Matrix
	SeeAlso:	GRenderRegion::SetGavinMatrix
	Notes:		Doesn't rely on current state of CurrentClipRect like the above version

********************************************************************************************/

GMATRIX GRenderRegion::MakeGavinMatrix(Matrix NewRenderMatrix, DocRect ClipRect, double dPixelsPerInch, BOOL bMasterCapture)
{
	GMATRIX gmat;
	// Get the scale factors from the original render matrix
	FIXED16 abcd[4];
	INT32 ef[2];
	NewRenderMatrix.GetComponents( abcd, ef );

	// Use them to call DocRect to win on the clipping region, without attempting any translation.
	// Thus, the DocCoord cliprect gets turned directly into a Windows pixel values cliprect!
	Matrix UnscrolledRender(abcd[0], abcd[1], abcd[2], abcd[3], 0, 0);

	// in order to get Gavin's matrix calcs to agree with ours, we have to force him to
	// round before shifting down. We do this by adding half a pixel to the offsets in
	// his matrices. There is a school of thought which says that his matrix code
	// should be made to round, but it might have unpleasant side-effects. For now, we do
	// it this way. A pixel is 1<<(FX+16), so half a pixel is this incantation:

	// sjk's (5/12/00) preferred version as this works with the decimal places that may be in PixelsPerInch
	const XLONG Mult = (INT32)(dPixelsPerInch*(double)(1 << FX) + 0.5);

	gmat.AX = ( (XLONG)abcd[0].GetRawLong() * Mult ) / XLONG(72000);
	gmat.AY = ( (XLONG)abcd[1].GetRawLong() * Mult ) / XLONG(72000);
	gmat.BX = ( (XLONG)abcd[2].GetRawLong() * Mult ) / XLONG(72000);
	gmat.BY = ( (XLONG)abcd[3].GetRawLong() * Mult ) / XLONG(72000);

#if 0 //_DEBUG
	// Check the translation in the matrix is a pixel multiple
	// Only run# check on integer ppi where an exact divisor into 72000
	if ( !Error::IsUserName("Gavin") )
		if (dPixelsPerInch == floor(dPixelsPerInch))
		{
			INT32 pixelmp = (INT32)(72000.0 / dPixelsPerInch);
			if ((double)(72000/pixelmp) == dPixelsPerInch)
			{
				if ( (((INT64)(ef[0])) % pixelmp) || (((INT64)(ef[1])) % pixelmp) )
				{
//					ERROR3("Render matrix on unaligned pixel boundary");
					TRACE(_T("***********************************************"));
					TRACE(_T("** Render matrix on unaligned pixel boundary **"));
					TRACE(_T("***********************************************"));
				}
			}
		}
#endif

	// Use a version of OSRenderRegion::DocRectToWin that uses the actual dpi we have stored
	// in PixelsPerInch
	WinRect UnscrolledClip = OSRenderRegion::DocRectToWin(UnscrolledRender, ClipRect, dPixelsPerInch);

	// Now, the offsets to the Gavin bitmap are simply the corner of the Windows cliprect.
	// This needs to be the exclusive bottom value rather than the inclusive value returned 
	// by wxRect::GetBottom.  Hence we call our extra GetBottomEx function.
	INT32 xdisp = +UnscrolledClip.GetLeft();
	INT32 ydisp = -UnscrolledClip.GetBottomEx();

	// Make the offsets to the cliprect suitable for use by Gavin.
	// Just shift them up so the binary point is where he expects it...
	gmat.CX = -( (XLONG)xdisp * (XLONG)(1<<(FX+16)) );
	gmat.CY = -( (XLONG)ydisp * (XLONG)(1<<(FX+16)) );

//	TRACE(_T("%08x %08x  %08x %08x\n"), gmat.AX, gmat.BX, (UINT32)(gmat.CX >> 32), (UINT32)(gmat.CX & 0xFFFFFFFF));
//	TRACE(_T("%08x %08x  %08x %08x\n"), gmat.AY, gmat.BY, (UINT32)(gmat.CY >> 32), (UINT32)(gmat.CY & 0xFFFFFFFF));

	return gmat;
}

/********************************************************************************************

>	BOOL GRenderRegion::SetGCol( DocColour &Col ) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (Andy)
	Created:	15/04/94 (7/12/93)
	Inputs:		A DocColour object
	Outputs:	-
	Returns:	TRUE if not transparent, FALSE if transparent (i.e. don't bother rendering)
	Purpose:	Sets Gavins drawing colour. He does not distinguish between line & fill
				colours - the caller has to do that.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GRenderRegion::SetGCol( DocColour &Col ) const
{
	if ( Col.IsTransparent() )
		return FALSE;

	COLORREF rgbt;
	rgbt = ConvertColourToScreenWord(CurrentColContext, &Col);

	// get RGB value, or RGBT value if deep bitmap	
	// Find out about the line quality
	if (uBitmapDepth==32)
	{
		// Only do transparency if the line quality is better than black outline AND
		// the overall quality it not guide layer quality
		if ((RRQuality.GetLineQuality() >  Quality::BlackLine) && 
			(RRQuality.GetQuality()     != Quality::QualityGuideLayer))
		{
			DWORD Transp = *RR_STROKETRANSP()->GetStartTransp();

			DWORD Style = RR_STROKETRANSP()->GetTranspType();

			if (!(Style == 1 && Transp == 0))
			{
				if (RenderFlags.HitDetect)
					Style = T_REFLECTIVE;

				rgbt |= Transp<<24;
				GetDrawContext()->SetTransparency( rgbt, (TransparencyEnum)Style );
				return(TRUE);
			}
		}
	}

	if (UseSolidColours)
	{
		// Determine the correct hinting value for the current output mode, and call gavin to
		// set up his offscreen bitmap.
		DWORD GavinHint = DIBUtil::GetGavinBlitFormat(ScreenDepth, uBitmapDepth, ScreenHinting);
		GetDrawContext()->SetSolidColour( rgbt, ScreenDepth, GavinHint);
	}
	else
		GetDrawContext()->SetColour( rgbt );

	return TRUE;
}

/********************************************************************************************

>	BOOL GRenderRegion::SetFillGCol( DocColour &Col ) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (Andy)
	Created:	15/04/94 (7/12/93)
	Inputs:		A DocColour object
	Outputs:	-
	Returns:	TRUE if not transparent, FALSE if transparent (i.e. don't bother rendering)
	Purpose:	Sets Gavins drawing colour. He does not distinguish between line & fill
				colours - the caller has to do that.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GRenderRegion::SetFillGCol( DocColour &Col ) const
{
	if ( Col.IsTransparent() )
		return FALSE;

	COLORREF rgbt;

	// If rendering to the hit-detect bitmap force all fill colours to black.
	if (RenderFlags.VeryMono)
		rgbt = RGB(0,0,0);
	else
		rgbt = ConvertColourToScreenWord(CurrentColContext, &Col);

	if (UseSolidColours)
	{
		// Determine the correct hinting value for the current output mode, and call gavin to
		// set up his offscreen bitmap.
		DWORD GavinHint = DIBUtil::GetGavinBlitFormat(ScreenDepth, uBitmapDepth, ScreenHinting);
		GetDrawContext()->SetSolidColour( rgbt, ScreenDepth, GavinHint);
	}
	else
		GetDrawContext()->SetColour( rgbt );

	return TRUE;
}

/********************************************************************************************

>	void GRenderRegion::SetSolidColours(BOOL SetSolid)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		TRUE to turn off dithering of colours
	Purpose:	Allows the dithering of colours to be turned off so they look the same
				using GDraw as they do with the GDI.

********************************************************************************************/

void GRenderRegion::SetSolidColours(BOOL SetSolid)
{
	UseSolidColours = SetSolid;
}

/********************************************************************************************

>	MILLIPOINT GRenderRegion::CalcPixelWidth()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/94
	Returns:	The size of a pixel in MILLIPOINTS
	Purpose:	Calculates the size of a Pixel in MILLIPOINTS, based on the output DPI
				but IGNORES the scale factor.
	SeeAlso:	GetScaledPixelWidth()

********************************************************************************************/

MILLIPOINT GRenderRegion::CalcPixelWidth()
{
	return MILLIPOINT( MILLIPOINTS_PER_INCH / PixelsPerInch);
}

/********************************************************************************************

>	double GRenderRegion::CalcPixelWidthDouble()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/2006
	Returns:	The size of a pixel in a double
	Purpose:	Calculates the size of a Pixel in MILLIPOINTS, based on the output DPI
				but IGNORES the scale factor.
	SeeAlso:	GetScaledPixelWidth()

********************************************************************************************/

double GRenderRegion::CalcPixelWidthDouble()
{
	return MILLIPOINTS_PER_INCH / PixelsPerInch;
}

/********************************************************************************************

>	MILLIPOINT GRenderRegion::CalcScaledPixelWidth()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/94
	Returns:	The size of a pixel in MILLIPOINTS
	Purpose:	Calculates the size of a Pixel in MILLIPOINTS, based on the output DPI
				and current scale factor.
	SeeAlso:	CalcPixelWidth()

********************************************************************************************/

MILLIPOINT GRenderRegion::CalcScaledPixelWidth()
{
	MILLIPOINT spw = (MILLIPOINT)( MILLIPOINTS_PER_INCH/(PixelsPerInch*ScaleFactor.MakeDouble()) + 0.5 );
	ERROR3IF(spw==0, "CalcScaledPixelWidth attempting to set a 0 value");
	return spw;
}



/********************************************************************************************

>	double GRenderRegion::CalcScaledPixelWidthDouble()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/2006
	Returns:	The size of a pixel in a double
	Purpose:	Calculates the size of a Pixel in MILLIPOINTS, based on the output DPI
				and current scale factor.
	SeeAlso:	CalcPixelWidthDouble()

********************************************************************************************/

double GRenderRegion::CalcScaledPixelWidthDouble()
{
	double spw = MILLIPOINTS_PER_INCH/(PixelsPerInch*ScaleFactor.MakeDouble());
	ERROR3IF(spw==0, "CalcScaledPixelWidthDOuble attempting to set a 0 value");
	return spw;
}



/********************************************************************************************

>	double GRenderRegion::GetScaledPixelWidthDouble()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25 September 2000

	Purpose:	Returns the true scaled pixel width of this GRenderRegion, to the accuracy
				of a double. GetScaledPixelWidth() returns a whole number, which is highly
				inaccurate at high zooms - eg SPW at 6031% is usually 750 / 60.31 = 12.44ish
				but GetScaledPixelWidth() returns 12. This method is useful if your code
				needs precise conversion between millipoints and pixels.

	Notes:		USE AT YOUR PERIL!!!

				PLEASE Note that if you mix and match GetScaledPixelWidthDouble() with
				GetScaledPixelWidth(), you are HIGHLY likely to run into problems. Most of
				Camelot uses the original GetScaledPixelWidth(), so if you do decide to use
				this version, watch out for any interaction problems with the rest of the App!

	See also:	GetScaledPixelWidth().

********************************************************************************************/
double GRenderRegion::GetScaledPixelWidthDouble()
{
	return dScaledPixelWidth;
}



void GRenderRegion::InitClipping()
{
	RememberMe("InitClipping");

	GetDrawContext()->ClipRectangle( (LPRECT)&CurrentClipRect );
	IsPaperRendered = FALSE;
}

void GRenderRegion::InitAttributes()
{
}

void GRenderRegion::SetLineAttributes()
{
}

void GRenderRegion::SetFillAttributes()
{
}

void GRenderRegion::SetOSDrawingMode()
{
	ENSURE( DrawingMode != DM_EORPEN, "Gdraw cannot EOR");
}

/********************************************************************************************

>	void GRenderRegion::SetQualityLevel()

	Author:	   	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Purpose:	Called after the Quality setting has been changed on the region. We have
				to recalculate the antialiasing & flatness settings. Also used by
				StartRender.

********************************************************************************************/

void GRenderRegion::SetQualityLevel()
{
	const BOOL Anti = (RRQuality.GetAntialiasQuality() < Quality::FullAntialias) ? FALSE : TRUE;
	GetDrawContext()->SetAntialiasFlag( Anti );

	if (Anti)
	{
		GetDrawContext()->SetAntialiasQualityFlag(HighQualityAA);
	}

	// Set up flatness...
	// Gavin recommends half-pixel smoothing. One pixel is 72000/dpi millipoints
	//INT32 Flatness = (72000L / PixelsPerInch.MakeLong()) / 2;
	INT32 Flatness = (INT32)((MILLIPOINTS_PER_INCH / PixelsPerInch) / 2);
	Flatness = LongDivFixed16( Flatness, ScaleFactor );
 	if (Anti)
		Flatness /= 5;
	GetDrawContext()->SetFlatness( Flatness );
}


void GRenderRegion::Restore(RenderRegion*)
{
}

/********************************************************************************************

>	BOOL GRenderRegion::AttachDevice(View* ViewToAttach, CDC *pCDC, Spread *pSpread)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/93
	Inputs:		ViewToAttach is the output view, pCDC is the destination DC, pSpread is which
				spread we are rendering.
	Returns:	TRUE if attached ok;
				FALSE if not.
	Purpose:	Gets ready to do actual rendering. In this case it does nothing.
				bitmap.
	Errors:		Same as base class.
	SeeAlso:	RenderRegion::AttachDevice; GRenderRegion::InitDevice

********************************************************************************************/

BOOL GRenderRegion::AttachDevice(View* ViewToAttach, wxDC *pDC, Spread *pSpread, bool fOwned /*= false*/)
{
	RememberMe("AttachDevice");

	// Call the base class *first*
	BOOL Result = RenderRegion::AttachDevice( ViewToAttach, pDC, pSpread, fOwned );

	// Gives us a colour context if there is no render view
	if (RenderView==NULL)
		CurrentColContext = ColourContext::GetGlobalDefault(COLOURMODEL_RGBT);

	// return the result
	return Result;
}

/********************************************************************************************

>	BOOL GRenderRegion::InitDevice()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/05/94
	Returns:	TRUE if initialised ok;
				FALSE if not.
	Purpose:	Initialise the device specific mechanisms for this render region. IN this
				case the bitmap is allocated, so can fail.
	Errors:		Same as base class.
	SeeAlso:	RenderRegion::InitDevice; GRenderRegion::AttachDevice

********************************************************************************************/

BOOL GRenderRegion::InitDevice()
{
	ENSURE(MasterCaptureIsCurrent() || GetCaptureDepth()==0, "InitDevice called when captures running");
	RememberMe("InitDevice");

	// Call the base class *first*
	BOOL Worked = RenderRegion::InitDevice();

	if (!Worked)
		return FALSE;

	// Make sure we only set WRect ONCE!
	if ( LocalBitmap && WRect==wxRect(0,0,0,0) )
	{
//		WRect = OSRenderRegion::DocRectToWin( RenderView, RenderMatrix, CurrentClipRect, 0,0,0,0 );
		WRect = OSRenderRegion::DocRectToWin( RenderMatrix, CurrentClipRect, PixelsPerInch );
	}

	// Make sure rectangle is sensible size...
	ERROR2IF(WRect.width==0 || WRect.height==0, FALSE, "GRenderRegion::InitDevice was passed a rectangle with zero size");
	if (WRect.width == 0 || WRect.height == 0)
		return FALSE;
	
	// Make sure the rectangle is ok
	ERROR2IF(WRect.width<0  || WRect.height<0 , FALSE, "GRenderRegion::InitDevice was passed a rectangle with -ve size");
	if (WRect.width < 0 || WRect.height < 0)
		return FALSE;

	// Nothing rendered yet
	SetClean(FALSE, FALSE);

	return TRUE;
}


/********************************************************************************************

>	RenderRegion* GRenderRegion::Create(DocRect ClipRegion, Matrix ConvertMatrix, 
										FIXED16 ViewScale,  RenderType rType,
										View *pView = NULL)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/93
	Inputs:		-
	Outputs:	-
	Returns:	Pointer to a render region if we can use gdraw, else NULL (if not a screen
				type, or gdraw disabled, or not compatible with this machine).
	Purpose:	Determines whether a GRenderRegion can be created for rendering. A NULL return
				should NOT be tested with ENSURE, it is a perfectly valid return result.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

RenderRegion* GRenderRegion::Create(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale,
									RenderType rType, View* pView, BOOL bForce32BPP)
{
	if (CanDoGDraw && WantDoGDraw)
	{
		if (rType==RENDERTYPE_SCREEN)
		{
PORTNOTE("other","We no longer handle less than 32bit per pixel")
			UINT32 BitmapDepthWanted = 32; // ScreenDepth;			// default to screen
			if (WantGDrawDepth)
				BitmapDepthWanted = WantGDrawDepth;					// override if user wants to

			// Find out from the associated DocView/Document whether the document requires
			// 32-bit transparency mode.  By default we don't.
			if (pView != NULL)
			{
				// if in outline quality then don't go transparent.
				// ideally would like it to be 1-bit, but the pastboard won't render in that case
				if (pView->RenderQuality.GetLineQuality() != Quality::BlackLine)
				{
					BitmapDepthWanted = 32;
				}

				// If we are in less than 16 bpp and we want error diffusing then force 32 bpp
				if (BitmapDepthWanted < 16)
				{
					if ((ViewDither == 0) || (ViewDither == 1) || (ViewDither == 3))
						BitmapDepthWanted = 32;
				}
			}

			// FORCE depth to 32bpp when needed (input flag)
			if (bForce32BPP)
				BitmapDepthWanted = 32;

			// GDraw cannot do 24-bit bitmaps, so use 32-bit ones
			if (BitmapDepthWanted == 24)
				BitmapDepthWanted = 32;

			GRenderRegion * pRegion = NULL;

//			#if REAL_DDB
//			if ((BlitMode == BLITMODE_BADDDB) && (BitmapDepthWanted == 8))
//				pRegion = new GRenderDDB(ClipRegion, ConvertMatrix, ViewScale, BitmapDepthWanted, LogPixelsX);
//			else
//			#endif
//			if (WantWinG && GRenderWinG::CanWinG(BitmapDepthWanted))
//				pRegion = new GRenderWinG(ClipRegion, ConvertMatrix, ViewScale, BitmapDepthWanted, LogPixelsX);
//			else
//				TRACE(_T("Creating SCREEN region (%d, %d) - (%d, %d)  GRenderDIB\n"), ClipRegion.lo.x, ClipRegion.lo.y, ClipRegion.hi.x, ClipRegion.hi.y);
				pRegion = new GRenderDIB(ClipRegion, ConvertMatrix, ViewScale, BitmapDepthWanted, LogPixelsX);

			if ((pView != NULL) && (pRegion != NULL))
			{
				// If we have a view and we got a region then set the dither style
				pRegion->SetDitherStyle8Bit(ViewDither);

				// Now check for separated rendering and alter the RRCaps of the new region
				// to simulate a printing render region
			if (pView->GetColourPlate() != NULL && !pView->GetColourPlate()->IsDisabled())
				{
					ColourPlateType PlateType = pView->GetColourPlate()->GetType();
					if (PlateType != COLOURPLATE_NONE && PlateType != COLOURPLATE_COMPOSITE)
					{
						// First try to set this view to using a printing colour context
						// so the separations appear the same on screen as on the printer
PORTNOTE("cms", "DisabledXaraCMS")
#ifndef EXCLUDE_FROM_XARALX
						XaraCMS* ptheCMS=GetApplication()->GetCMSManager();
						if (ptheCMS != NULL)
						{
							String_256 PrintProfile;
							ptheCMS->GetPrinterProfile(&PrintProfile);
							ColourContext *pContext = new ColourContextCMYK(pView, &PrintProfile);
							if (pContext)
								pView->SetColourContext(COLOURMODEL_CMYK, pContext);
						}
#endif
						// Now check what sort of printer we have to get the RenderRegion Caps correct
						UINT32 PrintType = CCPrintDialog::IsPostscript() ? 2 : 1;
						pRegion->SetSimulatePrinting(PrintType);
					}
				}
			}

			// return the region pointer
			return(pRegion);
		}
		else if ((rType==RENDERTYPE_PRINTER) || (rType==RENDERTYPE_PRINTER_PS))
		{
			// Always use 24 bit for printers, which means 32bit because Gavin can't do
			// 24-bit bitmaps.
			UINT32 BitmapDepth = 32;

			// Get document pointer
			Document* pDoc = pView->GetDoc();

			// Get print information for this document.
			PrintComponent *pPrint = 
				(PrintComponent *) pDoc->GetDocComponent(CC_RUNTIME_CLASS(PrintComponent));
			ERROR2IF(pPrint == NULL, NULL, "Unable to find PrintComponent in document.");

			PrintControl *pPrintControl = pPrint->GetPrintControl();
			ERROR2IF(pPrintControl == NULL, NULL, 
					 "Unable to find PrintControl object in document component.");

			// Find out what DPI to use...
			INT32 PrintDPI = pPrintControl->GetDotsPerInch();

			return new GRenderPrint(ClipRegion, ConvertMatrix, ViewScale, 
									BitmapDepth, PrintDPI);
		}
	}

	return NULL;
}



/********************************************************************************************

>	void GRenderRegion::DisplayLtoHBitmap()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com> (based on Gavin)
	Created:	4/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Slightly misleading name for historic reasons. Actually plots any depth
				bitmap on any device, without any special handling. Use as a fallback
				when the more specialist versions are not suitable.

********************************************************************************************/

void GRenderRegion::DisplayLtoHBitmap()
{
	DWORD *rgb = (DWORD *)pBitmapInfo->bmiColors ;

	// note that the number of RGBs used here must agree with the number allocated in
	// AllocDIB for each and every bitmap depth

	switch ( uBitmapDepth )
	{
		case 1 :
			*rgb++ = 0x000000 ;
			*rgb++ = 0xffffff ;
			break ;
		case 4 :
			*rgb++ = 0x000000 ; *rgb++ = 0x800000 ; *rgb++ = 0x008000 ; *rgb++ = 0x808000 ;
			*rgb++ = 0x000080 ; *rgb++ = 0x800080 ; *rgb++ = 0x008080 ; *rgb++ = 0x808080 ;
			*rgb++ = 0xc0c0c0 ; *rgb++ = 0xff0000 ; *rgb++ = 0x00ff00 ; *rgb++ = 0xffff00 ;
			*rgb++ = 0x0000ff ; *rgb++ = 0xff00ff ; *rgb++ = 0x00ffff ; *rgb++ = 0xffffff ;
			break ;
		case 16 :
			*rgb++ = 0x7c00 ;
			*rgb++ = 0x03e0 ;
			*rgb++ = 0x001f ;
			break ;
		case 32 :
			*rgb++ = 0x00ff0000 ;
			*rgb++ = 0x0000ff00 ;
			*rgb++ = 0x000000ff ;
			break ;
	}

	PlotBitmap( DIB_RGB_COLORS );
}

/********************************************************************************************

>	void GRenderRegion::DisplayHto8Bitmap()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com> (based on Gavin)
	Created:	4/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Plot high colour bitmap (16 or 32-bits) to a 256 colour device. If we let
				GDI do this it would take a very long time, so we convert to an intermediate
				bitmap first. NEW - also plots to a 16-colour device. Will ENSURE if not 16-
				or 256-colours.

********************************************************************************************/
/*
void GRenderRegion::DisplayHto8Bitmap()
{
	// this is a test clause that skips the colour conversion.
	// Draws very slowly though.
	if (WantHighColourDirect)
	{
		DisplayLtoHBitmap();
		return;
	}

	//if ((ScreenDepth != 4) && (ScreenDepth != 8))
	//{
	//	ERROR3("Wrong screen depth in DisplayHto8");
	//	return;
	//}

	LPBITMAPINFOHEADER lpBitmapHeader = &lpBitmapInfo->bmiHeader;
	INT32 BWidth  = (INT32)lpBitmapHeader->biWidth;
	INT32 BHeight = (INT32)lpBitmapHeader->biHeight;

	INT32 DestX = WRect.x;
	INT32 DestY = WRect.y;
	INT32 RectWidth = WRect.width;
	INT32 RectHeight= WRect.height;

	INT32 SourceX = ScreenRect.x;
	INT32 SourceY = ScreenRect.y;

	INT32 StripBottom = BHeight;
	INT32 StripTop = 0;

	RECT UsedRect;

	if (GetDrawContext()->GetChangedBBox(&UsedRect) &&
		UsedRect.right > UsedRect.left &&
		UsedRect.bottom > UsedRect.top)
	{
		// blit less, thanks to changedBBox

		// Gavin usefully returns his rectangles the wrong way up
		// he returns x1,y1,x2,y2 and we want
		// x1,h-y2,x2,h-y1

		const INT32 ChangeX = UsedRect.left;
		SourceX += ChangeX;
		DestX += ChangeX;

		INT32 ChangeY = RectHeight - UsedRect.bottom;
	
		SourceY += ChangeY;
		DestY += ChangeY;

		RectWidth = UsedRect.right - UsedRect.left;
		RectHeight = UsedRect.bottom - UsedRect.top;
		
	
		if (UsedRect.top > 4)
		{
			// Allow 4 pixels extra
			StripTop = UsedRect.top - 4;

			// Round down to multiple of 4 to avoid dither alignment problems
			StripTop = StripTop	& ~3;
		}

		// Allow 4 pixels extra
		StripBottom = UsedRect.bottom + 4;
	
		// Round up to multiple of 4 to avoid dither alignment problems
		StripBottom = (StripBottom + 3) & ~3;
		
		// Clip to bitmap height
		if (StripBottom > BHeight)
			StripBottom = BHeight;
	}
	else
	{
		TRACEUSER( "Gerry", _T("Empty clip rectangle in DisplayHto8\n"));
		return;
	}

	INT32 StripHeight = StripBottom - StripTop;

	// Copy the source bitmap header for the strip header
	BITMAPINFOHEADER SourceHeader = *lpBitmapHeader;

	// Calculate the size of a source scanline
	DWORD SourceScanSize = DIBUtil::ScanlineSize(SourceHeader.biWidth, SourceHeader.biBitCount);

	// Set the height and size of the strip
	SourceHeader.biHeight = StripHeight;
	SourceHeader.biSizeImage = StripHeight * SourceScanSize;

	INT32 StripDepth = ScreenDepth;

	// this is like a BITMAPINFO but with 256 colours
	struct
	{
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD bmiColors[256];
	} DBitmapInfo;

	DBitmapInfo.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	DBitmapInfo.bmiHeader.biWidth			= BWidth;
	DBitmapInfo.bmiHeader.biHeight			= StripHeight;
	DBitmapInfo.bmiHeader.biPlanes			= 1;
	DBitmapInfo.bmiHeader.biBitCount		= StripDepth;
	DBitmapInfo.bmiHeader.biCompression		= BI_RGB;
	DBitmapInfo.bmiHeader.biSizeImage		= DIBUtil::ScanlineSize(BWidth, StripDepth) * StripHeight;
	DBitmapInfo.bmiHeader.biXPelsPerMeter	= 100;
	DBitmapInfo.bmiHeader.biYPelsPerMeter	= 100;
	DBitmapInfo.bmiHeader.biClrUsed			= 1 << StripDepth;
	DBitmapInfo.bmiHeader.biClrImportant	= 0;

	const LPBYTE DBitmap = (LPBYTE)CCMalloc( DBitmapInfo.bmiHeader.biSizeImage );

	if (DBitmap == NULL)
	{
		ERROR3( "Low mem in Hto8 - will display v. slowly" );
		// display something at least, though this is not an ideal solution.
		DisplayLtoHBitmap();
		return;
	}

	// Get the right context for the convert bitmap
	GDrawContext *GDC = GetStaticDrawContext(DitherStyle8Bit);

	// Offset source coord by distance of strip in from edge
	SourceY -= (BHeight - StripBottom);
	if (SourceY < 0)
		SourceY = 0;

	// Get the pointer to the correct scan line
	LPBYTE SourceBits = lpBits + (StripTop * SourceScanSize);

	INT32 ColourFlag;			// ColourFlag for the PlotBitmap call below

	if (StripDepth == 4)
	{
		// In 4 bpp we need a LOGPALETTE structure so we can
		// call GDrawContext::SetConversionPalette()
		struct {
			WORD palVersion;
			WORD palNumEntries;
			PALETTEENTRY palPalEntry[16];
		} LogPal = {0x300, 16};

		// Get the screen DC
		HDC hDC = ::GetDC(NULL);

		if (hDC != NULL)
		{
			// Get the 16 palette entries from the system palette
			// into the LOGPALETTE
			GetSystemPaletteEntries(hDC,
								0,
								16,
								LogPal.palPalEntry);

			RGBQUAD *rgb = DBitmapInfo.bmiColors;

			// Swap R and B.
			size_t i;
			for (i = 0; i < 16; i++ )
			{
				PALETTEENTRY &PalRef = LogPal.palPalEntry[i];

				rgb->rgbRed = PalRef.peRed;
				rgb->rgbGreen = PalRef.peGreen;
				rgb->rgbBlue = PalRef.peBlue;
				rgb->rgbReserved = 0;
				rgb++;
			}

			// Release the DC
			::ReleaseDC(NULL, hDC);
		}

		ColourFlag = DIB_RGB_COLORS;

		// Tell GDraw to use the palette
		GDC->SetConversionPalette((PLOGPALETTE)&LogPal);

		GDC->ConvertBitmap(&SourceHeader, SourceBits,
							&(DBitmapInfo.bmiHeader), DBitmap, DitherStyle8Bit);

		// Stop GDraw using the palette before it goes out of scope
		GDC->SetConversionPalette(NULL);
	}
	else
	{	
	
		ColourFlag = SetPaletteEntries((LPBITMAPINFO)&DBitmapInfo, RenderDC);

		GDC->ConvertBitmap(&SourceHeader, SourceBits,
							&(DBitmapInfo.bmiHeader), DBitmap, DitherStyle8Bit);
	}

	PlotBitmap(RenderDC, ColourFlag, (LPBITMAPINFO)&DBitmapInfo, DBitmap, DestX, DestY,
				RectWidth, RectHeight, hPalette, SourceX, SourceY);

	CCFree(DBitmap);
}
*/
/********************************************************************************************

>	UINT32 SetPaletteEntries( LPBITMAPINFO lpBmi );

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com> (based on Gavin)
	Created:	4/3/94
	Inputs:		lpBmi will usually be lpBitmapInfo
	Outputs:	-
	Returns:	DIB_PAL_INDICES or DIB_RGB_COLORS
	Purpose:	If can do PAL_INDICES, just does that, else
				fills in the RGB quads in the DIB and returns DIB_RGB_COLOR. Only really
				make sense to call on 16 or 256 colour bitmaps.
	Errors:		Will ensure if >256 colours (then mess upon memory).

********************************************************************************************/

UINT32 GRenderRegion::SetPaletteEntries( LPBITMAPINFO lpBmi, wxDC* pOutputDC )
{
	PORTNOTETRACE("other","GRenderRegion::SetPaletteEntries - do nothing");
#ifndef EXCLUDE_FROM_XARALX
#if WIN32
// This has been commented out as we now always use the GDraw palette

//	if (CanDoPalIndices)
//		return(DIB_PAL_INDICES);						// always best option if possible
#endif

	LPBITMAPINFOHEADER BitmapHeader = &lpBmi->bmiHeader;
	const INT32 Depth  = BitmapHeader->biBitCount;

	if (Depth > 8 )
	{
	//	ERROR3("Too deep for SetPal");
	//	return(DIB_RGB_COLORS);
	}

	RGBQUAD *rgb = lpBmi->bmiColors;
	const size_t Count = 1<<Depth;

	if (Fixed256Palette)
	{
		// if fixed palette then use it as is
		memcpy(rgb, Fixed256Palette, sizeof(RGBQUAD) * 256);
		return(DIB_RGB_COLORS);
	}

	// This now uses the ErrorDiffPalette
	// Which is set up to avoid problems when the palette changes
	if (Count == 256)
	{
		if (ViewDither == 2)
			memcpy(rgb, &(StandardPalette.palPalEntry[0]), sizeof(PALETTEENTRY) * 256 );
		else
			memcpy(rgb, &(ErrorDiffPalette.palPalEntry[0]), sizeof(PALETTEENTRY) * 256 );
	}
	else
	{
		HDC hDC = ::GetDC(NULL);
		if (hDC != NULL)
		{
			GetSystemPaletteEntries(hDC,0,Count,(LPPALETTEENTRY) rgb);
			::ReleaseDC(NULL, hDC);
		}
	}

	// Swap R and B.
	size_t i;
	for ( i=0 ; i<Count ; i++ )
	{
		BYTE t = rgb->rgbRed;
		rgb->rgbRed = rgb->rgbBlue;
		rgb->rgbBlue = t;
		rgb ++;
	}
#endif
	return(DIB_RGB_COLORS);
}

// set this to 1 to evaluate 16-bit DIB plotting. I (Andy) have yet to find a Win16 driver
// that understands 16-bit DIBs
#define	TEST16BIT	0

#if TEST16BIT
// like RGB macro, except defines 15-bit RGB word
#define	C16(r,g,b)	(WORD) ( ((r>>3)<<10) | ((g>>3)<<5) | (b>>3) )

// a table of visible 15-bit colours
static WORD ColDebugTable[32] =
{
	C16(255,0,0),			// Red
	C16(255,0,0),			// Red
	C16(0,255,0),			// Green
	C16(0,255,0),			// Green
	C16(0,0,255),			// Blue
	C16(0,0,255),			// Blue

	C16(128,0,0),			// half Red
	C16(0,128,0),			// half green
	C16(0,0,128),			// half blue


	C16(128,255,0),
	C16(128,128,0),

	C16(128,0,255),
	C16(128,0,128),


	C16(255,128,0),
	C16(128,128,0),

	C16(0,128,255),
	C16(0,128,128),


	C16(255,0,128),
	C16(128,0,128),

	C16(0,255,128),
	C16(0,128,128)


};
#endif

/********************************************************************************************

>	void GRenderRegion::PlotBitmap( HDC hDC, UINT32 ColourFlag, LPBITMAPINFO BitmapInfo,
			LPBYTE Bits, INT32 Left, INT32 Top, INT32 Width, INT32 Height, HPALETTE hPal,
			INT32 SourceLeft, INT32 SourceTop )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/94
	Inputs:		ColourFlag as used in SetDIBitsToDevice (e.g. DIB_RGB_COLORS). hPal must
				already be selected into the device. Left & Top are the destination in
				screen coords. SourceLeft and SourceTop are the offset from logical top left
				of the source bitmap to be plotted.
	Outputs:	-
	Returns:	-
	Purpose:	Plot the DIB as fast as possible to the display surface. IMPORTANT: Only
				works on SCREEN devices.
	Errors:		-
	Scope:		Static

********************************************************************************************/

void GRenderRegion::PlotBitmap( wxDC* hDC, UINT32 ColourFlag,
		INT32 Left,INT32 Top, UINT32 Width,UINT32 Height, wxPalette* hPalette, INT32 SourceLeft,INT32 SourceTop )
{
	pBits=StaticPlotBitmap(hDC, ColourFlag, pBitmapInfo, pBits, Left, Top, Width, Height, hPalette, SourceLeft, SourceTop);
}

LPBYTE GRenderRegion::StaticPlotBitmap( wxDC* hDC, UINT32 ColourFlag,
		LPBITMAPINFO lpBitmapInfo, LPBYTE lpBits,
		INT32 Left,INT32 Top, UINT32 Width,UINT32 Height, wxPalette* hPalette, INT32 SourceLeft,INT32 SourceTop )
{
	if( Width==0 || Height==0 )
		return lpBits;

	// Test preconditions
	ERROR3IF((lpBitmapInfo->bmiHeader.biSize==0x00000000) /*|| 
			 (lpBitmapInfo->bmiHeader.biSize==0xdddddddd) || 
			 (lpBitmapInfo->bmiHeader.biSize==0xcdcdcdcd) */,"Illegal BitmapInfo structure passed to PlotBitmap");
			// AB commented second two checks out as bmiHeader.biSize is a WORD, i.e. 16 bits in size, according to compatdef.h
//	TRACE( _T("SrcRect(%d, %d, %d, %d) - (%d, %d)"), rectDraw.GetLeft(), rectDraw.GetTop(),
//		rectDraw.GetWidth(), rectDraw.GetHeight(), pBitmap->GetWidth(), pBitmap->GetHeight() );

	CamProfile cp(CAMPROFILE_BLIT);

#if !USE_wxBITMAP

	ERROR3IF(lpBitmapInfo->bmiHeader.biBitCount!=32,"Image must (currently) be 32bpp");

	wxBitmap Bitmap(Width,Height,32);
	wxAlphaPixelData BitmapData(Bitmap);
	//
	// Copy source (or part of source) into wxBitmap.
	//
	DWORD* pSBuffer = 
			(DWORD*)lpBits + 
			lpBitmapInfo->bmiHeader.biWidth*(lpBitmapInfo->bmiHeader.biHeight-1-SourceTop) +
			SourceLeft;
	DWORD* pDBuffer = (DWORD*)Bitmap.GetRawData(BitmapData,32);
	DWORD* pDLine = pDBuffer;
	INT32 nStep = Width;
	if ( BitmapData.GetRowStride()<0 )
		nStep = -nStep;
	for( UINT32 y=0 ; y<Height ; ++y )
	{
		//
		// Set all pixels to be opaque. (We don't want to plot with alpha here
		// so inverting the alpha channel would not be correct, and it's
		// possible that the transparency channel will not always be zero).
		//
#if defined(__WXGTK__)
		for( UINT32 x=0; x<Width; ++x )
		{
			BYTE* pS = pBYTE(pSBuffer+x);
			BYTE* pD = pBYTE(pDLine  +x);
			pD[0] = pS[2];
			pD[1] = pS[1];
			pD[2] = pS[0];
			pD[3] = 0xFF;
		}
#else
		for( UINT32 x=0; x<Width; ++x )
			pDLine[x] = pSBuffer[x];
#endif
		pSBuffer -= lpBitmapInfo->bmiHeader.biWidth;
		pDLine += nStep;
	}
	if ( ScreenHinting==CONVHINT_FINAL16  ||
		 ScreenHinting==CONVHINT_FINAL555 ||
		 ScreenHinting==CONVHINT_FINAL565 ||
		 ScreenHinting==CONVHINT_FINAL655 ||
		 ScreenHinting==CONVHINT_FINAL664 )
	{
		BITMAPINFOHEADER Info = { sizeof(BITMAPINFOHEADER),Width,Height,1,32 } ;
		if ( nStep<0 )
			pDBuffer += (Height-1)*nStep;
		UINT32 uHint;
		switch (ScreenHinting)
		{
		case CONVHINT_FINAL565: uHint = 8+0; break;
		case CONVHINT_FINAL655: uHint = 8+1; break;
		case CONVHINT_FINAL664: uHint = 8+3; break;
		default:				uHint = 8+2; break;
		}
		GRenderRegion::GetStaticDrawContext()->ConvertBitmap(&Info,(BYTE*)pDBuffer,&Info,(BYTE*)pDBuffer,uHint);
	}
	Bitmap.UngetRawData(BitmapData) ;

#ifdef _DEBUG
	// Alex's test to test scaling
	if ((hDC->LogicalToDeviceX(wxCoord(72000)))!=wxCoord(72000))
	{
		ERROR3("X coordinate out");
	}
	if ((hDC->LogicalToDeviceY(wxCoord(72000)))!=wxCoord(72000))
	{
		ERROR3("Y coordinate out");
	}
#endif

	{
//		CamProfile cp(CAMPROFILE_BLIT);
		hDC->DrawBitmap(Bitmap,Left,Top) ;
	}

#else

#if defined(__WXGTK__)
	RGBT* pBuffer = (RGBT*)lpBits + pBitmapData->m_width*SourceTop + SourceLeft;
	for( UINT32 y=0 ; y<Height ; ++y )
	{
		//
		// Set all pixels to be opaque. (We don't want to plot with alpha here
		// so inverting the alpha channel would not be correct as it's
		// possible that the transparency channel will not always be zero).
		//
		for( UINT32 x=0; x<Width; ++x )
			pBuffer[x].Transparency = 0xFF;
		pBuffer += pBitmapData->m_width;
	}
#endif

	BITMAPINFOHEADER Info = { sizeof(BITMAPINFOHEADER),Width,Height,1,32 } ;

	bool bIs16Bit = false ;
	UINT32 uHint = 8+2;
	if ( ScreenHinting==CONVHINT_FINAL16  ||
		 ScreenHinting==CONVHINT_FINAL555 ||
		 ScreenHinting==CONVHINT_FINAL565 ||
		 ScreenHinting==CONVHINT_FINAL655 ||
		 ScreenHinting==CONVHINT_FINAL664 )
	{
		switch (ScreenHinting)
		{
		case CONVHINT_FINAL565: uHint = 8+0; break;
		case CONVHINT_FINAL655: uHint = 8+1; break;
		case CONVHINT_FINAL664: uHint = 8+3; break;
		default:							 break;
		}
		bIs16Bit = true ;
	}
	if ( SourceLeft || SourceTop || Width!=pBitmap->GetWidth() || Height!=pBitmap->GetHeight() )
	{
		pBitmap->UngetRawData(*pBitmapData) ;
		wxBitmap bmp = pBitmap->GetSubBitmap( wxRect(SourceLeft,SourceTop,Width,Height) ) ;
		if ( bIs16Bit )
		{
			wxAlphaPixelData bmpData(bmp);
			BYTE* pData = (BYTE*)bmp.GetRawData(bmpData,32);
			if ( bmpData.GetRowStride()<0 )
				pData += bmpData.GetRowStride()*(Height-1);
			GRenderRegion::GetStaticDrawContext()->ConvertBitmap(&Info,pData,&Info,pData,uHint);
			bmp.UngetRawData(bmpData) ;
		}
		{
//			CamProfile cp(CAMPROFILE_BLIT);
			hDC->DrawBitmap(bmp,Left,Top);
		}
		TRACE( _T("DrawBitmap(%08x,%04x,%04x),%04x,%04x\n"),hDC,Left,Top,Width,Height);
	}
	else
	{
		if ( bIs16Bit )
			GRenderRegion::GetStaticDrawContext()->ConvertBitmap(&Info,lpBits,&Info,lpBits,uHint);
		pBitmap->UngetRawData(*pBitmapData) ;
		{
//			CamProfile cp(CAMPROFILE_BLIT);
			hDC->DrawBitmap(*pBitmap,Left,Top) ;
		}
		TRACE( _T("DrawBitmap(%08x,%04x,%04x)\n"),hDC,Left,Top);
	}

	lpBits = (BYTE*)pBitmap->GetRawData(*pBitmapData,uBitmapDepth);
#endif
	return lpBits;
}


/********************************************************************************************

>	void GRenderRegion::PlotBitmap( UINT32 ColourFlag )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/94
	Inputs:		ColourFlag as used in SetDIBitsToDevice (e.g. DIB_RGB_COLORS).
	Outputs:	-
	Returns:	-
	Purpose:	Plot the DIB as fast as possible to the display surface. Member function
				version. There is also a more general static version (which this uses).
				This member fn one uses the ChangedBBox for optimal blitting.
	Errors:		-

********************************************************************************************/

void GRenderRegion::PlotBitmap( UINT32 ColourFlag )
{
	RememberMe("PlotBitmap");

	#if REMEMBER_GDRAW==2
	TRACE( _T("Rect=(%d,%d,%d,%d) BM=%dx%d\n"),WRect.left,WRect.top,WRect.right,WRect.bottom,
				lpBitmapInfo->bmiHeader.biWidth,
				lpBitmapInfo->bmiHeader.biHeight
		);
	#endif

	#if FILL_BITMAP
	if (IsUserName("Andy") && (BitmapDepth==8) )
	{
		LPBYTE p = lpBits;
		// is the top scanline is all the same value?
		const BYTE value = *p;
		INT32 i = (INT32)lpBitmapInfo->bmiHeader.biWidth; 				// check first scanline
		while (i--)
		{
			if (*p != value)
				break;
			p++;
		}
		if (i<0)
		{
			if (value==FILL_BITMAP)
			{
				TRACE( _T("Got FILL_BITMAPed bitmap\n"));
				RememberDump();
			}
			else if (value==FILL_BITMAP+1)
			{
				TRACE( _T("Got FILL_BITMAPed bitmap+1 bitmap\n"));
				RememberDump();
			}
		}
	}
	#endif

	#if 0
	{
		// helper code to render hatching before the blit to detect if it fails
		static INT32 BrushNum = 2;
		CBrush Brush;

		// different colour every time
		Brush.CreateHatchBrush( BrushNum, PALETTEINDEX( rand() & 0xFF ) );

		if (++BrushNum > 5)
			BrushNum = 2;

		RenderDC->FillRect( &WRect, &Brush );
	}
	#endif

	// default values, assume blitting everything
	INT32 DestX = WRect.x;
	INT32 DestY = WRect.y;
	INT32 Width = WRect.width;
	INT32 Height= WRect.height;

	INT32 SourceX = ScreenRect.x;
	INT32 SourceY = ScreenRect.y;

	RECT UsedRect;
	if (GetDrawContext()->GetChangedBBox( &UsedRect ))
	{
//		TRACE( _T("ChangedBBox = (%d, %d, %d, %d)"), UsedRect.left, UsedRect.top, UsedRect.right, UsedRect.bottom );
		
		if ((UsedRect.right < UsedRect.left) ||
			(UsedRect.bottom< UsedRect.top ))
		{
			// Changed box is illegal so exit
			TRACE(_T("Bad ChangedBBox\n"));
			return;
		}
		else
		{
			// blit less, thanks to changedBBox

			// Gavin usefully returns his rectangles the wrong way up
			// he returns x1,y1,x2,y2 and we want
			// x1,h-y2,x2,h-y1

			const INT32 ChangeX = UsedRect.left;
			SourceX += ChangeX;
			DestX += ChangeX;

			INT32 ChangeY = Height - UsedRect.bottom;
			SourceY += ChangeY;
			DestY += ChangeY;

			Width  = UsedRect.right  - UsedRect.left;
			Height = UsedRect.bottom - UsedRect.top;

//			TRACE( _T("Reg    %d - %d\n"),WRect.top, WRect.bottom);
//			TRACE( _T("Gav    %d - %d\n"),UsedRect.top, UsedRect.bottom);
//			TRACE( _T("Height %d\n"), Height);
			
//			if (FALSE) 
//			{
//				TRACE( _T("Was %dx%d now %dx%d\n"), WRect.right-WRect.left, WRect.bottom-WRect.top,
//												Width, Height );
//			}
		}
	}

//	TRACE(_T("Blitting (%d, %d) (%d, %d) (%d, %d)\n"), DestX, DestY, Width, Height, SourceX, SourceY);
	
// GAT
//	PlotBitmap( RenderDC, ColourFlag, lpBitmapInfo, lpBits, DestX, DestY,
//				Width, Height, hPalette, SourceX, SourceY );
	PlotBitmap( RenderDC, ColourFlag, DestX,DestY, Width,Height, hPalette, SourceX,SourceY );
}

/********************************************************************************************

>	void GRenderRegion::Display1to1Bitmap()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com> (based on Gavin)
	Created:	4/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Plot a bitmap to a device of the same colour depth.

********************************************************************************************/

void GRenderRegion::Display1to1Bitmap()
{
	UINT32 ColourFlag = DIB_RGB_COLORS ;

	DWORD *rgb = (DWORD *)pBitmapInfo->bmiColors ;

	switch (uBitmapDepth)
	{
		case 16:
			*rgb++ = 0x7c00 ;
			*rgb++ = 0x03e0 ;
			*rgb++ = 0x001f ;
			break;
	
		case 32:
			*rgb++ = 0x00ff0000 ;
			*rgb++ = 0x0000ff00 ;
			*rgb++ = 0x000000ff ;
			break;

		default:
			ColourFlag = SetPaletteEntries( pBitmapInfo, RenderDC );
			break;
	}

	PlotBitmap( ColourFlag );
}


/********************************************************************************************

>	const LOGPALETTE* GRenderRegion::GetRecommendedPalette()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/94
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to a logical palette, or NULL if the screen does not do palettes.
	Purpose:	For the main window to get a Gavin-approved logical palette.
	Scope:		Static

********************************************************************************************/

const LOGPALETTE* GRenderRegion::GetRecommendedPalette()
{
	// as this can be called very early in the startup sequence, it does not rely
	// on ::Init having been called first

	const LOGPALETTE* lpPal = NULL;


	if (pRealGD == NULL)
		pRealGD = new GDrawAsm;

	if (pRealGD->Init())
	{
		// always call this regardless of screen depth. For 8-bit and less screens,
		// call it with 0 (dithering), else 1 (error diffusion)

		// In fact we will always call it with 0 as the palette returned is the same
		// We will try to keep the static GD context set up for ordered dithering
		PORTNOTETRACE("other","GRenderRegion::GetRecommendedPalette - removed palette code");
#ifndef EXCLUDE_FROM_XARALX
		const INT32 ScreenDepth = ::wxDisplayDepth();
		lpPal = pRealGD->SelectPalette(0);

		// the returned pointer is only interesting to 8-bit palette devices
		if ((ScreenDepth!=8) ||
			!(GetDeviceCaps( hdc, RASTERCAPS ) & RC_PALETTE ) ||
			WantNoPalette)
#endif
		{
			lpPal = NULL;
		}
	}

	GD = pRealGD;

	return(lpPal);
}



void GRenderRegion::DrawPixel(const DocCoord &Point)
{
	// If we are not drawing complex shapes and this shape is, then return
	if ((!RenderComplexShapes) && (TestForComplexShape(&Caps)))
		return;

	RememberMe("DrawPixel");

	if (!SetGCol(RR_STROKECOLOUR()))				
		return;

	POINT PlotMe;
	PlotMe.x = Point.x-(ScaledPixelWidth/2);		// >>>> Bodge to make grid dots align with crosses
	PlotMe.y = Point.y-(ScaledPixelWidth/2);		// >>>>
	GetDrawContext()->FillPoint( &PlotMe );

	HaveRenderedSomething = TRUE;
}

/********************************************************************************************

>	void GRenderRegion::DrawCross(const DocCoord &Point, const UINT32 Size)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/94
	Inputs:		Point - the centre point of the cross.
				Size - the length of the lines used to draw the cross with.
	Purpose:	Draws a cross in the render region.  The cross consists of a vertical
				line 'Size' millipoints high, and a horizontal line 'Size' millipoints
				wide.

********************************************************************************************/

void GRenderRegion::DrawCross(const DocCoord &Point, const UINT32 Size)
{
	// If we are not drawing complex shapes and this shape is, then return
	if ((!RenderComplexShapes) && (TestForComplexShape(&Caps)))
		return;

	RememberMe("DrawCross");

	if (!SetGCol(RR_STROKECOLOUR()))
		return;

	HaveRenderedSomething = TRUE;

	const INT32 Length = Size * ScaledPixelWidth;

	static PathVerb Verbs[] =
	{
		PT_MOVETO, PT_LINETO,
		PT_MOVETO, PT_LINETO
	};
	DocCoord Coords[4];

	DocCoord NewPoint = Point;

	Coords[0].x = NewPoint.x - Length;
	Coords[0].y = NewPoint.y;
	Coords[1].x = NewPoint.x + Length;
	Coords[1].y = NewPoint.y;

	Coords[2].x = NewPoint.x;
	Coords[2].y = NewPoint.y - Length;
	Coords[3].x = NewPoint.x;
	Coords[3].y = NewPoint.y + Length;

	GetDrawContext()->StrokePath( (POINT*)Coords, Verbs, sizeof(Verbs), FALSE,
											0, CAPS_BUTT, JOIN_MITER, NULL );
}

/********************************************************************************************

>	BOOL GRenderRegion::StrokePathAvailable()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if StrokePathToPath is available, FALSE if it is not.
	Purpose:	Used to know whether StrokePathToPath will fail. If we don't have GDraw,
				we cannot do this, if we do, we can.
	Scope:		Static

********************************************************************************************/

BOOL GRenderRegion::StrokePathAvailable()
{
	return CanDoGDraw;
//	return TRUE;
}

/********************************************************************************************

>	INT32 GRenderRegion::StrokePathToPath(
	 	CONST DocCoord *IPoints,
		CONST BYTE FAR *ITypes,
		DWORD ILength,
		DocCoord *OPoints,
		BYTE  *OTypes,
		DWORD OLength,
		BOOL Close,
		DWORD LineWidth,
		DWORD Flattening,
		LineCapType LineCaps,
		JointType LineJoin,
		CONST DashType *Dash)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/94
	Inputs:		IPoints is the path of points to be stroked, ITypes its verb list and ILength
				the length. OPoints, OTypes and OLength are the output path. If Close is
				TRUE then the path will be closed. Flattening is how flat it is required.
	Outputs:	-
	Returns:	Length of resultant path. Returns -1 if errored.
	Purpose:	Used for rendering complex strokes when the native system cannot do it (e.g.
				GDI16).
	Scope:		Static

********************************************************************************************/

INT32 GRenderRegion::StrokePathToPath(
	 	CONST DocCoord *IPoints,
		CONST BYTE FAR *ITypes,
		size_t ILength,
		DocCoord *OPoints,
		BYTE  *OTypes,
		DWORD OLength,
		BOOL Close,
		DWORD LineWidth,
		DWORD Flattening,
		LineCapType LineCaps,
		JointType LineJoin,
		CONST DashType *Dash)
{
	if (!CanDoGDraw)
		return -1;

	CapStyles CapS = (LineCaps==LineCapButt) ? CAPS_BUTT : (LineCaps==LineCapRound) ? CAPS_ROUND : CAPS_SQUARE;
	JoinStyles JoinS = (LineJoin==MitreJoin) ? JOIN_MITER :	(LineJoin==RoundJoin) ? JOIN_ROUND : JOIN_BEVEL;

	DASH GDash = { 0, 0, { 0 } };

	if(Dash)
		memcpy(&GDash,Dash,sizeof(GDash));

	PPOINT SPoints = NULL;
	PBYTE STypes = NULL;
//	INT32 SLength = 0;

	GenStroke GStroke;
	INT32 Result = GStroke.StrokePathToPath((LPPOINT)IPoints, ITypes, (UINT32)ILength,
											&SPoints, &STypes,
											Close, LineWidth, (UINT32)(10<<16), Flattening,
											CapS, JoinS, &GDash);

	if (Result < 0)
		TRACE( _T("GDraw_StrokePtoP error=%lx\n"), 0 );

	// If the Resultant length is larger than the Output path length then fail and return the overflow!
	if(Result > (INT32)OLength)
		return (INT32)OLength - Result;

	for (INT32 i = 0; i < Result; i++)
	{
		OPoints[i].x = SPoints[i].x;
		OPoints[i].y = SPoints[i].y;
		OTypes[i] = STypes[i];
	}

	delete SPoints;
	delete STypes;

	return Result;
}




/********************************************************************************************

>	void GRenderRegion::GColInit( HDC hdc, BOOL Force = FALSE )

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com> (Rewritten by Jason. Originally by AndyP)
	Created:	2/5/96 (30/6/94)

	Inputs:		A device context and whether the GDraw fn should be called regardless. If
				hdc is NULL and Force is TRUE then the next time someone calls this it
				will always do a Force.
	Outputs:	-
	Purpose:	Initialises GDraw for rendering into 8bpp bitmaps

				Actually, GColour Init is not very expensive (5 milliseconds if the
				palette hasn't changed) but this function is more efficient (0.5 millisecs
				if it hasn't changed).

				This function will also force a redraw of all windows if it detects that
				the palette has changed - this is because a palette change detected in the
				middle of a background redraw could leave the already-drawn areas invalid.

	Scope:		Static public

********************************************************************************************/

void GRenderRegion::GColInit( wxDC* hdc, BOOL Force )
{
	PORTNOTETRACE("other","GRenderRegion::GColInit - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	static PALETTEENTRY OldPal[260];

	//BOOL DoRedraw = TRUE;

//	TRACEUSER( "Gerry", _T("GColInit() : DC = %08x  (%d)\n"), hdc, Force);

	if (hdc != NULL)
	{
		// If this isn't a palette device then don't do anything...
		if (!(::GetDeviceCaps(hdc, RASTERCAPS) & RC_PALETTE))
			return;
		
		// The user wants us to make sure GDraw is ready to render. We will check
		// to see if the palette has changed, and if it has, we'll redraw
		PALETTEENTRY NewPal[260];

		if (GetSystemPaletteEntries(hdc, 0, 256, NewPal) == 0)
		{
			TRACE( _T("GColInit() : GetSystemPaletteEntries failed %d\n"), GetLastError());
			return;
		}

		for (DWORD Index = 0; Index < 256; Index++)
			NewPal[Index].peFlags = 0;

		if (memcmp(NewPal, OldPal, sizeof(PALETTEENTRY) * 256) != 0)
		{
//			TRACEUSER( "Gerry", _T("GColInit() : Palette has changed\n"));

			// The palette has changed, so we copy the palette
			// data across to update OldPal
			memcpy(OldPal, NewPal, sizeof(PALETTEENTRY) * 256);

			// And ensure that all palette-reliant windows are redrawn
			PaletteManager::RedrawAllPalettedWindows();
		}
	}
	else
	{
//		TRACEUSER( "Gerry", _T("GColInit() : Vaping OldPal record\n"));
		// The caller is just informing us that the palette has changed, so we should
		// flag the fact that GDraw needs initialisation next time.
		// The easiest way to do this is to vape the OldPal record.
		memset(OldPal, 0, sizeof(PALETTEENTRY) * 256);
	}
#endif
}

/********************************************************************************************

>	static void GRenderRegion::EnsurePalette(INT32 PaletteFlag)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/96

	Inputs:		PaletteFlag - value to pass to GDraw_SelectPalette. If -1 then uses the
							  correct value for the current WhichDither setting
	Outputs:	-
	Purpose:	Calls SelectPalette for the static GDraw context and sets the palette
	Scope:		Static public

********************************************************************************************/

void GRenderRegion::EnsurePalette(INT32 PaletteFlag)
{
	if (PaletteFlag == -1)
	{
		PaletteFlag = 0;
		if ((WhichDither == 0) || (WhichDither == 3))
			PaletteFlag = 1;
	}

	const LOGPALETTE* pPalette = GetStaticDrawContext()->SelectPalette(PaletteFlag);
	if (pPalette != NULL)
		GetStaticDrawContext()->InitialiseWithPalette(pPalette);
}


/********************************************************************************************

>	static void GRenderRegion::UpdateErrorDiffPalette(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		-
	Outputs:	-
	Purpose:	Sets up a sensible palette for error diffusion
				It sets 3 of the entries to black to avoid them being used
	Scope:		Static protected

********************************************************************************************/

void GRenderRegion::UpdateErrorDiffPalette(void)
{
	if (ErrorDiffContext != NULL)
	{
		const LOGPALETTE* pPal = ErrorDiffContext->SelectPalette(1);

		if (pPal != NULL)
		{
			// Calculate length of palette
			INT32 Length = 2 * sizeof(WORD) + (pPal->palNumEntries * sizeof(PALETTEENTRY));

			// Copy it away
			memcpy(&ErrorDiffPalette, pPal, Length);

			// Set the rampant entries that Windows changes to black so we don't use them
			INT32* pEntry = (INT32*) &(ErrorDiffPalette.palPalEntry[0]);
			pEntry[8] = 0;
			pEntry[9] = 0;
			pEntry[246] = 0;
			pEntry[247] = 0;

			memcpy(&StandardPalette, &ErrorDiffPalette, Length);

#ifdef RALPH
			// Set the silly entries to black...
			for(INT32 i=10; i<=15; i++)
				pEntry[i] = 0;
			for(i=24; i<=31; i++)
				pEntry[i] = 0;
			for(i=224; i<=231; i++)
				pEntry[i] = 0;
			for(i=240; i<=245; i++)
				pEntry[i] = 0;			

// DEFCONS: This is a marker for updating the default context tables
//			Change the following #if FALSE to TRUE
#if FALSE
			// Initialise the error diffusion context so we can save it
			ErrorDiffContext->InitialiseWithPalette(GetErrorDiffPalette());
#endif
			// Set the context up from the static block (or save it)
			((GDrawAsm*)ErrorDiffContext)->InitContext((GCONTEXT*)ErrorDiffContextData, 1);

#else // ifdef RALPH
			// And initialise the error diffusion context
			ErrorDiffContext->InitialiseWithPalette(GetErrorDiffPalette());
#endif

			// Either way set the stack limit here
			ErrorDiffContext->SetStackSize(102400);
		}
	}
}



/********************************************************************************************

>	static DWORD GRenderRegion::GetMaxBitmapWidth(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/96
	Inputs:		-
	Outputs:	-
	Returns:	Max width of off screen bitmap supported
	Scope:		Public
	SeeAlso:	GDrawAsm::GetMaxBitmapDepth()

	Notes:		This function does not require a GDraw critical section

********************************************************************************************/ 

DWORD GRenderRegion::GetMaxBitmapWidth(void)
{
	return XaDraw_GetMaxBitmapWidth();
}


	
/********************************************************************************************

>	static DWORD GRenderRegion::GetMaxBitmapDepth(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/96
	Inputs:		-
	Outputs:	-
	Returns:	Max depth of off screen bitmap supported
	Scope:		Public
	SeeAlso:	GDrawAsm::GetMaxBitmapWidth()

	Notes:		This function does not require a GDraw critical section

********************************************************************************************/ 

DWORD GRenderRegion::GetMaxBitmapDepth(void)
{
	return XaDraw_GetMaxBitmapDepth();
}



/********************************************************************************************

>	const REGION* GRenderRegion::MakeClipRegionFromClipAttr(ClipRegionAttribute* pClipAttr)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10 April 2000
	Inputs:		pClipAttr		ptr to the ClipRegionAttribute whose path we'll base the new
								clip region on.
	Outputs:	
	Returns:	Pointer to a new GDraw clipping region, or
				NULL if unsuccessful for some reason.

	Purpose:	Given a ptr to a ClipRegionAttribute, generate an appropriate GDraw clipping
				region and return it.
	Errors:		
	See also:	

********************************************************************************************/
const REGION* GRenderRegion::MakeClipRegionFromClipAttr(ClipRegionAttribute* pClipAttr)
{
	// we need a path from pClipAttr, to which we'll clip, but we'll
	// do one or two validation checks on it before we trust it.
	Path* pClipPath = pClipAttr->GetClipPath();
	if (pClipPath == NULL || pClipPath->GetNumCoords() < 2)
		return NULL;

// paranoia check...
#ifdef _DEBUG
	if (!pClipPath->CheckPathValid())
		return NULL;
#endif

	// ok, extract the raw data from our new path.
	DWORD		Winding	  = (RR_WINDINGRULE() == EvenOddWinding) ? 0 : 1;
	UINT32		NumCoords = pClipPath->GetNumCoords();
	DocCoord*	Coords	  = pClipPath->GetCoordArray();
	PathVerb*	Verbs	  = pClipPath->GetVerbArray();

	// now we make the new clipping region.

	// as an initial guess of the size of the region, we'll say ~16 bytes per scanline,
	// and we'll arbitrarily set the maximum region size to be 256K. we'll keep doubling
	// our guess until it works or it hits our imposed ceiling.
	//
	//	NOTE:
	//		This is ok for uncomplicated paths. Complex paths require proportionally more 
	//		memory, so we should really be slightly more intelligent in guesswork here...
	//
	// Regions require approximatly the following amount of memory per scanline:
	//		Non-antialiased	=  6 bytes
	//		LQ antialiased	= 20 bytes
	//		HQ antialiased	= 44 bytes
	//
	BYTE* pNewRegion		= NULL;
	UINT32	NumScans		= 500;		// initial guess - 500 scanlines.
	UINT32	nRegionLen		= NumScans * (RRQuality.GetAntialiasQuality() == Quality::FullAntialias ? 48 : 16);
	size_t	nMaxRegionLen	= 0x100000;	// max region size = 1M at the mo'.
	BOOL	bMadeRegion		= FALSE;
	for (INT32 attempts = 0; (nRegionLen < nMaxRegionLen) && !bMadeRegion; attempts ++)
	{
		// allocate an empty region-buffer at our guess-size.
		pNewRegion = new BYTE[nRegionLen];
		if (pNewRegion == NULL)
		{
			// we shout out in debug mode - in release we try to fail quietly.
#ifdef _DEBUG
			ERROR3( "Application has ran out of memory" );
#endif
			return NULL;
		}
		memset(pNewRegion, 0, nRegionLen);

		// attempt to make the new clipping region.
		// if unsuccessful, double our buffer size and try again.
		if ( GetDrawContext()->MakeRegion(	(POINT*)Coords, Verbs, NumCoords,
											Winding, (REGION*)pNewRegion, nRegionLen) == -1 )
		{
			delete [] pNewRegion;
			pNewRegion	= NULL;
			nRegionLen *= 2;
		}

		// otherwise, record a success.
		else
		{
			bMadeRegion = TRUE;
		}
	}

	if (pNewRegion != NULL)
	{
		RECT r = ((REGION*)pNewRegion)->Rect;
		if (r.left	== 0 &&
			r.right	== 0 &&
			r.bottom== 0 &&
			r.top	== 0)
		{
			if (RRQuality.GetAntialiasQuality() == Quality::FullAntialias)
				((REGION*)pNewRegion)->Type = 1;
			else
				((REGION*)pNewRegion)->Type = 0;
		}
	}

	return (REGION*)pNewRegion;
}


/********************************************************************************************

>	void GRenderRegion::SetDeepDIB(BOOL Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/99
	Inputs:		Value - the value to set
	Outputs:	
	Purpose:	Sets the flag which indicates that we are rendering to a 32bit bitmap

********************************************************************************************/


void GRenderRegion::SetDeepDIB(BOOL Value)
{
//	if (Value && ScreenDepth <= 16) // don't do it if we don't have a deep screen depth
//		return;

//	CanDoDeepDIBs = Value;
}

/********************************************************************************************

>	void GRenderRegion::FreeStoredClipRegions()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11 April 2000
	Outputs:	Any clip regions stored in this render region will be freed up, so pointers
				to them will become invalid.

	Purpose:	Delete and free memory used by all clip regions which this render region
				has stored.

	See also:	FreeClipRegionForAttr().

********************************************************************************************/
void GRenderRegion::FreeStoredClipRegions()
{
	// now delete all cached clip-regions *except* for GDraw's, which we've already freed.
//	POSITION pos = m_ClipRegionMap.GetStartPosition();
//	REGION*		pClipRegion	= NULL;
//	void*		refpKey		= NULL;
//	while (pos != NULL)
//	{
//		m_ClipRegionMap.GetNextAssoc(pos, refpKey, (void*&)pClipRegion);
//		if (pClipRegion != NULL)
//			delete [] ((BYTE *)pClipRegion);
//	}
//	m_ClipRegionMap.RemoveAll();

	ListT<Capture*>* pList = m_CaptureStack.GetListT();
	{
		Capture* pCapture = NULL;
		ListT<Capture*>::Iterator it = pList->End();
		INT32 nListSize = pList->Size();
		for (INT32 i = 0; i < nListSize; i++)
		{
			pCapture = *(--it);

			pCapture->RemoveAllCachedClipRegions();
		}
	}
}



/********************************************************************************************

>	void GRenderRegion::OffsetByHalfPixel()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/08/2000

	Purpose:	Intended solely for use by the feathering and shadowing systems, both of
				which make use of a bitmap blurring algorithm which can sometimes return
				a blurred bitmap up and right by half a pixel.

				To compensate for this, the source bitmap must be rendered at an offset
				of half a pixel down and left. This function sets GDraw and this render
				region up to do that.

********************************************************************************************/
void GRenderRegion::OffsetByHalfPixel()
{
	// offset our render matrix by half a pixel.
	INT32 OffsetX = (INT32)(0.5 + (0.5 * ScaledPixelWidth));
	INT32 OffsetY = (INT32)(0.5 + (0.5 * ScaledPixelWidth));
	RenderMatrix.translate(OffsetX, OffsetY);

//	if (GetTopCaptureBitmap())
//	{
//		GetTopCaptureBitmap()->CaptureMatrix = RenderMatrix;
//	}

	// offset GDraw's render-matrix by half a pixel.
	// one GDraw pixel-width is (1 << FX + 16).
	INT32	LEFTSHIFT	= FX + 15;
	CurrentGMatrix.CX -= (XLONG)(1<<LEFTSHIFT);
	CurrentGMatrix.CY -= (XLONG)(1<<LEFTSHIFT);
	GetDrawContext()->SetMatrix( &CurrentGMatrix );
}


/********************************************************************************************

>	virtual void GRenderRegion::GetRenderRegionCaps(RRCaps* pCaps)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/01/2004
	Outputs:	pCaps - the flags to set for the things we are able to render
	Purpose:	This function allows a GRenderRegion to declare what it can render and
				what it can not render. This allows us to pretend to be a simple render region
				when doing onscreen print previews.

********************************************************************************************/

void GRenderRegion::GetRenderRegionCaps(RRCaps* pCaps)
{
	if (m_ForcePrintingCaps == 1)
	{
		// Return the same caps as a printing OSRenderRegion
		pCaps->CanDoNothing();
		pCaps->GradFills = TRUE;
		pCaps->LineAttrs = TRUE;
		pCaps->ArrowHeads = TRUE;
		pCaps->DashPatterns = TRUE;
		pCaps->SimpleBitmaps = TRUE;
	}
	else if (m_ForcePrintingCaps == 2)
	{
		// Return the same caps as a PS render region
		pCaps->CanDoNothing();
		pCaps->GradFills = TRUE;
		pCaps->SimpleBitmaps = TRUE;
		pCaps->ArbitraryBitmaps = TRUE;
		pCaps->ClippedSimpleBitmaps = TRUE;
		pCaps->ClippedArbitraryBitmaps = TRUE;
		pCaps->ClippedOutput = TRUE;
	}
	else
	{
		// We are fabby!
		pCaps->CanDoAll();
	}
}

/********************************************************************************************

>	virtual void GRenderRegion::SetSimulatePrinting(UINT32 PrintType)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/01/2004
	Inputs:		PrintType - 0=Not simulating, 1=Normal printer, 2=Postscript
	Purpose:	This function tells a GRenderRegion to simulate printing
				This allows us to pretend to be a simple render region
				when doing onscreen print previews.

********************************************************************************************/

void GRenderRegion::SetSimulatePrinting(UINT32 PrintType)
{
	m_ForcePrintingCaps = PrintType;
	if (PrintType)
		RenderFlags.Printing = TRUE;

	// Set the render caps up again
	GetRenderRegionCaps(&Caps);
}


/********************************************************************************************

>	SlowJobResult GRenderRegion::DrawMaskedBitmap(const DocRect &Rect, KernelBitmap* pBitmap, 
								  MaskedRenderRegion* pMask, Progress *Progress)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/01/2004
	Inputs:		Point - the position to plot the bitmap
				pBitmap - The bitmap that needs plotting
				pMask - The mask render region to use to indicate which bits of pBitmap
				needs to be plotted
	Purpose:	Plots the bitmap using the mask supplied.

********************************************************************************************/

SlowJobResult GRenderRegion::DrawMaskedBitmap(const DocRect &Rect, KernelBitmap* pBitmap, 
								  	  MaskedRenderRegion* pMask, ProgressDisplay *Progress)
{
//	RELTRACE( _T("In GRenderRegion::DrawMaskedBitmap\n"));
	
	// Make sure the world is in one piece
	if ((pBitmap==NULL) || (pMask==NULL))
		return SLOWJOB_FAILURE;

	// If mask coverage is 0% we do not need to do anything. The TRUE param indicates
	// that we don't care about exact coverage, which makes it return as soon as it realises
	// that there are pixel(s) that we'll have to plot
	if (pMask->FindCoverage(TRUE) == 0)
		return SLOWJOB_SUCCESS;

	// Guess we will need to be doing something...
	if (pBitmap->ActualBitmap == NULL)
		return SLOWJOB_FAILURE;

	// Get the 'Actual' windows Bitmap
	CWxBitmap *WinBM = (CWxBitmap*)pBitmap->ActualBitmap;

	// Is it valid ?
	if ((WinBM->BMInfo==NULL) || (WinBM->BMBytes==NULL))
		return SLOWJOB_FAILURE;

	// Remember the Size of the Bitmap (in pixels)
	INT32 Width  = WinBM->GetWidth();
	INT32 Height = WinBM->GetHeight();

	if ((Width == 0) || (Height == 0))
		// Error - bitmap has no dimension
		return SLOWJOB_FAILURE;

	// Setup the Gavin control points
	POINT PGram[3];
	PGram[0].x = Rect.lo.x;	PGram[0].y = Rect.lo.y;
	PGram[1].x = Rect.hi.x;	PGram[1].y = Rect.lo.y;
	PGram[2].x = Rect.lo.x;	PGram[2].y = Rect.hi.y;

	// --- Colour-correct and/or Contone the bitmap as necessary
	RGBQUAD *Palette = NULL;

	// Do the colour correction. This may produce a new pointer in Palette,
	// which we should CCFree() when we are done with it - see the end of this function
//	ColourCorrectBitmap(NULL, WinBM->BMInfo, &Palette);

	// If it didn't create a new palette for us, then we'll use the original one
//	if (Palette == NULL)
		Palette = WinBM->BMInfo->bmiColors;

	DWORD Style = 0x0040;		// GDraw bitmap-plotting style flagword

	BOOL bForceToWhite = FALSE;
	BOOL bClearBitmapConv = FALSE;
	BYTE* pSepTables = NULL;
//	BGR *pCyanSepTable = NULL;
//	BGR *pMagentaSepTable = NULL;
//	BGR *pYellowSepTable = NULL;
//	BGR *pBlackSepTable = NULL;
//	BYTE *pUnderColourRemovalTable = NULL;
//	BYTE *pBlackGenerationTable = NULL;
//	BGR* pTable = NULL;

	// --- Add Separation Style bits as approriate to the current colour separation mode
	// Currently, the only "separation" option we use is composite print preview
	// SepStyle for the SetBitmapFill functions are always in the MS byte of Style
	if (WinBM->GetBPP() > 8)	// Only needed for deep bitmaps
	{
		// If we've got a valid colour plate and it is a composite preview
		if (RenderView->GetColourPlate() != NULL &&
			!RenderView->GetColourPlate()->IsDisabled())
		{
			if (RenderView->GetColourPlate()->GetType() == COLOURPLATE_COMPOSITE)
			{
				// Find the colour manager (if there is one), and ask it for the 15-bit RGB
				// printer colour matching lookup table
PORTNOTE("cms", "Disabled XaraCMS")
#ifndef EXCLUDE_FROM_XARALX
				XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();
				if (lpCMSMan != NULL)
				{
					DWORD *MatchTable = lpCMSMan->GetPaperViewTable();
					ERROR3IF(MatchTable == NULL, "Can't create/find colour matching table");
					if (MatchTable != NULL)
					{
						GetDrawContext()->SetBitmapConversionTable((BGR *)MatchTable);
						bClearBitmapConv = TRUE;
						// Bits 3 and 4 of bits 16-23 of Style specifies the conversion algorithm
						// Make sure it only affects the 2 bits
						Style |= (BitmapConversion & 3) << 19;
					}
				}
#endif
			}
			else if (RenderView->GetColourPlate()->GetType() == COLOURPLATE_SPOT)
			{
				bForceToWhite = TRUE;
			}
			else if (RenderView->GetColourPlate()->GetType() != COLOURPLATE_NONE)
			{
//				DWORD Plate = RenderView->GetColourPlate()->GetType() - COLOURPLATE_CYAN;
				pSepTables = (BYTE *) CCMalloc(5 * 256 * sizeof(BYTE));
				if (pSepTables != NULL)
				{
					String_256 PrintProfile;
PORTNOTE("cms", "Disabled XaraCMS")
#ifndef EXCLUDE_FROM_XARALX
					XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();
					if (lpCMSMan)
						lpCMSMan->GetPrinterProfile(&PrintProfile);
#endif
					ColourContextCMYK *cc = new ColourContextCMYK(RenderView, &PrintProfile);
					if (cc->GetProfileTables(pSepTables))
					{
						WinBM->ColourSeparate32to32(RenderView->GetColourPlate(), pSepTables);
					}
					delete cc;
				}
			}
		}
	}

	if (bForceToWhite)
	{
		// First we need to set all the pixels as transparent white
		DWORD* pPixel = (DWORD*)WinBM->BMBytes;
		for (INT32 y = 0; y < Height; y++)
		{
			for (INT32 x = 0; x < Width; x++)
			{
				*(pPixel++) = 0xFFFFFFFF;
			}
		}

		// Then we copy the mask information in
		MaskRegion MaskInfo;
		pMask->GetFirstMaskRegion(&MaskInfo);
		while (MaskInfo.Length != 0)
		{
//			MaskInfo.Length; // What was this meant to do? AMB
			pPixel = ((DWORD*)WinBM->BMBytes) + (MaskInfo.y * Width) + MaskInfo.x;
			for (INT32 x = 0; x < MaskInfo.Length; x++)
			{
				*(pPixel++) = 0xFFFFFF;
			}

			// Find the next bit of scan line to plot
			pMask->GetNextMaskRegion(&MaskInfo);
		}
	}
	else
	{
		// First we need to set all the pixels as transparent
		DWORD* pPixel = (DWORD*)WinBM->BMBytes;
		for (INT32 y = 0; y < Height; y++)
		{
			for (INT32 x = 0; x < Width; x++)
			{
				*(pPixel++) |= 0xFF000000;
			}
		}

		// Then we copy the mask information in
		MaskRegion MaskInfo;
		pMask->GetFirstMaskRegion(&MaskInfo);
		while (MaskInfo.Length != 0)
		{
//			MaskInfo.Length; // What was this meant to do?
			pPixel = ((DWORD*)WinBM->BMBytes) + (MaskInfo.y * Width) + MaskInfo.x;
			for (INT32 x = 0; x < MaskInfo.Length; x++)
			{
				*(pPixel++) &= 0xFFFFFF;
			}

			// Find the next bit of scan line to plot
			pMask->GetNextMaskRegion(&MaskInfo);
		}
	}

	BYTE *BitmapBits = WinBM->BMBytes;

//	GetDrawContext()->SetColour(0x00FF0000);
//	WinRect	DrawRect;
//	DrawRect.left = Rect.lox;
//	DrawRect.right = Rect.hix;
//	DrawRect.top = Rect.hiy;
//	DrawRect.bottom = Rect.loy;
//	GetDrawContext()->FillRectangle(&DrawRect);

	// Plot the Bitmap
	GetDrawContext()->PlotBitmap(	&(WinBM->BMInfo->bmiHeader),
					BitmapBits,
					Style,
					PGram,
					Palette,
					NULL, NULL, NULL,
					NULL
					);

	HaveRenderedSomething = TRUE;

	// Free any memory used for colour-corrected bitmap palettes.
	// If this pointer doesn't point at the original palette, then it has
	// been temporarily allocated by ColourCorrectBitmap, above.
	if (Palette != WinBM->BMInfo->bmiColors)
		CCFree(Palette);

	if (bClearBitmapConv)
	{
		GetDrawContext()->SetBitmapConversionTable(NULL);
	}

	if (pSepTables)
	{
		GetDrawContext()->SetSeparationTables();	// Defaults to setting everything to NULL
		CCFree(pSepTables);
	}
/*
	if (pCyanSepTable)
		CCFree(pCyanSepTable);
	if (pMagentaSepTable)
		CCFree(pMagentaSepTable);
	if (pYellowSepTable)
		CCFree(pYellowSepTable);
	if (pBlackSepTable)
		CCFree(pBlackSepTable);
	if (pUnderColourRemovalTable)
		CCFree(pUnderColourRemovalTable);
	if (pBlackGenerationTable)
		CCFree(pBlackGenerationTable);
*/
	return SLOWJOB_SUCCESS;
}




/********************************************************************************************

>	virtual CCachedBitmap GRenderRegion::GrabBitmap(DocRect* rectGrab, LPBITMAPINFO* plpBitmapInfo, LPBYTE* plpBits)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/2004
	Inputs:		rectGrab		- the rectangle to grab as a new bitmap
	Outputs:	plpBitmapInfo	- address of pointer to BITMAPINFO structure
				plpBits			- address of pointer to bitmap data
	Returns:	TRUE			if grab worked
				FALSE			otherwise
	Purpose:	Plots the bitmap using the mask supplied.

********************************************************************************************/

BOOL GRenderRegion::GrabBitmap(DocRect* rectGrab, LPBITMAPINFO* plpBitmapInfo, LPBYTE* plpBits)
{
	//							Alloc bitmap mem
	LPBITMAPINFO	bmInfo = NULL;
	LPBYTE			bmBits = NULL;

	// calculate their intersection.
	*rectGrab	= rectGrab->Intersection(CurrentClipRect);
	if (rectGrab->IsEmpty() || !rectGrab->IsValid())
		return FALSE;

	WinRect Rect = CalculateWinRect(RenderMatrix, *rectGrab, PixelsPerInch);
	UINT32 PixelWidth = Rect.width;
	UINT32 PixelHeight = Rect.height;

	bmInfo = AllocDIB( PixelWidth, PixelHeight, 32, &bmBits, NULL, FALSE); // don't use limited mem manager
//	bmInfo = AllocDIB( PixelWidth, PixelHeight, 32, &bmBits, NULL, TRUE); // use limited mem manager

	if(!bmInfo || !bmBits)
	{
		ENSURE(FALSE,"GRenderRegion::GrabBitmap couldn't create bitmap.");
		return FALSE;
	}

	LPBITMAPINFOHEADER bh = &bmInfo->bmiHeader;
	bh->biCompression	= BI_RGB;												// see GDraw docs on GDraw_SetDIBitmap
	bh->biXPelsPerMeter	= PixelsPerMeterAtCurrentViewScale(ScaleFactor);		// Not used by GDraw - just for completeness sake
	bh->biYPelsPerMeter	= PixelsPerMeterAtCurrentViewScale(ScaleFactor);		// Not used by GDraw

	// Blit the current bitmap data into the new bitmap
	WinRect rectBack = CalculateWinRect(CurrentClipRect);
	WinRect rectWinGrab = CalculateWinRect(*rectGrab);
	INT32 left = rectWinGrab.x - rectBack.x;
	INT32 top = rectBack.GetBottom() - rectWinGrab.GetBottom();
	DIBUtil::CopyBitmapSection(pBitmapInfo, pBits, bmInfo, bmBits, top, left);

	*plpBitmapInfo = bmInfo;
	*plpBits = bmBits;

	return TRUE;
}




/********************************************************************************************

>	DocRect GRenderRegion::GetChangedRect()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/06/2004
	Inputs:		-
	Outputs:	-
	Returns:	DocRect containing changed rect so far in this RenderRegion
	Purpose:	To get the changed rectangle so far
	SeeAlso:	OSRenderRegion::DocRectToWin;

********************************************************************************************/

DocRect GRenderRegion::GetChangedRect()
{
	RECT UsedRect;
	DocRect dr;

	// Gavin returns his rectangles the right way up - the mathematical way up!
	// he returns x1,y1,x2,y2
	//
	BOOL ok = GetDrawContext()->GetChangedBBox(&UsedRect);
	if (!(ok &&
		(UsedRect.right > UsedRect.left) &&
		(UsedRect.bottom > UsedRect.top)
		)
	   )
	{
		WinRect winr = WinRect(UsedRect.left, UsedRect.top, UsedRect.right, UsedRect.bottom);
		OilRect oilr = winr.ToOil( GetRenderView() );
		dr = oilr.ToDoc( GetRenderSpread(), GetRenderView() );
	}

	dr = OSRenderRegion::WinRectToDoc(RenderMatrix, WinRect(UsedRect.left, UsedRect.top, UsedRect.right, UsedRect.bottom), PixelsPerInch);

	return dr;
}




/********************************************************************************************

>	Capture* GRenderRegion::StartCapture(CCObject* pOwner,
								 DocRect CaptureRect,
								 CAPTUREINFO cinfo,
								 BOOL bTransparent,
								 BOOL bCaptureBackground,
								 double dPPI,
								 NodeRenderableInk* pDirectSupplier = NULL,
								 )

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/2004
	Inputs:		pOwnerNode		- Node in tree which "owns" this capture
				CaptureRect		- The rectangle to capture
				ctype			- The type of capture required
				bTransparent	- The capture could be transparent RGBT
				bCaptureBackground	- The capture should include the state of the background
				dPPI			- The resolution at which capture should be done
				pDirectSupplier	- Node in tree which supplies a bitmap directly into this capture
	Outputs:	-
	Returns:	TRUE if a Capture was successfully set up
	Purpose:	To start a bitmap capture of whatever is subsequently rendered
	SeeAlso:	GRenderRegion::StopCapture

********************************************************************************************/

Capture* GRenderRegion::StartCapture(CCObject* pOwner,
								 DocRect CaptureRect,
								 CAPTUREINFO cinfo,
								 BOOL bTransparent,
								 BOOL bCaptureBackground,
								 double dPPI,
								 NodeRenderableInk* pDirectSupplier
								 )
{
	ENSURE(cinfo.caType==ctNESTABLE, "We only support nestable captures at the moment\n");

	// 1) alloc new mem, and setup state variables for new offscreen rendering
		// mem alloced = bmp
		// vars setup = docrect, gmatrix
	// 2) save current
	// 3) change to new offscreen state

	// If there's already a capture running and this one is nested but wants to change 
	// the transparency type then we must notify existing captures of the change...
	Capture* pCurrentCapture = GetTopCapture();
	if (pCurrentCapture!=NULL && cinfo.caType==ctNESTABLE && !cinfo.caFlags.GrabRendered && pCurrentCapture->IsTransparent() && !bTransparent)
	{
		ChangeCapture(cinfo, bTransparent, bCaptureBackground);
	}

	// If the caller didn't specify the capture resolution in pixels per inch
	// Then set it to the default value of the render region
	// And use the default matrix in our setup calculations
	Matrix CaptureMatrix;
	if (dPPI==0)
	{
//		dPPI = PixelsPerInch;

		// Find current top bitmap capture and copy its res and res scale control flag
		Capture* pCap = GetTopCaptureBitmap();
		ERROR2IF(pCap==NULL, NULL, "There MUST be a bitmap capture in the stack - the master capture at least");
		dPPI = pCap->dPixelsPerInch;
		cinfo.caFlags.CalcScaledPixWidth = pCap->info.caFlags.CalcScaledPixWidth;

		CaptureMatrix = RenderMatrix;
	}
	else
	{
		// If the caller specified a particular DPI we will assume he wants the capture
		// to grab the rendered objects at 100% scale...
		// No need to construct a new matrix in a complex way - the clip rect and
		// object are all stored in doccoords.
//		CaptureMatrix = Identity;
	}

	ERROR3IF(cinfo.caFlags.CalcScaledPixWidth && dPPI!=GetTopCaptureBitmap()->dPixelsPerInch,
				"StartCapture asked to calc ScaledPixelWidth when PPI differs from base PPI - are you sure about this?");

	ERROR3IF(cinfo.caFlags.CalcScaledPixWidth && dPPI!=GetTopCaptureBitmap()->dPixelsPerInch,
				"StartCapture asked to compute scaled pixel width in capture when res differs from current res");

	// In non-Master Captures, make sure that we use a default colour context
	// to prevent multiple colour corrections when rendering bitmaps in "print preview"
	// type modes...
	ColourContext* pColContext = NULL;
	if (!cinfo.caFlags.Master)
	{
		BOOL bOldForce = RenderView->SetForceDefaultColourContexts();
		pColContext = RenderView->GetColourContext(COLOURMODEL_RGBT);
		RenderView->SetForceDefaultColourContexts(bOldForce);
	}
	else
	{
		// Must get RenderRegion's current colour context for Master Capture
		pColContext = CurrentColContext;
	}

	// Create a new capture instance...
	Capture* pNewCapture = new Capture(pOwner, cinfo, CaptureMatrix, CaptureRect, dPPI, pColContext);
	ENSURE(!pNewCapture->IsMaster(), "Can only have one master capture!");

	// ------------------------------------
	// If we only want to grab the rendering when it's complete then we don't need to make a bitmap
	if (pNewCapture && pNewCapture->info.caFlags.GrabRendered)
	{
		// Put this new capture on the top of the capture stack
		ENSURE(pNewCapture->lpBitmapInfo==NULL && pNewCapture->lpBits==NULL, "GrabRendered capture owns a bitmap!");
		m_CaptureStack.Push(pNewCapture);
		return pNewCapture;
	}

	// ------------------------------------
	// Optimisation to cut down the number of separate capture bitmaps that are used:
	//
	// If the caller thinks that a node can supply a bitmap directly then use that
	// instead of capturing
	if (pNewCapture && pNewCapture->info.caFlags.AllowDirect && pDirectSupplier)
	{
		BOOL bDirectCapture = SetupDirectCapture(pNewCapture, pDirectSupplier);
		if (bDirectCapture)
		{
			ENSURE(pNewCapture->lpBitmapInfo==NULL && pNewCapture->lpBits==NULL, "AllowDirectCapture capture owns a bitmap!");
			m_CaptureStack.Push(pNewCapture);
			return pNewCapture;
		}
	}

	// ------------------------------------
	// Grab a bitmap for the capture...
	LPBITMAPINFO	bmInfo = NULL;
	LPBYTE			bmBits = NULL;

	// NB Millipoint endpoints don't necessarily fall on exact pixel boundaries.
	// Hence use following function to find out which pixels the millipoints fall on
	WinRect Rect = CalculateWinRect(CaptureMatrix, CaptureRect, dPPI);
	UINT32 PixelWidth = Rect.width;
	UINT32 PixelHeight = Rect.height;
	if (PixelWidth==0 || PixelHeight==0 || 
		PixelWidth*PixelHeight>256*1024*1024 ||
		PixelWidth >GetDrawContext()->GetMaxBitmapWidth() ||
		PixelHeight>GetDrawContext()->GetMaxBitmapDepth())
	{
		delete pNewCapture;
		return NULL;
	}

	// This code gets the pixelised version of the capture rect
	// and stores that in the Capture to ensure ULTIMATE accuracy when blitting the bitmap
	// using RenderBits
	DocRect rectPix = OSRenderRegion::WinRectToDoc(CaptureMatrix, Rect, dPPI);
	ENSURE(rectPix.IsValid(), "Pixelised rect is not valid in StopCapture");
	pNewCapture->CaptureRect = rectPix;

	bmInfo = AllocDIB(PixelWidth, PixelHeight, 32, &bmBits, NULL, FALSE);
	if (bmInfo==NULL || bmBits==NULL)
	{
		ENSURE(FALSE,"Couldn't create bitmap for offscreen rendering.");
		return NULL;
	}

if (dPPI!=PixelsPerInch)
{
	ENSURE(!bCaptureBackground, "Sorry StartCapture can't scale blit background into capture yet\n");

	double xppm = dPPI * INCHES_PER_METRE;
	double yppm = dPPI * INCHES_PER_METRE;
	bmInfo->bmiHeader.biXPelsPerMeter = (INT32)(xppm + 0.5);
	bmInfo->bmiHeader.biYPelsPerMeter = (INT32)(yppm + 0.5);
}

	if (bTransparent)
		bmInfo->bmiHeader.biCompression = 0x80000001;

//	pNewCapture->AllocateDIB(bTransparent);
	pNewCapture->SetDIB(bmInfo, bmBits);

	// Initialise the bitmap contents...
	if (bCaptureBackground && dPPI==PixelsPerInch)
	{
		// Blit the background from the main bitmap
		WinRect rectBack = CalculateWinRect(CurrentClipRect);
		WinRect rectOffs = CalculateWinRect(CaptureRect);
		INT32 left = rectOffs.x - rectBack.x;
		INT32 top = rectBack.GetBottom() - rectOffs.GetBottom();
		DIBUtil::CopyBitmapSection(pBitmapInfo, pBits, pNewCapture->lpBitmapInfo, pNewCapture->lpBits, top, left);
	}
	else
	{
		BYTE* pSetBits = (BYTE*)pNewCapture->lpBits;

		// ensure all bitmap memory is initialised (ie don't use the 
		// bogus info from the bmp header dimensions)
		UINT32 BmpSize = GetDIBBitsSize( &(pNewCapture->lpBitmapInfo->bmiHeader) );
		UINT32 Index = 0;

		ENSURE(pNewCapture->lpBitmapInfo->bmiHeader.biBitCount==32,"Non-32bpp bitmap in offscreen rendering!");

		if (bTransparent)
			while (Index < BmpSize)
			{
				// initial values for using internal alpha channelled format
				// fully transparent black bacground

				pSetBits[Index++] = 0x00;
				pSetBits[Index++] = 0x00;
				pSetBits[Index++] = 0x00;
				pSetBits[Index++] = 0xFF;
			}
		else
			while (Index < BmpSize)
			{
				// fully opaque white background
				pSetBits[Index++] = 0xFF;
				pSetBits[Index++] = 0xFF;
				pSetBits[Index++] = 0xFF;
				pSetBits[Index++] = 0x00;
			}
	}

	// Put this new capture on the top of the capture stack
	m_CaptureStack.Push(pNewCapture);

	// Switch rendering to work into this new capture
	SetRenderToCapture(pNewCapture);

	// We need to clear the current clipregion here because when the capture is stopped
	// and plotted into the parent capture it will itself be clipped
	// If the cached bitmap is to be used to speed up dragging then we want the cached
	// image to be unclipped
	if (cinfo.caFlags.UnClip)
	{
		SaveContext();
		ClipRegionAttribute* pClipRegion = new ClipRegionAttribute();	// Default ClipRegion has no path, thus infinite
		if (pClipRegion)
		{
// Don't try to record clip region attr because during forced deletion
// it will be deleted before the destructor tries to restore all attributes
// contexts down to the ground.
// Instead mark the attr as Temporary so that it will be deleted when
// StopCapture calls RestoreContext below.
//			pNewCapture->SetClipRegionAttrValue(pClipRegion);
//			SetClipRegion(pClipRegion, FALSE);
			SetClipRegion(pClipRegion, TRUE);		// Delete automatically when it goes out of scope
		}
	}

	return pNewCapture;
}




/********************************************************************************************

>	BOOL GRenderRegion::StopCapture(CCObject* pOwner,
									BOOL bRender = TRUE,
									BOOL bReleaseBitmap = FALSE,
									LPBITMAPINFO* plpBitmapInfo = NULL,
									LPBYTE* plpBits = NULL,
									DocRect* pCaptureRect = NULL,
									Matrix* pmatTransform = NULL,
									double* pdResolution = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/2004
	Inputs:		pEndNode	- Node in tree which "owns" this capture
	Outputs:	-
	Returns:	TRUE if the Capture resulted in a bitmap being usefully created
	Purpose:	To stop a bitmap capture
	SeeAlso:	GRenderRegion::StartCapture

********************************************************************************************/

BOOL GRenderRegion::StopCapture(CCObject* pOwner,
								BOOL bRender,
								BOOL bReleaseBitmap,
								LPBITMAPINFO* plpBitmapInfo,
								LPBYTE* plpBits,
								DocRect* pCaptureRect,
								Matrix* pmatTransform,
								double* pdResolution)
{
	Capture* pCapture = GetTopCapture();
	if (pCapture==NULL)
		return FALSE;

	if (pCapture->IsMaster())
	{
		ERROR2(FALSE, "StopCapture called on the master capture!\n");
		return FALSE;
	}

	// If we own the current rendering bitmap then we can do useful things with it...
	BOOL bOK = TRUE;
	if (pCapture->OwnsBitmap())
	{
		// We may need to restore the previous clipping state
		if (pCapture->IsUnclipped())
		{
			RestoreContext();
		}

		// Ensure output values are cleared before we go any further
		if (plpBitmapInfo && plpBits)
		{
			*plpBitmapInfo = NULL;
			*plpBits = NULL;
		}

		if (pCaptureRect)
		{
			*pCaptureRect = DocRect(0,0,0,0);
		}

		// Pop this capture off the stack
		m_CaptureStack.Pop();

		// And Return rendering to the previous capture
		SetRenderToCapture(GetTopCaptureBitmap(), TRUE);

		// Get convenient access to capture details
		LPBITMAPINFO lpCapturedBitmapInfo = pCapture->lpBitmapInfo;
		LPBYTE lpCapturedBits = pCapture->lpBits;
		DocRect rectCaptured = pCapture->CaptureRect;
		ENSURE(rectCaptured.IsValid(), "Invalid rect in StopCapture");

		// We have to normalise the bitmap if it was transparent because GDraw
		// produces a pre-multiplied format that it CAN'T render!
		if (lpCapturedBitmapInfo && lpCapturedBits && lpCapturedBitmapInfo->bmiHeader.biCompression==0x80000001)
		{
			GetDrawContext()->ConvertBitmap(
										&lpCapturedBitmapInfo->bmiHeader, lpCapturedBits,
										&lpCapturedBitmapInfo->bmiHeader, lpCapturedBits,
										0
									);
			// Leave biCompression as 80000001... Otherwise cache system won't know a transp from a non-transp
		}

		// If the caller wants the resulting bitmap back then give it to him...
		if (plpBitmapInfo && plpBits && pCaptureRect && !bReleaseBitmap)
		{
			*plpBitmapInfo = lpCapturedBitmapInfo;
			*plpBits = lpCapturedBits;
			*pCaptureRect = rectCaptured;
		}

		// If required, render the captured bitmap back to the main rendering bitmap
		if (bRender && lpCapturedBitmapInfo && lpCapturedBits)
		{
			// Assume that a capture is always offscreen and will always want to
			// blit back to screen at the moment...
			RenderBits(lpCapturedBitmapInfo, lpCapturedBits, rectCaptured);
		}

		// If required, release the captured bitmap because no one needs to hold on to it...
		if (bReleaseBitmap)
			pCapture->FreeDIB();

		delete pCapture;
	}
	else
	{
		// Ensure output values are cleared before we go any further
		if (plpBitmapInfo && plpBits)
		{
			*plpBitmapInfo = NULL;
			*plpBits = NULL;
		}

		// Else return a 24bpp grab to the caller (if he wants a bitmap)...
		if (pCaptureRect && plpBitmapInfo && plpBits && pCapture->info.caFlags.GrabRendered)
		{
			ENSURE(pCaptureRect->IsValid(), "Invalid rect in StopCapture");

			// Semi-bodge:
			// If we capture colour corrected bitmap then things will go wrong when
			// the plot this bitmap in future because it will get colour corrceted
			// again.
			// If we knew that the stored bitmap had been corrceted we could avoid further
			// correction - but we don't store that info with cached bitmaps
			// So, for simplicity we will just prevent cacheing in that situation
			//
			Capture* pMasterCapture = GetMasterCapture();
			ENSURE(pMasterCapture, "Can't find master capture!");
			if (pMasterCapture->pColContext == ColourContext::GetGlobalDefault(COLOURMODEL_RGBT))
				bOK = GrabBitmap(pCaptureRect, plpBitmapInfo, plpBits);			// NOTE! may change CaptureRect!
			else
				bOK = FALSE;
		}

		NodeRenderableInk* pSupplier = pCapture->GetDirectSupplier();
		if (pCapture->info.caFlags.Direct && pSupplier)
		{
			bOK = pSupplier->GetDirectBitmap(this, plpBitmapInfo, plpBits, pCaptureRect, pmatTransform, pdResolution);
		}

		// Pop the capture off the stack now
		// (If it doesn't own the current bitmap then it must have already released it)
		m_CaptureStack.Pop();

		delete pCapture;
	}

#ifdef DEBUG_ATTACH_CAPTURED_BMP
{
	if (*plpBitmapInfo && *plpBits)
	{
		// Create a Kernel bitmap from the bmp data.
		CWxBitmap* wBitmap			= new CWxBitmap(*plpBitmapInfo, *plpBits);
		KernelBitmap* OffscreenBitmap	= new KernelBitmap(wBitmap,TRUE);
		OffscreenBitmap->AttachDebugCopyToCurrentDocument("Captured Bitmap");
		wBitmap->BMBytes = ((CWxBitmap*)OILBitmap::Default)->BMBytes;
		delete OffscreenBitmap;
	}
}
#endif

	// reset the ChangedBBox
	if (MasterCaptureIsCurrent())
	{
		// Restored from rendering into offscreen bmp to GRenderRegions orignal invalid bmp
		// Reset the ChangedBBox so full blitting occurs to update invalid region
		RECT bbox;
		bbox.left = 0;
		bbox.right = WRect.width;
		bbox.top = 0;
		bbox.bottom = WRect.height; // NB upside down (GDI coords)

		if (!GetDrawContext()->SetChangedBBox(&bbox))
		{
			ERROR3("GDraw_SetChangedBBox failed");
		}
	}

	return bOK;
}

	
	
	
/********************************************************************************************

>	BOOL GRenderRegion::ChangeCapture(CAPTUREINFO cinfo,
									BOOL bTransparent,
									BOOL bCaptureBackground)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/2004
	Inputs:		-
	Outputs:	-
	Purpose:	To inform the capture stack that things (transparency) have changed

********************************************************************************************/

BOOL GRenderRegion::ChangeCapture(CAPTUREINFO cinfo,
								BOOL bTransparent,
								BOOL bCaptureBackground)
{
	// If there's a current RGBT capture active and we are changing to a non-transparent capture
	// Then mark that Capture as no longer being transparent
	// Blit whatever we have so far into the parent bitmap
	// Tell all enclosing captures (further up the stack) that they can no longer be transparent
	// either
	Capture* pCurrentCapture = GetTopCapture();
	if (pCurrentCapture==NULL)
		return FALSE;

	if (pCurrentCapture->IsMaster())
	{
		ENSURE(m_CaptureStack.Size()==1, "Master Capture not bottom capture in ChangeCapture");
		return FALSE;
	}

	// If the current capture has same transparency setting than that requested
	// we can just exit as if we succeeded...
	if (bTransparent == pCurrentCapture->IsTransparent())
		return TRUE;

	ERROR2IF(bTransparent, FALSE, "ChangeCapture doesn't cope with changing UP to transparency yet");

	// Mark the current capture as being no longer transparent...
	ListT<Capture*>* pList = m_CaptureStack.GetListT();

	{
		// If any capture in the stack is locked to be transparent, no matter what, then
		// we should not change any of the existing captures...
		Capture* pCapture = NULL;
		ListT<Capture*>::Iterator it = pList->End();
		INT32 nListSize = pList->Size();
		for (INT32 i = 0; i < nListSize; i++)
		{
			pCapture = *(--it);

			// If the capture is locked transparent then we can stop all further processing
			if (pCapture->IsTransparent() && pCapture->info.caFlags.LockedTransparent)
				return TRUE;

			// If the master capture is transparent, treat as if it has LockedTransparent set
			if (pCapture->IsMaster() && pCapture->IsTransparent())
				return TRUE;
		}
	}

	{
		// We are allowed to remove this capture and others from the stack until we reach
		// an opaque capture...
		Capture* pCapture = NULL;
		ListT<Capture*>::Iterator it = pList->End();
		INT32 nListSize = pList->Size();
		for (INT32 i = 0; i < nListSize; i++)
		{
			pCapture = *(--it);

			// If the captured bitmap is non-transparent then we can stop changing captures now
			if (pCapture->IsMaster())
				break;
			if (!pCapture->IsTransparent())
				break;

			// If this capture can be collapsed
			if (pCapture->info.caFlags.CollapseToMaster)
			{
				// First restore the previous state (so we can draw into the original bitmap inside the GoingOutOfScope function)
				// and free the offscreen state stack memory which held this info
				Capture* pPrevCapture = GetTopCaptureBitmap(pCapture);
				ENSURE(pPrevCapture, "Failed to find parent capture with bitmap!");
				ENSURE(pPrevCapture->OwnsBitmap(), "At least one parent capture should own a bitmap!");

				// We may need to restore the previous clipping state
				if (pCapture->info.caFlags.UnClip)
				{
					RestoreContext();
					pCapture->info.caFlags.UnClip = FALSE;
				}

				SetRenderToCapture(pPrevCapture);

				LPBITMAPINFO lpCapturedBitmapInfo = pCapture->lpBitmapInfo;
				LPBYTE lpCapturedBits = pCapture->lpBits;
				DocRect rectCaptured = pCapture->CaptureRect;

				// We have to normalise the bitmap if it was transparent because GDraw
				// produces a pre-multiplied format that it CAN'T render!
				if (lpCapturedBitmapInfo && lpCapturedBits && lpCapturedBitmapInfo->bmiHeader.biCompression==0x80000001)
				{
					GetDrawContext()->ConvertBitmap(
												&lpCapturedBitmapInfo->bmiHeader, lpCapturedBits,
												&lpCapturedBitmapInfo->bmiHeader, lpCapturedBits,
												0
											);
				}

				// Render the captured bitmap back to the main rendering bitmap
				if (lpCapturedBitmapInfo && lpCapturedBits)
				{
					RenderBits(lpCapturedBitmapInfo, lpCapturedBits, rectCaptured);
				}

				// Release this bitmap (marks capture as no longer owning a bitmap)
				pCapture->FreeDIB();
			}
			else if (pCapture->info.caFlags.LockedTransparent)
			{
				// Do nothing - we want to retain 32BPP even though someone wants less!
				// Rely on GDraw to make it's best attempt from now on!
				//
				// Break the loop here because since this capture is going to remain 32BPP
				// there's no point fiddling around with any of the others
				ERROR3("Shouldn't get here!");
				break;
			}
			else
			{
				// We must convert this capture into a non-transparent bitmap but we are not allowed
				// to collapse it back to the master bitmap.
				// To achieve this we will:
				// 1. Create a new bitmap with the same parameters as the current capture, except opaque
				// 2. Grab the background (and current nested capture state) into it
				// 3. Remove current capture from stack and replace it with the new one

				// 1.
				Capture* pNewCapture = new Capture(pCapture, TRUE);
				pNewCapture->SetTransparent(FALSE);

				// 2.
				SetRenderToCapture(pNewCapture);
				RenderCurrentCaptureState(TRUE);

#ifdef DEBUG_ATTACH_TCHANGE_BMP
{
	// Create a Kernel bitmap from the bmp data.
	CWxBitmap* wBitmap			= new CWxBitmap(pNewCapture->lpBitmapInfo, pNewCapture->lpBits);
	KernelBitmap* OffscreenBitmap	= new KernelBitmap(wBitmap,TRUE);
	OffscreenBitmap->AttachDebugCopyToCurrentDocument("Transp Change Capture Bitmap");
	wBitmap->BMBytes = ((CWxBitmap*)OILBitmap::Default)->BMBytes;
	delete OffscreenBitmap;
}
#endif
				// 3.
				ENSURE(!pNewCapture->IsMaster(), "Can only have one master capture!");
				ListT<Capture*>::Iterator itCap = it;		// Get temp iterator pointing at position of pCapture
				it = pList->Insert(itCap, pNewCapture);			// Insert new capture before pCapture
				pList->Erase(itCap);						// Remove pCapture from stack

				pCapture->FreeDIB();
				delete pCapture;
			}
		}
	}

	SetRenderToCapture(GetTopCaptureBitmap());

	// reset the ChangedBBox
	if (MasterCaptureIsCurrent())
	{
		// Restored from rendering into offscreen bmp to GRenderRegions orignal invalid bmp
		// Reset the ChangedBBox so full blitting occurs to update invalid region
		RECT bbox;
		bbox.left = 0;
		bbox.right = WRect.width;
		bbox.top = 0;
		bbox.bottom = WRect.height; // NB upside down (GDI coords)

		if (!GetDrawContext()->SetChangedBBox(&bbox))
		{
			ERROR3("GDraw_SetChangedBBox failed");
		}
	}

	return TRUE;
}




/********************************************************************************************

>	BOOL GRenderRegion::SetupDirectCapture(Capture* pNewCapture, NodeRenderableInk* pSupplier)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/05/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the Direct capture was setup successfully
	Purpose:	To setup a direct capture if that is possible
	SeeAlso:	GRenderRegion::StartCapture

********************************************************************************************/

BOOL GRenderRegion::SetupDirectCapture(Capture* pNewCapture, NodeRenderableInk* pSupplier)
{
	// Checks:
	ERROR2IF(pNewCapture->info.caFlags.Master, FALSE, "Can't set Master capture for Direct capturing!");

	if (pSupplier==NULL || !pSupplier->CanSupplyDirectBitmap())
		return FALSE;

	// All checks passed OK, so allow this capture to be Direct!
	pNewCapture->info.caFlags.Direct = TRUE;
	pNewCapture->SetDirectSupplier(pSupplier);
	return TRUE;
}




/********************************************************************************************

>	BOOL GRenderRegion::RenderCurrentCaptureState(BOOL bStartFromMaster)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/2004
	Inputs:		-
	Outputs:	-
	Purpose:	Render the current state of the capture stack into the specified bitmap
				(For use when rendering is interrupted)
				(For use when capture must be collapsed from 32BPP to 24BPP without affecting
				the master bitmap)

********************************************************************************************/

BOOL GRenderRegion::RenderCurrentCaptureState(BOOL bStartFromMaster)
{
	if (MasterCaptureIsCurrent() && !TopCaptureIsDirect())
	{
		return FALSE;
	}

	// First, temporarilly capture rendering by
	// setting up a local capture referring to the DisplayBitmap
	// so that RenderBits operates into it - not the master bitmap
//	SetRenderToCapture(pDisplayCapture);
	{
		// In this scope we are rendering into the Display bitmap...
		LPBITMAPINFO lpCapturedBitmapInfo = pBitmapInfo;
		LPBYTE lpCapturedBits = pBits;
		DocRect rectCaptured = CurrentClipRect;

		// Iterate through the capture stack from bottom to top, rendering whatever bitmap
		// state they have reached so far into the specified display bitmap
		//
		Capture* pCapture = NULL;
		ListT<Capture*>* pList = m_CaptureStack.GetListT();

		ListT<Capture*>::Iterator it = pList->Begin();
		INT32 nListSize = pList->Size();
		for (INT32 i = 0; i < nListSize; i++)
		{
			pCapture = *(it++);

			NodeRenderableInk* pSupplier = pCapture->GetDirectSupplier();
			if (pCapture->info.caFlags.Direct && pSupplier /*&& pCapture->info.caFlags.HasBeenSuppliedWithBitmap*/)
			{
				// This capture gets its bitmap directly from a supplier node
				// The fact that this capture exists means that the owner node
				// hasn't done it own processing yet but the supplier node
				// may have a bitmap ready to plot...
				pSupplier->RenderDirectBitmapState(this);
			}
			else
			{
				if (pCapture && pCapture->OwnsBitmap() && (i>0 || bStartFromMaster))
				{
					lpCapturedBitmapInfo = pCapture->lpBitmapInfo;
					lpCapturedBits = pCapture->lpBits;
					rectCaptured = pCapture->CaptureRect;

					// We have to normalise the bitmap if it was transparent because GDraw
					// produces a pre-multiplied format that it CAN'T render!
					// The bitmap is stil in use so we have to normalise a copy...
					if (lpCapturedBitmapInfo && lpCapturedBits && lpCapturedBitmapInfo->bmiHeader.biCompression==0x80000001)
					{
						// Make a temp copy
						DIBUtil::CopyBitmap(lpCapturedBitmapInfo, lpCapturedBits, &lpCapturedBitmapInfo, &lpCapturedBits);
						lpCapturedBitmapInfo->bmiHeader.biCompression = 0x80000001;		// CopyBitmap corrupts biCompression!

						GetDrawContext()->ConvertBitmap(
													&lpCapturedBitmapInfo->bmiHeader, lpCapturedBits,
													&lpCapturedBitmapInfo->bmiHeader, lpCapturedBits,
													0
												);
						// Render the captured bitmap to the supplied display bitmap
						if (lpCapturedBitmapInfo && lpCapturedBits)
						{
TRACEUSER( "Phil", _T("RCCS: Rendering Collapsed %x %s\n"), pCapture, pCapture->GetRuntimeClass()->m_lpszClassName);
							RenderBits(lpCapturedBitmapInfo, lpCapturedBits, rectCaptured);
						}

						// Release the temp copy
						::FreeDIB(lpCapturedBitmapInfo, lpCapturedBits, NULL, FALSE);
					}
					else
					{
						// Render the captured bitmap to the supplied display bitmap
						if (lpCapturedBitmapInfo && lpCapturedBits)
						{
TRACEUSER( "Phil", _T("RCCS: Rendering non-transp %x %s\n"), pCapture, pCapture->GetRuntimeClass()->m_lpszClassName);
							RenderBits(lpCapturedBitmapInfo, lpCapturedBits, rectCaptured);
						}
					}
				}
			}
		}
	}

	// Reset the state back to normal
//	SetRenderToCapture(GetTopCapture());

	// Update changed Bounding Box
	RECT bbox;
	bbox.left = 0;
	bbox.right = WRect.width;
	bbox.top = 0;
	bbox.bottom = WRect.height; // NB upside down (GDI coords)

	if (!GetDrawContext()->SetChangedBBox(&bbox))
	{
		ERROR3("GDraw_SetChangedBBox failed");
	}

	return TRUE;
}




/********************************************************************************************

>	Capture* GRenderRegion::GetTopCaptureBitmap(Capture* pFromCapture = NULL) const

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/07/2004
	Inputs:		-
	Outputs:	-
	Purpose:	Search down the Capture stack from the top or from the capture specified
				until we find one that owns a bitmap. The master capture at the bottom
				of the stack should ALWAYS own a bitmap!

********************************************************************************************/

Capture* GRenderRegion::GetTopCaptureBitmap(Capture* pFromCapture) const
{
	const ListT<Capture*>* pList = m_CaptureStack.GetListT();

	Capture* pCapture = NULL;
	ListT<Capture*>::ConstIterator it = pList->End();
	INT32 nListSize = pList->Size();
	INT32 i=0;

	if (pFromCapture)
	{
		for (i = 0; i < nListSize; i++)
		{
			pCapture = *(--it);
			if (pCapture == pFromCapture)
				break;
		}
	}

	for (/*use i from above*/; i < nListSize; i++)
	{
		pCapture = *(--it);
		if (pCapture->OwnsBitmap())
			return pCapture;
	}

	return NULL;
}




/********************************************************************************************

>	BOOL GRenderRegion::DumpCaptureStack()

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/07/2004
	Inputs:		-
	Outputs:	-
	Purpose:	Render the current state of the capture stack into the specified bitmap
				(For use when rendering is interrupted)

********************************************************************************************/

#ifdef _DEBUG
BOOL GRenderRegion::DumpCaptureStack()
{
	if (m_CaptureStack.Empty())
	{
		return FALSE;
	}

	TRACE( _T("Capture stack:\n"));

//	LPBITMAPINFO lpCapturedBitmapInfo = pBitmapInfo;
//	LPBYTE lpCapturedBits = pBits;
	DocRect rectCaptured = CurrentClipRect;

	Capture* pCapture = m_CaptureStack.Top();
	ListT<Capture*>* pList = m_CaptureStack.GetListT();

	ListT<Capture*>::Iterator it = pList->End();
	INT32 nListSize = pList->Size();
	for (INT32 i = 0; i < nListSize; i++)
	{
		pCapture = *(--it);

//		CCObject* pOwner = pCapture->GetOwner();

//		TRACE( _T("  %x Capture %x %s %d\n"), pCapture, pOwner, pOwner->GetRuntimeClass()->m_lpszClassName, pCapture->IsTransparent());
	}

	return TRUE;
}
#endif




/********************************************************************************************

>	BOOL GRenderRegion::SetRenderToCapture(Capture* pCapture, BOOL ApplyClipRegion = FALSE, BOOL bSetBitmap = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/07/2004
	Inputs:		pCapture - Pointer to capture object which is to receiev all further rendering
	Outputs:	-
	Returns:	TRUE if worked
	Purpose:	To connect this renderregion to a bitmap along with clipping and view
				transformation information
				(a "capture")
	SeeAlso:	GRenderRegion::StartCapture

********************************************************************************************/

BOOL GRenderRegion::SetRenderToCapture(Capture* pCapture, BOOL ApplyClipRegion, BOOL bSetBitmap)
{
	ERROR2IF(pCapture==NULL, FALSE, "SetRenderToCapture passed NULL capture pointer");
	// We may be called on captures that don't own a bitmap to restore
	// their other settings into the RenderRegion.
	// So we must cope with the bitmap pointers being NULL
	if (pCapture->lpBitmapInfo && pCapture->lpBits && bSetBitmap)
	{
		LPBITMAPINFOHEADER pbmiHeader = &(pCapture->lpBitmapInfo->bmiHeader);

		// This test just here to notify developers of old legacy use of GDraw
		// at anything less than 24BPP
//		if (pbmiHeader->biBitCount < 24)
//		{
//			TRACE( _T("FYI: Attempt to use < 24BPP bitmap with GDraw\n"));
//		}

		if (pbmiHeader->biWidth!=0 && pbmiHeader->biHeight!=0 && pbmiHeader->biBitCount!=0)
		{
			// default is 2 - see gdrawcon.h, gconsts.h
//			DWORD Format = (pbmiHeader->biBitCount == 8) ? 0 : 2;
//			if (!GetDrawContext()->SetBitmap(pbmiHeader, pCapture->lpBits, Format))
			if (!GetDrawContext()->SetBitmap(pbmiHeader, pCapture->lpBits))
			{
				ERROR3("GDraw_SetDIBitmap failed");
				return FALSE;
			}
		}
	}

	// Set GRenderRegion member variables as well
	//
	// Only set clip rect if bitmap actually changes
	// This allows routines that fiddle with the cliprect (RenderOptimalBitmapPhase)
	// to retain their fiddles while calling StartRender.
	if (pBitmapInfo != pCapture->lpBitmapInfo || pBits != pCapture->lpBits)
		CurrentClipRect	= pCapture->CaptureRect;

	RenderMatrix	= pCapture->CaptureMatrix;
	GMATRIX gmat = MakeGavinMatrix(RenderMatrix, CurrentClipRect, pCapture->dPixelsPerInch, pCapture->IsMaster());
	SetGavinMatrix(&gmat);

	if (!GetDrawContext()->SetMatrix(&gmat))
	{
		ERROR3("GDraw_SetMatrix failed");
		return FALSE;
	}

	pBitmapInfo		= pCapture->lpBitmapInfo;
	pBits			= pCapture->lpBits;
	CurrentColContext = pCapture->pColContext;
	RenderView->SetForceDefaultColourContexts(!pCapture->IsMaster());
	PixelsPerInch	= pCapture->dPixelsPerInch;

	// Calc new values for the stored pixel width members based on PixelsPerInch
	// (First set all values to unscaled version)
	PixelWidth		= CalcPixelWidth();
	ScaledPixelWidth = PixelWidth;
	dScaledPixelWidth = CalcPixelWidthDouble();

	// Only set ScaledPixelWidth to its true value for bitmaps
	// Otherwise it's set identically to PixelWidth so that nested captures
	if (pCapture->lpBitmapInfo && pCapture->lpBits && bSetBitmap)
	{
		if (pCapture->info.caFlags.CalcScaledPixWidth || pCapture->info.caFlags.Master)
		{
			ScaledPixelWidth = CalcScaledPixelWidth();
			dScaledPixelWidth = CalcScaledPixelWidthDouble();
		}
	}

	// Update the Windows rectangle version of the current clip rect
// Nope, don't do that, leave WRect referring to Windows version of Master Capture at all times...
//	WRect			= CalculateWinRect(RenderMatrix, CurrentClipRect, PixelsPerInch);

	// we must do this after we've changed CurrentClipRect.
	SetupFuzzyClipRects();

	// set gdraw REGION or RECT for new GDraw context
//	if (!ApplyClipRegion)
//	{
		// general case is to draw offscreen bmp without clip region applied
		// We apply the clip region when restoring so that it is in place
		// if we draw the offscreen bitmap to the original bmp (where the 
		// clipregion was applied)
		if ( !GetDrawContext()->ClipRectangle((LPRECT)&pCapture->CaptureRect) )
		{
			ERROR3("GDraw_ClipRectangle failed");
			return FALSE;
		}
//	}
//	else
//	{
		// placed after CurrentClipRect reset
		ApplyCurrentClipRegion();
//	}

	// Following code from StartRender()
	BOOL bHintStrokedLines = (pCapture->info.caFlags.Master || pCapture->dPixelsPerInch==GetMasterCapture()->dPixelsPerInch);
//TRACEUSER( "Phil", _T("Hint flag = %d, IsMaster = %d\n"), bHintStrokedLines, pCapture->info.caFlags.Master);
	SetQualityLevel();
	GetDrawContext()->SetHintingFlag(bHintStrokedLines);
	GetDrawContext()->SetDashAdjustmentFlag(FALSE);

	// work out the brush alignment
	// algorithm based on code from OSRenderRegion::InitAttributes
	POINT DocOrigin, OSOrigin;
	DocOrigin.x = 0;
	DocOrigin.y = 0;

	// Then we transform it into OSCoords
	GetDrawContext()->TransformPath( &DocOrigin, &OSOrigin, 1, &gmat );

	// Once we've shifted then right by FX, they're in pixel co-ords, which is what
	// SetHalftoneOrigin is expecting them to be in
	const INT32 BrushX = OSOrigin.x >> FX;
	const INT32 BrushY = -(OSOrigin.y >> FX);					// has to be negated to work

	GetDrawContext()->SetHalftoneOrigin( BrushX, BrushY );

	if (pCapture->info.caFlags.QualityNormal)
	{
		// TODO: Set normal quality, maybe only if current quality is >= NORMAL
		this->SetDefaultQuality();
	}

	// Set smoothing flags according to cfBITMAPSMOOTHING and cfTILFILTERING
	SetSmoothingFlags(NULL);

	return TRUE;
}




/********************************************************************************************

>	BOOL GRenderRegion::TranspTypeIsRGBTCompatible(UINT32 ttype) const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/02/2005
	Inputs:		ttype - Camelot-style transparency type
	Outputs:	-
	Returns:	TRUE if the transparency type can be rendered accurately into RGBT bitmap
				by GDraw
	Purpose:	Discover whether this transparency type can be rendered into RGBT bitmap
				by GDraw. (Things like stain and bleach cannot be accurately captured in
				the T channel of the RGBT bitmap.)
	SeeAlso:	GRenderRegion::ChangeCapture
	Notes:		Gavin says (02/02/2005):
				The following are I think OK: 0,1,4,7,19,20,21,22,23,24,25,26,27
				and, I think, the bevel types 34,35,36,37,38,39.

********************************************************************************************/

static BOOL TC[40] = {
						TRUE,		// 0
						TRUE,		// 1
						FALSE,		// 2
						FALSE,		// 3
						TRUE,		// 4
						FALSE,		// 5
						FALSE,		// 6
						TRUE,		// 7
						FALSE,		// 8
						FALSE,		// 9
						FALSE,		// 10
						FALSE,		// 11
						FALSE,		// 12
						FALSE,		// 13
						FALSE,		// 14
						FALSE,		// 15
						FALSE,		// 16
						FALSE,		// 17
						FALSE,		// 18
						TRUE,		// 19
						TRUE,		// 20
						TRUE,		// 21
						TRUE,		// 22
						TRUE,		// 23
						TRUE,		// 24
						TRUE,		// 25
						TRUE,		// 26
						TRUE,		// 27
						FALSE,		// 28
						FALSE,		// 29
						FALSE,		// 30
						FALSE,		// 31
						FALSE,		// 32
						FALSE,		// 33
						TRUE,		// 34
						TRUE,		// 35
						TRUE,		// 36
						TRUE,		// 37
						TRUE,		// 38
						TRUE,		// 39
					  };

BOOL GRenderRegion::TranspTypeIsRGBTCompatible(UINT32 ttype) const
{
	// First we must map the internal transparency type to GDraw's type
	TransparencyEnum gType = MapTranspTypeToGDraw(ttype, FALSE);		// Assume flat fill...
	return TC[gType];
}




/********************************************************************************************

>	TransparencyEnum GRenderRegion::MapTranspTypeToGDraw(UINT32 ttype, BOOL bGraduated) const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/02/2005
	Inputs:		ttype - Camelot-style transparency type
	Outputs:	-
	Returns:	GDraw's TransparencyEnum
	Purpose:	Map Camelot's internal transp type to GDraw's transp type
	Notes:		This function only understands FLAT and GRAD - it will never map a Camelot
				transparency type to one of GDraw's "Colour and transp" values

********************************************************************************************/

TransparencyEnum GRenderRegion::MapTranspTypeToGDraw(UINT32 ttype, BOOL bGraduated) const
{
	if (ttype == 0)
		return T_NONE;

	if (bGraduated)
	{
		if (RenderFlags.HitDetect)
			return (enum TransparencyEnum) (T_GRAD_REFLECTIVE);

		if (ttype <= TT_Bleach)
		{
			// These are the old style for
			// Solid = 0, Mix = 1, Bleach = 2 and Stainedglass = 3
			return (enum TransparencyEnum) (ttype-TT_Mix + T_GRAD_REFLECTIVE);
		}

		if (ttype >= TT_CONTRAST && ttype <= TT_BEVEL)
		{
			// These are the new Transparency types Contrast = 13, Saturation = 16, Darken = 19,
			// Lighten = 22, Brightness = 25, Luminosity = 28, Hue = 31, Bevel = 34
			return (enum TransparencyEnum) (ttype-TT_CONTRAST + T_GRAD_CONTRAST);
		}
	}
	else
	{
		if (RenderFlags.HitDetect)
			return (enum TransparencyEnum) (T_FLAT_REFLECTIVE);

		if (ttype <= TT_Bleach)
		{
			// These are the old style for
			// Solid = 0, Mix = 1, Bleach = 2 and Stainedglass = 3
			return (enum TransparencyEnum) (ttype-TT_Mix + T_FLAT_REFLECTIVE);
		}

		if (ttype >= TT_CONTRAST && ttype <= TT_BEVEL)
		{
			// These are the new Transparency types Contrast = 13, Saturation = 16, Darken = 19,
			// Lighten = 22, Brightness = 25, Luminosity = 28, Hue = 31, Bevel = 34
			return (enum TransparencyEnum) (ttype-TT_CONTRAST + T_FLAT_CONTRAST);
		}
	}

	ERROR3("Invalid transp type");
	return T_NONE;
}




#ifdef _DEBUG
void GRenderRegion::DebugTrace()
{
//	DIBUtil::FillColour24(lpBitmapInfo, lpBits);
}
#endif




/********************************************************************************************

>	void GRenderRegion::SetBitmapPointers(LPBITMAPINFO bi, LPBYTE by)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/2004
	Inputs:		bi - new BITMAPINFO pointer
				by - new BYTES pointer
	Outputs:	-
	Returns:	-
	Purpose:	To set the bitmap pointers for the master capture only, updating both the
				working members and the versions stored in the master capture.
	SeeAlso:	GRenderRegion::StartCapture

********************************************************************************************/

void GRenderRegion::SetBitmapPointers(LPBITMAPINFO bi, LPBYTE by)
{
	Capture* pCapture = GetTopCaptureBitmap();
	ENSURE(pCapture, "SetBitmapPointers being called while there are no captures!\n");
	if (pCapture)
	{
		ENSURE(pCapture->IsMaster(), "SetBitmapPointers being called while child Captures are running\n");
		if (pCapture->IsMaster())
		{
			pCapture->SetDIB(bi, by);
		}
	}

	pBitmapInfo = bi;
	pBits = by;
}
	
	
	
/********************************************************************************************

>	BOOL GRenderRegion::GetBitmapPointers(LPBITMAPINFO* ppInfo, LPBYTE* ppBits, BOOL bCorrectTransparency = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/06/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	To get the bitmap pointers for the master capture only.
	SeeAlso:	GRenderRegion::SetBitmapPointers

********************************************************************************************/

BOOL GRenderRegion::GetBitmapPointers(LPBITMAPINFO* ppInfo, LPBYTE* ppBits, BOOL bCorrectTransparency)
{
	Capture* pCapture = GetMasterCapture();
	ERROR2IF(!pCapture || !pCapture->IsMaster(), FALSE, "GetBitmapPointers can't find master capture!\n");

	if (bCorrectTransparency && pBitmapInfo->bmiHeader.biCompression==0x80000001)
	{
		GetDrawContext()->ConvertBitmap(&pBitmapInfo->bmiHeader, pBits,
										&pBitmapInfo->bmiHeader, pBits,
										0);
	}

	*ppInfo = pBitmapInfo;
	*ppBits = pBits;

	return TRUE;
}
	
	
	
// Operations to control GDraw options
// (will probably be in a dialog at some point)



/*******************************************************************

>	void OpGDraw::Do(OpDescriptor*)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/94
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Toggles state of gdraw flag then forces a redraw.
	Errors:		None

*******************************************************************/

void OpGDraw::Do(OpDescriptor*)
{
	GRenderRegion::WantDoGDraw ^= TRUE;

	// Iterate over all the documents, forcing a redraw of all the views.
	ListItem* pItem = Camelot.Documents.GetHead();
	while (pItem != NULL)
	{
		((Document*) pItem)->ForceRedraw();
		pItem = Camelot.Documents.GetNext(pItem);
	}

	End();
}


/*******************************************************************

>	OpGDraw::OpGDraw()

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/93
	Inputs:
	Outputs:
	Returns:
	Purpose:	Constructor for OpGDraw operation. It is not undoable.
	Errors:		None

*******************************************************************/

OpGDraw::OpGDraw()
{
}


/*******************************************************************

>	OpState OpGDraw::GetState(String_256*, OpDescriptor*)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/93
	Inputs:
	Outputs:
	Returns:
	Purpose:	This item is always available when GDraw is available.
	Errors:		None

*******************************************************************/

OpState OpGDraw::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	if (!GRenderRegion::CanDoGDraw)
	{
		OpSt.Greyed = TRUE;
		return OpSt;
	}

	if (GRenderRegion::WantDoGDraw)
		OpSt.Ticked = TRUE;

	return OpSt;
}
