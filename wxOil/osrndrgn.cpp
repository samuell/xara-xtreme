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

// OSRenderRegion class is responsible for rendering through GDI

/*
*/


#include "camtypes.h"

#include "osrndrgn.h"	// This also includes 'rndrgn.h' and 'paths.h'
#include <math.h>		// Uses 'atn' to find the angle of a line
#include "app.h"		// Need definition of 'Camelot'
// #include "mainfrm.h"
#include "docview.h"
#include "winrect.h"
#include "ensure.h"
#include "camelot.h"
#include "fixmem.h"
#include "rendbits.h"
#include "grndrgn.h"
#include "attr.h"
#include "fillattr.h"
#include "devcolor.h"
#include "oilbitmap.h"
#include "bitmap.h"
#include "attrmgr.h"
#include "tool.h"
// #include "oiltool.h"
#include "camview.h"
// #include "fonts.h"		// For FixedSystem font from the FontFactory
#include "dibutil.h"
#include "fuzzclip.h"
#include "nodeelip.h"
#include "noderect.h"
#include "princomp.h"
#include "printctl.h"
#include "psrndrgn.h"
#include "palman.h"
#include "rrcaps.h"
#include "maskedrr.h"
// #include "prdlgctl.h"
#include "textfuns.h"
#include "colcontx.h"
// #include "prncamvw.h"
#include "nodebmp.h"
#include "fontman.h"
// #include "metaview.h"
#include "gradtbl.h"
#include "unicdman.h"  // For MBCS support
// #include "mrhbits.h"
#include "strkattr.h"
#include "cartprov.h"

DECLARE_SOURCE("$Revision$");

/********************************************************************************************

	Preference:	GDIPalette
	Section:	Screen
	Range:		1 or 0
	Purpose:	Allows the user to determine whether a special palette is used when rendering
				with GDI. Ingored except on modes that support palettes (e.g. 256 colours).
				Defaults to 1.

********************************************************************************************/

static BOOL WantGDIPalette = TRUE;					// TRUE if GDI palette wanted




/********************************************************************************************

	Preference:	BetterLines
	Section:	Screen
	Range:		0 (auto), 1 (yes), 2 (no)
	Purpose:	Allows the user to determine whether we let GDI do lines (2) or we do them
				(1). When we do them we do end caps, mitres etc properly. 0 means let GDI32
				do them, but we do them on GDI16.
				Defaults to 0

********************************************************************************************/

static BOOL WantBetterLines;




/********************************************************************************************

	Preference:	PrintRotatedTextAsPaths
	Section:	Printing
	Range:		1 or 0
	Purpose:	Allows the user to force printing on non-Postscript printers to output all
				rotated characters as paths instead of using GDI text rendering.
				Note that "Rotated" means rotated on the physical paper.
				Defaults to FALSE (0).

********************************************************************************************/

static BOOL PrintRotatedTextAsPaths = FALSE;		// FALSE because we want to allow the printer
													// to attempt to render rotated text.



/********************************************************************************************

	Preference:	HitTestRadius
	Section:	Mouse
	Range:		0 to 10
	Purpose:	Allows the user to determine how close a mouse click can be to an object
				before the object is hit-tested successfully.  Measured in pixels.
				Defaults to 3.

********************************************************************************************/

INT32 OSRenderRegion::HitTestRadius = 3;				// default hit-detect sensitivity




/********************************************************************************************

	Preference:	SlowRectangles
	Section:	DisplayKludges
	Range:		0 or 1
	Purpose:	Set to 1 for video drivers that cannot plot rectangles with line borders
				correctly e.g. Avance 1.5F in 256 colours. Defaults to 0.

********************************************************************************************/

BOOL BodgeRectangles = FALSE;						// TRUE if hollow rects need drawing with Polygon




// Other, unlabelled prefs and things...
EORCacheClass OSRenderRegion::EORCache;				// new implementation of the EOR colour cache

BOOL OSRenderRegion::DoBetterLines;

wxPoint OSRenderRegion::PointArray[SIZEOF_POLYLINE_BUFFER];	// for flattening paths

static INT32 GradFillQuality = 1;

CC_IMPLEMENT_DYNAMIC(OSRenderRegion, RenderRegion)
CC_IMPLEMENT_DYNAMIC(PaperRenderRegion, OSRenderRegion)

#define new CAM_DEBUG_NEW


OSRRFontInfo::OSRRFontInfo()
{
	Rotation = FIXED16(0);
}



/********************************************************************************************

>	BOOL OSRenderRegion::Init()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/94
	Inputs:		-
	Returns:	TRUE if worked, FALSE if didnt (cannot fail at present)
	Purpose:	To be called during startup. Reads preferences.
	Scope:		Static

********************************************************************************************/

BOOL OSRenderRegion::Init()
{
	if (Camelot.DeclareSection(TEXT("DisplayKludges"), 10))
	{
		Camelot.DeclarePref( NULL, TEXT("SlowRectangles"), &BodgeRectangles, FALSE, TRUE );
	}

	if (Camelot.DeclareSection(TEXT("Printing"), 10))
	{
		Camelot.DeclarePref( NULL, TEXT("PrintRotatedTextAsShapes"), &PrintRotatedTextAsPaths, FALSE, TRUE );
	}

	if (Camelot.DeclareSection(TEXT("Screen"), 10))
	{
		Camelot.DeclarePref(NULL, TEXT("GDIPalette"), &WantGDIPalette, FALSE, TRUE );
		Camelot.DeclarePref(NULL, TEXT("BetterLines"), &WantBetterLines, 0, 2 );
		
		switch (WantBetterLines)
		{
			case 0:
				DoBetterLines = TRUE;
				break;

			case 1:
				DoBetterLines = TRUE;
				break;

			case 2:
				DoBetterLines = FALSE;
				break;
		}
	}

	if (Camelot.DeclareSection(TEXT("Mouse"), 10))
	{
		Camelot.DeclarePref(NULL, TEXT("HitTestRadius"), &HitTestRadius, 0, 10);
	}

	return TRUE;
}



/********************************************************************************************

>	RenderRegion *OSRenderRegion::Create(DocRect ClipRect, Matrix ConvertMatrix, 
											FIXED16 ViewScale, RenderType rType,
											DocView* pView = NULL,
											BOOL UseOSRendering = FALSE)


	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/93

	Inputs:		ClipRect - a DocRect defining the invalid rectangle to be rendered.

				ConvertMatrix - a Matrix for converting Doc coords to OS coords.

				ViewScale - the scale factor of the view, used to calculate how much to
				flatten paths etc.

				rType - helps the choice of 32-bit rendering etc.

				pView - The View (if any) for which the RenderRegion is to be used

				UseOSRendering - TRUE to force the system to give you an OSRenderRegion,
				rather than diverting this call to really give you a GRenderRegion if
				GDraw is enabled (used by kernel-rendered dialogues which don't want
				transparency/grad fills, etc, and wish to render simple FixedSystem text)

	Purpose:	Creates an RenderRegion, allowing a invalid rectangle to be specified.
				The matrix passed is used to convert Document coords into device coords,
				taking into account scaling, scroll offsets etc.
				This function should be used to create all RenderRegions - it chooses the
				best RenderRegion subclass to use depending on the host environment.
				(e.g. on NT it chooses an OSRenderRegion that can use Win32 GDI calls).

********************************************************************************************/

RenderRegion* OSRenderRegion::Create(DocRect ClipRect, Matrix ConvertMatrix, 
									 FIXED16 ViewScale, RenderType rType,
									 View* pView, BOOL UseOSRendering, BOOL bForce32BPP)
{
//	const bool bCanDo32 = true;
/* GAT
	BOOL bCanDo32 = FALSE;

#if WIN32
	if ( IsWin32NT() || IsWin32c() )
	{
		// Running on 32-bit GDI - use special NT-only Win32 GDI functions for those devices that
		// make sense. Currently only 16-bit Metafiles are not done using 32-bit GDI
		// so that paths are manually flattened.
		if (rType != RENDERTYPE_METAFILE16)
			bCanDo32 = TRUE;
	}
#endif
*/
	OSRenderRegion *pOS = NULL;

	if (rType == RENDERTYPE_MONOBITMAP || rType == RENDERTYPE_COLOURBITMAP || rType == RENDERTYPE_HITDETECT)
	{
		RenderRegion *pRender = OSRenderBitmap::Create(ClipRect, ConvertMatrix, ViewScale, rType);
		
		if (pRender)
		{
			// if its not a OSRenderRegion, then return it directly, otherwise fall through
			// to set the flags on it
			if (!pRender->IsKindOf( CC_RUNTIME_CLASS(OSRenderRegion) ))
				return pRender;

			pOS = (OSRenderRegion*) pRender;
		}
	}
	else if (rType == RENDERTYPE_SCREENPAPER)
	{
		// Special render region for paper rendering.
//		TRACE(_T("Creating SCREENPAPER region (%d, %d) - (%d, %d)\n"), ClipRect.lo.x, ClipRect.lo.y, ClipRect.hi.x, ClipRect.hi.y);
		pOS = new PaperRenderRegion(ClipRect, ConvertMatrix, ViewScale);
	}
	else if ((rType == RENDERTYPE_PRINTER) || (rType == RENDERTYPE_PRINTER_PS))
	{
		PORTNOTETRACE("other","OSRenderRegion::Create - printing disabled");
#if !defined(EXCLUDE_FROM_XARALX)
#ifndef STANDALONE
		// Work out what kind of printer region to get - find document's print control info.
		Document *pDoc = pView->GetDoc();
		ERROR3IF(pDoc == NULL, "No document attached to view when printing!")
		if (pDoc != NULL)
		{
			// Get print information for this document.
			PrintComponent *pPrint = 
				(PrintComponent *) pDoc->GetDocComponent(CC_RUNTIME_CLASS(PrintComponent));
			ERROR2IF(pPrint == NULL, NULL, "Unable to find PrintComponent in document.");

			PrintControl *pPrintControl = pPrint->GetPrintControl();
			ERROR2IF(pPrintControl == NULL, NULL,
					 "Unable to find PrintControl object in document component.");

			PrintMethodType PrintType = pPrintControl->GetPrintMethod();

			// Work out whether or not to use straight Gavin bitmap for printing
			if ((PrintType == PRINTMETHOD_AABITMAP) ||
				(PrintType == PRINTMETHOD_BITMAP))
			{
				// GDraw - use GRenderPrint region.
				return GRenderRegion::Create(ClipRect, ConvertMatrix, ViewScale, 
											 rType, pView);
			}
		}

		// Is this a PostScript printer?
		if (rType == RENDERTYPE_PRINTER_PS)
		{
			// Yes - make a PostScript render region
			return new PrintPSRenderRegion(ClipRect, ConvertMatrix, ViewScale);
		}
#endif
#endif
		// If we get here, then we should use normal OS rendering.
//		TRACE(_T("Creating PRINTER region (%d, %d) - (%d, %d)  OSRenderRegion\n"), ClipRect.lo.x, ClipRect.lo.y, ClipRect.hi.x, ClipRect.hi.y);
		return new OSRenderRegion(ClipRect, ConvertMatrix, ViewScale);
	}
	else
	{
		// Always try for a GRenderRegion first - unless the caller specifically asked for
		// the OSRenderRegion, the whole OSRenderRegion, and nothing but the OSRenderRegion.
		// (But by default, we'll give 'em a GRenderRegion if we can)
		if ((!UseOSRendering) && (rType != RENDERTYPE_PRINTER))
		{
			RenderRegion *pRender;
			pRender = GRenderRegion::Create(ClipRect, ConvertMatrix, ViewScale, rType, pView, bForce32BPP);
			if (pRender)
				return pRender;
		}

		// if failed, use normal ones
//		TRACE(_T("Creating OTHER region (%d, %d) - (%d, %d)  OSRenderRegion\n"), ClipRect.lo.x, ClipRect.lo.y, ClipRect.hi.x, ClipRect.hi.y);
		pOS = new OSRenderRegion(ClipRect, ConvertMatrix, ViewScale);
	}

	if (!pOS)
		return NULL;								// if call failed

	// created a new one OK - must set RFlags suitably

	if (
		(rType == RENDERTYPE_METAFILE16) ||
		(rType == RENDERTYPE_METAFILE32)
	   )
		pOS -> RFlags.Metafile = TRUE;				// mark metafiles correctly
													// (this fn is a friend so can get to it)

	if (rType == RENDERTYPE_MONOBITMAP)
	{
		pOS -> RenderFlags.VeryMono = TRUE;
	}

	if (rType == RENDERTYPE_HITDETECT)
	{
		pOS -> RenderFlags.VeryMono = FALSE;
		pOS -> RenderFlags.HitDetect = TRUE;
		pOS -> m_DoCompression = TRUE;
	}

	if (rType == RENDERTYPE_COLOURBITMAP)
	{
		pOS -> RenderFlags.VeryMono = FALSE;
		pOS -> m_DoCompression = TRUE;
	}

	if (
		(rType == RENDERTYPE_SCREEN) ||
		(rType == RENDERTYPE_SCREENPAPER) ||
		(rType == RENDERTYPE_SCREENXOR)
	   )
		{
			// if going to the screen, might have a palette selected
			if (WantGDIPalette && PaletteManager::UsePalette())
				pOS->RFlags.UsePalette = TRUE;
		}

//	pOS->RFlags.GDI32 = bCanDo32;

	return pOS;
}



/********************************************************************************************

>	OSRenderRegion::OSRenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93
	Inputs:		ClipRect is a DocRect defining the invalid rectangle to be rendered.
				ConvertMatrix is a Matrix for converting Doc coords to OS coords.
				ViewScale is the scale factor of the view, used to calculate how much to
				flatten paths etc.
	Purpose:	Constructor for OSRenderRegion, allowing a invalid rectangle to be specified.
				The matrix passed is used to convert Document coords into device coords,
				taking into account scaling, scroll offsets etc. All RFlags values default
				to FALSE. The creator should set them.

********************************************************************************************/

OSRenderRegion::OSRenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale)
	: RenderRegion(ClipRect, ConvertMatrix, ViewScale)
{
	CurrentPen = 0;
	CurrentBrush = 0;
//	OldBrush = NULL;
//	OldPen = NULL;
	OldFont = NULL;
	OSClipRegion = NULL;
	OldPalette = NULL;
	FontInfo.pRenderFont = NULL;
	FontInfo.pOldFont = NULL;

	RFlags.Metafile   = FALSE;
//	RFlags.GDI32      = FALSE;
	RFlags.ValidPen   = FALSE;
	RFlags.ValidBrush = FALSE;
	RFlags.UsePalette = FALSE;

	// We don't bother rendering paper for OSRenderRegions cos it's already been done by the
	// PaperRenderRegion
	IsPaperRendered = TRUE;

	// Set the render caps up
	GetRenderRegionCaps(&Caps);

	// Initialise the SepTables pointer
	SepTables = NULL;
}

/********************************************************************************************

>	OSRenderRegion::OSRenderRegion(const OSRenderRegion &other)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/7/93
	Inputs:		An OSRenderRegion to copy
	Purpose:	Copy Constructor for OSRenderRegion.  Constructs an OSRenderRegion that is a
				copy of another. Calls RenderRegion copy constructor to make a copy of the
				current ContextStack.
	SeeAlso:    RenderRegion(const RenderRegion &other)

********************************************************************************************/

OSRenderRegion::OSRenderRegion(const OSRenderRegion &other)
	 : RenderRegion(other)
{
	// We assume that the RenderRegion being copied is not curently rendering.
	CurrentPen = 0;
	CurrentBrush = 0;
//	OldBrush = NULL;
//	OldPen = NULL;
	OldFont = NULL;
	OSClipRegion = NULL;
	OldPalette = NULL;
	FontInfo.pRenderFont = NULL;
	FontInfo.pOldFont = NULL;

	RFlags = other.RFlags;

	// Set the render caps up
	GetRenderRegionCaps(&Caps);

	// Initialise the SepTables pointer
	SepTables = NULL;
}

/********************************************************************************************

>	OSRenderRegion::~OSRenderRegion()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/93
	Purpose:	Default Destructor for OSRenderRegion Class

********************************************************************************************/

OSRenderRegion::~OSRenderRegion()
{
	if (RenderFlags.Rendering)
	{
		TRACE( _T("StopRender() was not called before destructor\n") );
		StopRender();
	}

	// A bug used to cause OSClipRegion to be left undeleted when a clipping rect was set
	// externally.  This check is here just to make sure it doesn't creep back !!
	ENSURE(OSClipRegion == NULL, "Clip Region wasn't deleted");

	if (SepTables != NULL)
	{
		CCFree(SepTables);
		SepTables = NULL;
	}
}



/********************************************************************************************

>	BOOL OSRenderRegion::AttachDevice(View* ViewToAttach, CDC* DCToAttach, Spread* SpreadToAttach)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/05/94
	Inputs:		A View ptr ,CDC ptr and Spread ptr to attach to the RenderRegion
	Returns:	TRUE if the device is attached successfully;
				FALSE if not.
	Purpose:	Attach an OSRenderRegion to a particular device context.  This merely
				remembers the information - the real work is done by 
				[OS]RenderRegion::InitDevice().
	Errors:		Same as base class.
	SeeAlso:	OSRenderRegion::InitDevice; RenderRegion::InitDevice

********************************************************************************************/

BOOL OSRenderRegion::AttachDevice(View* ViewToAttach, wxDC* DCToAttach, Spread* SpreadToAttach, bool fOwned /*= false*/)
{
	// Call the base class first
	if (!RenderRegion::AttachDevice( ViewToAttach, DCToAttach, SpreadToAttach, fOwned ))
		return FALSE;
	
	ENSURE(RenderDC != NULL,"Attempted to attach Invalid DC to RenderRegion");
	// If it is a PrinterDC then set our flag to say we are Printing
	PORTNOTE("other","OSRenderRegion::AttachDevice - removed printing support");
#if !defined(EXCLUDE_FROM_XARALX)
	RenderFlags.Printing = RenderDC->IsPrinting();
#else
	RenderFlags.Printing = false;
#endif

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL OSRenderRegion::InitDevice()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/05/94
	Returns:	TRUE if the device context is initialised ok;
				FALSE if not.
	Purpose:	Initialise the device specific mechanisms for this render region.
	Errors:		Same as base class.
	SeeAlso:	RenderRegion::InitDevice

********************************************************************************************/

BOOL OSRenderRegion::InitDevice()
{
	// Call base class
	if (!RenderRegion::InitDevice())
		return FALSE;

	// Get the HDC so we can reconstruct the CDC later.
//	DCHandle = RenderDC->GetSafeHdc();

	// Ignore any error from GBrush initialisation - we can live without GBrush.
	// However, we only want GBrush on the screen thanks
	if (IsPrinting())
	{
		// if we don't do this, bitmaps printed at the same DPI as the printer come
		// out wrong on NT drivers
		// Note: although this is claimed to be a Win32s-compatible function,
		// GDI16 generates an error for it (invalid value 4)
		// Note2: The MFC version of this function looks like the 16-bit API call,
		// so we call the API directly here
		//::SetStretchBltMode( RenderDC->m_hDC, HALFTONE );
	}
	else
	{
		GDrawBrush.Init( RenderDC );
	}

	// Find out what grad fill quality to use.
	PrintControl *pPrintControl = RenderView->GetPrintControl();
	PrintFillQuality FillQuality;

	if (pPrintControl != NULL)
		FillQuality = pPrintControl->GetFillQuality();
	else
		FillQuality = PRINTFILLQUALITY_MEDIUM;

	switch (FillQuality)
	{
		case PRINTFILLQUALITY_LOW:
			GradFillQuality = 2;
			break;

		case PRINTFILLQUALITY_HIGH:
			GradFillQuality = 0;
			break;

		case PRINTFILLQUALITY_MEDIUM:
		default:
			GradFillQuality = 1;
			break;
	}


	// All ok
	return TRUE;
}



/********************************************************************************************

>	void OSRenderRegion::InitClipping()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/6/93
	Purpose:	Creates a Windows CRgn object from the DocRect passed in the constructor.
				The CRgn is set as the current clipping region during reendering.
	Scope:		Private

********************************************************************************************/

void OSRenderRegion::InitClipping()
{

	// OLD COMMENT and CODE:
	// Windows rects are slightly differnt to DocView rects so we expand them by 1 pixel
	//WinRect OSClipRect = DocRectToWin(CurrentClipRect, 0,0,1,1);

	// now the seemingly spurious 1,1s have been removed as they are indeed spurious
	WinRect OSClipRect = DocRectToWin(CurrentClipRect, 0,0,0,0);

//	ENSURE(OSClipRect.top >= 0, "Blobby!");

	ERROR3IF((OSClipRect.x < 0) || (OSClipRect.y < 0), "OSRenderRegion::InitClipping bad clip");

	// we can't clip within a metafile, but we can set the origin & extent based on it
//	if (RFlags.Metafile)
//	{
//		RenderDC->SetWindowOrg( OSClipRect.x, OSClipRect.y );
//		RenderDC->SetWindowExt( OSClipRect.width, OSClipRect.height );
//	}
//	else
	{
		delete OSClipRegion;	// This will be NULL if no previous CRgn defined.
		OSClipRegion = new wxRegion(OSClipRect);
		ENSURE(OSClipRegion != NULL,"'new' failed when creating OSClipRegion");

		// Set the Clipping Region
		if (OSClipRegion)
			RenderDC->SetClippingRegion(*OSClipRegion);
	}
}

/********************************************************************************************

>	void OSRenderRegion::DeInitClipping()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/94
	Purpose:	Destroys the GDI clipping region object, and unsets the clipping region for
				our DC.

********************************************************************************************/

void OSRenderRegion::DeInitClipping()
{
	delete OSClipRegion;			// Delete the CRgn
	OSClipRegion = NULL;

	// unset the Clipping Region
	if (!RFlags.Metafile)
		RenderDC->DestroyClippingRegion();
}

/********************************************************************************************

>	void OSRenderRegion::InitAttributes()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/93
	Purpose:	Sets up a default GDI pen and brush for rendering, and calculates the Brush
				origin for aligned fill patterns.
	Scope:		Private

********************************************************************************************/

void OSRenderRegion::InitAttributes()
{
	CurrentPen = 0;
	CurrentBrush = 0;
//	OldBrush = NULL;
//	OldPen = NULL;
	OldFont = NULL;

	if (!RFlags.Metafile)
	{
		// Set the default font (intended for kernel-rendered dialogues)
		// Don't bother for metafiles (because we can't read text from them anyhow)
	PORTNOTE("other","OSRenderRegion::InitAttributes - removed setting of default font");
#if !defined(EXCLUDE_FROM_XARALX)
		wxFont* FixedSystemFont = FontFactory::GetCFont(STOCKFONT_RNDRGNFIXEDTEXT);
		if (FixedSystemFont != NULL)
			OldFont = RenderDC->SetFont(*FixedSystemFont);
#endif
	}

	SetOSDrawingMode();
	SetLineAttributes();	// Create a default Pen
	SetFillAttributes();	// Create a default Brush

	if (!RFlags.Metafile)
	{
		// Metafiles don't need brush origin calculations

		// Now we calculate the Origin for the Brush fill pattern, so that fill patterns
		// are always aligned as we scroll around the page.
	
		// (First we get the Origin of the Document in DocCoords)
		// Actually, at Jim's suggestion Andy now bases this on (0,0).
		DocCoord DocOrigin;
		DocOrigin.x = 0; 					//was MinDocCoord+0x20000;
		DocOrigin.y = 0;					//was MaxDocCoord-0x20000;
		
//		f1 = f;
		// Then we transform it into OSCoords
		WinCoord OSOrigin;
		OSOrigin = DocCoordToWin(DocOrigin);
	
		// when running on some machine combinations, we need to further adjust the point
		// relative to the screen. The only combination that works is a 32-bit version
		// running on NT. Everyone else has to fix it up.
PORTNOTE("other", "Check this");
//		if (IsWin32s())
		{
			if (RenderView)
			{
				// Get the OIL window and get it to do the conversion for us.
				CCamView* RenderWindow = RenderView->GetConnectionToOilView();
				if ( RenderWindow!=NULL && RenderWindow->GetFrame()!=NULL )
					RenderWindow->GetFrame()->ClientToScreen(OSOrigin);
				// In galleries, RenderView used to be NULL, so this code was never called.
				// This has changed, so suddenly this ensure was going off. Now it won't.
				//else
				//	ERROR2RAW("No render window in OSRenderRegion::InitAttributes()");
			}
		}
	
		// The Brush Origin is set to a value between 0 and 7 calculated from the position of the
		// transformed origin.
		NewBrushOrg.x = OSOrigin.x & 7;
		NewBrushOrg.y = OSOrigin.y & 7;
	} 

}

/********************************************************************************************

>	void OSRenderRegion::DeInitAttributes()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/94
	Purpose:	Deletes the GDI pen and brush objects.

********************************************************************************************/

void OSRenderRegion::DeInitAttributes()
{
	// Get rid of pen and/or brush if we used them.
/*	if (OldPen != NULL)
	{
		//ENSURE(OldPen->m_hObject != NULL, "NULL object selected!");
		RenderDC->SetPen(*OldPen);
	}

	if (OldBrush != NULL)
	{
		//ENSURE(OldBrush->m_hObject != NULL, "NULL object selected!");
		RenderDC->SetBrush(*OldBrush);
	}
*/

	// Deselect the FixedSystem font from the DC
	if (OldFont != NULL)
	{
		RenderDC->SetFont(*OldFont);
		OldFont = NULL;
	}

	// Deselect the normal rendering font from the DC
	if (FontInfo.pRenderFont != NULL)
	{
		RenderDC->SetFont(*FontInfo.pOldFont);
		FontInfo.pOldFont = NULL;
		delete FontInfo.pRenderFont;
		FontInfo.pRenderFont = NULL;
	}
}


/********************************************************************************************

>	void OSRenderRegion::SetLineAttributes(ChangeLineAttrType Type = CHANGELINEATTR_ALL)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/93
	Purpose:	When a line attribute is changed, this functions is called to create a new
				pen to mirror the new attribute value.
	Scope:		Private

********************************************************************************************/

void OSRenderRegion::SetLineAttributes(ChangeLineAttrType Type)
{
	RFlags.ValidPen = FALSE;
}

/********************************************************************************************

>	 void OSRenderRegion::SetFillAttributes(ChangeAttrType Type = CHANGEATTR_ALL)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/93
	Inputs:		Ignored for OSRenderRegion.
	Purpose:	When a fill attribute is changed, this functions is called to create a new
				brush to mirror the new attribute value.
	Scope:		Private

********************************************************************************************/

void OSRenderRegion::SetFillAttributes(ChangeAttrType)
{
	RFlags.ValidBrush = FALSE;
}

/********************************************************************************************

>	void OSRenderRegion::SetQualityLevel()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/93
	Purpose:	Called when Quality level changes. We do nothing currently.

********************************************************************************************/

void OSRenderRegion::SetQualityLevel()
{
}

/********************************************************************************************

>	COLORREF OSRenderRegion::CalcEORColour( DocColour &Wanted, COLORREF Background = WHITE)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/93
	Inputs:		Wanted is the colour that is finally required. Background is the colour you
				are assumed to be drawing onto.
	Returns:	The RGB value such that value XOR Background = Wanted (approx).
	Purpose:	To calculate correct colours when draing in EOR mode. Intended to be called
				by OSRenderRegion code only (normally SetxxxColur)
	Errors:		If cannot calculate for some reason, returns Background, which works if Wanted
				is black.
	Scope:		Protected

********************************************************************************************/

wxColour OSRenderRegion::CalcEORColour( DocColour &Wanted, COLORREF Background )
{
	// This routine caches the last EOR colour it calculated so that we don't create
	// a bitmap every time we e.g. draw a selection blob.
	INT32 R,G,B;
	Wanted.GetRGBValue(&R,&G,&B);
	return wxColour(
				GetRValue(Background)^R,
				GetGValue(Background)^G,
				GetBValue(Background)^B
			);
/*

	if (EORCache.Valid && (EORCache.SourceColour == Wanted))
		return(EORCache.EORColour);

	COLORREF Result;
	INT32 Red, Green, Blue;
	Wanted.GetRGBValue(&Red, &Green, &Blue);

	if (RFlags.UsePalette)
	{
		// a palette device has to be quite different - get palette indices and XOR
		// them together, then return that as a palette index
		const UINT32 BGIndex = PaletteManager::GetPalette()->GetNearestPaletteIndex( Background );
		const UINT32 FGIndex = PaletteManager::GetPalette()->GetNearestPaletteIndex( RGB(Red,Green,Blue) );

		Result = PALETTEINDEX( BGIndex ^ FGIndex );
	}
	else
	{
		// non-palette devices we XOR into a little bitmap to work it out
		CBitmap Tiny, *OldBitmap;
		CDC MemDC;

		BOOL bStatus;

		bStatus = MemDC.CreateCompatibleDC( RenderDC );
		ENSURE(bStatus, "cant create comp DC");

		bStatus = Tiny.CreateCompatibleBitmap( RenderDC, 1, 1 );		// make a tiny bitmap
																		// compatible with screen
																		// so colours OK
		ENSURE(bStatus, "cant create Tiny bitmap");

		OldBitmap = MemDC.SelectObject( &Tiny );
		if (OldBitmap == NULL)
		{
			ENSURE(FALSE, "Cant select tiny bitmap");
			return RGB(255,255,255);									// return WHITE
		}

		MemDC.SetPixel( 0,0, Background );
		MemDC.SetROP2( R2_XORPEN );										// into XOR mode

		MemDC.SetPixel( 0,0, RGB(Red, Green, Blue) );


		Result = MemDC.GetPixel( 0,0 );
		ENSURE( Result!=0xFFFFFFFF, "GetPixel failed");

		//TRACE( _T("Converted %d.%d.%d to %d.%d.%d\n"), Wanted.Red, Wanted.Green, Wanted.Blue,
		//											GetRValue(Result), GetGValue(Result), GetBValue(Result) );

		MemDC.SelectObject( OldBitmap );								// restore normality

		// automatic destruction of HDC and Bitmaps will occur naturally
	}


	// Cache this result
	EORCache.SourceColour = Wanted;
	EORCache.EORColour = Result;
	EORCache.Valid = TRUE;

	return Result;
*/
}



/********************************************************************************************

>	static void OSRenderRegion::FlushEORCache(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/95
	Inputs:		-
	Returns:	-
	Purpose:	Flushes the EOR colour cache.  Currently called when screen mode changes
	Scope:		Public

********************************************************************************************/

void OSRenderRegion::FlushEORCache(void)
{
	EORCache.Valid = FALSE;
}



/********************************************************************************************

>	void OSRenderRegion::CalcLogBrush(wxBrush* lpBrush, DocColour &Col)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/94
	Inputs:		lpBrush will be filled with a LOGBRUSH.
				Col is used to calculate said brush.
	Returns:	-
	Purpose:	To calculate a brush object given a Colour. Normally used for
				brushes to fill things.
	Errors:		-
	Scope:		Protected

********************************************************************************************/

void OSRenderRegion::CalcLogBrush(wxBrush* lpBrush, DocColour &Col)
{
	if (Col.IsTransparent())
	{
		lpBrush->SetColour(*wxBLACK);
		lpBrush->SetStyle(wxTRANSPARENT);
	}
	else
	{
		// VeryMono devices have everything that is not transparent as Black
		if (RenderFlags.VeryMono)
			// Unless the quality says don't fill!!!
			if (RRQuality.GetFillQuality() <= Quality::NoFill)
			{
				lpBrush->SetColour(*wxBLACK);
				lpBrush->SetStyle(wxTRANSPARENT);
			}
			else
			{
				lpBrush->SetStyle(wxSOLID);
				lpBrush->SetColour(*wxBLACK);
			}
		else if (DrawingMode != DM_EORPEN)
		{
			if (GDrawBrush.Available())
				GDrawBrush.GetLogBrush( CurrentColContext, Col, lpBrush );
			else
			{
				INT32 R,G,B;
				Col.GetRGBValue(&R, &G, &B);
				lpBrush->SetStyle(wxSOLID);
				lpBrush->SetColour(R, G, B);
			}
		}
		else
		{
			lpBrush->SetStyle(wxSOLID);
			lpBrush->SetColour(CalcEORColour(Col));
		}
	}
}

/********************************************************************************************

>	void OSRenderRegion::CreateNewPen()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/93
	Purpose:	Creates a new Pen, based on the current line attributes.
	Scope:		Private

********************************************************************************************/

void OSRenderRegion::CreateNewPen()
{

	BOOL EmptyPen = RR_STROKECOLOUR().IsTransparent();
	const enum Quality::Line LineQuality = RRQuality.GetLineQuality();

	// if no filling going on then must have a pen
	if ( RRQuality.GetFillQuality() <= Quality::NoFill )
		EmptyPen = FALSE;

	if (
//		RFlags.GDI32 &&								// must be GDI32
		!RenderFlags.VeryMono &&   					// not to bitmaps (special colour rules)
		!RenderFlags.bImmediateRender &&			// not when immediate (XORing messes up)
		(LineQuality >= Quality::FullLine) &&		// not on simple lines
		!EmptyPen									// not on NULL pens (over complex)
	   )
	{
///		Windows version would have used brush calculation to set pen properties
///		CalcLogBrush( &Brush, RR_STROKECOLOUR() );

		INT32 PenWidth = MPtoLP(RR_LINEWIDTH());
		if (IsPrinting())
		{
			// Make sure we don't user single pixel width pens as they won't dither.
			if (PenWidth < 2)
				PenWidth = 2;
		}

		INT32 R,G,B;
		RR_STROKECOLOUR().GetRGBValue(&R,&G,&B);
		RenderPen[CurrentPen] = wxPen(wxColour(R,G,B), PenWidth, wxSOLID);

		// work out pen type
		switch (RR_STARTCAP())
		{
			case LineCapRound:
				RenderPen[CurrentPen].SetCap(wxCAP_ROUND);
				break;

			case LineCapSquare:
				RenderPen[CurrentPen].SetCap(wxCAP_PROJECTING);
				break;

			case LineCapButt:
				RenderPen[CurrentPen].SetCap(wxCAP_BUTT);
				break;

			default:
				ENSURE(FALSE, "Bad startcap");
				break;
		}
		switch (RR_JOINTYPE())
		{
			case BevelledJoin:
				RenderPen[CurrentPen].SetJoin(wxJOIN_BEVEL);
				break;

			case MitreJoin:
				RenderPen[CurrentPen].SetJoin(wxJOIN_MITER);
				break;

			case RoundJoin:
				RenderPen[CurrentPen].SetJoin(wxJOIN_ROUND);
				break;

			default:
				ENSURE(FALSE, "bad jointype");
				break;
		}
		return ;
	}

	// fall through to use old-fashioned 16-bit GDI pen creation if failed

	if (EmptyPen)
		RenderPen[CurrentPen] = wxPen(*wxTRANSPARENT_PEN);
	else
	{
		INT32 PenWidth = MPtoLP(RR_LINEWIDTH());

		if (RR_LINEWIDTH()==0 || LineQuality < Quality::FullLine)
			PenWidth = 0;
		else
		{
			MILLIPOINT LineWidth = RR_LINEWIDTH();
			if ((RenderFlags.VeryMono || RenderFlags.HitDetect) && LineWidth < GetScaledPixelWidth())
				PenWidth = MPtoLP(GetScaledPixelWidth());
		}

		wxColour Colour(0,0,0);
		if (RenderFlags.VeryMono || LineQuality < Quality::ThinLine)
			Colour = *wxBLACK;
		else if (DrawingMode==DM_EORPEN)
			Colour = CalcEORColour(RR_STROKECOLOUR());
		else
		{
			INT32 R,G,B;
			RR_STROKECOLOUR().GetRGBValue(&R,&G,&B);
			Colour = wxColour(R, G, B);
		}

		RenderPen[CurrentPen] = wxPen(Colour, PenWidth, wxSOLID);
	}
}

void OSRenderRegion::SelectNewPen()
{
	// Swap pens
	CurrentPen = 1 - CurrentPen;

	// Create and select the new pen
	CreateNewPen();

	RenderDC->SetPen(RenderPen[CurrentPen]);

	RFlags.ValidPen = TRUE;
}


/********************************************************************************************

>	void OSRenderRegion::CreateNewBrush()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/93
	Purpose:	Creates a new Brush, based on the current fill attributes.
	Scope:		Private

********************************************************************************************/

void OSRenderRegion::CreateNewBrush()
{
	if (RRQuality.GetFillQuality() <= Quality::NoFill )
	{
		RenderBrush[CurrentBrush].SetColour(*wxBLACK);
		RenderBrush[CurrentBrush].SetStyle(wxTRANSPARENT);
	}
	else
		CalcLogBrush( &RenderBrush[CurrentBrush], RR_FILLCOLOUR() );
}

/********************************************************************************************

>	void OSRenderRegion::SelectNewBrush()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/93
	Purpose:	Selects RenderBrush while setting its new origin. A common operation that is in its
				own function to cope with things such as metafiles which cannot do this.
	Scope:		Private

********************************************************************************************/

void OSRenderRegion::SelectNewBrush()
{
	// Swap brushes
	CurrentBrush = 1 - CurrentBrush;

	// Make the new brush
	CreateNewBrush();

	// Set up the brush origin
	if (!RFlags.Metafile)
	{
//		RenderBrush[CurrentBrush]->UnrealizeObject();
		// this fn will assert if done on a metafile
	PORTNOTE("other","OSRenderRegion::SelectNewBrush - removed setting of brush origin");
#if !defined(EXCLUDE_FROM_XARALX)
		RenderDC->SetBrushOrg(NewBrushOrg);
#endif
	}

	// And select the new brush
	RenderDC->SetBrush(RenderBrush[CurrentBrush]);

	// Set winding rule as required
	if (RR_WINDINGRULE() == EvenOddWinding)
		nFillStyle = wxODDEVEN_RULE;
	else
		nFillStyle = wxWINDING_RULE;

	// We've now got a valid brush
	RFlags.ValidBrush = TRUE;
}


void OSRenderRegion::GetValidPen()
{
	if (!RFlags.ValidPen)
		SelectNewPen();
}

void OSRenderRegion::GetValidBrush()
{
	if (!RFlags.ValidBrush)
		SelectNewBrush();
}


/********************************************************************************************

>	BOOL OSRenderRegion::SelectNewFont(WORD Typeface, BOOL Bold, BOOL Italic, 
									   MILLIPOINT Width, MILLIPOINT Height, ANGLE Rotation)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/28/95
	Inputs:		Details of the font to select.
				NB. Width is not the actual width; it expresses the aspect ratio required.
					So, e.g. if Width == Height/2 then the aspect ratio is 50%, and so on.
	Returns:	TRUE if the font was selected ok;
				FALSE if not.
	Purpose:	Selects the specified font into the DC.  Performs clever stuff to cache
				the font so that subsequent requests for the same font don't cause anything
				to happen.

********************************************************************************************/

BOOL OSRenderRegion::SelectNewFont(WORD Typeface, BOOL Bold, BOOL Italic, 
								   MILLIPOINT Width, MILLIPOINT Height, ANGLE Rotation)
{
	PORTNOTETRACE("text","OSRenderRegion::SelectNewFont - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Check to see if it is cached
	if ((FontInfo.pRenderFont != NULL) &&
		(FontInfo.Typeface == Typeface) &&
		(FontInfo.Bold == Bold) &&
		(FontInfo.Italic == Italic) &&
		(FontInfo.Width == Width) &&
		(FontInfo.Height == Height) &&
		(FontInfo.Rotation == Rotation))
	{
		// It is the same as the currently selected/cached font
		TRACEUSER( "Tim", _T("Using the cached font\n"));
		return TRUE;
	}

	// Is it rotated, and if so, can this DC do rotated fonts?
	if (Rotation != FIXED16(0))
	{
		// Don't use GetDeviceCaps to ask printers about their capabilities!
		// It tells lies.
		// Even Worse - many printers claim to be unable to render rotated
		// text, when they are actually perfectly cpaable of doing so!
		//
		// So, to maintain compatibility with previous versions just test
		// a Preference here and if it's set return FALSE like 1.5 and all
		// previous versions did.
		if (PrintRotatedTextAsPaths)
			return FALSE;
	}

	TRACEUSER( "Tim", _T("Font cache invalid - generating font to use\n"));

	// Not the cached font - ok, deselect and delete the cached font.
	if (FontInfo.pOldFont != NULL)
	{
		RenderDC->SelectObject(FontInfo.pOldFont);
		FontInfo.pOldFont = NULL;
	}

	if (FontInfo.pRenderFont != NULL)
	{
		delete FontInfo.pRenderFont;
		FontInfo.pRenderFont = NULL;
	}


	ENUMLOGFONT *pEnumLogFont = FONTMANAGER->GetEnumLogFont(Typeface);
	if (pEnumLogFont == NULL)
		// Error
		return FALSE;

	// Create the font and select it into the DC
	TRY
	{
		FontInfo.pRenderFont = new wxFont;
	}
	CATCH (CMemoryException, e)
	{
		return FALSE;
	}
	END_CATCH

	// Work out the font weight - bold or normal?
	INT32 FontWeight = Bold ? FW_BOLD : FW_NORMAL;


	// Work out how big the font is, in logical pixels
	FIXED16 fxPixWidth = 0;
	FIXED16 fxPixHeight = 0;

//	if (RFlags.Metafile)
//	{
		// Metafile - don't care about the view scale
//		RenderView->GetPixelSize(&fxPixWidth, &fxPixHeight);
//	}
//	else
//	{
		RenderView->GetScaledPixelSize(&fxPixWidth, &fxPixHeight);
//	}

	INT32 FontWidth = (INT32) (Width / fxPixWidth.MakeDouble());
	INT32 FontHeight = (INT32) (Height / fxPixHeight.MakeDouble());

	if (FontHeight == FontWidth)
	{
		// Aspect ratio is 100% - use width of 0, and the GDI font engine gives us 100%
		// ratio automatically
		FontWidth = 0;
	}
	else
	{
		// Aspect ratio is not 100% - we need to do some clever stuff to work out
		// how wide we want the font to be.  Because fonts are generally much taller
		// than they are wide, we can't just pass in FontWidth directly - we must
		// transform it to the correct width.

		// BODGETEXT: ideally, we should use a NEWTEXTMETRIC structure here, to work out
		//			  the correct width, but this involves calling EnumLogFontFamily() or
		//			  whatever, which is too slow.  As the text code caches the LOGFONT
		//			  for each font, it could also cache the NEWTEXTMETRIC structure,
		//			  which this routine could then use.
		//			  Having said all that, this code seems to work!

		//			  Indeed the font manager code to cache the LOGFONTS could also
		//			  cache the NEWTEXTMETRIC structures (or the TEXTMETRIC structures
		//			  since NEWTEXTMETRIC structures only exist for TrueType fonts.)
		//			  But at the moment it doesn't - the whole of the font system
		//			  needs thinking through and rewriting.
		//			 														ach - 15/08/96

		// Create the font - the first time is to find out how wide the characters are,
		// so we can scale the width correctly. (So we give a width of 0 to get the normal
		// width for the given height)
		// (We do it at 100 units high so we get decent accuracy for the aspect ratio.)
		if (FontInfo.pRenderFont->CreateFont(-100, 0, 0, 0, FontWeight, Italic, 
										 	 FALSE, FALSE, DEFAULT_CHARSET, 
										 	 OUT_TT_PRECIS, CLIP_TT_ALWAYS,
						   				 	 PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
						   				 	 pEnumLogFont->elfLogFont.lfFaceName) == 0)
		{
			// Unable to select the font.
			delete FontInfo.pRenderFont;
			FontInfo.pRenderFont = NULL;
			return FALSE;
		}

		// Used to hold information about the font
		TEXTMETRIC Metrics;

		if (TRUE) //RFlags.Metafile)
		{
			// Metafile - we need to use the screen DC to do this
			wxDC* pDesktopDC = CWnd::GetDesktopWindow()->GetDC();

			// Get the metrics
			FontInfo.pOldFont = pDesktopDC->SetFont(FontInfo.pRenderFont);
			pDesktopDC->GetTextMetrics(&Metrics);

			// Select old font back into screen DC
			pDesktopDC->SetFont(FontInfo.pOldFont);
			CWnd::GetDesktopWindow()->ReleaseDC(pDesktopDC);
		}
		else
		{
			// Normal rendering - use the DC as we can use 'Getxxx' functions on it
			// because it is not a metafile DC.
			FontInfo.pOldFont = RenderDC->SelectObject(FontInfo.pRenderFont);

			// Get the metrics
			RenderDC->GetTextMetrics(&Metrics);

			// Select old font back into screen DC
			RenderDC->SelectObject(FontInfo.pOldFont);
		}

		// Scale the width by the font's aspect ratio: 
		// ActualWidth = (RealWidth / Height) * Width
		FontWidth = MulDiv(Metrics.tmAveCharWidth, FontWidth, 100);

		// Delete the font
		delete FontInfo.pRenderFont;

		// Create the 'proper' font and select it into the DC
		TRY
		{
			FontInfo.pRenderFont = new CFont;
		}
		CATCH (CMemoryException, e)
		{
			return FALSE;
		}
		END_CATCH
	}

	// Work out the rotation of the font, in tenths of degrees.
	INT32 lfRotation;
	if (Rotation == FIXED16(0))
		lfRotation = 0;
	else
		lfRotation = (INT32) ( (Rotation.MakeDouble() * 360.0 * 10.0) / (2 * PI) );

	// Create the font with the correct width	
	if (FontInfo.pRenderFont->CreateFont(-FontHeight, FontWidth, lfRotation, 0, FontWeight, Italic, 
									 	 FALSE, FALSE, DEFAULT_CHARSET, 
									 	 OUT_TT_PRECIS, CLIP_TT_ALWAYS,
					   				 	 PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
					   				 	 pEnumLogFont->elfLogFont.lfFaceName) == 0)
	{
		// Unable to select the font - probably because device cannot rotate fonts
		// arbitrarily.
		delete FontInfo.pRenderFont;
		FontInfo.pRenderFont = NULL;
		return FALSE;
	}

	// Select the real font into the DC.
	FontInfo.pOldFont = RenderDC->SetFont(FontInfo.pRenderFont);

	// Validate the cache
	FontInfo.Typeface = Typeface;
	FontInfo.Bold 	  = Bold;
	FontInfo.Italic   = Italic;
	FontInfo.Width    = Width;
	FontInfo.Height   = Height;
	FontInfo.Rotation = Rotation;
#endif
	// Font created and selected ok
	return TRUE;
}

/********************************************************************************************

>	void OSRenderRegion::SetOSDrawingMode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/93
	Purpose:	Sets the current drawing mode.
	Scope:		Private

********************************************************************************************/

void OSRenderRegion::SetOSDrawingMode()
{
	// Set ROP2 code accordingly.
	switch (DrawingMode)
	{
		case DM_COPYPEN:
			RenderDC->SetLogicalFunction(wxCOPY);
			break;
		case DM_EORPEN:
		case DM_EORDITHER:
			// Same as EORPEN, except we want the brush to be set to what we ask for,
			// and not messed about with, so we use this enum value instead.
			// The ROP2 code is the same as we use for DM_EORPEN - it is CalcLogBrush()
			// that behaves differently with DM_EORDITHER.
			RenderDC->SetLogicalFunction(wxXOR);
			break;

		default:
			ERROR3("Bad drawing mode in OSRenderRegion::SetOSDrawingMode()");
	}

	// When we change the ROP2 code, we also have to recreate the pen and brush objects,
	// otherwise we don't get an awful lot happening.  This was found out by trial and
	// error, and not by any of Microsoft's manuals telling us this...(Tim)
	SetLineAttributes();
	SetFillAttributes();
}

/********************************************************************************************

>	BOOL OSRenderRegion::StartRender()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93
	Purpose:	Initialises an OSRenderRegion for rendering.
	SeeAlso:	OSRenderRegion::StopRender()

********************************************************************************************/

BOOL OSRenderRegion::StartRender()
{
	// Call base class first
	if (!RenderRegion::StartRender())
		return FALSE;

	ENSURE(RenderDC != NULL,"StartRender Called when DC was Invalid");

	// Check that Initialise hasn't been called already
	ENSURE(RenderFlags.Rendering == FALSE, "Initialise called whilst already rendering");
	TRACEUSER("Gavin",_T("OSRenderRegion::StartRender - RenderFlags.Rendering = TRUE;\n"));
	RenderFlags.Rendering = TRUE;

	PixelScale = RenderView->GetPixelWidth();

	InitClipping();
	InitAttributes();

	if (RFlags.UsePalette)
		OldPalette = PaletteManager::StartPaintPalette(RenderDC);

	GDrawBrush.Start();

#if defined(_DEBUG) && defined(__WXMSW__)
// **** DEBUG BODGE - Make GDI redraw everything immediately
// instead of buffering stuff, so we can see exactly when stuff goes wrong
GdiSetBatchLimit(1);
#endif

	return TRUE;
}

/********************************************************************************************

>	BOOL OSRenderRegion::StopRender()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93
	Inputs:		RenderState is a Node* pointing at the current rendering node in the tree,
				or NULL if all objects rendered.
	Purpose:	Stops the rendering of a OSRenderRegion, saving it's current renderstate so
				that rendering can continue where it left off, later on.  If the RenderState
				passed is NULL then the RenderRegion will be unlinked from the list and will
				then delete itself.
	SeeAlso:	OSRenderRegion::StartRender()

********************************************************************************************/

BOOL OSRenderRegion::StopRender()
{
// ************************************************************
// Some Debug code to plot the clipping rectangle
#if 0
	{
		TRACE( _T("OSRR# DebugClipRect = (%d, %d) - (%d, %d)\n"), CurrentClipRect.lo.x, CurrentClipRect.lo.y, CurrentClipRect.hi.x, CurrentClipRect.hi.y);
		DocRect TempRect(CurrentClipRect);
		TempRect.Inflate(-GetScaledPixelWidth());
		TRACE( _T("OSRR# Filling = (%d, %d) - (%d, %d)\n"), TempRect.lo.x, TempRect.lo.y, TempRect.hi.x, TempRect.hi.y);
		Path ClipPath;
		ClipPath.Initialise();
		ClipPath.CreatePathFromDocRect(&TempRect);

		SaveContext();
		SetLineColour(COLOUR_TRANS);
		SetFillColour(COLOUR_RED);
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
// ************************************************************

	// Check that Initialise was called
	ENSURE(RenderFlags.Rendering, "DeInitialise called before Initialise");

	DeInitClipping();
	DeInitAttributes();

	if (OldPalette)
	{
		PaletteManager::StopPaintPalette(RenderDC, OldPalette);
		OldPalette = NULL;
	}

	// Clean out the Fuzzy Rectangles
	InnerRect.MakeEmpty();
	OuterRect.MakeEmpty();

	TRACEUSER("Gavin",_T("OSRenderRegion::StopRender - RenderFlags.Rendering = FALSE;\n"));
	RenderFlags.Rendering = FALSE;

	GDrawBrush.Stop();

	if (SepTables != NULL)
	{
		CCFree(SepTables);
		SepTables = NULL;
	}

	return TRUE;
}



/********************************************************************************************

>	virtual void OSRenderRegion::SetSolidColours(BOOL SetSolid)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/95

	Inputs:		SetSolid - TRUE to turn off dithering of colours

	Purpose:	Allows the dithering of colours to be turned off so they look the same
				using GDraw as they do with the GDI. In OSRenderRegions, this causes
				any attached GBrush technology to drop into/out of solid colour mode.

********************************************************************************************/

void OSRenderRegion::SetSolidColours(BOOL SetSolid)
{
	if (GDrawBrush.Available())
		GDrawBrush.SetSolidColours(SetSolid);
}



/********************************************************************************************

>	void OSRenderRegion::DrawPathToOutputDevice(Path* PathToDraw)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/93
	Inputs:		PathToDraw is a pointer to a Path object to render
	Purpose:	Renders a path object to the GDI
	SeeAlso:	OSRenderRegion::DrawRect()

********************************************************************************************/

void OSRenderRegion::DrawPathToOutputDevice(Path* PathToDraw, PathShape)
{
	// If we are not drawing complex shapes and this shape is, then return
	if ((!RenderComplexShapes) && (TestForComplexShape(&Caps)))
		return;

	// We want to draw it, so get pens and brushs
	GetValidPen();
	GetValidBrush();

	// and draw the path
	RenderPath(PathToDraw);					// Render the Path

	// draw the arrow heads on to it.
	DrawPathArrowHeads(NORMALPATH(PathToDraw));
}

/********************************************************************************************

	static BOOL SlowRectangle( CDC* pDC, const RECT& Rect)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		A rendering context and a rectangle.
	Returns:	TRUE if worked, FALSE if failed
	Purpose:	Renders a rectangle using Ploygon instead of Rectangle. Used for BodgeRectangles
				devices whi cannot render rectangles correctly. Should really only be used
				when no fill brush has been set as solid ones with XORing on don't render
				correctly.

********************************************************************************************/
/*
static BOOL SlowRectangle( wxDC* pDC, const wxRect& Rect)
{
	wxPoint NotRect[4];

	NotRect[0].x = Rect.left;
	NotRect[0].y = Rect.top;

	NotRect[1].x = Rect.right;
	NotRect[1].y = Rect.top;
							 
	NotRect[2].x = Rect.right;
	NotRect[2].y = Rect.bottom;

	NotRect[3].x = Rect.left;
	NotRect[3].y = Rect.bottom;

	return pDC->DrawPolygon(4,NotRect);
}
*/
/********************************************************************************************

>	void OSRenderRegion::DrawRect(DocRect* RectToDraw)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/93
	Inputs:		RectToDraw is a pointer to a DocRect object to render
	Purpose:	Renders a Rectangle to the GDI
	SeeAlso:	OSRenderRegion::DrawPath()

********************************************************************************************/

void OSRenderRegion::DrawRect(DocRect* RectToRend)
{
	// If we are not drawing complex shapes and this shape is, then return
	if ((!RenderComplexShapes) && (TestForComplexShape(&Caps)))
		return;

	GetValidPen();
	GetValidBrush();

//	TRACEUSER("Gavin",_T("RectToRend      : %08x %08x %08x %08x"),
//		  RectToRend->lo.x,RectToRend->lo.y,RectToRend->hi.x,RectToRend->hi.y);
//	TRACEUSER("Gavin",_T("CurrentClipRect : %08x %08x %08x %08x"),
//		  CurrentClipRect.lo.x,CurrentClipRect.lo.y,CurrentClipRect.hi.x,CurrentClipRect.hi.y);

	// First see if the rect intersects with the clip rect
	if (!RectToRend->IsIntersectedWith(CurrentClipRect))
		return;

	// OK, it intersects, but we should try to clip it with the inner rect
	DocRect DrawRect = RectToRend->Intersection(InnerRect);

//	TRACEUSER("Gavin",_T("DrawRect        : %08x %08x %08x %08x *"),
//		  DrawRect.lo.x,DrawRect.lo.y,DrawRect.hi.x,DrawRect.hi.y);

// NOTE
// Sometimes the PasteBoard and Page rects are dissapearing at high zoom.
// This seems to be because of the GDI 16 bit limit being exceeded.
// We need to check for the overflow, or clip the rects ourselves. Hence the
// TRUE args on the end of DocRectToWin
	WinRect Rect;

	// Ok, all the Fuzzy clipping has been taken care of, now convert and draw the rectangle we have left
/*	if (BodgeRectangles)
	{
		// if using Ploygon to draw rectangles, no bodging of co-ords is required at all
		// (as you would expect)
*/		Rect = DocRectToWin(DrawRect, 0,0,0,0, TRUE);
/*	}
	else if (RR_STROKECOLOUR().IsTransparent())
	{
		Rect = DocRectToWin(DrawRect, 0,0,1,1, TRUE);
	}
	else
	{
		Rect = DocRectToWin(DrawRect, 0,-1,1,0, TRUE);
	}

	BOOL RectOK;

	// Draw the Rectangle
	if (BodgeRectangles)
	{
		RectOK = SlowRectangle(RenderDC, Rect);
		ENSURE(RectOK,"CDC::Rectangle failed in DrawRect");
	}
	else
*/		RenderDC->DrawRectangle(Rect);

}

/********************************************************************************************

>	void OSRenderRegion::DrawDragRect(DocRect* RectToDraw)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/93
	Inputs:		RectToDraw is a pointer to a DocRect object to render
	Purpose:	Renders a Rectangle to the GDI . Used to claim "without shifting it about"
				bit it does adjust it now for the differences in rendering models.
	SeeAlso:	OSRenderRegion::DrawPath()

********************************************************************************************/

void OSRenderRegion::DrawDragRect(DocRect* RectToDraw)
{
	GetValidPen();
	GetValidBrush();

// NOTE
// Sometimes the PasteBoard and Page rects are dissapearing at high zoom.
// This seems to be because of the GDI 16 bit limit being exceeded.
// We need to check for the overflow, or clip the rects ourselves.

	WinRect Rect = DocRectToWin(*RectToDraw, 0,0,0,0);

	// for some reason, all flavours of GDI can screw up when asked to render zero-width/
	// height rectangles when in XOR mode, as it often leaves little pixels behind. Our fix
	// for this is to simply not render anything if zero width or height in EOR mode

	if ( DrawingMode != DM_EORPEN ||					// if not EOR then always
		(
			Rect.width !=1 &&			// else must have width
			Rect.height!=1				// and height
		) )
		RenderDC->DrawRectangle(Rect);		// Draw the Rectangle
}



/********************************************************************************************

>	void OSRenderRegion::DrawDragBounds(DocRect *RectToRender)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/95

	Inputs:		RectToRender - The bounding rectangle we wish to render.

	Purpose:	Used to draw a "rectangle" to screen (for dragging). However, the corners of
				the rectangle are passed through the current rendering matrix, so the rect
				may well be rotated or skewed by the time it reaches the screen.

				The rectangle is never filled, and will be stroked in XOR mode, using the
				current line colour. (i.e. only line colour needs to be set before calling
				this method, and it'll *always* be XOR'd regardless of other settings)

				This is used for XOR dragging of selected objects as they are skewed, rotated,
				translated, etc

	Notes:		DO NOT confuse this method with DrawDragRect (which will draw a rectangle
				without skew or rotation)

	SeeAlso:	OSRenderRegion::DrawDragRect; RenderRegion::DrawDragBounds;
				Range::RenderXOROutlinesOn

********************************************************************************************/

void OSRenderRegion::DrawDragBounds(DocRect *RectToRender)
{
	INT32 OurOldBkMode = RenderDC->GetBackgroundMode() ;
	INT32 OurOldDrawingMode = RenderDC->GetLogicalFunction();
	wxPen OurOldPen = RenderDC->GetPen() ;

	wxPen DotPen(CalcEORColour(RR_STROKECOLOUR()),1,wxDOT);
	RenderDC->SetPen(DotPen);
	RenderDC->SetBackgroundMode(wxTRANSPARENT);
	RenderDC->SetLogicalFunction(wxXOR);

	// Now, render the 4 lines, converting the corners of the rectangle with the
	// current rendering matrix
	WinCoord Points[5];
	Points[0] = 
	Points[4] = DocCoordToWin(RectToRender->lo);
	Points[1] = DocCoordToWin(DocCoord(RectToRender->lo.x, RectToRender->hi.y));
	Points[2] = DocCoordToWin(RectToRender->hi);
	Points[3] = DocCoordToWin(DocCoord(RectToRender->hi.x, RectToRender->lo.y));

	// And close the shape
	RenderDC->DrawLines(5,Points);

	// And restore the previous GDI settings
	RenderDC->SetLogicalFunction(OurOldDrawingMode);
	RenderDC->SetBackgroundMode(OurOldBkMode);
	RenderDC->SetPen(OurOldPen);
}


/********************************************************************************************

>	void OSRenderRegion::DrawDashLine(const DocCoord &StartPoint, const DocCoord &EndPoint)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Inputs:		StartPoint  - start of the line
				EndPoint	- end of the line
	Purpose:	Draws a dashed line between the two points.
				Uses the technology from DrawDragBounds()
	SeeAlso:	OSRenderRegion::DrawDragBounds()

********************************************************************************************/

void OSRenderRegion::DrawDashLine(const DocCoord &StartPoint, const DocCoord &EndPoint)
{
	INT32 OurOldBkMode = RenderDC->GetBackgroundMode() ;
	INT32 OurOldDrawingMode = RenderDC->GetLogicalFunction();
	wxPen OurOldPen = RenderDC->GetPen() ;

	wxPen DotPen(CalcEORColour(RR_STROKECOLOUR()),1,wxDOT);
	RenderDC->SetPen(DotPen);
	RenderDC->SetBackgroundMode(wxTRANSPARENT);
	RenderDC->SetLogicalFunction(wxXOR);

	// Now, render the line
	WinCoord StartPt = DocCoordToWin(StartPoint);
	WinCoord EndPt   = DocCoordToWin(EndPoint  );
	RenderDC->DrawLine(StartPt,EndPt);

	// And restore the previous GDI settings
	RenderDC->SetLogicalFunction(OurOldDrawingMode);
	RenderDC->SetBackgroundMode(OurOldBkMode);
	RenderDC->SetPen(OurOldPen);
}

/********************************************************************************************

>	void OSRenderRegion::DrawLine(const DocCoord &StartPoint, const DocCoord &EndPoint)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/93
	Inputs:		StartPoint is the Inclusive start coordinate.
				EndPoint is thw Exclusive end coordinate.
	Purpose:	Renders a Line to the GDI
	SeeAlso:	OSRenderRegion::DrawPath(); OSRenderRegion::DrawRect()

********************************************************************************************/

void OSRenderRegion::DrawLine(const DocCoord &StartPoint, const DocCoord &EndPoint)
{
	// If we are not drawing complex shapes and this shape is, then return
	if ((!RenderComplexShapes) && (TestForComplexShape(&Caps)))
		return;

	GetValidPen();
	GetValidBrush();
	
	const UINT32 LINEVERTICES = 2;
	wxPoint LinePoints[LINEVERTICES];
	LinePoints[0] = DocCoordToWin(StartPoint);	//LinePoints[0].y--;
	LinePoints[1] = DocCoordToWin(EndPoint);	//LinePoints[1].y--;

	RenderDC->DrawLine(LinePoints[0],LinePoints[1]);	// Draw the Line
#if 0
	if (!StartArrow.IsNull)
		DrawLineArrow(StartArrow, StartPoint, EndPoint);

	if (!EndArrow.IsNull)
		DrawLineArrow(EndArrow, EndPoint, StartPoint);
#endif
}

/********************************************************************************************

>	void OSRenderRegion::DrawPixel(const DocCoord &Point)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/02/94
	Inputs:		Point - the coordinate to plot the point at.
	Purpose:	Plot a single pixel in a render region.

********************************************************************************************/

void OSRenderRegion::DrawPixel(const DocCoord &Point)
{
	wxPoint LinePoint = DocCoordToWin(Point);
//	RenderDC->SetPixel(LinePoint,
//				ConvertColourToScreenWord(CurrentColContext, &RR_STROKECOLOUR()));
	INT32 R,G,B;
	RR_STROKECOLOUR().GetRGBValue(&R,&G,&B);
	RenderDC->SetPen(wxColour(R,G,B));
	RenderDC->DrawPoint(LinePoint);
}

/********************************************************************************************

>	void OSRenderRegion::DrawCross(const DocCoord &Point, const UINT32 Size)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/02/94
	Inputs:		Point - the centre point of the cross.
				Size - the length of the lines used to draw the cross with.
	Purpose:	Draws a cross in the render region.  The cross consists of a vertical
				line 'Size' millipoints high, and a horizontal line 'Size' millipoints
				wide.

********************************************************************************************/

void OSRenderRegion::DrawCross(const DocCoord &Point, const UINT32 Size)
{
	GetValidPen();
	
	const MILLIPOINT Length = Size * ScaledPixelWidth;

	// Work out cross coordinates in documenty coords
	DocCoord Coords[3];
	Coords[0] = Point;

	Coords[1].x = Point.x - Length;
	Coords[1].y = Point.y - Length;

	Coords[2].x = Point.x + Length + ScaledPixelWidth;  
	Coords[2].y = Point.y + Length + ScaledPixelWidth;

	// Convert to window coordinates
	wxPoint Points[3];
	Points[0] = DocCoordToWin(Coords[0]);
	Points[1] = DocCoordToWin(Coords[1]);
	Points[2] = DocCoordToWin(Coords[2]);

	// Draw the horizontal line
	RenderDC->DrawLine(Points[1].x,Points[0].y,Points[2].x,Points[0].y);
	// Draw the vertical line
	RenderDC->DrawLine(Points[0].x,Points[1].y,Points[0].x,Points[2].y);
}



/********************************************************************************************

>	virtual void OSRenderRegion::DrawFixedSystemText(StringBase *TheText,
														DocRect &BoundsRect)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
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

	SeeAlso:	OSRenderRegion::GetFixedSystemTextSize;
				OSRenderRegion::SetFixedSystemTextColours

********************************************************************************************/

void OSRenderRegion::DrawFixedSystemText(StringBase *TheText, DocRect &BoundsRect, UINT32 uFormat /* = FORMAT_SINGLELINE | FORMAT_NOPREFIX | FORMAT_VCENTER */)
{
	PORTNOTETRACE("text","OSRenderRegion::DrawFixedSystemText - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	WinRect Rect(0,0,0,0);
	if (uFormat & FORMAT_CALCRECT) // just calculate the rect needed to draw the text and return
	{
		INT32 LineHeight = 0;
		// This won't actually draw the text, instead it returns a rectangle in 'Rect'	
		LineHeight = RenderDC->DrawText((TCHAR *) (*TheText), -1, &Rect, uFormat);

		INT32 XDPI = GetDeviceCaps(RenderDC->m_hDC, LOGPIXELSX);
		INT32 YDPI = GetDeviceCaps(RenderDC->m_hDC, LOGPIXELSY);

		if(XDPI == 0 || YDPI == 0 || LineHeight == 0)
		{
			ERROR3("Can not calculate text size");
	  		BoundsRect = DocRect(0, 0, 0, 0);
			return;
		}
		// For some reason, Rect.bottom and Rect.top seem to be incorrect, so we have
		// to use the returned LineHeight value
		BoundsRect = DocRect(0, 0,
							  (INT32)(((double)Rect.width * IN_MP_VAL) / XDPI),
							  (INT32)(((double)LineHeight * IN_MP_VAL) / YDPI) );
		return;
	}
	
	
	Rect = DocRectToWin(BoundsRect, 0,-1,1,0, TRUE);

	// Small 'fix' - If we DrawRect the 'BoundsRect' then windows draws the text 1 pixel
	// further to the right than the rectangle. This just ensures that the text is always
	// clipped inside the rectangle rather than outside (at worst, a pixel too far inside)
	if (Rect.width>0)		// Still a valid rectangle?
	{
		Rect.x--;
		INT32 LineHeight;

		RenderDC->SetBackgroundMode(wxTRANSPARENT);

		LineHeight = RenderDC->DrawText((TCHAR *) (*TheText), -1, &Rect, uFormat);

		ENSURE(LineHeight > 0, "OSRenderRegion::DrawFixedSystemText failed");
	}
#endif
}



/********************************************************************************************

>	virtual void OSRenderRegion::SetFixedSystemTextColours(DocColour *TextCol,
														 DocColour *Background)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
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

	SeeAlso:	OSRenderRegion::DrawFixedSystemText

********************************************************************************************/

void OSRenderRegion::SetFixedSystemTextColours(DocColour *TextCol, DocColour *Background)
{
	if (TextCol != NULL)
	{
		COLORREF Clr = ConvertColourToScreenWord(CurrentColContext, TextCol);
		RenderDC->SetTextForeground(Clr);
	}

	if (Background != NULL)
	{
		COLORREF Clr = ConvertColourToScreenWord(CurrentColContext, Background);
		RenderDC->SetTextBackground(Clr);
	}
}



/********************************************************************************************

>	virtual void OSRenderRegion::GetFixedSystemTextSize(StringBase *TheText,
														DocRect *BoundsRect)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/3/95

	Inputs:		TheText - The string to obtain the rendered size from

	Outputs:	BoundsRect - Returned with the size of the rectangle needed to display
				said text string. This rect always has one corner at (0,0)

	Returns:	-

	Purpose:	To determine how much room is needed to plot a bit of text with
				OSRenderRegion::DrawFixedSystemText

	Notes:		If for any reason the call fails, an origin based rectangle with zero
				height and width will be returned.
				
	SeeAlso:	OSRenderRegion::DrawFixedSystemText

********************************************************************************************/

void OSRenderRegion::GetFixedSystemTextSize(StringBase *TheText, DocRect *BoundsRect, double* atDpi)
{
	PORTNOTETRACE("text","OSRenderRegion::GetFixedSystemTextSize - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR3IF(TheText == NULL, "OSRenderRegion::GetFixedSystemTextSize given a null text pointer");
	ERROR3IF(BoundsRect == NULL, "OSRenderRegion::GetFixedSystemTextSize given a null bounds rect pointer");
	if(TheText == NULL || BoundsRect == NULL)
		return;

	WinRect Rect(0,0,0,0);
	INT32 LineHeight = 0;

	// This won't actually draw the text, instead it returns a rectangle in 'Rect'	
	LineHeight = RenderDC->DrawText((TCHAR *) (*TheText), -1, &Rect,
									DT_SINGLELINE | DT_NOPREFIX | DT_CALCRECT);

	INT32 XDPI = GetDeviceCaps(RenderDC->m_hDC, LOGPIXELSX);
	INT32 YDPI = GetDeviceCaps(RenderDC->m_hDC, LOGPIXELSY);

	if(XDPI == 0 || YDPI == 0 || LineHeight == 0)
	{
		ERROR3("OSRenderRegion::GetFixedSystemTextSize failed");
	  	*BoundsRect = DocRect(0, 0, 0, 0);
		return;
	}

	// For some reason, Rect.bottom and Rect.top seem to be incorrect, so we have
	// to use the returned LineHeight value

	*BoundsRect = DocRect(0, 0,
						  (INT32)(((double)Rect.width * IN_MP_VAL) / XDPI),
						  (INT32)(((double)LineHeight * IN_MP_VAL) / YDPI) );
#endif
}



#define MAX_POLYGONS	256


/********************************************************************************************

>	INT32 OSRenderRegion::RawRenderPath( DocCoord *const Coords, PathVerb *const Verbs, 
									   INT32 NumCoords, INT32 *PolyCount, INT32 *ResCount,
									   INT32 Flatness = 0, INT32 *pActualFlatness = NULL )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/94
	Inputs:		A path to be drawn (in DocCoords).
				Flatness - the value to use when flattening a curve.
				pActualFlatness - if the caller wants to know what flatness was eventually
								  reached while flattening the path, it passes in a pointer
								  here to be filled in.  If the path could not be flattened,
								  this contains the last flatness level that was tried
								  before giving up.
	Outputs:	PolyCounts must point to an array of MAX_POLYGONS ints which will be filled
				in with a list of polygon sizes (can be just 1 big if ResCount is NULL). 
				*ResCount will be updated with the number of polygons. If it is NULL then the
				routine returns after the end of the first sub-path has been found.
				The points calculated from this will be in the PointArray variable.
	Returns:	Number of points read, 0 if failed (e.g. not enough memory or too complex).
	Purpose:	Low level primitive for rendering paths without GDI32. Note that PointArray
				should be freed by the caller upon completion.
	Errors:		-
	Scope:		Private
	SeeAlso:	RawRenderPath32

********************************************************************************************/

INT32 OSRenderRegion::RawRenderPath( DocCoord *const Coords, PathVerb *const Verbs, INT32 NumCoords, 
								   INT32 *PolyCounts, INT32 *ResCount,
								   INT32 Flatness, INT32 *pActualFlatness )
{
	// Flatness less than 0 will screw up, so let's ignore it
	if (Flatness < 0)
		return 0;

	// We have to manually flatten the beziers in the curve and decide when to draw them.
	INT32 MaxFlatness;

	// Decide how flat we are going to make this curve, if the caller has not specified
	if (Flatness == 0)
	{
		// (Tim says: these figures are a bit arbitrary, especially the MaxFlatness ones)
		if (RFlags.Metafile)
		{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
			// If this is a Metafile view then go and find out the flatness
			if (RenderView->IS_KIND_OF(MetafileView))
				Flatness = ((MetafileView*)RenderView)->GetMetafileFlatness();
			else
#endif
				Flatness = 512;
		}
		else
		{
			Flatness = CalcPathFlattening();
		}
	}

	MaxFlatness = Flatness * 8;

	// The number of polygons found so far
	INT32 Count = 0;

	// The number of points that have been placed in complete polygons
	INT32 PointsSoFar = 0;

	// Position we are reading points from
	INT32 ReadPos = 0;

	// Flatten curve until it works or flatness becomes too inaccurate.
	BOOL Worked = FALSE;
	while (!Worked && (Flatness <= MaxFlatness) &&(Flatness > 0))
	{
		// Position we are adding points at
		InsertPos = 0;

		ReadPos = 0;
		Count = 0;
		PointsSoFar = 0;

		Worked = TRUE;
		BOOL KeepGoing = TRUE;

		// loop through the whole path
		while (
				KeepGoing &&
				(ReadPos < NumCoords) &&
				Worked
			  )
		{
			if ( InsertPos >= SIZEOF_POLYLINE_BUFFER )
			{
				// this check is enough for those times when CurrentPoint is simply incrememented,
				// but is insufficient when it is increased by more than one, so additional checks
				// are needed elsewhere
				TRACE( _T("Path too complex to flatten\n"));
				InsertPos = 0;
				Worked = FALSE;
				break;
			}

			// Find out the type of element that we are over (after the close flag has been removed)
			Coord P[4];
			INT32 OldInsertPos;

			switch ( (Verbs[ReadPos]) & (~PT_CLOSEFIGURE) )
			{
				case PT_MOVETO:
					// This represents the start of a new polygon, so finish the last one
					if (InsertPos>0)
					{
						// We have put points into the polygon, so this is not the first one
						PolyCounts[Count] = InsertPos - PointsSoFar;

						// Keep tabs on how many points have been put in polygons so far
						PointsSoFar = InsertPos;

						// Count the number of polygons
						Count++;

						// Make sure its not too many
						if (Count == MAX_POLYGONS)
						{
							TRACE( _T("Too many polygons in path to render\n") );
							InsertPos = 0;
							Worked = FALSE;
							break;
						}

						if (ResCount==NULL)
						{
							// sub-path has ended, stop the loop now
							KeepGoing = FALSE;
							break;
						}
					}

					// Put the MoveTo into the point array
					PointArray[InsertPos] = DocCoordToWin(Coords[ReadPos]);
					InsertPos++;
					ReadPos++;
					break;


				case PT_LINETO:
					// Put the LineTo into the point array
					PointArray[InsertPos] = DocCoordToWin(Coords[ReadPos]);
					InsertPos++;
					ReadPos++;
					break;


				case PT_BEZIERTO:
					// If this point is a bezier, then the next 2 points should be beziers to
					ENSURE((Verbs[ReadPos+1]) & (~PT_CLOSEFIGURE), "Bezier found with 1 point");
					ENSURE((Verbs[ReadPos+2]) & (~PT_CLOSEFIGURE), "Bezier found with 2 points");
				
					// Make sure that this is not at the start of the path
					ENSURE(ReadPos>0, "Broken path found while flattening" );
					OldInsertPos = InsertPos;

					// Flatten the bezier out
					P[0] = DocCoordToOS256(Coords[ReadPos-1]);
					P[1] = DocCoordToOS256(Coords[ReadPos]);
					P[2] = DocCoordToOS256(Coords[ReadPos+1]);
					P[3] = DocCoordToOS256(Coords[ReadPos+2]);

					if (!Bezier(P[0].x,P[0].y, P[1].x,P[1].y, P[2].x,P[2].y, P[3].x,P[3].y, Flatness))
					{
						// Not enough room to flatten bezier.
						Worked = FALSE;
						break;
					}

		 			// If the curve was already flat then check to see if the curve was a vertical
					// or horizontal line.  If it was then ensure the coords are the same to avoid kinks.
					if (InsertPos == OldInsertPos+1)
					{
						if (Coords[ReadPos-1].x == Coords[ReadPos+2].x)
							PointArray[OldInsertPos].x = PointArray[OldInsertPos-1].x;
						if (Coords[ReadPos-1].y == Coords[ReadPos+2].y)
							PointArray[OldInsertPos].y = PointArray[OldInsertPos-1].y;
					}

					// Update the positions and break
					// InsertPos is updated by the bezier functions as the number of points
					// that they add depends on the curve
					ReadPos+=3;
					break;

				default:
					ENSURE( FALSE, "We found a Path Element that does not exist!" );
					break;
			}

			if (ResCount==NULL)
			{
				// caller wants sub-paths, better check for close markers
				if (Verbs[ReadPos-1] & PT_CLOSEFIGURE)
					KeepGoing = FALSE;
			}
		}

		// Increase flatness in case we need to try again.
		Flatness *= 2;
	}
	
	if (!Worked)
	{
		// oops - didn't work
	}
	// complete the information about the last polygon in the list
	else if (InsertPos>0)
	{
		PolyCounts[Count] = InsertPos - PointsSoFar;
		Count++;

		if (ResCount)
			*ResCount = Count;
	}
	else
	{
		if (ResCount)
			*ResCount = 0;
		else
		{
			ENSURE(FALSE, "strange short path");
			Worked = FALSE;										// don't use incomplete data
		}
	}

	// Tell the caller what flatness we used if necessary
	if (pActualFlatness != NULL)
		*pActualFlatness = Flatness;

	// return value is number of verb/coord pairs read.
	return Worked ? ReadPos : 0;
}

/********************************************************************************************

>	BOOL OSRenderRegion::SetClipToPathTemporary( Path *const PathToDraw)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/94
	Inputs:		A path to be clipped to (in DocCoords).
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed (e.g. not enough memory or cannot do it).
	Purpose:	To set a complex clip path. This clip is pretty temporary and should be reset
				afterwards using OSClipRegion before anything much else is done.
	Errors:		-
	Scope:		Private

********************************************************************************************/

BOOL OSRenderRegion::SetClipToPathTemporary( Path *const PathToDraw)
{
	PORTNOTETRACE("other","OSRenderRegion::SetClipToPathTemporary - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	BOOL Worked;
	Worked = RawRenderPath32( PathToDraw );
	if (Worked)
	{
//		Worked = ::SelectClipPath( RenderDC, RGN_AND );
		if (!Worked)
			TRACE( _T("SelectClipPath failed"));
	}
	else
		TRACE( _T("RawPath32 failed in Clip"));
	return Worked;
#else
	return FALSE;
#endif
}

// define this to 1 for testing manual line flattening, 0 for real code
#define	TEST_MANUAL_LINES	0

#ifdef _MAC
	// usefully Macs are limited to 32k of local vars (!)
	#define	LONGEST_SEGMENT	2048
#else
	#define	LONGEST_SEGMENT 8192
#endif
static DocCoord ConvPoints[ LONGEST_SEGMENT ];
static PathVerb ConvVerbs[ LONGEST_SEGMENT ];

// this required a null pen to have been selected. Fill doesn't matter

BOOL OSRenderRegion::StrokeProperly( Path *const DrawPath )
{

	const INT32 Flatten = ScaledPixelWidth/2;

	DashType* pDashRec = NULL;
	DashType GavinDash;

	// Don't do it if we have transparent stroke colour
	DocColour Col = RR_STROKECOLOUR();
	if (Col.IsTransparent())
		return TRUE;

	// Don't do dash patten if there isn't one, or if the line has zero width.
	if ((RR_DASHPATTERN().Elements == 0) || (RR_LINEWIDTH() == 0))
	{
		// No dash pattern
		pDashRec = &GavinDash;
		MakeDashType(RR_DASHPATTERN(), pDashRec);
	}
	else
	{
		// Dash pattern - convert to Gavin-ness...
		MILLIPOINT Width = RR_LINEWIDTH();
		DashRec &DashPattern = RR_DASHPATTERN();

		if ((RRQuality.GetLineQuality() >= Quality::FullLine) && DashPattern.Elements > 0)
		{
			INT32 Length = DashPattern.Elements;

			if (Length > 8) Length = 8;

			BOOL DoScale = DashPattern.ScaleWithLineWidth;
			FIXED16 Scale = DoScale ? (double(Width) / double(DashPattern.LineWidth)) : 1;

			GavinDash.Length = Length;
			GavinDash.Offset = LongMulFixed16(DashPattern.DashStart, Scale);

			for (INT32 el = 0; el < Length; el++)
			{
				GavinDash.Array[el] = LongMulFixed16(DashPattern.ElementData[el], Scale);
			}

			pDashRec = &GavinDash;
		}
	}

	INT32 Result = GRenderRegion::StrokePathToPath( DrawPath->GetCoordArray(),
									DrawPath->GetVerbArray(),
			   						DrawPath->GetNumCoords(),
									ConvPoints,
									ConvVerbs,
					 				sizeof(ConvVerbs),
									FALSE,
									RR_LINEWIDTH(),
									Flatten,
									RR_STARTCAP(),
									RR_JOINTYPE(),
									pDashRec );
	if (Result>0)
	{
		// Gavin has converted to another path - lets flatten & render that

		// switch filling colour to lines
		wxBrush NewBrush;
		#if TEST_MANUAL_LINES
		// for debugging, lets have a nice red pen and no fill
		NewBrush.CreateStockObject( NULL_BRUSH );
		wxPen DPen(wxBLACK,0);
		wxPen DOldPen = GetPen();
		RenderDC->SetPen( &DPen );
		#else
		// no quality checks here cos we only do this for high quality
//		LOGBRUSH BrushType;
//		CalcLogBrush( &BrushType, RR_STROKECOLOUR() );
//		NewBrush.CreateBrushIndirect( &BrushType );
		CalcLogBrush(&NewBrush,RR_STROKECOLOUR());
		#endif
		wxBrush OldBrush = RenderDC->GetBrush();
		RenderDC->SetBrush(NewBrush);

		// we whip through Gavin's sub-paths manually so we are less likely to
		// run out of memory when flattening them. It also stops PolyPolygon from
		// rampantly filling them wrongly. The PointArray will be allocated the first
		// time and we leave it alone

		INT32 PathSizeLeft = Result;						// items in Gavins path
		DocCoord* PathSoFar = ConvPoints;
		PathVerb* VerbSoFar = ConvVerbs;
		INT32 PolySize;									// size of this chunk

		while (PathSizeLeft > 0)
		{
			// flatten little sub-path				
			INT32 Read = RawRenderPath( PathSoFar, VerbSoFar, PathSizeLeft, &PolySize, NULL );

			if (Read)
			{
				// (We don't bother to try again if GDI fails, as it is extremely unlikely as
				// we are only doing small Gavin sub-paths from the path stroking routines).
				RenderDC->DrawPolygon(PolySize,PointArray,0,0,nFillStyle);		// render it
				PathSoFar += Read;								// inc pointers etc
				VerbSoFar += Read;
				PathSizeLeft -= Read;
				ENSURE(PathSizeLeft>=0, "Path backwards too far");
			}
			else
			{
				TRACE( _T("RawRenderPath failed\n"));
				break;						// stop as we don't know how far to continue
			}
		}
		RenderDC->SetBrush( OldBrush );

		#if TEST_MANUAL_LINES
		RenderDC->SetPen( DOldPen );
		DPen.DeleteObject();
		#endif

		return TRUE;
	}
	else if (Result<0)
	{
		return FALSE;								// as Gavin couldn't do it
	}
	else
		return TRUE;								// no points is not an error
}

void OSRenderRegion::MakeDashType(DashRec& KernelDash, DashType* GavinDash)
{
	INT32 Length = KernelDash.Elements;

	if (Length > 8) Length = 8;
		
	GavinDash->Length = Length;
	GavinDash->Offset = KernelDash.DashStart;

	for (INT32 el = 0; el < Length; el++)
	{
		GavinDash->Array[el] = KernelDash.ElementData[el];
	}
}

/********************************************************************************************

>	void OSRenderRegion::RenderPath( Path *DrawPath )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/93
	Purpose:	Renders a path to the GDI. Can recurse in the case of arrow heads.
	Scope:		Private

********************************************************************************************/


void OSRenderRegion::RenderPath( Path *DrawPath )
{
	/*
	26/9/94. Will. 

	Changed RFlag test, because of problems with NT bezier flattening.

	if (RFlags.GDI32 && (RR_DASHPATTERN().Elements == 0))
	{
		RenderPath32( DrawPath );
		return;
	}
	*/

	// Flags to indicate whether fill-providers or stroke-providers have filled or stroke
	// the path yet.
	BOOL ExtendedFill = FALSE;

	// If this is not a simple fill, set the flag to indicate this.
	FillGeometryAttribute *pFillProvider = 
		(FillGeometryAttribute *) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;

	if (pFillProvider->GetRuntimeClass() != CC_RUNTIME_CLASS(FlatFillAttribute))
		ExtendedFill = TRUE;

	// we can't do fancy fills if quality is low
	if (RRQuality.GetFillQuality() < Quality::Graduated)
		ExtendedFill = FALSE;

	//
	// We ought to check for stroke-providers in here, when the great day comes that
	// we actually have them...
	//
	// DY - 27/11/2000 The Great Day has arrived!!! If we have an AttrStrokeType
	// AND an AttrVariableWidth then we don't want Gavin to mess with our line widths
	
	BOOL StrokeProvided = FALSE;
	StrokeTypeAttrValue* pStroke = (StrokeTypeAttrValue*) GetCurrentAttribute(ATTR_STROKETYPE);
	VariableWidthAttrValue* pVarWidth = (VariableWidthAttrValue*) GetCurrentAttribute(ATTR_VARWIDTH);
	if (pVarWidth && pStroke)
	{
		// Test that these attrs are active and not just defaults that don't do anything
		if (pStroke->GetPathProcessor() != NULL && pVarWidth->GetWidthFunction() != NULL)
			StrokeProvided = TRUE;
	}

	// Get the fill provider to fill the path - if it can't handle this sort of render
	// region, then we fill the path for it.
	if (ExtendedFill)
	{
		if (!pFillProvider->RenderFill(this, DrawPath))
			// Unable to render fill for this render region - default to simple fill.
			ExtendedFill = FALSE;
	}

	wxPen NewPen,OldPen;

	// can ge get Gavin to manually flatten the path? Is it worth it?
	// Note that if we have already had a stroke provided (above then we don't want Gavin, 
	// and we also don't want GDI having a go either
	BOOL ManualStroke = !StrokeProvided;
	if (ManualStroke)
		ManualStroke = GRenderRegion::StrokePathAvailable();
	
	if (ManualStroke)
	{
		// is it worth it for this line? Must be wider than 1 pixel and high enough quality

		// First, is it disabled?
		if (!DoBetterLines)
		{
			ManualStroke = FALSE;
		}
		// We have to stroke for Dash patterns
		else if ((RR_DASHPATTERN().Elements != 0) && (RR_LINEWIDTH() > 0))
		{
			// Keep manual stroke as being TRUE...
		}
		else
		{
			INT32 dpLineWidth  = MPtoLP(RR_LINEWIDTH());

			// If no 32-bit GDI then we have to stroke via Gavin if the line is wider than 
			// a few device pixels, because	16-bit GDI can't do end-caps/joins for toffee.
			if (dpLineWidth > 0) //(!RFlags.GDI32 && (dpLineWidth > 3))
			{
				// Keep manual stroke as being TRUE...
			}
			// Don't bother with 0-width lines, or for very thin lines
			else if ((RR_LINEWIDTH() == 0) || (dpLineWidth <= 3))
			{
				ManualStroke = FALSE;
			}
			else if (RR_STROKECOLOUR().IsTransparent() ||				// if no line, don't bother
					 (RRQuality.GetLineQuality() < Quality::FullLine)	// if low quality, don't bother
			   		 )
			{
				ManualStroke = FALSE;
			}
		}
	}

	if (ManualStroke || StrokeProvided)
	{
		// if we're going to use Gavin for path stroking, or a stroke attribute has
		// already stroked the path then better stop GDI from outlining
		// the Polygons
//		NewPen.CreateStockObject( NULL_PEN );
		OldPen = RenderDC->GetPen();

		// AMB thinks (15-Mar-05) the following will cause problems, because wx on GTK doesn't
		// like drawing with the NULL pen. Use a transparent pen instead
		// RenderDC->SetPen(wxNullPen);
		RenderDC->SetPen(*wxTRANSPARENT_PEN);
	}

	INT32 Count;

	// An array of the number of points in each polygon
	INT32 PolyCounts[MAX_POLYGONS];

	// Use default flattening
	INT32 Flatness = 0;

	// In all, we reduce flattening 8 times before giving up (arbitrary number)
	INT32 Attempts = 8;

	BOOL Worked = FALSE;
	
	// Keep flattening until it works (or we have tried 8 times)
	while (!Worked && (Attempts > 0))
	{
		Worked = RawRenderPath( NORMALPATH(DrawPath), PolyCounts, &Count, Flatness, &Flatness );
		Attempts--;
	}

	if (Worked)
	{
		if (Count)
		{
#ifndef _MAC
			if (DrawPath->IsFilled && !ExtendedFill)
				RenderDC->DrawPolyPolygon(Count,PolyCounts,PointArray,0,0,nFillStyle);
			else
#endif
			{
				// render the little sub-paths. Win16 doesn't have Polypolyline
				wxPoint* PointList = PointArray;
				INT32 *CountList = PolyCounts;
				INT32 count;

				while (Count--)
				{
					count = *CountList++;
					RenderDC->DrawLines(count,PointList);
					PointList += count;
				}
			}
		}
	}
	else
	{
		TRACE( _T("Unable to flatten path into out buffer\n"));
	}

	// Let Gavin have a go at the lines now
	if (ManualStroke)
	{
		StrokeProperly( DrawPath );
		RenderDC->SetPen( OldPen );
	}
}


/********************************************************************************************

>	BOOL OSRenderRegion::Bezier(INT32 Px0,INT32 Py0, INT32 Px1,INT32 Py1,
								INT32 Px2,INT32 Py2, INT32 Px3,INT32 Py3, 
								INT32 Flatness)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/93
	Inputs:		Many INT32s - These are the x and y coords of the 4 bezier control points
				Flatness - the flatness of the curve
	Returns:	TRUE if bezier was successfully flattened to specified flatness;
				FALSE if buffer overflow occurred
	Purpose:	Flattens bezier path into an array of POINTS (for polyline).
	Scope:		Private

********************************************************************************************/

BOOL OSRenderRegion::Bezier(INT32 Px0,INT32 Py0, INT32 Px1,INT32 Py1,
							INT32 Px2,INT32 Py2, INT32 Px3,INT32 Py3, 
							INT32 Flatness)
{
	// We can't cope with negative flatness, so return if it is
	if (!(Flatness > 0))
		return FALSE;

	INT32 diff;

	INT32 dx0 = (Px1*3 - Px0*2 - Px3);
	dx0 = dx0 < 0 ? -dx0 : dx0;

	INT32 dy0 = (Py1*3 - Py0*2 - Py3);
	dy0 = dy0 < 0 ? -dy0 : dy0;
	
	// Get the line's distance from the curve
	if (dx0 >= dy0)
		diff = 3*dx0 + dy0;
	else
		diff = dx0 + 3*dy0;
		
	// Is the straight line close enough to the curve ?
	if (diff > Flatness)
	{
		// Not close enough so split it into two and recurse for each half
		return Split(Px0,Py0, Px1,Py1, Px2,Py2, Px3,Py3, Flatness);
    }
    
	INT32 dx1 = (Px2*3 - Px0 - Px3*2);
	dx1 = dx1 < 0 ? -dx1 : dx1;

	INT32 dy1 = (Py2*3 - Py0 - Py3*2);
	dy1 = dy1 < 0 ? -dy1 : dy1;
	
	// Get the line's distance from the curve
	if (dx1 >= dy1)
		diff = 3*dx1 + dy1;
	else
		diff = dx1 + 3*dy1;

	// Is the straight line close enough to the curve ?
	if (diff > Flatness)
	{
		// Not close enough so split it into two and recurse for each half
		return Split(Px0,Py0, Px1,Py1, Px2,Py2, Px3,Py3, Flatness);
    }

	if (InsertPos >= SIZEOF_POLYLINE_BUFFER)
		return FALSE;												// stop storing points

	// Line is now close enough so put it into our array
	PointArray[InsertPos].x = INT32((Px3+128)/256);
	PointArray[InsertPos].y = INT32((Py3+128)/256);
	InsertPos++;

	// Successfully flattened
	return TRUE;
}

/********************************************************************************************

>	BOOL OSRenderRegion::Split(INT32 Px0,INT32 Py0, INT32 Px1,INT32 Py1,
						   	   INT32 Px2,INT32 Py2, INT32 Px3,INT32 Py3, 
						   	   INT32 Flatness)


	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/93
	Inputs:		Many INT32s - These are the x and y coords for the 4 control points of
				a bezier
				Flatness - the flatness of the curve
	Returns:	TRUE if bezier was successfully split and flattened to specified flatness;
				FALSE if buffer overflow occurred
	Purpose:	Splits a bezier curve into two halves, and flattens each half.
	Scope:		Private

********************************************************************************************/

BOOL OSRenderRegion::Split(INT32 Px0,INT32 Py0, INT32 Px1,INT32 Py1,
						   INT32 Px2,INT32 Py2, INT32 Px3,INT32 Py3, 
						   INT32 Flatness)
{

	// Calculate the first half of the curve
	INT32 lx0 = Px0;
	INT32 ly0 = Py0;
	INT32 lx1 = (Px0 + Px1)/2;
	INT32 ly1 = (Py0 + Py1)/2;
	INT32 lx2 = (Px0 + 2*Px1 + Px2)/4;
	INT32 ly2 = (Py0 + 2*Py1 + Py2)/4;
	INT32 lx3 = (Px0 + 3*Px1 + 3*Px2 + Px3)/8;
	INT32 ly3 = (Py0 + 3*Py1 + 3*Py2 + Py3)/8;

	// Calculate the second half of the curve
	INT32 rx0 = lx3;
	INT32 ry0 = ly3;
	INT32 rx1 = (Px1 + 2*Px2 + Px3)/4;
	INT32 ry1 = (Py1 + 2*Py2 + Py3)/4;
	INT32 rx2 = (Px2 + Px3)/2;
	INT32 ry2 = (Py2 + Py3)/2;
	INT32 rx3 = Px3;
	INT32 ry3 = Py3;

	if (InsertPos >= SIZEOF_POLYLINE_BUFFER)
		return FALSE;										// stop recursion when buffer fills

	// Recurse for first half of curve
	if (!Bezier(lx0,ly0, lx1,ly1, lx2,ly2, lx3,ly3, Flatness))
		// Failure - buffer overflow
		return FALSE;

	if (InsertPos >= SIZEOF_POLYLINE_BUFFER)
		return FALSE;										// stop recursion when buffer fills

	// Recurse for second half of curve
	return Bezier(rx0,ry0, rx1,ry1, rx2,ry2, rx3,ry3, Flatness);
}



/********************************************************************************************

>	INT32 OSRenderRegion::CalcPathFlattening()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/93
	Returns:	The max distance the lines can be away from the curve in MILLIPOINTS
	Purpose:	Adjust the bezier curve flattening according to the resolution of the
				destination device and the view scale.
	Scope:		Private

********************************************************************************************/

INT32 OSRenderRegion::CalcPathFlattening()
{
	MILLIPOINT PixelWidth = GetScaledPixelWidth();	// The width of a pixel in DocCoords
	INT32 Accuracy = PixelWidth/8;					// Accurate to 1/8 of a pixel

	return max(Accuracy, INT32(10));		// Ensure the value is greater than 10 MILLPOINTS
										// to avoid stack overflows.
}

/********************************************************************************************

>	INT32 OSRenderRegion::MPtoLP(MILLIPOINT MPtoConvert)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/93
	Inputs:		MPtoConvert is a value in MILLIPOINTS
	Returns:	The size in logical units
	Purpose:	Converts a value in MilliPoints into a GDI Logical value.
				IT ASSUMES A MM_TEXT MAPPING MODE AT PRESENT.
	Scope:		Private

********************************************************************************************/

INT32 OSRenderRegion::MPtoLP(MILLIPOINT MPtoConvert)
{
	MILLIPOINT PixelWidth = GetScaledPixelWidth();
	return INT32(MPtoConvert/PixelWidth);	// Note this assumes MM_TEXT mapping mode	
}

/********************************************************************************************

>	MILLIPOINT OSRenderRegion::CalcScaledPixelWidth()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Returns:	The size of a pixel in MILLIPOINTS
	Purpose:	Calculates the size of a Pixel in MILLIPOINTS, based on the output DPI
				and current scale factor.
	SeeAlso:	CalcPixelWidth()

********************************************************************************************/

MILLIPOINT OSRenderRegion::CalcScaledPixelWidth()
{
//	return (RenderView->GetPixelWidth() / ScaleFactor).MakeLong();
	return (MILLIPOINT)( (RenderView->GetPixelWidth().MakeDouble() / ScaleFactor.MakeDouble()) + 0.5 );
/*
	return MILLIPOINT(double(72000) /
			(double(RenderDC->GetDeviceCaps(LOGPIXELSX)) * ScaleFactor.MakeDouble()));
*/
}



/********************************************************************************************

>	MILLIPOINT OSRenderRegion::CalcPixelWidth()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/93
	Returns:	The size of a pixel in MILLIPOINTS
	Purpose:	Calculates the size of a Pixel in MILLIPOINTS, based on the output DPI
				but IGNORES the scale factor.
	SeeAlso:	GetScaledPixelWidth()

********************************************************************************************/

MILLIPOINT OSRenderRegion::CalcPixelWidth()
{
PORTNOTE("other","Can't handle different DPIs, using X")
	return MILLIPOINT(double(72000) / double(OSRenderRegion::GetFixedDCPPI(*RenderDC).x)); // x
}

/********************************************************************************************

>	void OSRenderRegion::DrawBlob(DocCoord p, BlobType type)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/93
	Inputs:		p - The Coord of the point to draw a blob over
				type - the Type of Blob to draw. It can be either BT_SELECTED for a Selected
				point or BT_UNSELECTED for an unselected point
	Purpose:	Draw a blob. This is the type of blob that appear round objects to show that
				they have been selected.
	SeeAlso:	BlobType

********************************************************************************************/

void OSRenderRegion::DrawBlob(DocCoord p, BlobType type)
{
	DocRect MyBlob;

	GetBlobRect( ScaleFactor, p, type, &MyBlob);

	// I do not need to set the colour of the rect as that
	// should be done before this function is called
	// Draw Drag Rect draws a rect without shifting the sides about, so it is what
	// we need.
	DrawDragRect( &MyBlob );
}



/********************************************************************************************

>	void OSRenderRegion::DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/95
	Inputs:		Point - The Coord specifying the bottom left of the Bitmap.
				pBitmap, the KernelBitmap to plot.
	Purpose:	Draws an un-scaled Bitmap.
				Use this for drawing Icons for dialogue boxes etc.
				The Bitmap can be any size, but will be plotted unscaled.
				Note that 'Point' specifies where the bottom left of the bitmap will be.

********************************************************************************************/

void OSRenderRegion::DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap)
{
	PORTNOTETRACE("other","OSRenderRegion::DrawBitmap - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// If we are not drawing complex shapes and this shape is, then return
	if ((!RenderComplexShapes) && (TestForComplexShape(&Caps)))
		return;

	// Is there a Bitmap ??
	if (pBitmap == NULL || pBitmap->ActualBitmap == NULL)
	{
		ERROR3("NULL Bitmap passed to DrawBitmap");
		return;
	}

	// Get the 'Actual' windows Bitmap
	WinBitmap *WinBM = (WinBitmap*)pBitmap->ActualBitmap;

	// Is it valid ?
	if (
		(WinBM->BMInfo==NULL) ||
		(WinBM->BMBytes==NULL)
	   )
		return;		// if not valid

	// Remember the Size of the Bitmap (in pixels)
	INT32 Width  = WinBM->GetWidth();
	INT32 Height = WinBM->GetHeight();

	// Convert the DocCoord into a windows coord
	POINT Origin = DocCoordToWin(Point);
	Origin.y = Origin.y - Height;	// Translate bottom left to top left

	CDC MemDC;
	// Create a memory DC based on this render region
	MemDC.CreateCompatibleDC(RenderDC);

	CBitmap Bitmap;
	// Create a CBitmap the same size as our one
	Bitmap.CreateCompatibleBitmap(RenderDC, Width, Height);

	// Select the CBitmap into the memory DC so that we have
	// a blank bitmap to copy into
	CBitmap* OldBmp = MemDC.SelectObject(&Bitmap);

	if (OldBmp == NULL) 
	{
		TRACE( _T("Couldn't select Bitmap into CDC in DrawBitmap\n"));
		return;
	}

	// Now copy the Bits from our Kernel Bitmap into the Memory DC
	SetDIBitsToDevice(			MemDC.m_hDC,
								0,0,
								Width,
								Height,
								0,0,
								0,
								Height,
								WinBM->BMBytes,
								WinBM->BMInfo,
								DIB_RGB_COLORS
								);

	// Copy the Bitmap onto this render region
	RenderDC->BitBlt(			Origin.x, Origin.y,
								Width,
								Height,
								&MemDC,
								0,0,
								SRCCOPY
								);

	// Unselect the bitmap and delete it
	MemDC.SelectObject(OldBmp);
	Bitmap.DeleteObject();
#endif
}



/********************************************************************************************

>	void OSRenderRegion::DrawBitmap(const DocCoord &Point, UINT32 BitmapID, UINT32 ToolID = NULL)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/95
	Inputs:		Point - The Coord specifying the bottom left of the Bitmap.
				BitmapID, the resource ID of the bitmap to plot.
   				ToolID, the optional ToolID of the tool containing the Bitmap resource.
	Purpose:	Draws an un-scaled Bitmap.
				Use this for drawing Icons for dialogue boxes etc.
				The Bitmap can be any size, but will be plotted unscaled.
				Note that 'Point' specifies where the bottom left of the bitmap will be.

********************************************************************************************/

void OSRenderRegion::DrawBitmap(const DocCoord &Point, UINT32 BitmapID, UINT32 ToolID)
{
	PORTNOTETRACE("other","OSRenderRegion::DrawBitmap - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// If we are not drawing complex shapes and this shape is, then return
	if ((!RenderComplexShapes) && (TestForComplexShape(&Caps)))
		return;

	HBITMAP hBmp;

	if (ToolID != NULL)
	{
		// Find the tool that contains our Bitmap	
		OILTool* pTheTool = Tool::GetOILTool(ToolID);

		if (pTheTool == NULL)
		{
			TRACE( _T("Couldn't find the tool in DrawBitmapBlob\n"));
			return;
		}

		// Ask the Tool to load the bitmap and return a Handle
		hBmp = pTheTool->LoadBitmap(BitmapID);
	}
	else
	{
		// No tool was specified, so we'll try the Kernel
		hBmp = ::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(BitmapID));
	}

	if (hBmp == 0) 
	{	
		TRACE( _T("Bitmap failed to Load in DrawBitmapBlob\n"));
		return;
	}
	
	// Make a Bitmap Object 
	CBitmap Bitmap;
	BOOL ok = Bitmap.Attach(hBmp);

	if (!ok) 
	{	
		TRACE( _T("CBitmap failed to create in DrawBitmapBlob\n"));
		return;
	}

	// Find out how big the Bitmap Info is
	INT32 NumBytes = GetObject(hBmp, 0, NULL);

	// Make a Info Structure big enough to hold it
	BITMAPINFOHEADER* pBmpInfo = (BITMAPINFOHEADER*)new BYTE[NumBytes];

	if (pBmpInfo == NULL) 
	{
		TRACE( _T("Not enough memory for BitmapInfo in DrawBitmapBlob\n"));
		return;
	}

	// Get Info on the Bitmap
	GetObject(hBmp, NumBytes, pBmpInfo);

	// Extract the Width and Height
	INT32 Width  = pBmpInfo->biWidth;
	INT32 Height = pBmpInfo->biHeight;

	delete pBmpInfo;	// Don't need this any more

	// Convert the DocCoord into a windows coord
	POINT Origin = DocCoordToWin(Point);
	Origin.y = Origin.y - Height;	// Translate bottom left to top left

	CDC MemDC;
	// Create a Memory DC based on this render region
	MemDC.CreateCompatibleDC(RenderDC);
	// And select the Bitmap into it
	CBitmap* OldBmp = MemDC.SelectObject(&Bitmap);

	if (OldBmp == NULL) 
	{
		TRACE( _T("Couldn't select Bitmap into CDC in DrawBitmapBlob\n"));
		return;
	}

	// Plot the bitmap onto this render region
	RenderDC->BitBlt(			Origin.x, Origin.y,
								Width,
								Height,
								&MemDC,
								0,0,
								SRCCOPY
								);

	// Unselect our bitmap and delete it
	MemDC.SelectObject(OldBmp);

	ok = Bitmap.DeleteObject();
	if (!ok)
	{
	 	TRACE( _T("Delete Bitmap failed in DrawBitmapBlob\n"));
	}
#endif
}

BOOL OSRenderRegion::DrawTransformedBitmap(NodeBitmap *pNodeBitmap)
{
	PORTNOTETRACE("other","OSRenderRegion::DrawTransformedBitmap - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// If we are not printing, then we'll always render as a bitmap fill
	// (eg. Gallery items).
	if (IsPrinting())
	{
		// If we are not drawing complex shapes and this shape is, then return
		if ((!RenderComplexShapes) && (TestForComplexShape(&Caps)))
			return TRUE;

		if ((!RenderComplexShapes) && (pNodeBitmap->NeedsTransparency()))
			return TRUE;

		// If we've not got a valid colour plate or it is disabled
		if (CurrentColContext->GetColourPlate() == NULL ||
			CurrentColContext->GetColourPlate()->IsDisabled())
		{
			// If it is a simple rectangular shape then 
			// we can use GDI to render it
			if (pNodeBitmap->HasSimpleOrientation(this))
			{							 
				// Get hold of the coord array for the bitmap.
				DocCoord *Coords = pNodeBitmap->InkPath.GetCoordArray();

				// Work out where to render the bitmap on the device context - find top left corner
				OilCoord TopLeft(Coords[0].x, Coords[0].y);
				RenderMatrix.transform(&TopLeft);
				WinCoord DestTopLeft = TopLeft.ToWin(RenderView);

				// Find required size of bitmap on device context.
				MILLIPOINT Width  = Coords[1].x - Coords[0].x;
				MILLIPOINT Height = Coords[0].y - Coords[3].y;

				FIXED16 fxPixelSize = 0;
				RenderView->GetScaledPixelSize(&fxPixelSize, &fxPixelSize);
				double dPixelSize = fxPixelSize.MakeDouble();
				INT32 DestWidth = (INT32) ((((double) Width) / dPixelSize) + 0.5);
				INT32 DestHeight = (INT32) ((((double) Height) / dPixelSize) + 0.5);

				// Make sure the Blit is the correct quality
				INT32 OldMode = RenderDC->SetStretchBltMode(HALFTONE);

				// Must call SetBrushOrgEx() after setting the stretchblt mode 
				// to HALFTONE (see Win32 SDK docs).
				POINT OldOrg;
				SetBrushOrgEx(RenderDC->m_hDC, 0, 0, &OldOrg);

				// Get handle to bitmap (must be a WinBitmap as we are in winoil!)
				WinBitmap *WinBM = (WinBitmap *) pNodeBitmap->GetBitmap()->ActualBitmap;

				// Blit the bitmap to the DC
				INT32 Result = StretchDIBits( RenderDC->m_hDC, 

						  					DestTopLeft.x, DestTopLeft.y,
						  					DestWidth, DestHeight,

						  					0,0,										// source 0,0
						  					WinBM->BMInfo->bmiHeader.biWidth,			// source W
						  					WinBM->BMInfo->bmiHeader.biHeight,			// source H

						  					WinBM->BMBytes,
						  					WinBM->BMInfo,
						  					DIB_RGB_COLORS,
						  					SRCCOPY );

				// Restore StretchBlt mode and brush origin
				RenderDC->SetStretchBltMode(OldMode);
				SetBrushOrgEx(RenderDC->m_hDC, OldOrg.x, OldOrg.y, NULL);

				if (Result != GDI_ERROR)
				{
					// All done
					return TRUE;
				}
			}
		}
	}

	// We can't do arbitrarily transformed bitmaps - use a bitmap fill.
	RenderComplexShapes = TRUE;
	BOOL bOk = RenderRegion::DrawTransformedBitmap(pNodeBitmap);
	RenderComplexShapes = FALSE;

	return bOk;
#else
	return TRUE;
#endif
}

/********************************************************************************************

>	void OSRenderRegion::DrawBitmapBlob(const DocCoord &Point, KernelBitmap* BlobShape)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		Point - The Coord specifying the centre of the Blob.
				BlobShape - A Kernel Bitmap describing the Shape of the Blob.
							This is assumed to be a simple Monochrome Bitmap.
	Purpose:	Draws a Bitmap Blob. It is used for drawing more complex blob shapes, like
				'rotation' arrows etc.
				The Bitmap can be any size, but must be Monochrome.
				Black pixels are transparent.  White pixels will EOR.
				Note that 'Point' currently specifies where the centre of the bitmap will be.
	SeeAlso:	KernelBitmap

********************************************************************************************/

void OSRenderRegion::DrawBitmapBlob(const DocCoord &Point, KernelBitmap* BlobShape)
{
	PORTNOTETRACE("other","OSRenderRegion::DrawBitmapBlob - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ENSURE(BlobShape != NULL, "NULL Bitmap passed to DrawBitmapBlob");

	// Is there a Bitmap ??
	if (BlobShape->ActualBitmap == NULL)
		return;		// if no bitmap

	// Get the 'Actual' windows Bitmap
	WinBitmap *WinBM = (WinBitmap*)BlobShape->ActualBitmap;

	// Is it valid ?
	if (
		(WinBM->BMInfo==NULL) ||
		(WinBM->BMBytes==NULL)
	   )
		return;		// if not valid

	// Remember the Size of the Bitmap (in pixels)
	INT32 Width  = WinBM->BMInfo->bmiHeader.biWidth;
	INT32 Height = WinBM->BMInfo->bmiHeader.biHeight;

	// Convert the DocCoord into a windows coord
	POINT Origin = FindBitmapOrigin(Point, Width, Height);

	CDC MemDC;
	// Create a memory DC based on this render region
	MemDC.CreateCompatibleDC(RenderDC);

	CBitmap Bitmap;
	// Create a CBitmap the same size as our one
	Bitmap.CreateCompatibleBitmap(RenderDC, Width, Height);

	// Select the CBitmap into the memory DC so that we have
	// a blank bitmap to copy into
	CBitmap* OldBmp = MemDC.SelectObject(&Bitmap);

	if (OldBmp == NULL) 
	{
		TRACE( _T("Couldn't select Bitmap into CDC in DrawBitmapBlob\n"));
		return;
	}

	// Now copy the Bits from our Kernel Bitmap into
	// the Memory DC
	SetDIBitsToDevice(			MemDC.m_hDC,
								0,0,
								Width,
								Height,
								0,0,
								0,
								Height,
								WinBM->BMBytes,
								WinBM->BMInfo,
								DIB_RGB_COLORS
								);

	// EOR the Bitmap onto this render region
	RenderDC->BitBlt(			Origin.x, Origin.y,
								Width,
								Height,
								&MemDC,
								0,0,
								SRCINVERT
								);

	// Unselect the bitmap and delete it
	MemDC.SelectObject(OldBmp);
	Bitmap.DeleteObject();
#endif
}

/********************************************************************************************

>	void OSRenderRegion::DrawBitmapBlob(const DocCoord &Point, 
											UINT32 BitmapID, UINT32 ToolID = NULL)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/94
	Inputs:		Point - The Coord specifying the centre of the Blob.
				BitmapID - The Resource ID of the bitmap to use.
				ToolID - The Tool to fetch the bitmap from (or NULL for Kernel).
	Purpose:	Draws a Bitmap Blob. It is used for drawing more complex blob shapes, like
				'rotation' arrows etc.
				The Bitmap can be any size, but must be Monochrome.
				Black pixels are transparent.  White pixels will EOR.
				Note that 'Point' currently specifies where the centre of the bitmap will be.
	SeeAlso:	-

********************************************************************************************/

void OSRenderRegion::DrawBitmapBlob( const DocCoord &Point, ResourceID resID )
{
	wxBitmap *pBitmap = (CamArtProvider::Get())->FindBitmap( resID );
	if( NULL == pBitmap ) 
	{	
		TRACE( _T("wxBitmap failed to create in DrawBitmapBlob\n"));
		return;
	}

	// Extract the Width and Height
	INT32 Width  = pBitmap->GetWidth();
	INT32 Height = pBitmap->GetHeight();

	// Convert the DocCoord into a windows coord
	wxPoint Origin = FindBitmapOrigin(Point, Width, Height);

	// EOR the bitmap onto this render region
	wxMemoryDC	memDc;
	memDc.SelectObject( *pBitmap );
	RenderDC->Blit( Origin.x, Origin.y, Width, Height, &memDc, 0, 0, wxXOR );
}

/********************************************************************************************

>	POINT OSRenderRegion::FindBitmapOrigin(DocCoord Centre, INT32 Width, INT32 Height)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/94
	Inputs:		Centre - The DocCoord specifying the centre of the Bitmap.
				Width & Height of the bitmap in Pixels.
	Purpose:	Finds the windows origin to plot a bitmap at, so that it's centre will be
				at the DocCoord specified.

				Rewritten 20/06/95 by Phil: This routine now uses high-res coords until the
				final return command so that the centre of the bitmap can be matched to the
				centre of the pixel at the specified point. This allows these bitmap plots
				to be centered accurately using the same rules as thin lines and grid points.

				Karim 11/09/2000
				This method no longer uses GetScaledPixelWidth() or an integer pixel size,
				as these are inaccurate at high magnifications. Instead, it calculates its
				own floating-point pixel width, and converts to integer arithmetic as late
				as possible, for max. accuracy.

	Scope:		Protected
	SeeAlso:	-

********************************************************************************************/

wxPoint OSRenderRegion::FindBitmapOrigin(DocCoord Centre, INT32 Width, INT32 Height)
{
	DocCoord Origin;
	double PelSize;

	DocView *pDocView = DocView::GetSelected();
	ERROR3IF(pDocView == NULL, "OSRenderRegion::FindBitmapOrigin() when no view is selected!");
	if (pDocView)
		PelSize = (pDocView->GetPixelWidth() / pDocView->GetViewScale()).MakeDouble();
	else
		PelSize = 750;

	// Subract half the Width and Height
	Origin.x = Centre.x - (MILLIPOINT)(0.5 + (((double)Width * PelSize)/2));
	Origin.y = Centre.y + (MILLIPOINT)(0.5 + (((double)Height * PelSize)/2));
	
	return DocCoordToWin(Origin);
}

/********************************************************************************************

>	static void OSRenderRegion::GetBlobRect( FIXED16 Scale, const DocCoord& BlobPoint, BlobType bType, DocRect *pResult )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/93
	Inputs:		Scale - scale of view, normally DocView->GetViewScale()
				BlobPoint - The Coord of the centre of the blob
				bType - the Type of Blob to draw. It can be either BT_SELECTED for a Selected
				point or BT_UNSELECTED for an unselected point or BT_CLICKME for the click
				rectangle or BT_SELECTEDLARGEST for the larger of the two selected options.
	Outputs:	pResult - pointer to DocRect that will contain result
	Purpose:	To determine the DocRect of a blob without access to a render region

				Karim 22/05/2000 - added BT_CLIPVIEW blobtype, which lets me return the
				correct size for ClipView ToolObject blobs.

	SeeAlso:	BlobType
	Scope:		Public

********************************************************************************************/

void OSRenderRegion::GetBlobRect( FIXED16 Scale, const DocCoord& BlobPoint, BlobType bType, DocRect *pResult )
{
	// ideally we would call GetScaledPixelWidth(), but that is a member fn of
	// OSRenderRegion as we don't have a render region, so we do similar code but
	// here instead. (Andy's understanding is that this only works for square pixels BTW)

	// Note from Tim: this function is a problem because it used to use the now defunct
	// OilCoord::PixelWidth() - hence we need to get a view from somewhere.  We assume
	// that the selected DocView is a safe view to use because:
	//	(a) Hopefully we only draw blobs into the selected view
	//  (b) All DocViews will share the same pixel size... until we port to the Mac(!)
	MILLIPOINT BlobSize;
	DocView *pDocView = DocView::GetSelected();
	ERROR3IF(pDocView == NULL, "OSRenderRegion::GetBlobRect() when no view is selected!");

	// Karim 22/06/2000 - we're gonna use a double instead. Not much accuracy
	// is needed for PelSize, but a MILLIPOINT gives us far too little!
//	const MILLIPOINT PelSize = (pDocView->GetPixelWidth() / Scale).MakeLong();
	const double PelSize = (pDocView->GetPixelWidth() / Scale).MakeDouble();

	// All of the sizes extracted by this Switch statement are RADII.
	// So, for the blob to be centered correctly, the diameter of the blob will be
	// 		PIXEL_DIAMETER = 2 * RADIUS + 1 Pixel!!!!!
	//
	switch (bType)
	{
		case BT_UNSELECTED:
			BlobSize = (MILLIPOINT)((double)UnSelectedBlobSize * PelSize);
			break;

		case BT_SELECTED:
			BlobSize = (MILLIPOINT)((double)SelectedBlobSize * PelSize);
			break;

		case BT_CLICKME:
			BlobSize = (MILLIPOINT)((double)HitTestRadius * PelSize);
			break;

		case BT_SELECTEDLARGEST:
			BlobSize = max(	(MILLIPOINT)((double)UnSelectedBlobSize * PelSize),
							(MILLIPOINT)((double)SelectedBlobSize * PelSize) );
			break;

		case BT_MSTAGEFILLUNSELECTED:
			BlobSize = (MILLIPOINT)((double)MultiStageUnSelectedBlobSize * PelSize);
			break;

		case BT_MSTAGEFILLSELECTED:
			BlobSize = (MILLIPOINT)((double)MultiStageSelectedBlobSize * PelSize);
			break;

		case BT_MSTAGESELECTEDLARGEST:
			BlobSize = max(	(MILLIPOINT)((double)MultiStageSelectedBlobSize * PelSize),
							(MILLIPOINT)((double)MultiStageUnSelectedBlobSize * PelSize) );
			break;

		case BT_CLIPVIEW:
			BlobSize = (MILLIPOINT)((double)ClipViewBlobSize * PelSize);
			break;

		default:
			ENSURE( FALSE, "GetBlobRect() was called with an invalid BlobType" );
			BlobSize = 0;							// so we get an empty rectangle
			break;
	}

	// Build the rectangle of the appropriate size and centre it on the DocCoord given
	pResult->lo.x = BlobPoint.x - BlobSize - (MILLIPOINT)(PelSize/2.0);
	pResult->lo.y = BlobPoint.y - BlobSize - (MILLIPOINT)(PelSize/2.0);
	pResult->hi.x = BlobPoint.x + BlobSize + (MILLIPOINT)(PelSize/2.0);
	pResult->hi.y = BlobPoint.y + BlobSize + (MILLIPOINT)(PelSize/2.0);

}



/********************************************************************************************
>	static MILLIPOINT OSRenderRegion::GetHitTestRadius(DocView *pDocView)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/94
	Inputs:		The DocView that we are doing hit-testing in.
	Outputs:	-
	Returns:	The hit-test sensitivity measured in millipoints.  An object with given
				document coordinates is considered to be clicked upon if it lies within
				this radius of the click position.
	Purpose:	Allows user to adjust the sensitivity of hit-testing.
	Errors:		-
	SeeAlso:	OSRenderRegion::GetBlobRect; NodeRenderableInk::FindFirstAtPoint
********************************************************************************************/

MILLIPOINT OSRenderRegion::GetHitTestRadius(DocView *pDocView)

{
	FIXED16 fxViewScale = pDocView->GetViewScale();
	FIXED16 fxPelSize = ((FIXED16) HitTestRadius) * pDocView->GetPixelWidth();
	return (MILLIPOINT) (fxPelSize / fxViewScale).MakeLong();
}



/********************************************************************************************

>	WinCoord OSRenderRegion::DocCoordToWin(const DocCoord& DocPoint)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/93
	Inputs:		Matrix elements and pixel scale.
	Returns:	A WinCoord
	Purpose:	Converts a DocCoord to a WinCoord. Used during rendering

********************************************************************************************/

WinCoord OSRenderRegion::DocCoordToWin(const DocCoord& DocPoint)
{
	OilCoord OilPoint;

	OilPoint.x = MatrixCalc( RenderMatrix.a, DocPoint.x, RenderMatrix.c, DocPoint.y) +
				 RenderMatrix.e;

	OilPoint.y = MatrixCalc( RenderMatrix.b, DocPoint.x, RenderMatrix.d, DocPoint.y) +
				 RenderMatrix.f;	// This was F1

	return OilPoint.ToWin(RenderView);
}

/********************************************************************************************

>	Coord OSRenderRegion::DocCoordToOS256(const DocCoord& DocPoint)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/93
	Inputs:		Matrix elements and pixel scale.
	Returns:	A WinCoord
	Purpose:	Converts a DocCoord to a OS256 Coord. Used during rendering

********************************************************************************************/

Coord OSRenderRegion::DocCoordToOS256(const DocCoord& DocPoint)
{
	OilCoord OilPoint;

	OilPoint.x = MatrixCalc( RenderMatrix.a, DocPoint.x, RenderMatrix.c, DocPoint.y) +
				 RenderMatrix.e;

	OilPoint.y = MatrixCalc( RenderMatrix.b, DocPoint.x, RenderMatrix.d, DocPoint.y) +
				 RenderMatrix.f;	// This was f1

	Coord OS256Point;
	
	FIXED16 PixelWidth = RenderView->GetPixelWidth();
	OS256Point.x = MPtoOS256(OilPoint.x, PixelWidth); 
//	OS256Point.y = -(MPtoOS256(OilPoint.y, PixelWidth) + 256); 

//	Above line commented out by Phil, 11/2/97
//	The above "+256" is spurious (see comments for OilCoordToWin)
//	Fixes a nasty printing bug.
	OS256Point.y = -(MPtoOS256(OilPoint.y, PixelWidth));

	return OS256Point;
}

/*********************************************************************************************

>	WinRect OSRenderRegion::DocRectToWin(const Matrix &RenderMatrix, DocRect& DocRect,
					  		 INT32 leftshift, INT32 topshift, INT32 rightshift, INT32 bottomshift,
					  		 BOOL MightClip=FALSE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/93
	Inputs:		pView - the view to convert in.
				DocRect is a rectangle on document co-ords. The various shift parameters
				allow the resulting rectangle to be moved about a little, to allow for
				different rendering models etc.
				If MightClip is TRUE then the rectangle is clipped to fit within 16-bit
				GDI constraints, if running on 16-bit GDI.
	Returns:	Object containing the new rectangle coordinates.     			
	Purpose:	To convert a rectangle in Doc coordinates to a rectangle in Win coordinates.
	Scope:		Static

**********************************************************************************************/  


WinRect OSRenderRegion::DocRectToWin(View *pView,
									 const Matrix &RenderMatrix, 
									 const DocRect& DRect,
				  					 INT32 leftshift, INT32 topshift, 
				  					 INT32 rightshift, INT32 bottomshift,
				  					 BOOL MightClip)
{
	WinRect WinRect;

	// Don't do ERROR2 as this is in redraw code.
	ERROR3IF(pView == NULL, "NULL view in OSRenderRegion::DocRectToWin()");

	OilCoord OilLo;
	OilCoord OilHi;

	OilLo.x = MatrixCalc( RenderMatrix.a, DRect.lo.x, RenderMatrix.c, DRect.lo.y ) + RenderMatrix.e;
	OilLo.y = MatrixCalc( RenderMatrix.b, DRect.lo.x, RenderMatrix.d, DRect.lo.y ) + RenderMatrix.f;	// This was f1
	OilHi.x = MatrixCalc( RenderMatrix.a, DRect.hi.x, RenderMatrix.c, DRect.hi.y ) + RenderMatrix.e;
	OilHi.y = MatrixCalc( RenderMatrix.b, DRect.hi.x, RenderMatrix.d, DRect.hi.y ) + RenderMatrix.f;	// this was f1

	// RenderMatrix can involve a rotation (but only multiples of 90 degrees), so we swap
	// the corners here if necessary
	MILLIPOINT Temp;

	if (OilLo.x > OilHi.x)
	{
		Temp = OilLo.x;
		OilLo.x = OilHi.x;
		OilHi.x = Temp;
	}

	if (OilLo.y > OilHi.y)
	{
		Temp = OilLo.y;
		OilLo.y = OilHi.y;
		OilHi.y = Temp;
	}

	WinCoord WinLo;
	WinCoord WinHi;

	WinLo = OilLo.ToWin(pView);
	WinHi = OilHi.ToWin(pView);

	WinRect.x      = WinLo.x + leftshift;
	WinRect.y      = WinHi.y + topshift;
	WinRect.width  = WinHi.x + rightshift -WinRect.x;
	WinRect.height = WinLo.y + bottomshift-WinRect.y;

PORTNOTE("other", "Check this use of WIN32")
	if (
		MightClip
//#if WIN32
//		 && IsWin32s()
//#endif
	   )
	{
		// assume MM_TEXT, clip to limits without being too close else we might effect
		// the actual rendering (e.g. if a line crosses the clip rect boundary)
		wxRect MaxRect(-100, -100, 32100, 32100 );
		WinRect.Intersect(MaxRect);
	}

	return WinRect;
}


/*********************************************************************************************

>	WinRect OSRenderRegion::DocRectToWin(const Matrix &RenderMatrix, DocRect& DocRect,
					  		 const double dpi)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		DocRect is a rectangle on document co-ords.
				dpi is the resolution of the device we are rendering to
	Returns:	Object containing the new rectangle coordinates.     			
	Purpose:	To convert a rectangle in Doc coordinates to a rectangle in Win coordinates
				taking account of the destination dpi rather than assuming screen dpi as the
				above form does. Does not need the extra parameters that the above uses.
				INT32 dpi changed to double dpi (12/12/95) to improve the range of values allowed
				at the < 1000dpi settings that we will be using.
	Scope:		Static

**********************************************************************************************/  


WinRect OSRenderRegion::DocRectToWin(const Matrix &RenderMatrix, const DocRect& DRect,
							  		 const double dpi)
{
	WinRect WinRect;

	OilCoord OilLo;
	OilCoord OilHi;

	OilLo.x = MatrixCalc( RenderMatrix.a, DRect.lo.x, RenderMatrix.c, DRect.lo.y ) + RenderMatrix.e;
	OilLo.y = MatrixCalc( RenderMatrix.b, DRect.lo.x, RenderMatrix.d, DRect.lo.y ) + RenderMatrix.f;	// This was f1
	OilHi.x = MatrixCalc( RenderMatrix.a, DRect.hi.x, RenderMatrix.c, DRect.hi.y ) + RenderMatrix.e;
	OilHi.y = MatrixCalc( RenderMatrix.b, DRect.hi.x, RenderMatrix.d, DRect.hi.y ) + RenderMatrix.f;	// this was f1

	// RenderMatrix can involve a rotation (but only multiples of 90 degrees), so we swap
	// the corners here if necessary
	MILLIPOINT Temp;

	if (OilLo.x > OilHi.x)
	{
		Temp = OilLo.x;
		OilLo.x = OilHi.x;
		OilHi.x = Temp;
	}

	if (OilLo.y > OilHi.y)
	{
		Temp = OilLo.y;
		OilLo.y = OilHi.y;
		OilHi.y = Temp;
	}

	WinCoord WinLo;
	WinCoord WinHi;

	WinLo = OilLo.ToWin(dpi);
	WinHi = OilHi.ToWin(dpi);

	WinRect.x      = WinLo.x;
	WinRect.y      = WinHi.y;
	WinRect.width  = WinHi.x-WinRect.x;
	WinRect.height = WinLo.y-WinRect.y;

	return WinRect;
}




/*********************************************************************************************

>	DocRect OSRenderRegion::WinRectToDoc(const Matrix &RenderMatrix,
										 WinRect& WRect,
					  					 const double dpi)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/08/2004
	Inputs:		WinRect is a rectangle in Windows co-ords.
				dpi is the resolution of the device we are rendering to
	Returns:	Object containing the new rectangle coordinates.     			
	Purpose:	To convert a rectangle in Windows coordinates to a rectangle in Document (spread) coordinates
				taking account of the destination dpi. Does not need the extra parameters that the above uses.
	Scope:		Static

**********************************************************************************************/  

DocRect OSRenderRegion::WinRectToDoc(const Matrix &RenderMatrix, const WinRect& WRect,
							  		 const double dpi)
{
	DocRect dr;

	WinCoord WinLo(WRect.x, WRect.GetBottomEx());
	WinCoord WinHi(WRect.GetRightEx(), WRect.y);
	OilCoord OilLo = OilCoord(	LongMulFixed16(WinLo.x, 72000L / dpi),
				   				-LongMulFixed16(WinLo.y, 72000L / dpi)
				   				);
	OilCoord OilHi = OilCoord(	LongMulFixed16(WinHi.x, 72000L / dpi),
				   				-LongMulFixed16(WinHi.y, 72000L / dpi)
				   				);

	Matrix inv = RenderMatrix.Inverse();

	dr.lo.x = MatrixCalc( inv.a, OilLo.x, inv.c, OilLo.y ) + inv.e;
	dr.lo.y = MatrixCalc( inv.b, OilLo.x, inv.d, OilLo.y ) + inv.f;
	dr.hi.x = MatrixCalc( inv.a, OilHi.x, inv.c, OilHi.y ) + inv.e;
	dr.hi.y = MatrixCalc( inv.b, OilHi.x, inv.d, OilHi.y ) + inv.f;

	// RenderMatrix can involve a rotation (but only multiples of 90 degrees), so we swap
	// the corners here if necessary
	MILLIPOINT Temp;

	if (dr.lo.x > dr.hi.x)
	{
		Temp = dr.lo.x;
		dr.lo.x = dr.hi.x;
		dr.hi.x = Temp;
	}

	if (dr.lo.y > dr.hi.y)
	{
		Temp = dr.lo.y;
		dr.lo.y = dr.hi.y;
		dr.hi.y = Temp;
	}

	return dr;
}




/*********************************************************************************************

>	WinRect OSRenderRegion::DocRectToWin(const Matrix &RenderMatrix, DocRect& DocRect,
					  		 const double dpi)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> and Gavin
	Created:	4/10/96
	Inputs:		DocRect is a rectangle on document co-ords.
				dpi is the resolution of the device we are rendering to
	Returns:	Object containing the new rectangle coordinates.     			
	Purpose:	To convert a rectangle in Doc coordinates to a rectangle in Win coordinates
				taking account of the destination dpi rather than assuming screen dpi as the
				above form does.
				Same as above but tries to create the rectangle better to try and help pixel
				problems.
	Scope:		Static

**********************************************************************************************/  


WinRect OSRenderRegion::BitmapDocRectToWin(Matrix &RenderMatrix, const DocRect& DRect,
					  					   const double dpi)
{
	// This is a version from Gavin, which should improve things
	OilRect OilRec;
	OilRec.lo.x = MatrixCalc( RenderMatrix.a, DRect.lo.x, RenderMatrix.c, DRect.lo.y ) + RenderMatrix.e;
	OilRec.lo.y = MatrixCalc( RenderMatrix.b, DRect.lo.x, RenderMatrix.d, DRect.lo.y ) + RenderMatrix.f;
	OilRec.hi.x = MatrixCalc( RenderMatrix.a, DRect.hi.x, RenderMatrix.c, DRect.hi.y ) + RenderMatrix.e;
	OilRec.hi.y = MatrixCalc( RenderMatrix.b, DRect.hi.x, RenderMatrix.d, DRect.hi.y ) + RenderMatrix.f;

	if (OilRec.lo.x > OilRec.hi.x)
	{
		MILLIPOINT Temp = OilRec.lo.x;
		OilRec.lo.x = OilRec.hi.x;
		OilRec.hi.x = Temp;
	}
	if (OilRec.lo.y > OilRec.hi.y)
	{
		MILLIPOINT Temp = OilRec.lo.y;
		OilRec.lo.y = OilRec.hi.y;
		OilRec.hi.y = Temp;
	}

	OilCoord OilSize(OilRec.hi.x-OilRec.lo.x, OilRec.hi.y-OilRec.lo.y);

#if 0
	//
	// Note that this relies on ToWin performing the calculation
	//		+round(X*dpi/72000), -round(Y*dpi/72000)
	//
	WinCoord WinSize = OilSize.ToWin(dpi) ;			// This negates the Y.
#else
	//
	// The following performs the same calculation as ToWin. Doing it here ensures
	// that the subsequent calculation of NewOilSize will be compatible.
	//
	WinCoord    WinSize( +(INT32)floor(OilSize.x*dpi/72000.0+0.5), -(INT32)floor(OilSize.y*dpi/72000.0+0.5) ) ;
#endif
	// This should be the bitmap size.

	//
	// WinSize should now be the size of the export bitmap.
	// NewRenderMatrix.x and NewRenderMatrix.y should be the e and f
	// components of the new render matrix.
	//

	OilCoord OilLo = OilCoord(OilRec.lo.x,OilRec.lo.y);
	WinCoord WinLo = OilLo.ToWin(dpi);

	WinRect WinRectangle;
	WinRectangle.x      = WinLo.x;
	WinRectangle.y      = WinLo.y + WinSize.y;
	WinRectangle.width  = WinLo.x + WinSize.x-WinRectangle.x;
	WinRectangle.height = WinLo.y-WinRectangle.y;

	return WinRectangle;
}

/********************************************************************************************

>	static BOOL OSRenderRegion::CalculateGMatrixOffsetsWinRect( const Matrix& RenderMatrix,
																const DocRect& docrect,
																const double dpi,
																GMATRIX* GMat,
																BOOL bCentralise
																double* pdXCentralAdjust
																double* pdYCentralAdjust)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> and Gavin
	Created:	4/10/96
	Inputs:		DocRect is a rectangle on document co-ords.
				dpi is the resolution of the device we are rendering to
				RenderMatrix is the rendering matrix
	Outputs:	An updated version of the Gavin matrix
	Returns:	Object containing the new rectangle coordinates.     			
	Purpose:	To add in the offsets to the Gavin matrix.
				This is used when exporting bitmaps. There used to a lot of pixel alignements
				problems whereby on a rectangle with line colour of say 0.5pt width, you would
				seemingly randomly loose one or more of the lines on the exported bitmap.
				This is an attempt to fix this for the bitmap export case only, as other render
				regions have their own problems and perculiarities.
	SeeAlso:	OSRenderRegion::DocRectToWin;

********************************************************************************************/

BOOL OSRenderRegion::CalculateGavinOffsetsWinRect( const Matrix& RenderMatrix,
												   const DocRect& DRect,
												   const double dpi,
												   GMATRIX* GMat,
												   BOOL bCentralise,
												   double* pdXCentralAdjust,
												   double* pdYCentralAdjust)
{
	OilRect OilRec ;
	OilRec.lo.x = MatrixCalc( RenderMatrix.a, DRect.lo.x, RenderMatrix.c, DRect.lo.y ) + RenderMatrix.e ;
	OilRec.lo.y = MatrixCalc( RenderMatrix.b, DRect.lo.x, RenderMatrix.d, DRect.lo.y ) + RenderMatrix.f ;
	OilRec.hi.x = MatrixCalc( RenderMatrix.a, DRect.hi.x, RenderMatrix.c, DRect.hi.y ) + RenderMatrix.e ;
	OilRec.hi.y = MatrixCalc( RenderMatrix.b, DRect.hi.x, RenderMatrix.d, DRect.hi.y ) + RenderMatrix.f ;

	if (OilRec.lo.x > OilRec.hi.x)
	{
		MILLIPOINT Temp = OilRec.lo.x ;
		OilRec.lo.x = OilRec.hi.x ;
		OilRec.hi.x = Temp ;
	}
	if (OilRec.lo.y > OilRec.hi.y)
	{
		MILLIPOINT Temp = OilRec.lo.y ;
		OilRec.lo.y = OilRec.hi.y ;
		OilRec.hi.y = Temp ;
	}
	OilCoord OilSize( OilRec.hi.x-OilRec.lo.x, OilRec.hi.y-OilRec.lo.y ) ;

	/*	double OilXOffset = (NewOilSize.x-OilSize.x)/2.0-OilRec.lo.x ;
	double OilYOffset = (NewOilSize.y-OilSize.y)/2.0-OilRec.lo.y ;
	double Scale = (dpi/72000.0)*(1<<16)*(1<<FX) ;
	double WinXOffset = OilXOffset*Scale ;
	double WinYOffset = OilYOffset*Scale ;
*/

	double Scale = (dpi/72000.0)*(1<<16)*(1<<FX) ;
	double WinXOffset = -OilRec.lo.x * Scale;
	double WinYOffset = -OilRec.lo.y * Scale;

	if (bCentralise)
	{
		//
		// The following performs the same calculation as ToWin. Doing it here ensures
		// that the subsequent calculation of NewOilSize will be compatible.
		//
		WinCoord    WinSize( +(INT32)floor(OilSize.x*dpi/72000.0+0.5), -(INT32)floor(OilSize.y*dpi/72000.0+0.5) ) ;
		OilCoord NewOilSize( +(INT32)floor(WinSize.x*72000.0/dpi+0.5), -(INT32)floor(WinSize.y*72000.0/dpi+0.5) ) ;

		if (pdXCentralAdjust) *pdXCentralAdjust = ((NewOilSize.x-OilSize.x)/2.0) * Scale;
		if (pdYCentralAdjust) *pdYCentralAdjust = ((NewOilSize.y-OilSize.y)/2.0) * Scale;

	}

	// Adjust offsets either by calculated value or supplied value
	if (pdXCentralAdjust) WinXOffset += *pdXCentralAdjust;
	if (pdYCentralAdjust) WinYOffset += *pdYCentralAdjust;

	//
	// The matrix is modified so that the drawing is centred within the export bitmap.
	//
	GMat->CX = MakeXLong(WinXOffset) ;
	GMat->CY = MakeXLong(WinYOffset) ;

	return TRUE;
}

/********************************************************************************************
********************************************************************************************/


/********************************************************************************************

>	BOOL OSRenderRegion::NewPolyDraw( const POINT* PointList, const BYTE* VerbList, INT32 NumCoords)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/94
	Inputs:		A list of points (in GDI coords), a list of PT_ verbs and a count.
	Outputs:	-
	Errors:		-
	Returns:	TRUE if worked, FALSE if failed
	Purpose:	Like PolyDraw but works on Chicago. Does not work on Win32s.
	Scope:		Protected

********************************************************************************************/
/*
BOOL OSRenderRegion::NewPolyDraw( const POINT* PointList, const BYTE* VerbList, INT32 NumCoords)
{
	if (IsWin32NT())
	{
		// NT can do PolyDraw, which expects our path structure exactly
		return PolyDraw( RenderDC, PointList, VerbList, NumCoords );
	}
	else
	{
		// Chicago is damn stupid and cannot do it, so we whip through it manually
		BOOL Worked = TRUE;

		while (NumCoords && Worked)
		{
			switch (*VerbList++ & ~PT_CLOSEFIGURE)
			{
				case PT_MOVETO:
					Worked = MoveToEx( RenderDC, PointList->x, PointList->y, NULL );
					PointList++;
					NumCoords--;
					break;

				case PT_LINETO:
					Worked = LineTo( RenderDC, PointList->x, PointList->y );
					PointList++;
					NumCoords--;
					break;

				case PT_BEZIERTO:
					Worked = PolyBezierTo( RenderDC, PointList, 3 );
					PointList += 3;
					VerbList += (3-1);							// as 1 taken away above
					NumCoords -= 3;
					break;

				default:
					ENSURE(FALSE, "Bad verb in path");
					PointList++;
					NumCoords--;
					break;
			}
			ENSURE( NumCoords>=0, "Messed up verb list");
		}
		return Worked;
	}
}
*/

static inline INT32 GetDiagonal( const WinRect& Rectangle )
{
	const double W = Rectangle.width;
	const double H = Rectangle.height;
	return (INT32) sqrt(W*W+H*H);
}

/********************************************************************************************

>	BOOL OSRenderRegion::RenderBitmapFill(Path *PathToDraw, BitmapFillAttribute* Fill)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Inputs:		A path to render and a bitmap fill attribute.
	Outputs:	-
	Errors:		-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Render bitmap fills clipped through a path. BODGED to simple plot the bitmap.
	Scope:		Public

********************************************************************************************/

BOOL OSRenderRegion::RenderBitmapFill(Path *PathToDraw, BitmapFillAttribute* Fill)
{
	PORTNOTETRACE("other","OSRenderRegion::RenderBitmapFill - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (Fill->GetBitmap() == NULL || Fill->GetBitmap()->ActualBitmap == NULL)
		return FALSE;									// if no bitmap

	// if rendering to a bitmap, return FALSE so it is rendered solid. Test should be in
	// caller code in RenderBitmapFillAttribute but here is OK for now.
	if ((RenderFlags.VeryMono) || (RFlags.Metafile))
		return FALSE;

	ENSURE( Fill->GetBitmap()->ActualBitmap->IsKindOf( CC_RUNTIME_CLASS( WinBitmap ) ), "Strange bitmapfill");

// Since the GDI cannot cope with anything other than very simple bitmaps,
// we will get GDraw to render the bitmap into a screen compatible DIB, and
// then get the GDI to plot that

	GDrawContext* GD = GRenderRegion::GetStaticDrawContext();
	WinBitmap *WinBM = NULL;
	WinBitmap *OrigWinBM = NULL;

	const INT32 bpp = Fill->GetBitmap()->GetBPP();
	BYTE *pGreyTable = NULL;

	if (Fill->GetStartColour() != NULL && Fill->GetEndColour() != NULL)
	{
		// The bitmap is contoned ... so we need to use the greyscale version
		WinBM = (WinBitmap*)Fill->GetBitmap()->GetGreyscaleVersion();

		if (WinBM == NULL && bpp == 8)
		{
			// Aha, we're gunna do some clever palette jiggery pokery
			pGreyTable = Fill->GetBitmap()->ActualBitmap->GetGreyscaleTable();
		}
	}	

	// Now make sure that if the WinBitmap is 32 that we RENDER it into a tempory FULLY TRANSPARENT WHITE BMP.
	// This stops the CONVERT function later on stripping the alpha channel off producing solid, non-transparent
	// coloured objects on black black backgrounds! - MarkH 20/7/99.
	if (WinBM == NULL)
	{
		if (bpp != 32)
		{
			WinBM = (WinBitmap*)Fill->GetBitmap()->ActualBitmap;
		}
		else
		{
			WinBM = (WinBitmap*)CBMPBits::RenderBMPFillAttrToTransparentWhiteRect(Fill);
		}
	}

	if (!WinBM)				// we really should check this pointer!
	{
		return (FALSE);
	}

	if( (WinBM->BMInfo==NULL) || (WinBM->BMBytes==NULL) )
		return FALSE;

	// Work out what Tempory bitmap we need

// Note we are forcing the bitmap to be a horizontal rectangular bitmap, 
// as the GDI can't cope with rotating them

	wxPoint BottomLeft  = DocCoordToWin( Fill->StartPoint );
	wxPoint BottomRight = DocCoordToWin( Fill->EndPoint   );
	wxPoint TopLeft     = DocCoordToWin( Fill->EndPoint2  );

	INT32 DestWidth  = BottomRight.x - BottomLeft.x;
	INT32 DestHeight = BottomLeft.y  - TopLeft.y;
	if (DestWidth==0 || DestHeight==0)
	{
		return TRUE;
	}

	if (IsPrinting())
	{
		if (CurrentColContext->GetColourPlate() == NULL ||
			CurrentColContext->GetColourPlate()->IsDisabled())
		{
			// going to a printer? - let its driver stretch it about etc
			const LPBITMAPINFO bmInfo = WinBM->BMInfo;

			#if 1
			// this is the correct legit code
			const BOOL SendDirectToPrinter = true;
			#else
			// test code
			const BOOL SendDirectToPrinter = RFlags.Metafile;			// only metafiles
			#endif

			if (SendDirectToPrinter)
			{
				// pass bitmap straight to driver. Might have to band this call
				// up on low-capacity machines at some point. Also does this for
				// metafiles too.
				StretchDIBits( RenderDC->m_hDC, 
									BottomLeft.x, TopLeft.y,				// dest XY
									DestWidth, DestHeight, 					// dest WH
									0,0,									// source 0,0
									bmInfo->bmiHeader.biWidth,			// source W
									bmInfo->bmiHeader.biHeight,			// source H
									WinBM->BMBytes,
									bmInfo,
									DIB_RGB_COLORS,
									SRCCOPY );

				return TRUE;
			}

			// if 16- or 32-bit, leave for later. We get Gavin to scale it to an output bitmap
			// then PlotDeepDIB it. Sadly, this requires serious amounts of RAM, so is prone
			// to failure. The solution would be a ScaleDeepDIB call, like PlotDeepDIB but
			// that can scale appropriately. Such a function would then be called here, avoiding
			// the need for a huge intermediate bitmap
		}
	}

// Setup a default matrix (we'll do everything in pixel-speak)

	GMATRIX GMatrix;

	GMatrix.AX = 1<<(16 + FX);
	GMatrix.AY = 0;
	GMatrix.BX = 0;
	GMatrix.BY = 1<<(16 + FX);

	GMatrix.CX = 0;
	GMatrix.CY = 0;

// Create a Tempory bitmap compatible with the output device

	LPBITMAPINFO TempInfo;
	LPBYTE TempBits;
	INT32 DeviceDepth;

	if (IsPrinting())
	{
		// if printing, always convert to 24-bit (which as Gavin can't do 24- means 32-bit)
		// as all drivers can handle those (source bitmap only 16- or 32-bit anyway)
		DeviceDepth = 32;
	}
	else
	{
		DeviceDepth = GetDeviceCaps( RenderDC->m_hDC, BITSPIXEL ) * 
										GetDeviceCaps( RenderDC->m_hDC, PLANES );

		if (DeviceDepth ==24)
			DeviceDepth = 32;	// GDraw cannot plot to 24-bit bitmaps
	}

	TempInfo = AllocDIB(DestWidth, DestHeight, DeviceDepth, &TempBits);

	if (TempInfo==NULL)
	{
		TRACEALL( _T("Out of memory during OSRenderRegion::RenderBitmapFill") );
		return FALSE;
	}

// We may need to get a palette for the DIB.

	UINT32 DIBPal = DIB_RGB_COLORS;

	if (DeviceDepth <= 8)	// We only need a palette for 256 colours or less.
	{
		DIBPal = GRenderRegion::SetPaletteEntries( TempInfo, RenderDC );
	}

// Setup GDraw with our Tempory Bitmap and Identity Matrix

	GD->SetupBitmap(TempInfo->bmiHeader.biWidth,
					TempInfo->bmiHeader.biHeight,
					TempInfo->bmiHeader.biBitCount, 
					TempBits );

	GD->SetMatrix( &GMatrix );

	COLORREF DefaultColour = 0xFFFFFFFF;

	// --- Colour-correct and/or Contone the bitmap as necessary
	RGBQUAD *Palette = NULL;

	// Do the colour correction. This may produce a new pointer in Palette or BitmapBits,
	// which we should CCFree() when we are done with it - see the end of this function
	ColourCorrectBitmap(Fill, WinBM->BMInfo, &Palette);

	// Now see if we need to muck around with the palette for the contoning
	if (pGreyTable != NULL)
	{
		ERROR3IF(bpp != 8, "Greytable should only be here when rendering an 8bpp bitmap");
		RGBQUAD *OldPalette = Palette;

		// Create a new palette
		Palette = (RGBQUAD *) CCMalloc(256 * sizeof(RGBQUAD));
		if (Palette == NULL)
		{
			ERROR3("No memory for palette");
			return FALSE;
		}
		
		// Copy the entries from the contone palette into the new one,
		// using the Grey table as a guide
		for (INT32 i=0; i<256; i++)
		{
			Palette[i] = OldPalette[pGreyTable[i]];
		}

		if (OldPalette != WinBM->BMInfo->bmiColors)
			CCFree(OldPalette);			// Don't need the contone palette any more
	}

	// If we didn't create a temporary palette, then we'll use the original bitmap
	if (Palette == NULL)
		Palette = WinBM->BMInfo->bmiColors;

	// Search for a transparent colour setting the Style flags if necessary...
	if (bpp <= 8)
	{
		INT32 NumCols;
		switch (bpp)
		{
			case 1:
				NumCols = 2;
				break;

			case 2:
				NumCols = 4;
				break;

			case 4:
				NumCols = 16;
				break;

			case 8:
				NumCols = 256;
				break;

			default:
				NumCols = 256;
				break;
		}

		for (INT32 i=0; i<NumCols; i++)
		{
			if (Palette[i].rgbReserved == 0xFF)
			{
				RGBQUAD* TempPalette = (RGBQUAD*)CCMalloc(NumCols * sizeof(RGBQUAD));
				if (TempPalette)
				{
					// We'll use a copy of the palette ...
					memcpy(TempPalette, Palette, NumCols*sizeof(RGBQUAD));

					// so we can force this entry to be white
					TempPalette[i].rgbRed   	= 0xFF;
					TempPalette[i].rgbGreen 	= 0xFF;
					TempPalette[i].rgbBlue  	= 0xFF;

					Palette = TempPalette;
					break;
				}
			}
		}
	}

// Now set the bitmap fill

	POINT PGram[3];
	PGram[0].x = 0;				PGram[0].y = 0;
	PGram[1].x = DestWidth;		PGram[1].y = 0;
	PGram[2].x = 0;				PGram[2].y = DestHeight;

	DWORD Style = 1;
	BOOL bDoBitmapFill = TRUE;
	DWORD* pConvTable = NULL;
	BYTE* pSepTables = NULL;
	BGR *pCyanSepTable = NULL;
	BGR *pMagentaSepTable = NULL;
	BGR *pYellowSepTable = NULL;
	BGR *pBlackSepTable = NULL;
	BYTE *pUnderColourRemovalTable = NULL;
	BYTE *pBlackGenerationTable = NULL;
	WinBitmap* pNewBitmap = NULL;
	
	// --- Add Separation Style bits as approriate to the current colour separation mode
	if (bpp > 8)	// Only needed for deep bitmaps
	{
		// If we've got a valid colour plate and it is a composite preview
		if (CurrentColContext->GetColourPlate() != NULL &&
			!CurrentColContext->GetColourPlate()->IsDisabled())
		{
			if (CurrentColContext->GetColourPlate()->GetType() == COLOURPLATE_COMPOSITE)
			{
				// Fall through (shouldn't be used at the moment)
			}
			else if (CurrentColContext->GetColourPlate()->GetType() == COLOURPLATE_SPOT)
			{
				GD->SetColour(0xFFFFFF);
				bDoBitmapFill = FALSE;
			}
			else if (CurrentColContext->GetColourPlate()->GetType() != COLOURPLATE_NONE)
			{
				// Create a colour separated copy of the bitmap and render that instead
				pSepTables = (BYTE *) CCMalloc(5 * 256 * sizeof(BYTE));
				if (pSepTables != NULL)
				{
					XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();
					String_256 PrintProfile;
					if (lpCMSMan)
						lpCMSMan->GetPrinterProfile(&PrintProfile);
					ColourContextCMYK *cc = new ColourContextCMYK(RenderView, &PrintProfile);
					if (cc->GetProfileTables(pSepTables))
					{
						// Make a copy of the bitmap
						pNewBitmap = (WinBitmap*)WinBM->MakeSeparatedCopy(RenderView->GetColourPlate(), pSepTables);
						OrigWinBM = WinBM;					// Save original bitmap pointer
						WinBM = pNewBitmap;					// Use this bitmap instead
					}
					delete cc;
				}
			}
		}
	}

	if (bDoBitmapFill)
	{
		GD->SetBitmapFill(	&(WinBM->BMInfo->bmiHeader),
							WinBM->BMBytes,
							Style,
							PGram,
							DefaultColour,
							Palette,
							NULL, NULL, NULL,
							NULL,
							0
							);
	}

// Now plot a filled rectangle

	RECT BmpRect;
	BmpRect.left 	=  0;
	BmpRect.top 	=  DestHeight;
	BmpRect.right 	=  DestWidth;
	BmpRect.bottom 	=  0;

	GD->FillRectangle(&BmpRect);


	if (IsPrinting())
	{
		// we are going to the printer. We had a 16- or 32-bit DIB which we have now converted
		// into a 32-bit one, so send it out

		// PlotDeepDIB ends up doing a SetDIBitsToDevice in 24-bit slices
		DIBUtil::PlotDeepDIB( RenderDC->m_hDC, TempInfo, TempBits, 
							BottomLeft.x, TopLeft.y,
							DestWidth,DestHeight,
							0,0, 
							CONVHINT_PRINTER );
	}
	else
	{
		// get the HPALETTE to pass to the plot bitmap call - crucial for quality Win32s DDB plotting
		HPALETTE hPal = NULL;
		if (RFlags.UsePalette)
			hPal = *(PaletteManager::GetPalette());		// Sneaky HPALETTE operator

		// Finally call PlotBitmap, to render the DIB using the GDI.
		// If the screen cannot cope with the depth of the bitmap, it will be converted
		// for us. Isn't PlotBitmap wonderful?

		GRenderRegion::PlotBitmap( 	RenderDC->m_hDC, 
								DIBPal, 
								TempInfo, 
								TempBits,
								BottomLeft.x, 
								TopLeft.y, 
								DestWidth, 
								DestHeight, 
								hPal,
								0,0
							 );
	}

	FreeDIB(TempInfo, TempBits);

	if (pNewBitmap)
	{
		WinBM = OrigWinBM;
		delete pNewBitmap;
	}

	// Free any memory used for colour-corrected bitmap palettes.
	// If this pointer doesn't point at the original palette, then it has
	// been temporarily allocated by ColourCorrectBitmap, above.
	if (Palette != WinBM->BMInfo->bmiColors)
		CCFree(Palette);
	
	if (pSepTables)
	{
		GD->SetSeparationTables();	// Defaults to setting everything to NULL
		CCFree(pSepTables);
	}

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
#endif
   	return TRUE;
}


MILLIPOINT OSRenderRegion::CalcDistance(DocCoord a, DocCoord b)
{
	const double Width = (double) a.x - b.x;
	const double Height = (double) a.y - b.y;
	double Diag = Width * Width + Height * Height;
	const INT32 Distance = (INT32) sqrt(Diag);
	return Distance;
}

void OSRenderRegion::MakeEllipticalPath(Path *pPath, DocCoord Parallel[4])
{
	// Get an array to put the 12 different coords needed to specify an ellipse
	DocCoord NewCoords[12];

	// Calculate the 3 coordinates along each side of the parallelogram
	NodeEllipse::CalcEllipseEdge(Parallel[0], Parallel[1], &NewCoords[11], &NewCoords[0], &NewCoords[1]);
	NodeEllipse::CalcEllipseEdge(Parallel[1], Parallel[2], &NewCoords[2], &NewCoords[3], &NewCoords[4]);
	NodeEllipse::CalcEllipseEdge(Parallel[2], Parallel[3], &NewCoords[5], &NewCoords[6], &NewCoords[7]);
	NodeEllipse::CalcEllipseEdge(Parallel[3], Parallel[0], &NewCoords[8], &NewCoords[9], &NewCoords[10]);

	// build a path
	pPath->ClearPath();
	pPath->FindStartOfPath();

	// Start at bottom left corner
	PathFlags NewFlags;
	NewFlags.IsRotate = TRUE;
	pPath->InsertMoveTo(NewCoords[0], &NewFlags);
	pPath->InsertCurveTo(NewCoords[1], NewCoords[2], NewCoords[3], &NewFlags);
	pPath->InsertCurveTo(NewCoords[4], NewCoords[5], NewCoords[6], &NewFlags);
	pPath->InsertCurveTo(NewCoords[7], NewCoords[8], NewCoords[9], &NewFlags);
	pPath->InsertCurveTo(NewCoords[10], NewCoords[11], NewCoords[0], &NewFlags);

	// Close the path properly
	pPath->CloseSubPath();
}

/********************************************************************************************

>	BOOL OSRenderRegion::RenderGradFillPath(Path *PathToDraw, GradFillAttribute* Fill)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/94
	Inputs:		A path to render and a fill attribute.
	Outputs:	-
	Errors:		-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Render grad fills to GDI devices using whatever means available.
	Scope:		Public

********************************************************************************************/

BOOL OSRenderRegion::RenderGradFillPath(Path *PathToDraw, GradFillAttribute* Fill)
{
	CCRuntimeClass *FillType = Fill->GetRuntimeClass();

	if (FillType == CC_RUNTIME_CLASS(RadialFillAttribute))
		return RenderRadialFill(PathToDraw, (RadialFillAttribute *) Fill);
	else if (FillType == CC_RUNTIME_CLASS(LinearFillAttribute))
		return RenderLinearFill(PathToDraw, (LinearFillAttribute *) Fill); 
	else if (FillType == CC_RUNTIME_CLASS(ConicalFillAttribute))
		return RenderConicalFill(PathToDraw, (ConicalFillAttribute *) Fill); 
	else if (FillType == CC_RUNTIME_CLASS(SquareFillAttribute))
		return RenderSquareFill(PathToDraw, (SquareFillAttribute *) Fill); 
	else if (FillType == CC_RUNTIME_CLASS(FourColFillAttribute))
		return RenderFourColFill(PathToDraw, (FourColFillAttribute *) Fill); 
	else
		return FALSE;
}



/********************************************************************************************

>	static INT32 ColDifference(DocColour &Start, DocColour &End, INT32 Depth, EFFECTTYPE EffectType)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>? Rik? Tim?
	Created:	?
	Inputs:		Start, End - The Start & end colours for the fill
				EffectType - The fill type (EFFECT_RGB, EFFECT_HSV_SHORT, EFFECT_HSV_LONG)

	Returns:	The number of gradient steps you should generate when rendering this fill

	Purpose:	To determine the number of grad fill steps that will be necessary to render
				the given graduation to a reasonable quality

	Scope:		Private static (in osrndrgn.cpp)

********************************************************************************************/

static INT32 ColDifference(DocColour &Start, DocColour &End, INT32 Depth, EFFECTTYPE EffectType)
{
	// Get RGB values of start and end colours...
	INT32 StartRed, StartGreen, StartBlue;
	Start.GetRGBValue(&StartRed, &StartGreen, &StartBlue);

	INT32 EndRed, EndGreen, EndBlue;
	End.GetRGBValue(&EndRed, &EndGreen, &EndBlue);

	// Find maximum difference between colours
	INT32 Diff = Abs(StartRed-EndRed);
	INT32 Tmp = StartGreen - EndGreen;
	Diff = max(Abs(Tmp), Diff);
	Tmp = StartBlue - EndBlue;
	Diff = max(Abs(Tmp), Diff);

	// Spot colours in a fill coerce the fill to a simple RGB mix
	if (Start.GetSpotParent() != NULL || End.GetSpotParent() != NULL)
		EffectType = EFFECT_RGB;

	// See if HSV route requires larger number of steps...
	if (EffectType == EFFECT_HSV_SHORT || EffectType == EFFECT_HSV_LONG)
	{
		// Get an HSV context (quite fast - simple array lookup)
		ColourContext *pContext = ColourContext::GetGlobalDefault(COLOURMODEL_HSVT);
		ERROR3IF(pContext == NULL, "No HSV context?!");

		ColourHSVT StartDef;
		pContext->ConvertColour(&Start, (ColourGeneric *) &StartDef);

		ColourHSVT EndDef;
		pContext->ConvertColour(&End, (ColourGeneric *) &EndDef);

		// HSV blend! We can go 2 ways, as HSV can 'wrap' from 1.0 back to 0.0
		BOOL BlendNormally = TRUE;

		// Calc. the "simple" (non-wrapping) distance between the hues
		const double StartHue	= StartDef.Hue.MakeDouble();
		const double EndHue		= EndDef.Hue.MakeDouble();

		double SimpleDist = StartHue - EndHue;
		if (SimpleDist < 0.0)
			SimpleDist = -SimpleDist;

		// Determine whether we do a simple blend, or we have to "wrap"
		if (SimpleDist <= 0.5)
			BlendNormally = !(EffectType == EFFECT_HSV_LONG);
		else
			BlendNormally = (EffectType == EFFECT_HSV_LONG);

		// Convert to 9 bit value
		INT32 HSVDiff = (INT32) (360.0 * SimpleDist);
		if (!BlendNormally)			// Go the long way around
			HSVDiff = 360 - HSVDiff;

		if (HSVDiff > Diff)			// Use this if it's larger.
			Diff = HSVDiff;
	}

	return((Depth >= 24) ? Diff : Diff / 3);	// Decide how many steps to do based on colour depth.
}



const INT32 MAX_FILL_STEPS = 128;
const BOOL USE_GDI_CLIPPING = FALSE;

typedef enum
{
	GF_USE_GDICLIPPING,
	GF_USE_GDIEOR,
	GF_USE_GAVINCLIPPING
} GradFillMethodType;

/********************************************************************************************

>	BOOL OSRenderRegion::RenderRadialFill(Path *PathToDraw, RadialFillAttribute *Fill)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>? Rik? Tim?
	Created:	?
	Inputs:		PathToDraw	- A pointer to the path to be rendered.
				Fill		- A pointer to the radial fill applied.
	Returns:	TRUE		- Success.
				FALSE		- An error occured.
	Purpose:	Renders the path with a gradient fill.

********************************************************************************************/

BOOL OSRenderRegion::RenderRadialFill(Path *PathToDraw, RadialFillAttribute *Fill)
{
	GradFillMethodType RenderMethod = GF_USE_GDICLIPPING;

	INT32 Radius1 = CalcDistance(Fill->StartPoint, Fill->EndPoint);
	INT32 Radius2 = CalcDistance(Fill->StartPoint, Fill->EndPoint2);

	// We want to change the rendering context, so we save it.
	SaveContext();

	// No stroking while rendering the fill.
	SetLineColour(COLOUR_TRANS);

	// Use device pixel size to work out how many steps to do.
	FIXED16 PixelSize = 0;
	GetRenderView()->GetScaledPixelSize(&PixelSize,&PixelSize);
	INT32 FillSteps = INT32( max( Radius1, Radius2 ) / PixelSize.MakeDouble() );

	// Do a slice every 4 pixels (arbitrary!)
	FillSteps /= 4;

	// Also base it on distance between colours
	EFFECTTYPE EffectType = GetFillEffect();
	FillSteps = min(ColDifference(Fill->Colour, Fill->EndColour, 
								  IsPrinting() ? 24 : 8, 	// Colour depth
								  EffectType ), 
								  FillSteps);

	// Limit fill steps to a maximum
	FillSteps = min(FillSteps, INT32(MAX_FILL_STEPS));

	// Use global Quality setting
	FillSteps = FillSteps / (1 << GradFillQuality);
	FillSteps = max(FillSteps, 10);

	// Work out what flatness and accuracy to use for Gavin's clipping functions.
	// Gavin recommends 1/2 pixel for flattening, and 1/4 pixel for tolerance
	// (but these were kind of off the top of his head - Tim).
	// PS. It seems we need maximum tolerance (i.e. 0) for accurate grad-fill rendering.
	INT32 ClipFlatness = PixelSize.MakeLong() / 16;
	INT32 ClipTolerance = 0;

	RGBQUAD ColourSteps[MAX_FILL_STEPS];
	GradTable32::BuildGraduatedPalette(Fill->Colour, Fill->EndColour,
										GetRenderView(), EffectType,
										FillSteps, ColourSteps);

	// If we are rendering into a metafile, clip the path ourselves
	if (RFlags.Metafile)
	{
		RenderMethod = GF_USE_GAVINCLIPPING;
	}
	else
	{
		if (!SetClipToPathTemporary(PathToDraw))
		{
			// Can't use clipping - fall back to EOR.
			RenderMethod = GF_USE_GDIEOR;
		}
	}

	INT32 RenderCount = 1;

	if (RenderMethod == GF_USE_GDIEOR)
	{
		SetDrawingMode(DM_EORDITHER);
		RenderCount = 2;
	}

	DocColour TempFillColour;

	while (RenderCount > 0)
	{
		COLORREF CurrentCol = RGB(ColourSteps[FillSteps - 1].rgbRed, 
									ColourSteps[FillSteps - 1].rgbGreen, 
									ColourSteps[FillSteps - 1].rgbBlue);

		// Put the RGB value back into a DocColour so we can set it as the fill colour. We set it
		// as non-separable, as it will have already been colour-separated by BuildGraduatedPalette
		TempFillColour.SetRGBValue(ColourSteps[FillSteps - 1].rgbRed, 
									ColourSteps[FillSteps - 1].rgbGreen, 
									ColourSteps[FillSteps - 1].rgbBlue);
		TempFillColour.SetSeparable(FALSE);

		SetFillColour(TempFillColour);

		GetValidBrush();
		GetValidPen();

		// First, fill in whole path with end colour
		//RenderPath(PathToDraw);
		Path PreviousPath;
		PreviousPath.Initialise(PathToDraw->GetNumCoords() + 2, 12);
		PreviousPath.CopyPathDataFrom(PathToDraw);

		// Work out relative positions of start and end points.
		DocCoord Steps[2];
		Steps[0].x = Fill->StartPoint.x - Fill->EndPoint.x;
		Steps[0].y = Fill->StartPoint.y - Fill->EndPoint.y;
		Steps[1].x = Fill->StartPoint.x - Fill->EndPoint2.x;
		Steps[1].y = Fill->StartPoint.y - Fill->EndPoint2.y;

		// Construct the parallelogram that describes the biggest ellipse.
		// (If I could draw a picture here, I would!)
		DocCoord Parallel[4];
		Parallel[0].x = Fill->StartPoint.x + (Steps[0].x + Steps[1].x);
		Parallel[0].y = Fill->StartPoint.y + (Steps[0].y + Steps[1].y);
		Parallel[1].x = Fill->StartPoint.x + (Steps[0].x - Steps[1].x);
		Parallel[1].y = Fill->StartPoint.y + (Steps[0].y - Steps[1].y);
		Parallel[2].x = Fill->StartPoint.x - (Steps[0].x + Steps[1].x);
		Parallel[2].y = Fill->StartPoint.y - (Steps[0].y + Steps[1].y);
		Parallel[3].x = Fill->StartPoint.x + (Steps[1].x - Steps[0].x);
		Parallel[3].y = Fill->StartPoint.y + (Steps[1].y - Steps[0].y);

		// Now convert the steps into the steps we should use to adjust the
		// parallelogram after each fill step
		Steps[0].x /= (FillSteps + 1);
		Steps[0].y /= (FillSteps + 1);
		Steps[1].x /= (FillSteps + 1);
		Steps[1].y /= (FillSteps + 1);

		// This are the paths we will use to create radial fills.
		Path RadialFill;
		RadialFill.Initialise(50,12);
		Path EllipsePath;
		EllipsePath.Initialise(15,12);

		// This is the path we use for all clipped rendering.
		Path ClippedPath;
		ClippedPath.Initialise(12, 12);


		// Now render the fill using concentric ellipses, starting with the biggest.
		for (INT32 i = FillSteps; i > 0;)
		{
			MakeEllipticalPath(&EllipsePath, Parallel);
			EllipsePath.IsFilled = TRUE;

			// Use this and the previous path to make an elliptical disc.
			RadialFill.ClearPath(FALSE);
			RadialFill.MakeSpaceInPath(PreviousPath.GetNumCoords() + EllipsePath.GetNumCoords() + 2);
			RadialFill.CopyPathDataFrom(&PreviousPath);
			RadialFill.IsFilled = TRUE;
			if (i > 1)
			{
				// For all except the last step, make a complex path (which is
				// usually an elliptical disc).
				ERROR2IF(!RadialFill.MergeTwoPaths(EllipsePath), FALSE, "could not merge paths");
			}

			// Remember the elliptical path for next time
			PreviousPath.ClearPath(FALSE);
			PreviousPath.MakeSpaceInPath(EllipsePath.GetNumCoords() + 2);
			PreviousPath.CopyPathDataFrom(&EllipsePath);

			if (RenderMethod != GF_USE_GAVINCLIPPING)
			{
				RenderPath(&RadialFill);
			}
			else
			{
				PathToDraw->ClipPathToPath(RadialFill, &ClippedPath, 2,
									   	   ClipTolerance, ClipFlatness, ClipFlatness);
				ClippedPath.IsFilled = TRUE;
				RenderPath(&ClippedPath);
			}

			// decrement loop counter here so that the next colour can be calculated
			i--;

			if (i > 0)
			{
				// Calculate next brush (except at the end of the loop)
				const COLORREF NewFill = RGB(ColourSteps[i].rgbRed, ColourSteps[i].rgbGreen,
												ColourSteps[i].rgbBlue);
				if (NewFill != CurrentCol)
				{
					// colour changed, make new brush
					CurrentCol = NewFill;

					// Put the RGB value back into a DocColour so we can set it as the fill colour. We set it
					// as non-separable, as it will have already been colour-separated by BuildGraduatedPalette
					TempFillColour.SetRGBValue(ColourSteps[i].rgbRed, 
												ColourSteps[i].rgbGreen, 
												ColourSteps[i].rgbBlue);
					TempFillColour.SetSeparable(FALSE);

					SetFillColour(TempFillColour);
					GetValidBrush();
					GetValidPen();
				}

				// Reduce size of paralleogram
				Parallel[0].x -= (Steps[0].x + Steps[1].x);
				Parallel[0].y -= (Steps[0].y + Steps[1].y);
				Parallel[1].x -= (Steps[0].x - Steps[1].x);
				Parallel[1].y -= (Steps[0].y - Steps[1].y);
				Parallel[2].x += (Steps[0].x + Steps[1].x);
				Parallel[2].y += (Steps[0].y + Steps[1].y);
				Parallel[3].x -= (Steps[1].x - Steps[0].x);
				Parallel[3].y -= (Steps[1].y - Steps[0].y);
			}
		}

		if ((RenderMethod == GF_USE_GDIEOR) && (RenderCount > 1))
		{
			// Render the path in white
			SetDrawingMode(DM_COPYPEN);
			SetFillColour(COLOUR_BLACK);
			SetLineColour(COLOUR_TRANS);
			GetValidBrush();
			GetValidPen();
			RenderPath(PathToDraw);

			// Set back to mode required for grad fill rendering
			SetDrawingMode(DM_EORDITHER);
		}

		RenderCount--;
	}

	// now tidy up
	RestoreContext();
	GetValidBrush();
	GetValidPen();

	// Lose the clipping region if we are using one.
	if (RenderMethod == GF_USE_GDICLIPPING)
		RenderDC->SetClippingRegion( *OSClipRegion );

	return TRUE;
}

/********************************************************************************************

>	BOOL OSRenderRegion::RenderLinearFill(Path *PathToDraw, LinearFillAttribute *Fill)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>? Rik? Tim?
	Created:	?
	Inputs:		PathToDraw	- A pointer to the path to be rendered.
				Fill		- A pointer to the radial fill applied.
	Returns:	TRUE		- Success.
				FALSE		- An error occured.
	Purpose:	Renders the path with a gradient fill.

********************************************************************************************/

BOOL OSRenderRegion::RenderLinearFill(Path *PathToDraw, LinearFillAttribute *Fill)
{
	GradFillMethodType RenderMethod = GF_USE_GDICLIPPING;

	// First, we need to find out the minimum width of rectangles we can
	// plot and still fill the entire shape.
	// To do this we find the maxiumum distance between the start point and all four
	// corners of the bounding box.
	// NB. This actually gives us *half* the minimum width we need.
	DocRect BoundsRect = PathToDraw->GetBoundingRect();

	DocCoord TestPoint = BoundsRect.lo;
	INT32 Distance = CalcDistance(Fill->StartPoint, TestPoint);
	TestPoint.x = BoundsRect.hi.x;
	INT32 NewDistance = CalcDistance(Fill->StartPoint, TestPoint);
	Distance = max(Distance, NewDistance);
	TestPoint = BoundsRect.hi;
	NewDistance = CalcDistance(Fill->StartPoint, TestPoint);
	Distance = max(Distance, NewDistance);
	TestPoint.x = BoundsRect.lo.x;
	NewDistance = CalcDistance(Fill->StartPoint, TestPoint);
	Distance = max(Distance, NewDistance);

	// Distance now holds the minimum distance either side of the line that we should
	// render.

	// We want to change the rendering context, so we save it.
	SaveContext();

	// No stroking while rendering the fill.
	SetLineColour(COLOUR_TRANS);

	// Use device pixel size to work out how many steps to do.
	FIXED16 PixelSize = 0;
	GetRenderView()->GetScaledPixelSize(&PixelSize,&PixelSize);

	// Work out how long the fill arrow is in millipoints.
	INT32 FillLength = CalcDistance(Fill->StartPoint, Fill->EndPoint);

	// Do a slice every 4 pixels (arbitrary!)
	INT32 FillSteps = (INT32) ((FillLength / PixelSize.MakeDouble()) / 4);

	// Also base it on distance between colours
	EFFECTTYPE EffectType = GetFillEffect();
	FillSteps = min(ColDifference(Fill->Colour, Fill->EndColour,
								  IsPrinting() ? 24 : 8, 				// Colour depth
								  EffectType), 
								  FillSteps);

	// Limit fill steps to a maximum
	FillSteps = min( FillSteps, INT32(MAX_FILL_STEPS) );

	// Use global Quality setting
	FillSteps = FillSteps / (1 << GradFillQuality);

	FillSteps = max(FillSteps, 10);

	// Work out what flatness and accuracy to use for Gavin's clipping functions.
	// Gavin recommends 1/2 pixel for flattening, and 1/4 pixel for tolerance
	// (but these were kind of off the top of his head - Tim).
	// PS. It seems we need maximum tolerance (i.e. 0) for accurate grad-fill rendering.
	INT32 ClipFlatness = PixelSize.MakeLong() / 16;
	INT32 ClipTolerance = 0;

	RGBQUAD ColourSteps[MAX_FILL_STEPS];
	GradTable32::BuildGraduatedPalette(Fill->Colour, Fill->EndColour,
										GetRenderView(), EffectType,
										FillSteps, ColourSteps);
	INT32 RenderCount = 1;

	// If we are rendering into a metafile, clip the path ourselves
	if (RFlags.Metafile)
	{
		RenderMethod = GF_USE_GAVINCLIPPING;
	}
	else
	{
		if (!SetClipToPathTemporary(PathToDraw))
		{
			// Can't use clipping - fall back to EOR.
			RenderMethod = GF_USE_GDIEOR;
			SetDrawingMode(DM_EORDITHER);
			RenderCount = 2;
		}
	}

	DocColour TempFillColour;

	while (RenderCount > 0)
	{
		COLORREF CurrentCol = RGB(ColourSteps[0].rgbRed, ColourSteps[0].rgbGreen, ColourSteps[0].rgbBlue);

		// Put the RGB value back into a DocColour so we can set it as the fill colour. We set it
		// as non-separable, as it will have already been colour-separated by BuildGraduatedPalette
		TempFillColour.SetRGBValue(ColourSteps[0].rgbRed, 
									ColourSteps[0].rgbGreen, 
									ColourSteps[0].rgbBlue);
		TempFillColour.SetSeparable(FALSE);

		SetFillColour(TempFillColour);
		GetValidBrush();
		GetValidPen();

		// This is the path we will use to create linear fills.
		Path LinearFill;
		LinearFill.Initialise(12,12);

		// This is the path we use for all clipped rendering.
		Path ClippedPath;
		ClippedPath.Initialise(12, 12);

		// Work out differences in x and y coords for fill arrow.
		double FillStepX = ((double) (Fill->EndPoint.x - Fill->StartPoint.x)) / (double) FillSteps;
		double FillStepY = ((double) (Fill->EndPoint.y - Fill->StartPoint.y)) / (double) FillSteps;

		double FillAngle = atan2((double)(Fill->EndPoint.x - Fill->StartPoint.x),
								 (double)(Fill->EndPoint.y - Fill->StartPoint.y));

		// Used to find perpendicular angles.
		const double HalfPI = PI/2.0;

		DocCoord FillPoint = Fill->StartPoint;
		DocCoord RectPoint[2];

		// Find first two corners of rectangle
		RectPoint[0].x = FillPoint.x + (INT32) (((double) Distance) * sin(FillAngle - HalfPI));
		RectPoint[0].y = FillPoint.y + (INT32) (((double) Distance) * cos(FillAngle - HalfPI));
		RectPoint[1].x = FillPoint.x + (INT32) (((double) Distance) * sin(FillAngle + HalfPI));
		RectPoint[1].y = FillPoint.y + (INT32) (((double) Distance) * cos(FillAngle + HalfPI));



		// Do the start colour shape - we need to find out how far we need 
		// to extend beyond the fill arrow.
		TestPoint = BoundsRect.lo;
		INT32 LastDistance = CalcDistance(Fill->EndPoint, TestPoint);
		TestPoint.x = BoundsRect.hi.x;
		NewDistance = CalcDistance(Fill->EndPoint, TestPoint);
		LastDistance = max(LastDistance, NewDistance);
		TestPoint = BoundsRect.hi;
		NewDistance = CalcDistance(Fill->EndPoint, TestPoint);
		LastDistance = max(LastDistance, NewDistance);
		TestPoint.x = BoundsRect.lo.x;
		NewDistance = CalcDistance(Fill->EndPoint, TestPoint);
		LastDistance = max(LastDistance, NewDistance);

		// Make the last (big) rectangle.	
		LinearFill.ClearPath();
		LinearFill.FindStartOfPath();

		// Create a rectangle
		PathFlags NewFlags;
		NewFlags.IsRotate = TRUE;

		// Insert first two corners of rectangle.
		LinearFill.InsertMoveTo(RectPoint[0], &NewFlags);
		LinearFill.InsertLineTo(RectPoint[1], &NewFlags);

		// Move past the fill arrow far enough to cover last bit of shape.
		FillPoint.x = Fill->EndPoint.x - (MILLIPOINT) (LastDistance * sin(FillAngle));
		FillPoint.y = Fill->EndPoint.y - (MILLIPOINT) (LastDistance * cos(FillAngle));

		// Find other two corners of rectangle.
		DocCoord EndPoint[2];
		EndPoint[0].x = FillPoint.x + (INT32) (((double) Distance) * sin(FillAngle - HalfPI));
		EndPoint[0].y = FillPoint.y + (INT32) (((double) Distance) * cos(FillAngle - HalfPI));
		EndPoint[1].x = FillPoint.x + (INT32) (((double) Distance) * sin(FillAngle + HalfPI));
		EndPoint[1].y = FillPoint.y + (INT32) (((double) Distance) * cos(FillAngle + HalfPI));

		// Insert last two corners of rectangle.
		LinearFill.InsertLineTo(EndPoint[1], &NewFlags);
		LinearFill.InsertLineTo(EndPoint[0], &NewFlags);

		// Close the path properly
		LinearFill.CloseSubPath();

		LinearFill.IsFilled = TRUE;
		if (RenderMethod != GF_USE_GAVINCLIPPING)
		{
			RenderPath(&LinearFill);
		}
		else
		{
			PathToDraw->ClipPathToPath(LinearFill, &ClippedPath, 2,
								   	   ClipTolerance, ClipFlatness, ClipFlatness);
			ClippedPath.IsFilled = TRUE;
			RenderPath(&ClippedPath);
		}


		// Now render the fill using rectangles.
		for (INT32 i = 0; i < FillSteps;)
		{
			LinearFill.ClearPath();
			LinearFill.FindStartOfPath();

			// Create a rectangle
			PathFlags NewFlags;
			NewFlags.IsRotate = TRUE;

			// Insert first two corners of rectangle.
			LinearFill.InsertMoveTo(RectPoint[0], &NewFlags);
			LinearFill.InsertLineTo(RectPoint[1], &NewFlags);

			// increment loop counter here so that the next colour can be calculated
			i++;

			// Move along the arrow one step
			FillPoint.x = Fill->StartPoint.x + (MILLIPOINT) (i * FillStepX);
			FillPoint.y = Fill->StartPoint.y + (MILLIPOINT) (i * FillStepY);

			// Find other two corners of rectangle.
			RectPoint[0].x = FillPoint.x + (INT32) (((double) Distance) * sin(FillAngle - HalfPI));
			RectPoint[0].y = FillPoint.y + (INT32) (((double) Distance) * cos(FillAngle - HalfPI));
			RectPoint[1].x = FillPoint.x + (INT32) (((double) Distance) * sin(FillAngle + HalfPI));
			RectPoint[1].y = FillPoint.y + (INT32) (((double) Distance) * cos(FillAngle + HalfPI));

			// Insert last two corners of rectangle.
			LinearFill.InsertLineTo(RectPoint[1], &NewFlags);
			LinearFill.InsertLineTo(RectPoint[0], &NewFlags);

			// Close the path properly
			LinearFill.CloseSubPath();

			LinearFill.IsFilled = TRUE;
			if (RenderMethod != GF_USE_GAVINCLIPPING)
			{
				RenderPath(&LinearFill);
			}
			else
			{
				PathToDraw->ClipPathToPath(LinearFill, &ClippedPath, 2,
									   	   ClipTolerance, ClipFlatness, ClipFlatness);
				ClippedPath.IsFilled = TRUE;
				RenderPath(&ClippedPath);
			}

			if (i != FillSteps)
			{
				// Calculate next brush
				const COLORREF NewFill = RGB(ColourSteps[i].rgbRed, ColourSteps[i].rgbGreen,
												ColourSteps[i].rgbBlue);
				if (NewFill != CurrentCol)
				{
					// colour changed, make new brush
					CurrentCol = NewFill;

					// Put the RGB value back into a DocColour so we can set it as the fill colour. We set it
					// as non-separable, as it will have already been colour-separated by BuildGraduatedPalette
					TempFillColour.SetRGBValue(ColourSteps[i].rgbRed, 
												ColourSteps[i].rgbGreen, 
												ColourSteps[i].rgbBlue);
					TempFillColour.SetSeparable(FALSE);

					SetFillColour(TempFillColour);
					GetValidBrush();
					GetValidPen();
				}
			}
		}

		// Do the end colour shape - we need to find out how far we need 
		// to extend beyond the fill arrow.
		TestPoint = BoundsRect.lo;
		LastDistance = CalcDistance(Fill->EndPoint, TestPoint);
		TestPoint.x = BoundsRect.hi.x;
		NewDistance = CalcDistance(Fill->EndPoint, TestPoint);
		LastDistance = max(LastDistance, NewDistance);
		TestPoint = BoundsRect.hi;
		NewDistance = CalcDistance(Fill->EndPoint, TestPoint);
		LastDistance = max(LastDistance, NewDistance);
		TestPoint.x = BoundsRect.lo.x;
		NewDistance = CalcDistance(Fill->EndPoint, TestPoint);
		LastDistance = max(LastDistance, NewDistance);

		// Make the last (big) rectangle.	
		LinearFill.ClearPath();
		LinearFill.FindStartOfPath();

		// Create a rectangle
		NewFlags.IsRotate = TRUE;

		// Insert first two corners of rectangle.
		LinearFill.InsertMoveTo(RectPoint[0], &NewFlags);
		LinearFill.InsertLineTo(RectPoint[1], &NewFlags);

		// Move past the fill arrow far enough to cover last bit of shape.
		FillPoint.x = Fill->EndPoint.x + (MILLIPOINT) (LastDistance * sin(FillAngle));
		FillPoint.y = Fill->EndPoint.y + (MILLIPOINT) (LastDistance * cos(FillAngle));

		// Find other two corners of rectangle.
		RectPoint[0].x = FillPoint.x + (INT32) (((double) Distance) * sin(FillAngle - HalfPI));
		RectPoint[0].y = FillPoint.y + (INT32) (((double) Distance) * cos(FillAngle - HalfPI));
		RectPoint[1].x = FillPoint.x + (INT32) (((double) Distance) * sin(FillAngle + HalfPI));
		RectPoint[1].y = FillPoint.y + (INT32) (((double) Distance) * cos(FillAngle + HalfPI));

		// Insert last two corners of rectangle.
		LinearFill.InsertLineTo(RectPoint[1], &NewFlags);
		LinearFill.InsertLineTo(RectPoint[0], &NewFlags);

		// Close the path properly
		LinearFill.CloseSubPath();

		LinearFill.IsFilled = TRUE;
		if (RenderMethod != GF_USE_GAVINCLIPPING)
		{
			RenderPath(&LinearFill);
		}
		else
		{
			PathToDraw->ClipPathToPath(LinearFill, &ClippedPath, 2,
								   	   ClipTolerance, ClipFlatness, ClipFlatness);
			ClippedPath.IsFilled = TRUE;
			RenderPath(&ClippedPath);
		}

		if ((RenderMethod == GF_USE_GDIEOR) && (RenderCount > 1))
		{
			// Render the path in white
			SetDrawingMode(DM_COPYPEN);
			SetFillColour(COLOUR_BLACK);
			SetLineColour(COLOUR_TRANS);
			GetValidBrush();
			GetValidPen();
			RenderPath(PathToDraw);

			// Set back to mode required for grad fill rendering
			SetDrawingMode(DM_EORDITHER);
		}

		RenderCount--;
	}

	// now tidy up
	RestoreContext();
	GetValidBrush();
	GetValidPen();

	// Lose the clipping region if we are using one.
	if (RenderMethod == GF_USE_GDICLIPPING)
		RenderDC->SetClippingRegion(*OSClipRegion);

	return TRUE;
}

/********************************************************************************************

>	BOOL OSRenderRegion::RenderConicalFill(Path *PathToDraw, LinearFillAttribute *Fill)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>? Rik? Tim?
	Created:	?
	Inputs:		PathToDraw	- A pointer to the path to be rendered.
				Fill		- A pointer to the radial fill applied.
	Returns:	TRUE		- Success.
				FALSE		- An error occured.
	Purpose:	Renders the path with a gradient fill.

********************************************************************************************/

BOOL OSRenderRegion::RenderConicalFill(Path *PathToDraw, ConicalFillAttribute *Fill)
{
	GradFillMethodType RenderMethod = GF_USE_GDICLIPPING;

	// First, we need to find out the minimum radius of the circle we can
	// plot and still fill the entire shape.
	// To do this we find the maxiumum distance between the start point and all four
	// corners of the bounding box.
	DocRect BoundsRect = PathToDraw->GetBoundingRect();

	DocCoord TestPoint = BoundsRect.lo;
	INT32 Radius = CalcDistance(Fill->StartPoint, TestPoint);
	TestPoint.x = BoundsRect.hi.x;
	INT32 NewRadius = CalcDistance(Fill->StartPoint, TestPoint);
	Radius = max(Radius, NewRadius);
	TestPoint = BoundsRect.hi;
	NewRadius = CalcDistance(Fill->StartPoint, TestPoint);
	Radius = max(Radius, NewRadius);
	TestPoint.x = BoundsRect.lo.x;
	NewRadius = CalcDistance(Fill->StartPoint, TestPoint);
	Radius = max(Radius, NewRadius);

	// Radius now holds the minimum radius we can use.

	// We want to change the rendering context, so we save it.
	SaveContext();

	// No stroking while rendering the fill.
	SetLineColour(COLOUR_TRANS);

	// Use device pixel size to work out how many steps to do.
	FIXED16 PixelSize = 0;
	GetRenderView()->GetScaledPixelSize(&PixelSize,&PixelSize);
	INT32 FillSteps = (INT32) ((Radius * 2 * PI) / PixelSize.MakeDouble());

	// Do a slice every 4 pixels (arbitrary!)
	FillSteps /= 4;

	// Also base it on distance between colours
	EFFECTTYPE EffectType = GetFillEffect();
	FillSteps = min(ColDifference(Fill->Colour, Fill->EndColour, 
								  IsPrinting() ? 24 : 8, 			// Colour depth
								  EffectType), 
								  FillSteps);

	// Limit fill steps to a maximum
	FillSteps = min( FillSteps, INT32(MAX_FILL_STEPS) );

	// Use global Quality setting
	FillSteps = FillSteps / (1 << GradFillQuality);

	// Note that with a conical fill we fade from the start colour to the end colour
	// and then back to the start colour, so we only need half as many fill steps as you
	// might think.
	FillSteps /= 2;

	// Limit to minimum of 10 steps
	FillSteps = max(FillSteps, 10);

	// Work out what flatness and accuracy to use for Gavin's clipping functions.
	// Gavin recommends 1/2 pixel for flattening, and 1/4 pixel for tolerance
	// (but these were kind of off the top of his head - Tim).
	// PS. It seems we need maximum tolerance (i.e. 0) for accurate grad-fill rendering.
	INT32 ClipFlatness = PixelSize.MakeLong() / 16;
	INT32 ClipTolerance = 0;

	RGBQUAD ColourSteps[MAX_FILL_STEPS];
	GradTable32::BuildGraduatedPalette(Fill->Colour, Fill->EndColour,
										GetRenderView(), EffectType,
										FillSteps, ColourSteps);


	// If we are rendering into a metafile, clip the path ourselves
	if (RFlags.Metafile)
	{
		RenderMethod = GF_USE_GAVINCLIPPING;
	}
	else
	{
		if (!SetClipToPathTemporary(PathToDraw))
		{
			// Can't use clipping - fall back to EOR.
			RenderMethod = GF_USE_GDIEOR;
		}
	}

	INT32 RenderCount = 1;

	if (RenderMethod == GF_USE_GDIEOR)
	{
		SetDrawingMode(DM_EORDITHER);
		RenderCount = 2;
	}

	DocColour TempFillColour;

	while (RenderCount > 0)
	{
		COLORREF CurrentCol = RGB(ColourSteps[FillSteps - 1].rgbRed, 
									ColourSteps[FillSteps - 1].rgbGreen, 
									ColourSteps[FillSteps - 1].rgbBlue);

		// Put the RGB value back into a DocColour so we can set it as the fill colour. We set it
		// as non-separable, as it will have already been colour-separated by BuildGraduatedPalette
		TempFillColour.SetRGBValue(ColourSteps[FillSteps - 1].rgbRed, 
									ColourSteps[FillSteps - 1].rgbGreen, 
									ColourSteps[FillSteps - 1].rgbBlue);
		TempFillColour.SetSeparable(FALSE);

		SetFillColour(TempFillColour);
		GetValidBrush();
		GetValidPen();

		// This is the path we will use to create conical fills.
		Path ConicalFill;
		ConicalFill.Initialise(12,12);

		// This is the path we use for all clipped rendering.
		Path ClippedPath;
		ClippedPath.Initialise(12, 12);

		double FillAngle = atan2((double)(Fill->EndPoint.x - Fill->StartPoint.x),
								 (double)(Fill->EndPoint.y - Fill->StartPoint.y));

		double AngleInc = PI / FillSteps; // Actually (2 * PI) / (2 * FillSteps)

		TestPoint.x = Fill->StartPoint.x + (INT32) (((double) Radius) * sin(FillAngle));
		TestPoint.y = Fill->StartPoint.y + (INT32) (((double) Radius) * cos(FillAngle));

		// Now render the fill using radiating triangles.
		INT32 FillInc = -1;

		// NB. This loop is unusual because we loop from FillSteps down to 0 and back 
		// up to FillSteps - because that's how conical fills work.
		for (INT32 i = FillSteps - 1; i <= FillSteps; i += FillInc)
		{
			ConicalFill.ClearPath();
			ConicalFill.FindStartOfPath();

			// Create a triangle
			PathFlags NewFlags;
			NewFlags.IsRotate = TRUE;
			ConicalFill.InsertMoveTo(Fill->StartPoint, &NewFlags);
			ConicalFill.InsertLineTo(TestPoint, &NewFlags);
			FillAngle += AngleInc;
			TestPoint.x = Fill->StartPoint.x + (INT32) (((double) Radius) * sin(FillAngle));
			TestPoint.y = Fill->StartPoint.y + (INT32) (((double) Radius) * cos(FillAngle));
			ConicalFill.InsertLineTo(TestPoint, &NewFlags);

			// Close the path properly
			ConicalFill.CloseSubPath();

			ConicalFill.IsFilled = TRUE;

			if (RenderMethod != GF_USE_GAVINCLIPPING)
			{
				RenderPath(&ConicalFill);
			}
			else
			{
				PathToDraw->ClipPathToPath(ConicalFill, &ClippedPath, 2,
									   	   ClipTolerance, ClipFlatness, ClipFlatness);
				ClippedPath.IsFilled = TRUE;
				RenderPath(&ClippedPath);
			}

			// decrement loop counter here so that the next colour can be calculated

			if (i <= 0)
				// We need to start going the other way!
				FillInc = 1;

			// Calculate next brush
			if (i != FillSteps)
			{
				const COLORREF NewFill = RGB(ColourSteps[i].rgbRed,
												ColourSteps[i].rgbGreen,
												ColourSteps[i].rgbBlue);
				if (NewFill != CurrentCol)
				{
					// colour changed, make new brush
					CurrentCol = NewFill;

					// Put the RGB value back into a DocColour so we can set it as the fill colour. We set it
					// as non-separable, as it will have already been colour-separated by BuildGraduatedPalette
					TempFillColour.SetRGBValue(ColourSteps[i].rgbRed, 
												ColourSteps[i].rgbGreen, 
												ColourSteps[i].rgbBlue);
					TempFillColour.SetSeparable(FALSE);

					SetFillColour(TempFillColour);
					GetValidBrush();
					GetValidPen();
				}
			}
		}

		if ((RenderMethod == GF_USE_GDIEOR) && (RenderCount > 1))
		{
			// Render the path in white
			SetDrawingMode(DM_COPYPEN);
			SetFillColour(COLOUR_BLACK);
			SetLineColour(COLOUR_TRANS);
			GetValidBrush();
			GetValidPen();
			RenderPath(PathToDraw);

			// Set back to mode required for grad fill rendering
			SetDrawingMode(DM_EORDITHER);
		}

		RenderCount--;
	}

	// now tidy up
	RestoreContext();
	GetValidBrush();
	GetValidPen();

	// Lose the clipping region if we are using one.
	if (RenderMethod == GF_USE_GDICLIPPING)
		RenderDC->SetClippingRegion(*OSClipRegion);

	return TRUE;
}

/********************************************************************************************

>	BOOL OSRenderRegion::RenderSquareFill(Path *PathToDraw, LinearFillAttribute *Fill)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>? Rik? Tim?
	Created:	?
	Inputs:		PathToDraw	- A pointer to the path to be rendered.
				Fill		- A pointer to the radial fill applied.
	Returns:	TRUE		- Success.
				FALSE		- An error occured.
	Purpose:	Renders the path with a gradient fill.

********************************************************************************************/

BOOL OSRenderRegion::RenderSquareFill(Path *PathToDraw, SquareFillAttribute *Fill)
{
	GradFillMethodType RenderMethod = GF_USE_GDICLIPPING;

	INT32 Radius1 = CalcDistance(Fill->StartPoint, Fill->EndPoint);
	INT32 Radius2 = CalcDistance(Fill->StartPoint, Fill->EndPoint2);

	// We want to change the rendering context, so we save it.
	SaveContext();

	// No stroking while rendering the fill.
	SetLineColour(COLOUR_TRANS);

	// Use device pixel size to work out how many steps to do.
	FIXED16 PixelSize = 0;
	GetRenderView()->GetScaledPixelSize(&PixelSize,&PixelSize);
	INT32 FillSteps = (INT32) (max(Radius1, Radius2) / PixelSize.MakeDouble());

	// Do a slice every 4 pixels (arbitrary!)
	FillSteps /= 4;

	// Also base it on distance between colours
	EFFECTTYPE EffectType = GetFillEffect();
	FillSteps = min(ColDifference(Fill->Colour, Fill->EndColour, 
								  IsPrinting() ? 24 : 8, 	// Colour depth
								  EffectType), 
								  FillSteps);

	// Limit fill steps to a maximum
	FillSteps = min( FillSteps, INT32(MAX_FILL_STEPS) );

	// Use global Quality setting
	FillSteps = FillSteps / (1 << GradFillQuality);
	FillSteps = max(FillSteps, 10);

	// Work out what flatness and accuracy to use for Gavin's clipping functions.
	// Gavin recommends 1/2 pixel for flattening, and 1/4 pixel for tolerance
	// (but these were kind of off the top of his head - Tim).
	// PS. It seems we need maximum tolerance (i.e. 0) for accurate grad-fill rendering.
	INT32 ClipFlatness = PixelSize.MakeLong() / 16;
	INT32 ClipTolerance = 0;

	RGBQUAD ColourSteps[MAX_FILL_STEPS];
	GradTable32::BuildGraduatedPalette(Fill->Colour, Fill->EndColour,
										GetRenderView(), EffectType,
										FillSteps, ColourSteps);

	// If we are rendering into a metafile, clip the path ourselves
	if (RFlags.Metafile)
	{
		RenderMethod = GF_USE_GAVINCLIPPING;
	}
	else
	{
		if (!SetClipToPathTemporary(PathToDraw))
		{
			// Can't use clipping - fall back to EOR.
			RenderMethod = GF_USE_GDIEOR;
		}
	}

	INT32 RenderCount = 1;

	if (RenderMethod == GF_USE_GDIEOR)
	{
		SetDrawingMode(DM_EORDITHER);
		RenderCount = 2;
	}

	DocColour TempFillColour;

	while (RenderCount > 0)
	{
		COLORREF CurrentCol = RGB(ColourSteps[FillSteps - 1].rgbRed, 
									ColourSteps[FillSteps - 1].rgbGreen, 
									ColourSteps[FillSteps - 1].rgbBlue);

		// Put the RGB value back into a DocColour so we can set it as the fill colour. We set it
		// as non-separable, as it will have already been colour-separated by BuildGraduatedPalette
		TempFillColour.SetRGBValue(ColourSteps[FillSteps - 1].rgbRed, 
									ColourSteps[FillSteps - 1].rgbGreen, 
									ColourSteps[FillSteps - 1].rgbBlue);
		TempFillColour.SetSeparable(FALSE);

		SetFillColour(TempFillColour);

		GetValidBrush();
		GetValidPen();

		// First, fill in whole path with end colour
		//RenderPath(PathToDraw);
		Path PreviousPath;
		PreviousPath.Initialise(PathToDraw->GetNumCoords() + 2, 12);
		PreviousPath.CopyPathDataFrom(PathToDraw);

		// Work out relative positions of start and end points.
		DocCoord Steps[2];
		Steps[0].x = Fill->StartPoint.x - Fill->EndPoint.x;
		Steps[0].y = Fill->StartPoint.y - Fill->EndPoint.y;
		Steps[1].x = Fill->StartPoint.x - Fill->EndPoint2.x;
		Steps[1].y = Fill->StartPoint.y - Fill->EndPoint2.y;

		// Construct the parallelogram that describes the extent of the fill
		DocCoord Parallel[4];
		Parallel[0].x = Fill->StartPoint.x + (Steps[0].x + Steps[1].x);
		Parallel[0].y = Fill->StartPoint.y + (Steps[0].y + Steps[1].y);
		Parallel[1].x = Fill->StartPoint.x + (Steps[0].x - Steps[1].x);
		Parallel[1].y = Fill->StartPoint.y + (Steps[0].y - Steps[1].y);
		Parallel[2].x = Fill->StartPoint.x - (Steps[0].x + Steps[1].x);
		Parallel[2].y = Fill->StartPoint.y - (Steps[0].y + Steps[1].y);
		Parallel[3].x = Fill->StartPoint.x + (Steps[1].x - Steps[0].x);
		Parallel[3].y = Fill->StartPoint.y + (Steps[1].y - Steps[0].y);

		// Now convert the steps into the steps we should use to adjust the
		// parallelogram after each fill step
		Steps[0].x /= (FillSteps + 1);
		Steps[0].y /= (FillSteps + 1);
		Steps[1].x /= (FillSteps + 1);
		Steps[1].y /= (FillSteps + 1);

		// These are the paths we will use to create square fills.
		Path SquareFill;
		SquareFill.Initialise(50,12);
		Path ParallelogramPath;
		ParallelogramPath.Initialise(15,12);

		// This is the path we use for all clipped rendering.
		Path ClippedPath;
		ClippedPath.Initialise(12, 12);

		// Now render the fill using concentric parallelograms, starting with the biggest.
		for (INT32 i = FillSteps; i > 0;)
		{
			// build a path
			ParallelogramPath.ClearPath();
			ParallelogramPath.FindStartOfPath();
			ParallelogramPath.InsertMoveTo(Parallel[0]);
			ParallelogramPath.InsertLineTo(Parallel[1]);
			ParallelogramPath.InsertLineTo(Parallel[2]);
			ParallelogramPath.InsertLineTo(Parallel[3]);
			ParallelogramPath.InsertLineTo(Parallel[0]);
			ParallelogramPath.IsFilled = TRUE;

			// Use this and the previous path to make a parallelogram shaped 'washer'
			SquareFill.ClearPath(FALSE);
			SquareFill.MakeSpaceInPath(PreviousPath.GetNumCoords() + ParallelogramPath.GetNumCoords() + 2);
			SquareFill.CopyPathDataFrom(&PreviousPath);
			SquareFill.IsFilled = TRUE;
			if (i > 1)
			{
				// For all except the last step, make a complex path (which is
				// usually a parallelogram).
				ERROR2IF(!SquareFill.MergeTwoPaths(ParallelogramPath), FALSE, "could not merge paths");
			}

			// Remember the parallelogram path for next time
			PreviousPath.ClearPath(FALSE);
			PreviousPath.MakeSpaceInPath(ParallelogramPath.GetNumCoords() + 2);
			PreviousPath.CopyPathDataFrom(&ParallelogramPath);

			if (RenderMethod != GF_USE_GAVINCLIPPING)
			{
				RenderPath(&SquareFill);
			}
			else
			{
				PathToDraw->ClipPathToPath(SquareFill, &ClippedPath, 2,
									   	   ClipTolerance, ClipFlatness, ClipFlatness);
				ClippedPath.IsFilled = TRUE;
				RenderPath(&ClippedPath);
			}

			// decrement loop counter here so that the next colour can be calculated
			i--;

			if (i > 0)
			{
				// Calculate next brush (except at the end of the loop)
				const COLORREF NewFill = RGB(ColourSteps[i].rgbRed, ColourSteps[i].rgbGreen,
												ColourSteps[i].rgbBlue);
				if (NewFill != CurrentCol)
				{
					// colour changed, make new brush
					CurrentCol = NewFill;

					// Put the RGB value back into a DocColour so we can set it as the fill colour. We set it
					// as non-separable, as it will have already been colour-separated by BuildGraduatedPalette
					TempFillColour.SetRGBValue(ColourSteps[i].rgbRed, 
												ColourSteps[i].rgbGreen, 
												ColourSteps[i].rgbBlue);
					TempFillColour.SetSeparable(FALSE);

					SetFillColour(TempFillColour);
					GetValidBrush();
					GetValidPen();
				}

				// Reduce size of paralleogram
				Parallel[0].x -= (Steps[0].x + Steps[1].x);
				Parallel[0].y -= (Steps[0].y + Steps[1].y);
				Parallel[1].x -= (Steps[0].x - Steps[1].x);
				Parallel[1].y -= (Steps[0].y - Steps[1].y);
				Parallel[2].x += (Steps[0].x + Steps[1].x);
				Parallel[2].y += (Steps[0].y + Steps[1].y);
				Parallel[3].x -= (Steps[1].x - Steps[0].x);
				Parallel[3].y -= (Steps[1].y - Steps[0].y);
			}
		}

		if ((RenderMethod == GF_USE_GDIEOR) && (RenderCount > 1))
		{
			// Render the path in white
			SetDrawingMode(DM_COPYPEN);
			SetFillColour(COLOUR_BLACK);
			SetLineColour(COLOUR_TRANS);
			GetValidBrush();
			GetValidPen();
			RenderPath(PathToDraw);

			// Set back to mode required for grad fill rendering
			SetDrawingMode(DM_EORDITHER);
		}

		RenderCount--;
	}

	// now tidy up
	RestoreContext();
	GetValidBrush();
	GetValidPen();

	// Lose the clipping region if we are using one.
	if (RenderMethod == GF_USE_GDICLIPPING)
		RenderDC->SetClippingRegion(*OSClipRegion);

	return TRUE;
}

/********************************************************************************************

>	BOOL OSRenderRegion::RenderThreeColFill ( Path					*PathToDraw,
											  ThreeColFillAttribute	*Fill )

	Author: 	Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/00
	Inputs:		PathToDraw	- A pointer to the path to be rendered.
				Fill		- A pointer to the radial fill applied.
	Returns:	TRUE		- Success.
				FALSE		- An error occured.
	Purpose:	Renders a three colour gradient fill to a Windows DC.

********************************************************************************************/

BOOL OSRenderRegion::RenderThreeColFill ( Path					*PathToDraw,
										  ThreeColFillAttribute	*Fill )
{
	// Create a new end point for the fill.
	DocCoord			EndPoint	 ( *( Fill->GetEndPoint () ) + *( Fill->GetEndPoint2 () )
									   - *( Fill->GetStartPoint () ) );
	DocColour			EndColour;
	LinearFillAttribute	LinearFill;

	// Mix the two end colours together to make a blend suitable for the end point.
	EndColour.Mix ( Fill->GetEndColour (), Fill->GetEndColour2 (),
					0.5f, NULL, FALSE, NULL );

	// Set up the fill attribute.
	LinearFill.SetStartPoint	( Fill->GetStartPoint () );
	LinearFill.SetEndPoint		( &EndPoint );
	LinearFill.SetStartColour	( Fill->GetStartColour () );
	LinearFill.SetEndColour		( &EndColour );

	// Export the fill as a two colour linear fill.
	RenderLinearFill ( PathToDraw, &LinearFill );

	return TRUE;
}

/********************************************************************************************

>	BOOL OSRenderRegion::RenderFourColFill ( Path					*PathToDraw,
											 FourColFillAttribute	*Fill )

	Author: 	Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/00
	Inputs:		PathToDraw	- A pointer to the path to be rendered.
				Fill		- A pointer to the radial fill applied.
	Returns:	TRUE		- Success.
				FALSE		- An error occured.
	Purpose:	Renders a four colour gradient fill to a Windows DC. This is done by breaking
				the fill up into a large number of tiles, and then applying these with the
				colour at that point of the fill. This is in common with the other fills, and
				much of the code in this function has been derived from the other fill
				rendering functions.

********************************************************************************************/

BOOL OSRenderRegion::RenderFourColFill ( Path					*PathToDraw,
										 FourColFillAttribute	*Fill )
{
	// Create a linear fill attribute.
	LinearFillAttribute	LinearFill;

	// Set it up.
	LinearFill.SetStartPoint	( Fill->GetStartPoint () );
	LinearFill.SetEndPoint		( Fill->GetEndPoint3 () );
	LinearFill.SetStartColour	( Fill->GetStartColour () );
	LinearFill.SetEndColour		( Fill->GetEndColour3 () );

	// Export the fill as a two colour linear fill.
	RenderLinearFill ( PathToDraw, &LinearFill );

	// Graeme (11-6-00) - Mark has asked me to drop everything, and concentrate on getting a
	// Photoshop file format export / import filter going. As a consequence, I'm expected to
	// drop everything, and follow his commands, and so I've replacing this partially
	// complete four colour fill function with a call to the linear fill rendering function.
	// Normally I'd remove a piece of commented out code, but in this case I'm leaving it in
	// so that either I can get back to it later, or someone else can fix it.
	//
	// At the present time, all the function needs is some way of extending the fill pattern
	// out to the edges of the space being filled. There might be some way of spoofing this
	// using a linear gradient fill, and some kind of clipping region, or just rolling your
	// own code, and putting it in here.
/*
	DocCoord			HeightVector	= Fill->EndPoint  - Fill->StartPoint;
	DocCoord			WidthVector		= Fill->EndPoint2 - Fill->StartPoint;
	INT32					FillHeight		= CalcDistance ( Fill->StartPoint, Fill->EndPoint );
	INT32					FillWidth		= CalcDistance ( Fill->StartPoint, Fill->EndPoint2 );
	INT32					HeightSteps		= 0;
	INT32					WidthSteps		= 0;
	INT32					HeightColDiff	= 0;
	INT32					WidthColDiff	= 0;
	FIXED16				PixelSize		= 0;
	GradFillMethodType	RenderMethod	= GF_USE_GDICLIPPING;
	EFFECTTYPE			EffectType		= GetFillEffect();
	INT32				ClipFlatness	= 0;
	INT32				ClipTolerance	= 0;
	INT32					RenderCount		= 1;
	RGBQUAD				StartColours	[MAX_FILL_STEPS];
	RGBQUAD				EndColours		[MAX_FILL_STEPS];
	DocCoord			RowOffsets		[MAX_FILL_STEPS + 1];
	DocCoord			ColumnOffsets	[MAX_FILL_STEPS + 1];

	// We want to change the rendering context, so we save it.
	SaveContext ();

	// No stroking while rendering the fill.
	SetLineColour ( COLOUR_TRANS );

	// Use device pixel size to work out how many steps to do.
	GetRenderView ()->GetScaledPixelSize ( &PixelSize, &PixelSize );

	// Calculate the number of steps to be made for the fill in each direction, making a
	// slice every four pixels.
	HeightSteps		= FillHeight / ( PixelSize.MakeLong () * 4);
	WidthSteps		= FillWidth  / ( PixelSize.MakeLong () * 4);

	// Calculate the distance between colours.
	HeightColDiff	= min ( ColDifference ( Fill->Colour, Fill->EndColour,
											IsPrinting () ? 24 : 8, EffectType ), 
							ColDifference ( Fill->EndColour2, Fill->EndColour3,
											IsPrinting () ? 24 : 8, EffectType ) );

	WidthColDiff	= min ( ColDifference ( Fill->Colour, Fill->EndColour2,
											IsPrinting () ? 24 : 8, EffectType ), 
							ColDifference ( Fill->EndColour, Fill->EndColour3,
											IsPrinting () ? 24 : 8, EffectType ) );

	// Use the number of discrete steps between colours, and the number of slices to
	// determine just how many colours are needed.
	HeightSteps		= min ( HeightSteps, HeightColDiff );
	WidthSteps		= min ( WidthSteps,  WidthColDiff  );

	// Limit fill steps to a maximum value.
	HeightSteps		= min ( HeightSteps, MAX_FILL_STEPS );
	WidthSteps		= min ( WidthSteps,  MAX_FILL_STEPS );

	// Use global Quality setting.
	HeightSteps		= max ( HeightSteps / (1 << GradFillQuality), 10 );
	WidthSteps		= max ( WidthSteps  / (1 << GradFillQuality), 10 );

	// Work out what flatness and accuracy to use for Gavin's clipping functions.
	// Gavin recommends 1/2 pixel for flattening, and 1/4 pixel for tolerance.
	// (But these were kind of off the top of his head - Tim).
	// PS. It seems we need maximum tolerance (i.e. 0) for accurate grad-fill rendering.
	ClipFlatness = PixelSize.MakeLong () / 16;

	// Build the gradient table. In the other gradient fills, they call the
	// GradTable32::BuildGraduatedPalette () function. Unfortunately I need to operate
	// in two dimensions...

	// The first thing to do is to get a pair of gradient tables to act as the start and
	// end colours for each row of the fill.
	GradTable32::BuildGraduatedPalette ( Fill->Colour,
										 Fill->EndColour,
										 GetRenderView (),
										 EffectType,
										 HeightSteps,
										 StartColours );

	GradTable32::BuildGraduatedPalette ( Fill->EndColour2,
										 Fill->EndColour3,
										 GetRenderView (),
										 EffectType,
										 HeightSteps,
										 EndColours );

	// If we are rendering into a metafile, clip the path ourselves.
	if ( RFlags.Metafile )
	{
		RenderMethod = GF_USE_GAVINCLIPPING;
	}
	else
	{
		if ( !SetClipToPathTemporary ( PathToDraw ) )
		{
			// Can't use clipping - fall back to EOR.
			RenderMethod = GF_USE_GDIEOR;
		}
	}

	// Set the drawing mode up.
	if ( RenderMethod == GF_USE_GDIEOR )
	{
		SetDrawingMode ( DM_EORDITHER );
		RenderCount = 2;
	}

	// Pre-calculate the vectors between the various rows of tiles.
	for ( INT32 i = 0; i <= HeightSteps; i++ )
	{
		DocCoord Temp ( ( HeightVector.x * i ) / HeightSteps,
						( HeightVector.y * i ) / HeightSteps);

		RowOffsets [i] = Temp + Fill->StartPoint;
	}

	// And between the various columns of tiles.
	for ( INT32 j = 0; j <= WidthSteps; j++ )
	{
		ColumnOffsets [j] = DocCoord ( ( WidthVector.x * j ) / HeightSteps,
									   ( WidthVector.y * j ) / HeightSteps );
	}

	// Create the fill pattern.
	while ( RenderCount > 0 )
	{
		// Loop through the colour table, and create the necessary tiles to emulate the fill.
		for ( INT32 y = 0; y < HeightSteps; y++ )
		{
			RGBQUAD		RowColours	[MAX_FILL_STEPS];
			DocColour	FirstColour;
			DocColour	LastColour;
			DocCoord	TileHeight	= RowOffsets [y+1] - RowOffsets [y];

			// Initialise the start and end colours with values from the RGB quad. I need to
			// do this because the BuildGraduatePalette method only takes DocColors, and only
			// returns RGBQuads.
			FirstColour.SetRGBValue ( StartColours [y].rgbRed,
									  StartColours [y].rgbGreen,
									  StartColours [y].rgbBlue );

			LastColour.SetRGBValue  ( EndColours [y].rgbRed,
									  EndColours [y].rgbGreen,
									  EndColours [y].rgbBlue );

			// Create a new palette for this row of the fill.
			GradTable32::BuildGraduatedPalette ( FirstColour,
												 LastColour,
												 GetRenderView (),
												 EffectType,
												 WidthSteps,
												 RowColours );

			// Now write out the row.
			for ( INT32 x = 0; x < WidthSteps; x++ )
			{
				// Create the path for the tile to be rendered.
				Path		TilePath;
				PathFlags	TileFlags;
				DocColour	FillColour;

				// Initialise the path.
				TilePath.Initialise ( 12, 12 );

				// Set up the path.
				TilePath.InsertMoveTo ( RowOffsets [y]   + ColumnOffsets [x],	&TileFlags );
				TilePath.InsertLineTo ( RowOffsets [y+1] + ColumnOffsets [x],	&TileFlags );
				TilePath.InsertLineTo ( RowOffsets [y+1] + ColumnOffsets [x+1],	&TileFlags );
				TilePath.InsertLineTo ( RowOffsets [y]   + ColumnOffsets [x+1],	&TileFlags );

				// Mark the path as closed and filled.
				TilePath.CloseSubPath ();
				TilePath.IsFilled = TRUE;

				// Set up the fill colour.
				FillColour.SetRGBValue ( RowColours [x].rgbRed,
										 RowColours [x].rgbGreen,
										 RowColours [x].rgbBlue );

				FillColour.SetSeparable ( FALSE );

				// Set it as the rendering colour.
				SetFillColour	( FillColour );
				GetValidBrush	();
				GetValidPen		();

				// And write it out.
				if ( RenderMethod != GF_USE_GAVINCLIPPING )
				{
					RenderPath ( &TilePath );
				}
				else
				{
					// Create a new path.
					Path		ClippedPath;

					// Initialise the path.
					ClippedPath.Initialise ( 12, 12 );

					// Clip the tile path to the drawing path to ensure that it fits into the
					// shape being rendered.
					PathToDraw->ClipPathToPath ( TilePath, &ClippedPath, 2, ClipTolerance,
												 ClipFlatness, ClipFlatness);

					// Set the IsFilled flag.
					ClippedPath.IsFilled = TRUE;

					// And render the path.
					RenderPath ( &ClippedPath );
				}
			}
		}

		// This comes at the end of the rendering loop.
		if ( ( RenderMethod == GF_USE_GDIEOR ) && ( RenderCount > 1 ) )
		{
			// Render the path in white
			SetDrawingMode	( DM_COPYPEN );
			SetFillColour	( COLOUR_BLACK );
			SetLineColour	( COLOUR_TRANS );
			GetValidBrush	();
			GetValidPen		();
			RenderPath		( PathToDraw );

			// Set back to mode required for grad fill rendering
			SetDrawingMode	( DM_EORDITHER );
		}

		RenderCount--;
	}

	// Restore the original rendering state.
	RestoreContext	();
	GetValidBrush	();
	GetValidPen		();

	// Lose the clipping region if we are using one.
	if ( RenderMethod == GF_USE_GDICLIPPING )
		RenderDC->SelectClipRgn ( OSClipRegion );
*/
	// Success.
	return TRUE;
}

BOOL OSRenderRegion::RawRenderPath32( Path *const DrawPath )
{
	PORTNOTETRACE("other","OSRenderRegion::RawRenderPath32 - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	BOOL Worked;

	// Find out some details about the path and fail if it is not valid
	INT32 NumCoords = DrawPath->GetNumCoords();
	ENSURE(NumCoords>0, "Tried to draw a path with no elements in it in OSRenderRegion::RawRenderPath32");
	if (NumCoords==0)
		return FALSE;

	// Find out about the coords
	DocCoord* Coords = DrawPath->GetCoordArray();

	// ideally this would be new POINT[NumCoords] but it can't be because that throws an exception
	POINT* NTCoords = (POINT*)CCMalloc( sizeof(POINT)*NumCoords );

	if (NTCoords==NULL)
	{
		TRACE( _T("No memory to convert curve\n") );
		return FALSE;
	}

	// Convert the points to windows coords
	for (INT32 i=0; i<NumCoords; i++)
		NTCoords[i] = DocCoordToWin(Coords[i]);

	// render them
	Worked = BeginPath( RenderDC );
	if (Worked)
	{
		Worked = NewPolyDraw( NTCoords, DrawPath->GetVerbArray(), NumCoords );
		if (Worked)
			Worked = EndPath( RenderDC );
	}

	CCFree( NTCoords );

	if (!Worked)
		TRACE( _T("Raw render32 failed"));

	return Worked;
#else
	ENSURE(FALSE, "Raw32 cannot work on win16");
	return FALSE;
#endif
}


/********************************************************************************************

>	void OSRenderRegion::RenderPath32( Path *DrawPath )

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/93
	Purpose:	Renders a path using Win32 GDI. Causes an ENSURE on Win16 as should never be
				called.
	Scope:		Private

********************************************************************************************/

void OSRenderRegion::RenderPath32( Path *DrawPath )
{
	PORTNOTETRACE("other","OSRenderRegion::RenderPath32 - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	const wxDC* dc = RenderDC;

	// Flags to indicate whether fill-providers or stroke-providers have filled or stroked
	// the path yet.
	BOOL ExtendedFill = FALSE;

	// If this is not a simple fill, set the flag to indicate this.
	FillGeometryAttribute *pFillProvider = 
		(FillGeometryAttribute *) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;

	if (pFillProvider->GetRuntimeClass() != CC_RUNTIME_CLASS(FlatFillAttribute))
		ExtendedFill = TRUE;

	// low quality displays can't do fancy fills
	if (RRQuality.GetFillQuality() < Quality::Graduated)
		ExtendedFill = FALSE;

	//
	// We ought to check for stroke-providers in here, when the great day comes that
	// we actually have them...
	//

	// Get the fill provider to fill the path - if it can't handle this sort of render
	// region, then we fill the path for it.
	if (ExtendedFill)
	{
		if (!pFillProvider->RenderFill(this, DrawPath))
			// Unable to render fill for this render region - default to simple fill.
			ExtendedFill = FALSE;
	}

	// actually do the rendering
	BOOL Result = RawRenderPath32( DrawPath );

	if (!Result)
	{
		TRACE( _T("RawRenderPath failed"));
		return;
	}

	// Draw it either filled or not.
	// NB. If the fill-provider has already filled this path then we don't bother.
	if (DrawPath->IsFilled && !ExtendedFill)
		StrokeAndFillPath( dc );
	else
		StrokePath( dc );

#endif
}



/********************************************************************************************

>	virtual void OSRenderRegion::GetRenderRegionCaps(RRCaps* pCaps)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95
	Outputs:	pCaps - the render region marks all the things it can not do into the pCaps
				param
	Purpose:	This function is used to help determine what an individual render region
				is capable of. A render can mark the things it can not render (eg transparancy)
				so that a more complex rendering of these parts of the document can be done.

********************************************************************************************/

void OSRenderRegion::GetRenderRegionCaps(RRCaps* pCaps)
{
	// Start off by saying I can't do anything
	pCaps->CanDoNothing();

	// but actually I can do these things
	pCaps->GradFills = TRUE;
	pCaps->LineAttrs = TRUE;
	pCaps->ArrowHeads = TRUE;
	pCaps->DashPatterns = TRUE;

	// We only try simple bitmaps with GDI if printing
	pCaps->SimpleBitmaps = IsPrinting();

//	pCaps->CanDoAll();
}



/********************************************************************************************

>	SlowJobResult OSRenderRegion::DrawMaskedBitmap(const DocRect &Rect, KernelBitmap* pBitmap, 
								  MaskedRenderRegion* pMask, Progress *Progress)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		Point - the position to plot the bitmap
				pBitmap - The bitmap that needs plotting
				pMask - The mask render region to use to indicate which bits of pBitmap
				needs to be plotted
	Purpose:	Plots the bitmap using the mask supplied.

********************************************************************************************/

SlowJobResult OSRenderRegion::DrawMaskedBitmap(const DocRect &Rect, KernelBitmap* pBitmap, 
								  	  MaskedRenderRegion* pMask, ProgressDisplay *Progress)
{
	PORTNOTETRACE("other","OSRenderRegion::DrawMaskedBitmap - do nothing");
#if !defined(STANDALONE) && !defined(EXCLUDE_FROM_XARALX)
	if (RenderView->GetColourPlate() != NULL &&
		!RenderView->GetColourPlate()->IsDisabled())
	{
		// We're colour separating. Indirect this call to the separation variant of this code
		// (see below)
		return(DrawSeparatedMaskedBitmap(Rect, pBitmap, pMask, Progress));
	}


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
	WinBitmap *WinBM = (WinBitmap*)pBitmap->ActualBitmap;

	// Is it valid ?
	if ((WinBM->BMInfo==NULL) || (WinBM->BMBytes==NULL))
		return SLOWJOB_FAILURE;

	// Remember the Size of the Bitmap (in pixels)
	INT32 Width  = WinBM->GetWidth();
	INT32 Height = WinBM->GetHeight();

	if ((Width == 0) || (Height == 0))
		// Error - bitmap has no dimension
		return SLOWJOB_FAILURE;

	// Convert the bitmap from a 32bpp bitmap to a 24bpp bitmap
	INT32 BitmapDepth = WinBM->GetBPP();
	if (BitmapDepth == 32)
	{
		// Can't plot 32bpp bitmaps to GDI as 16-bit GDI doesn't understand them,
		// so we convert to 24bpp bitmap in-situ and render that...

		// How many bytes to a source scanline?
		const INT32 ScanlineBytes = WinBM->GetScanlineSize();

		// How many bytes to a destination scanline
		const INT32 DestlineBytes = DIBUtil::ScanlineSize(Width, 24);

		// Now convert the bitmap in-situ
		LPBYTE OriginalBuffer  = WinBM->BMBytes;
		LPBYTE ConvertedBuffer = WinBM->BMBytes;

		for (INT32 i=0; i<Height; i++)
		{
			DIBUtil::Convert32to24(Width, OriginalBuffer, ConvertedBuffer);
			OriginalBuffer += ScanlineBytes;
			ConvertedBuffer += DestlineBytes;
		}

		// Update bitmap info to show it is now a 24bpp bitmap...
		WinBM->BMInfo->bmiHeader.biSizeImage = DestlineBytes * Height;
		WinBM->BMInfo->bmiHeader.biBitCount  = 24;
		BitmapDepth = 24;		
	}

	// make sure we have a 24bpp bitmap
	ERROR3IF(BitmapDepth!=24, "Non 24bpp bitmap found in DrawMaskedBitmap");

	// Work out the coords to blit to, taking into acount the differnt dpis of the
	// source and destination bitmaps.
	INT32 SrcDpi = pMask->FindMaskDpi();
	INT32 DestDpi = SrcDpi;
	if (RenderDC!=NULL)
		DestDpi = RenderDC->GetDeviceCaps(LOGPIXELSY);

	// Work out the ratio between to two
	double Ratio = DestDpi;
	Ratio = Ratio/SrcDpi;

	// Convert the DocCoord into a windows coord
	DocRect ClipRect = pMask->GetClipRect();
	WinRect WinClipRect = DocRectToWin(RenderMatrix, ClipRect, DestDpi);
	POINT Origin;
	Origin.x = WinClipRect.left;
	Origin.y = WinClipRect.bottom;

	// Inform progress object how high this band is
	if (PrintMonitor::PrintMaskType==PrintMonitor::MASK_MASKED)
	{
		if (Progress!=NULL)
			Progress->StartBitmapPhaseBand(Height);
	}

	// We need to create a tempory bitmap that we use to render each scan line
	LPBITMAPINFO	TempBitmapInfo = NULL;
	LPBYTE			TempBitmapBytes = NULL;

	// Get some memory for a tempory bmp 1 pixel high
	TempBitmapInfo = AllocDIB(Width, 1, BitmapDepth, &TempBitmapBytes);
	if (TempBitmapInfo==NULL)
		return SLOWJOB_FAILURE;

	// How many bytes to a destination scanline
	INT32 ScanLineBytes = DIBUtil::ScanlineSize(Width, BitmapDepth);
	INT32  BytesPerPixel = 3;

	// Now convert the bitmap in-situ
	LPBYTE SrcBuffer  = WinBM->BMBytes;
	LPBYTE DestBuffer = TempBitmapBytes;
	
	// Set the blit mode
	INT32 OldMode = SetStretchBltMode(RenderDC->GetSafeHdc(), HALFTONE);//COLORONCOLOR);

	// Must call SetBrushOrgEx() after setting the stretchblt mode 
	// to HALFTONE (see Win32 SDK docs).
	POINT OldOrg;
	SetBrushOrgEx(RenderDC->m_hDC, 0, 0, &OldOrg);

	// Now copy the Bits from our Kernel Bitmap into the Memory DC - either we can
	// do this as a masked blit (a scan line at a time), or in one go.
	// We do the masked blit under NT, or if the preference has been over-ridden by the user.
	if (PrintMonitor::PrintMaskType!=PrintMonitor::MASK_SIMPLE)
	{
		MaskRegion MaskInfo;
		pMask->GetFirstMaskRegion(&MaskInfo);
		while (MaskInfo.Length!=0)
		{
			// Calculate the source buffer address from the x and y position
			SrcBuffer = WinBM->BMBytes;
			SrcBuffer += ScanLineBytes * MaskInfo.y;
			SrcBuffer += MaskInfo.x*BytesPerPixel;
			INT32 RegionWidth = MaskInfo.Length;

			// Update bitmap info to show the new scan line size
			TempBitmapInfo->bmiHeader.biWidth = RegionWidth;
			TempBitmapInfo->bmiHeader.biSizeImage = (RegionWidth*BytesPerPixel);

			memcpy(DestBuffer, SrcBuffer, RegionWidth*BytesPerPixel);

			// Work out the coords of the destination rectangle
			INT32 DestX = Origin.x + INT32(ceil(MaskInfo.x*Ratio));
			INT32 DestY = Origin.y - INT32(ceil((MaskInfo.y+1)*Ratio));
			INT32 DestWidth = INT32(ceil(RegionWidth*Ratio));
			INT32 DestHeight = INT32(ceil(1*Ratio));
		
			// Blit the data to the screen
			StretchDIBits(	RenderDC->GetSafeHdc(),
							DestX, DestY, DestWidth, DestHeight,
							0, 0,
							RegionWidth, 1,
							TempBitmapBytes, TempBitmapInfo,
							DIB_RGB_COLORS, SRCCOPY);

			// Update the progress display if necessary.
			if (PrintMonitor::PrintMaskType==PrintMonitor::MASK_MASKED)
			{
				if ((Progress!=NULL) && (!Progress->BitmapPhaseBandRenderedTo(MaskInfo.y)))
				{
					// Put the blit mode back as it was
					SetStretchBltMode(RenderDC->GetSafeHdc(), OldMode);
					SetBrushOrgEx(RenderDC->m_hDC, OldOrg.x, OldOrg.y, NULL);

					// Free up the tempory DIB I made
					FreeDIB(TempBitmapInfo, TempBitmapBytes);

					return SLOWJOB_USERABORT;
				}
			}

			// Find the next bit of scan line to plot
			pMask->GetNextMaskRegion(&MaskInfo);
		}
	}
	else
	{
		// Blit in one go...

		// Work out the coords of the destination rectangle
		INT32 DestX = Origin.x;
		INT32 DestY = Origin.y - INT32(ceil(Height * Ratio));
		INT32 DestWidth = INT32(ceil(Width * Ratio));
		INT32 DestHeight = INT32(ceil(Height * Ratio));

		// Blit the data to the screen
		StretchDIBits(	RenderDC->GetSafeHdc(),
						DestX, DestY, DestWidth, DestHeight,
						0, 0,
						Width, Height,
						WinBM->BMBytes, WinBM->BMInfo,
						DIB_RGB_COLORS, SRCCOPY);

		// Update the progress display if necessary.
		if ((Progress!=NULL) && (!Progress->BitmapPhaseBandRenderedTo(Height)))
		{
			// Put the blit mode back as it was
			SetStretchBltMode(RenderDC->GetSafeHdc(), OldMode);
			SetBrushOrgEx(RenderDC->m_hDC, OldOrg.x, OldOrg.y, NULL);

			// Free up the tempory DIB I made
			FreeDIB(TempBitmapInfo, TempBitmapBytes);

			return SLOWJOB_USERABORT;
		}
	}

	// Put the blit mode back as it was
	SetStretchBltMode(RenderDC->GetSafeHdc(), OldMode);
	SetBrushOrgEx(RenderDC->m_hDC, OldOrg.x, OldOrg.y, NULL);

	// Update bitmap info to what it was before we started
	TempBitmapInfo->bmiHeader.biSizeImage = (Width*3);
    TempBitmapInfo->bmiHeader.biWidth = Width;

	// Free up the tempory DIB I made
	FreeDIB(TempBitmapInfo, TempBitmapBytes);

#endif

	// All ok
	return SLOWJOB_SUCCESS;
}




/********************************************************************************************

>	SlowJobResult OSRenderRegion::DrawSeparatedMaskedBitmap(const DocRect &Rect, KernelBitmap* pBitmap, 
								  MaskedRenderRegion* pMask, Progress *Progress)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/96
	Inputs:		Point - the position to plot the bitmap
				pBitmap - The bitmap that needs plotting
				pMask - The mask render region to use to indicate which bits of pBitmap
				needs to be plotted
				Progress - The progress object to be kept informed of progress

	Purpose:	Plots the bitmap using the mask supplied.
				The bitmap is colour separated according to the current ColourPlate settings,
				and output (a scanline at a time) in an 8bpp greyscale format.

	Notes:		This function is autoamtically called by DrawMaskedBitmap() if it is called
				while we are rendering a colour separation plate. It is essentially a copy
				of DrawMaskedBitmap which has been rejigged to colour separate the bitmap.

********************************************************************************************/

SlowJobResult OSRenderRegion::DrawSeparatedMaskedBitmap(const DocRect &Rect, KernelBitmap* pBitmap, 
								  	  MaskedRenderRegion* pMask, ProgressDisplay *Progress)
{
	PORTNOTETRACE("other","OSRenderRegion::DrawSeparatedMaskedBitmap - do nothing");
#ifndef EXCLUDE_FROM_XARALX
#ifndef STANDALONE
	SlowJobResult Result = SLOWJOB_SUCCESS;
	
	// Make sure the world is in one piece
	if ((pBitmap==NULL) || (pMask==NULL))
		return SLOWJOB_FAILURE;

	// If mask coverage is 0% we do not need to do anything. The TRUE param indicates
	// that we don't care about exact coverage, which makes it return as soon as it realises
	// that there are pixel(s) that we'll have to plot
	if (pMask->FindCoverage(TRUE) == 0)
		return SLOWJOB_SUCCESS;

	// Get the 'Actual' windows Bitmap
	WinBitmap *WinBM = (WinBitmap*)pBitmap->ActualBitmap;

	// Is it valid ?
	if (WinBM == NULL || WinBM->BMInfo == NULL || WinBM->BMBytes == NULL)
		return SLOWJOB_FAILURE;

	// We're colour separating, so make sure we've got some sep tables to work with.
	// We cache them once we've used them. I would do this sort of set-up in StartRender
	// but of course all the derived classes just override that behaviour and we're stuffed
	if (SepTables == NULL && RenderView->GetColourPlate() != NULL)
	{
		ColourContextCMYK *cc = (ColourContextCMYK *)RenderView->GetColourContext(COLOURMODEL_CMYK);
		if (cc != NULL)
		{
			SepTables = (BYTE *) CCMalloc(5 * 256 * sizeof(BYTE));
			if (SepTables != NULL)
			{
				if (!cc->GetProfileTables(SepTables))
				{
					CCFree(SepTables);
					SepTables = NULL;
				}
			}
		}

		ERROR3IF(SepTables == NULL, "Can't generate separation tables in OSRenderRegion");
		if (SepTables == NULL)
			return(SLOWJOB_FAILURE);
	}


	// Remember the Size of the Bitmap (in pixels)
	INT32 Width  = WinBM->GetWidth();
	INT32 Height = WinBM->GetHeight();
	INT32 BitmapDepth = WinBM->GetBPP();

	if (Width == 0 || Height == 0)
		return SLOWJOB_FAILURE;

	// Work out the coords to blit to, taking into acount the differnt dpis of the
	// source and destination bitmaps.
	INT32 SrcDpi = pMask->FindMaskDpi();
	INT32 DestDpi = SrcDpi;
	if (RenderDC!=NULL)
		DestDpi = RenderDC->GetDeviceCaps(LOGPIXELSY);

	// Work out the ratio between to two
	double Ratio = DestDpi;
	Ratio = Ratio/SrcDpi;

	// Convert the DocCoord into a windows coord
	DocRect ClipRect = pMask->GetClipRect();
	WinRect WinClipRect = DocRectToWin(RenderMatrix, ClipRect, DestDpi);
	POINT Origin;
	Origin.x = WinClipRect.left;
	Origin.y = WinClipRect.bottom;

	// Inform progress object how high this band is
	if (PrintMonitor::PrintMaskType==PrintMonitor::MASK_MASKED)
	{
		if (Progress!=NULL)
			Progress->StartBitmapPhaseBand(Height);
	}

	// Allocate a 32bpp scanline buffer
	Pixel32bpp *pScanline = (Pixel32bpp *) CCMalloc(Width * sizeof(Pixel32bpp));
	if (pScanline == NULL)
		return(SLOWJOB_FAILURE);

	// Create a bitmap header structure to use with our temporary scanline. As all output data
	// will be 8bpp, we set the header up as an 8bpp bitmap.
	LPBITMAPINFO pScanlineInfo = AllocDIB(Width, 1, 8, NULL);
	if (pScanlineInfo == NULL)
		return SLOWJOB_FAILURE;

	// Set up a simple greyscale palette for the bitmap
	for (INT32 i = 0; i < 256; i++)
	{
		pScanlineInfo->bmiColors[i].rgbRed = 
			pScanlineInfo->bmiColors[i].rgbGreen = 
				pScanlineInfo->bmiColors[i].rgbBlue = i;
		pScanlineInfo->bmiColors[i].rgbReserved = 0;
	}

	// Set the blit mode
	INT32 OldMode = SetStretchBltMode(RenderDC->GetSafeHdc(), HALFTONE);//COLORONCOLOR);

	// Must call SetBrushOrgEx() after setting the stretchblt mode 
	// to HALFTONE (see Win32 SDK docs).
	POINT OldOrg;
	SetBrushOrgEx(RenderDC->m_hDC, 0, 0, &OldOrg);

	ColourContext *OutputContext = RenderView->GetColourContext(COLOURMODEL_RGBT);
	ERROR3IF(OutputContext == NULL, "Where's me RGB colour context gone then?");

	// Now copy the Bits from our Kernel Bitmap into the Memory DC
	// as a masked blit (a scan line at a time, so we can colour separate it)
	MaskRegion MaskInfo;
	pMask->GetFirstMaskRegion(&MaskInfo);
	while (MaskInfo.Length != 0)
	{
		// Update bitmap info to show the new scan line size
		pScanlineInfo->bmiHeader.biWidth	 = MaskInfo.Length;
		pScanlineInfo->bmiHeader.biSizeImage = MaskInfo.Length * sizeof(BYTE);

		// Read out the scanline into our pScanline buffer, converting it to a generic
		// 32bpp format as we go. This is 
		WinBM->GetScanline32bpp(MaskInfo.y, TRUE, pScanline);

		// Now colour separate it. We only bother separating the unmasked portion of the scanline
		// which we place back in the left end of the scanline
		WinBM->ColourSeparateScanline32to8(OutputContext, SepTables,
											(BYTE *) pScanline,		// Output buffer (shared!)
											pScanline + MaskInfo.x,	// Input buffer - !pointer arithmetic!
											MaskInfo.Length);

		// Work out the coords of the destination rectangle
		const INT32 DestX = Origin.x + INT32(ceil(MaskInfo.x * Ratio));
		const INT32 DestY = Origin.y - INT32(ceil((MaskInfo.y + 1) * Ratio));
		const INT32 DestWidth = INT32(ceil(MaskInfo.Length * Ratio));
		const INT32 DestHeight = INT32(ceil(1 * Ratio));
	
		// Blit the data to the screen
		StretchDIBits(	RenderDC->GetSafeHdc(),
						DestX, DestY, DestWidth, DestHeight,
						0, 0,
						MaskInfo.Length, 1,
						(BYTE *) pScanline, pScanlineInfo,
						DIB_RGB_COLORS, SRCCOPY);

		// Update the progress display if necessary.
		if (PrintMonitor::PrintMaskType == PrintMonitor::MASK_MASKED &&
			Progress != NULL && !Progress->BitmapPhaseBandRenderedTo(MaskInfo.y))
		{
			Result = SLOWJOB_USERABORT;
			break;		// User aborted, so quit rendering, and return USERABORT
		}

		// Find the next bit of scan line to plot
		pMask->GetNextMaskRegion(&MaskInfo);
	}

	// Put the blit mode back as it was
	SetStretchBltMode(RenderDC->GetSafeHdc(), OldMode);
	SetBrushOrgEx(RenderDC->m_hDC, OldOrg.x, OldOrg.y, NULL);

	// Free up the tempory DIB I made
	FreeDIB(pScanlineInfo, NULL);
	CCFree(pScanline);

	return(Result);
#else
	return SLOWJOB_FAILURE;
#endif
#else
	return SLOWJOB_SUCCESS;
#endif
}



/********************************************************************************************

>	BOOL OSRenderRegion::RenderChar(WCHAR ch, Matrix* pMatrix)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/95
	Inputs:		ch      - unicode value of char
				pMatrix - matrix specifying transforms to place char correctly in document
	Returns:	FALSE if fails
	Purpose:	Render a character,
				using the specified transform and current attributes in the render region.

********************************************************************************************/

BOOL OSRenderRegion::RenderChar(WCHAR ch, Matrix* pMatrix)
{
	PORTNOTETRACE("text","OSRenderRegion::RenderChar - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// If it is stroked or not simple flat fill, or not a standard ASCII character then we 
	//	must do this as paths. Graham 8/8/96: And now UNICODE works too.
	BOOL FlatFill = IS_A(CurrentAttrs[ATTR_FILLGEOMETRY].pAttr, FlatFillAttribute);

	if (!FlatFill || 
		!RR_STROKECOLOUR().IsTransparent() || 
		(FlatFill & RR_FILLCOLOUR().IsTransparent()))
		return RenderRegion::RenderChar(ch, pMatrix);

	if (IsPrinting())
	{
		// Check for emulsion down printing, GDI cannot render text backwards
		PrintControl *pPrintCtl;
		View *pView = GetRenderView();
		if (pView && (pPrintCtl=pView->GetPrintControl())) 
		{ 
			if (pPrintCtl->GetTypesetInfo()->PrintEmulsionDown())
			return RenderRegion::RenderChar(ch, pMatrix);
		}
	}

	// get overall matrix - attribute matrix concatenated with given matrix if supplied
	Matrix matrix;
	if (GetCharAttributeMatrix(&matrix)==FALSE)
		return NULL;
	if (pMatrix)
		matrix*=*pMatrix;

	// Can we do this using a GDI font?
	// We can if the matrix only specifies scaling and translation
	FIXED16 abcd[4];
	INT32	ef[2];
	matrix.GetComponents(abcd, ef);

	// GDI can't do y-axis flips, so we do it as shapes if this is detected (and x-axis
	// flips, for consistency).
	if ((abcd[0] < FIXED16(0)) || (abcd[3] < FIXED16(0)))
	{
		// Flipped in one or both axes - render as a path.
		return RenderRegion::RenderChar(ch, pMatrix);
	}

	// Work out how complex the transformation is.
	FIXED16 ScaleX = 0;
	FIXED16 ScaleY = 0;
	ANGLE 	Rotation = 0;
	ANGLE	Shear = 0;

	if ((abcd[1] == FIXED16(0)) && (abcd[2] == FIXED16(0)) && 
		(abcd[0] >= FIXED16(0)) && (abcd[3] >= FIXED16(0)))
	{
		// Simple scaling transformation.
		ScaleX 	 = abcd[0];
		ScaleY   = abcd[3];
		Rotation = FIXED16(0);
		Shear 	 = FIXED16(0);
	}
	else
	{
		// Decompose the matrix to find out how complex it is.
		// Pass in NULL for translation as we already know it is in 'ef'.
		FIXED16 Aspect;
		BOOL Result = matrix.Decompose(&ScaleY, &Aspect, &Rotation, &Shear, NULL);

		if (!Result || (Shear != FIXED16(0)))
			// Either there was a problem, or the character is sheared, in which case
			// we can't do it with GDI.
			return RenderRegion::RenderChar(ch, pMatrix);

		// Set up the ScaleX based on the aspect ratio
		ScaleX = ScaleY * Aspect;
	}

	// Check for sideways printing - if the render matrix has rotation, then we are
	// printing at 270 degrees rotation, so adjust the rotation accordingly.
	FIXED16 RenderABCD[4];
	INT32	RenderEF[2];
	RenderMatrix.GetComponents(RenderABCD, RenderEF);
	if ((RenderABCD[1] != FIXED16(0)) || (RenderABCD[2] != FIXED16(0)))
		// Rotate by 270 degrees (angle is in radians)
		Rotation += FIXED16(1.5 * PI);

	// Work out required width and height of the font
	MILLIPOINT ReferenceSize = TextManager::GetDefaultHeight();
	MILLIPOINT Width  = ReferenceSize * ScaleX;
	MILLIPOINT Height = ReferenceSize * ScaleY;

	if (!SelectNewFont(RR_TXTFONTTYPEFACE(), RR_TXTBOLD(), RR_TXTITALIC(),
					   Width, Height, Rotation))
	{
		// Could not select font (maybe because device can't rotate fonts)
		return RenderRegion::RenderChar(ch, pMatrix);
	}

	// First, set up the text attributes that are not encoded in the font.
	UINT32 OldTextAlign = RenderDC->SetTextAlign(TA_BASELINE);
	INT32 OldBKMode = RenderDC->SetBkMode(TRANSPARENT);
	COLORREF OldTextColor = 
		RenderDC->SetTextColor(ConvertColourToScreenWord(CurrentColContext, &RR_FILLCOLOUR()));

	// Render the character in the specified position
	DocCoord DocPos(ef[0], ef[1]);
	WinCoord WinPos = DocCoordToWin(DocPos);


	// Graham 5/8/96: "ch" is presently in UNICODE or ASCII
	// We need to convert it over to MBCS to deal with Japanese strings
	//So convert ch, which is of form WCHAR, over to a MBCS UINT32 character index

	UINT32 uiCharNumber = UnicodeManager::UnicodeToMultiByte(ch);

	//Now we want to put that UINT32 value into an array of char ready to pass to
	//RenderDC->TextOut. We do this using UnicodeManager::DecomposeMultiBytes

	BYTE bCharArray[2];

	UnicodeManager::DecomposeMultiBytes(uiCharNumber, &bCharArray[0], &bCharArray[1]);

	//Now, is the character in bCharArray one or two bytes long?
	//If it is one byte long, the first byte in bCharArray will be zero.
	if (bCharArray[0]==0)
		//It's a standard ASCII character, one byte long
		//So pass that character (bCharArray[1]) to the TextOut function.
		//The last parameter in text out is the number of bytes - in this case 1.
		RenderDC->TextOut(WinPos.x, WinPos.y, (CHAR*) &bCharArray[1], 1);
	else
		//The character is two bytes long (that is, it's a foreign character)
		//So we pass bCharArray[0] to TextOut and tell TextOut that it should
		//use two bytes from that address. We do this by setting the last
		//parameter to 2.
		RenderDC->TextOut(WinPos.x, WinPos.y, (CHAR*) &bCharArray[0], 2);

	// Clean up text attributes
	RenderDC->SetTextAlign(OldTextAlign);
	RenderDC->SetBkMode(OldBKMode);
	RenderDC->SetTextColor(OldTextColor);
#elif !defined(DISABLE_TEXT_RENDERING)
	return RenderRegion::RenderChar(ch, pMatrix);
#endif
	return TRUE;
}




/********************************************************************************************

>	PaperRenderRegion::PaperRenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/06/94
	Inputs:		As OSRenderRegion's constructor.
	Purpose:	Construct a PaperRenderRegion - this is a simple derivation from
				OSRenderRegion's constructor.
	SeeAlso:	OSRenderRegion; OSRenderRegion::OSRenderRegion

********************************************************************************************/

PaperRenderRegion::PaperRenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale)
	: OSRenderRegion(ClipRect, ConvertMatrix, ViewScale)
{
}


/********************************************************************************************

>	PaperRenderRegion::~PaperRenderRegion()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/06/94
	Purpose:	Perform any closedown necessary for the paper render region.
	SeeAlso:	OSRenderRegion

********************************************************************************************/

PaperRenderRegion::~PaperRenderRegion()
{
}

/********************************************************************************************

>	BOOL PaperRenderRegion::AttachDevice(CWnd *pWnd, Spread *pSpread, CDC *pDC, 
									 Matrix& ViewMatrix, FIXED16 ViewScale, 
									 DocRect& ClipRect)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/06/94
	Inputs:		pView - the window to attach to.
				pSpread - the spread to attach to.
				pDC - the device context to render into
				ViewMatrix - the matrix needed to render this spread into this DC.
				ViewScale - the scale of the view to be rendered.
				ClipRect - the clip rectangle of the region.
	Returns:	TRUE if attached ok; FALSE if not.
	Purpose:	Change the device attached to the PaperRenderRegion.
	SeeAlso:	OSRenderRegion::AttachDevice

********************************************************************************************/

BOOL PaperRenderRegion::AttachDevice(View *pView, Spread *pSpread, wxDC* pDC, 
									 Matrix& ViewMatrix, FIXED16 ViewScale, 
									 DocRect& ClipRect, bool fOwned /*= false*/)
{
	if (!OSRenderRegion::AttachDevice(pView, pDC, pSpread))
		return(FALSE);

	// Update basic rendering variables for new device.
	// (These are normally set up the the constructor).
	CurrentClipRect = ClipRect;
	RenderMatrix = ViewMatrix;
	ScaleFactor = ViewScale;

	// Make sure we get rid of all attributes at Detach time - see DetachDevice() below.
	SaveContext();

	// Call base class to attach device.
//	return OSRenderRegion::AttachDevice(pView, pDC, pSpread);
	return(TRUE);
}

/********************************************************************************************

>	void PaperRenderRegion::DetachDevice()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/06/94
	Purpose:	Detach the render region form its device.  This is done so that we don't
				try to use the DC etc. after it has been deleted.
	SeeAlso:	PaperRenderRegion::AttachDevice

********************************************************************************************/

void PaperRenderRegion::DetachDevice()
{
	// Lose all the attributes on our rendering stack
	RestoreContext();

	// Lose our current attribute block.
	CCFree(CurrentAttrs);
	CurrentAttrs = NULL;

	// Make sure we don't try to use this device again.
	RenderDC = NULL;
	RenderView = NULL;
	RenderSpread = NULL;
	RenderFlags.ValidDevice = FALSE;		// make sure InitDevice is called subsequently
											// else GBrushes tend not to work
}


/********************************************************************************************

>	BOOL PaperRenderRegion::InitDevice()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/95

	Returns:	TRUE if the device context is initialised ok;
				FALSE if not.

	Purpose:	Initialise the device specific mechanisms for this render region.

	Notes:		We use a single static PaperRenderRegion to save creating one all the
				time. However, this means that the check to see if we should use the
				MainFrame window's palette only occurs once (in Create()), and unfortunately
				this means that we always have the palette disabled due to the point at
				which the original Create happens to occur in startup.

				Thus, this InitDevice has been overridden to check if we should use a palette
				*every* time the RndRgn is initialised for a redraw, so that it will use the
				palette in the same way as the render regions which are created when needed.

	Errors:		Same as base class.
	SeeAlso:	OSRenderRegion::InitDevice

********************************************************************************************/

BOOL PaperRenderRegion::InitDevice()
{
	// As we use a static PaperRenderRegion, it has to check every time we go to use it if
	// we should be using a palette - otherwise, we get the wrong idea when we're Create'd
	// and then we fail to use the palette forever onwards!
	RFlags.UsePalette = (WantGDIPalette && PaletteManager::UsePalette()) ? TRUE : FALSE;

	// Call base class
	if (!OSRenderRegion::InitDevice())
		return FALSE;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	static wxSize OSRenderRegion::GetFixedDCPPI(wxDC &DC)

	Author:		Alex Bligh
	Created:	07/01/2006

	Returns:	DPI in both directions as a wxSize

	Purpose:	Fix up crazy wxWindows screen DPI calculation

	Notes:		wxWindows REALLY tries to calculate the screen DPI. By that I mean it
				takes the width and depth of the screen (as specified somehow to gdk by
				the user) in millimeters, and then divides by the pixels. Camelot needs
				a fixed view of the world, so on any system 100% is the same number of
				pixels. We use a FIXED 96dpi on Windows & Linux and, presently, on the Mac
				too so that documents look identical on all platforms (render same area
				of pixels so that comparative timings make sense).

	Errors:		-
	SeeAlso:	wxDC::GetPPI

********************************************************************************************/

wxSize OSRenderRegion::GetFixedDCPPI(wxDC &DC)
{
	// Set system defaults
PORTNOTE("MacPort", "We should allow the user to configure this value on the mac, maybe on all platforms")
#ifdef __WXMAC__
	wxSize PPI(96,96);
#else
	wxSize PPI(96,96);
#endif

	// If it's not a Screen DC we MIGHT just believe it!	
	if (!DC.IsKindOf(CLASSINFO(wxScreenDC)))
		PPI=DC.GetPPI();

	return PPI;
}
