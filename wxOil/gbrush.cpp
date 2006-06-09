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

// class to use raw to obtain GDI brush patterns


#include "camtypes.h"
#include "gbrush.h"
//#include "ensure.h"
#include "devcolor.h"
//#include "doccolor.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "grndrgn.h"
//#include "app.h"
#include "gbrushop.h"
//#include "andy.h"
//#include "docview.h"
//#include "ccolbar.h"
#include "gdrawcon.h"
#include "gdraw.h"
//#include "dibutil.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "palman.h"

CC_IMPLEMENT_MEMDUMP(GBrush, ListItem)
CC_IMPLEMENT_DYNCREATE(OpGBrush, Operation)
#define new CAM_DEBUG_NEW


List			GBrush::BrushList;	   				// list of all active brushes
GBrush			*GBrush::Current;	   				// current brush, or NULL
static BOOL WantBetterBrushes = TRUE;				// TRUE to use these, FALSE to not


/********************************************************************************************

	Preference:	BetterBrushes
	Section:	Screen
	Range:		0 or 1
	Purpose:	Allows the user to determine whether GDraw brushes are used (1) or normal
				GDI brushes (0). Defaults to 1 because it produces a much nicer colour bar.

********************************************************************************************/

/********************************************************************************************

>	BOOL GBrush::InitGBrush( BOOL FirstTime )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		FirstTime is TRUE if called from starup, FALSE if called subsequently (e.g.
				after screen mode change).
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed (e.g. no memory). Returns TRUE if not
				required too (e.g. incompatible screen depth) i.e. FALSE is an error which
				could be serious.
	Purpose:	GBuilds the pretend bitmap which ise used to fool GDraw into suppyling us
				with brushes. Called from GRenderRegion::Init and relies on that being called
				first.
	SeeAlso:	GBrush::Init
	Errors:		-
	Scope:		Static

********************************************************************************************/

BOOL GBrush::InitGBrush(BOOL FirstTime)
{
	if (FirstTime && Camelot.DeclareSection( wxT("Screen"), 10))
	{
		Camelot.DeclarePref(NULL, wxT("BetterBrushes"), &WantBetterBrushes, FALSE, TRUE );
	}
	// Register the (debug) Op to enable/disable GBrush
	if (!Operation::RegisterOpDescriptor(
						0,
						_R(IDS_GBRUSH),
						CC_RUNTIME_CLASS(OpGBrush),
						OPTOKEN_GBRUSH,
						OpGBrush::GetState,
						0,	/* help ID */
						0, //_R(IDBBL_ANTIALIASOP),
						0	/* bitmap ID */
						))
	{
		TRACE( wxT("RegisterOpDescriptor( OpGBrush ) failed") );
		return FALSE;
	}

	// Ensure that none of the GBrushes are active
	NewBrushState();
	Current = NULL;

	return TRUE;
}



/********************************************************************************************

>	void GBrush::DeinitGBrush(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/9/95

	Purpose:	De-initialises the GBrush pretend bitmap. Called on DeInit of GRenderRegion
				to shut down without a memory leak taking place.

	SeeAlso:	GBrush::InitGBrush

	Errors:		ERROR3 with graceful exit if there are GBrushes still active at the time

	Scope:		Static

********************************************************************************************/

void GBrush::DeinitGBrush(void)
{
#ifdef _DEBUG
	// This was an ERROR3, but our stupid error handler doesn't handle errors very well,
	// (in fact, at this stage in program de-initialisation, it doesn' handle them at all)
	// so it's just a trace warning now.
	if (!BrushList.IsEmpty())
	{
		TRACEALL( wxT("Warning: Deinitialising GBrush when GBrush(es) still active\n\n" ) );
	}
#endif
}


/********************************************************************************************

>	GBrush::GBrush()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GBrush constructor. Call Init before trying to use.
	SeeAlso:	GBrush::Init
	Errors:		-

********************************************************************************************/

GBrush::GBrush()
{
	CanUse = FALSE;
	Valid = FALSE;

	BrushList.AddHead( this );

	UseSolidColours = FALSE;
}



/********************************************************************************************

>	GBrush::~GBrush()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GBrush destructor. Does necessary cleanup.
	Errors:		-

********************************************************************************************/

GBrush::~GBrush()
{
	if (Current == this)
		Current = NULL;

#ifdef _DEBUG
	ListItem *Old = BrushList.RemoveItem( this );
	ERROR3IF( Old == NULL, "GBrush being deleted not in list" );
#else
	BrushList.RemoveItem( this ); // to suppress compiler warning in both debug & retail builds
#endif
}


/********************************************************************************************

>	BOOL GBrush::Init( HDC RefDC )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		RefDC - an HDC of the output device (must be screen currently).
				This must remain a valid DC for the entire length of time that this
				brush is used.
	Outputs:	-
	Returns:	TRUE if GBrush is available for use, FALSE if not.
	Purpose:	Determines whether a GBrush can be used in the given device. Call Start
				before trying to use, assuming a TRUE result.
	Errors:		-

********************************************************************************************/

BOOL GBrush::Init( wxDC* RefDC )
{
	ENSURE( Current==NULL, "Only one current GBrush at a time" );

	Valid = FALSE;											// in case of failure

	ReferenceDC = RefDC;	  								// remember for Start time

	CanUse = TRUE;
	return CanUse;
}



/********************************************************************************************

>	void GBrush::Start()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Call before trying to use the GBrush. There can only be one active GBrush
				in use at a time. Will fail silently if not available.
	Errors:		GBrush::Available

********************************************************************************************/

void GBrush::Start()
{
	ENSURE( Current==NULL, "Only one current GBrush at a time" );

	if (CanUse && WantBetterBrushes)
	{
		GDrawContext *GContext = GRenderRegion::GetStaticDrawContext();

		// We checked this in Init() so it should be OK, but we might as well be safe
		if (GContext != NULL)
		{
			Valid = TRUE;
			Current = this;

			// Get the 16bpp screen hinting value so Gavin can do solid colours properly
			DWORD GavinHint = DIBUtil::GetGavinBlitFormat(GRenderRegion::ScreenDepth,
									32, GRenderRegion::ScreenHinting);

			// And set Gavin up to render into our brush bitmap
			GContext->SetupBitmap( 
				4,4, 
				GRenderRegion::ScreenDepth==24?32:GRenderRegion::ScreenDepth, 
				NULL, 
				GavinHint
			);

			// We want dithering not error diffusion please GDraw
			/*pcLOGPALETTE lpPal =*/ GContext->SelectPalette(0);

			// In 8bpp, make sure GDraw is aware of the current system palette
			if (GRenderRegion::ScreenDepth == 8)
				GRenderRegion::GColInit( ReferenceDC );

			// Attempt to ensure halftone patterns always use the same origin
			GContext->SetHalftoneOrigin(0, 0);
		}
		else
			ERROR3("GDraw Context not available in GBrush::Start()");
	}
}



/********************************************************************************************

>	void GBrush::Stop()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Says 'we have stopped using this for the moment'. Call Start to start again.
				It is not necessary to call this before destruction. Safe to call even if not
				Available.
	Errors:		-

********************************************************************************************/

void GBrush::Stop()
{
	if (Current == this)
		Current = NULL;

	Valid = FALSE;
}



/********************************************************************************************

>	inline void GBrush::GetLogBrushInternal( const COLORREF rgb, LPLOGBRUSH lpBrush )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com> (Jason)
	Created:	5/4/94 (28/6/94)
	Inputs:		rgb - a GDI/GDraw style RGB word defining the colour
	Outputs:	lpBrush is updated to create a suitably brush which can then be passed
				to CreateBrushIndirect.
	Returns:	-
	Purpose:	The whole point of the GBrush class is to get one of these. This is an
				inline macro used by the 2 overloaded GetLogBrush functions so the code
				need only be changed in one place.

	Errors:		Will ENSURE if not Available.
	Notes:		This is fundametally the same as the other overloaded GetLogBrush,
				but for efficiency I've repeated the code in both functions.

				SetSolidColours should be called to enable/disable dithering of this brush

	SeeAlso:	GBrush::Available; GBrush::SetSolidColours

********************************************************************************************/

void GBrush::GetLogBrushInternal( const COLORREF rgb, wxBrush* pBrush )
{
	ERROR3IF( !Valid, "GBrush called when not available");
	ERROR3IF( Current != this, "GBrush not current");

	pBrush->SetStyle(wxSOLID);

	// we let the Widgets handle primary colours cos they should be able to manage it (!)
	if ( ((rgb & 0x0000FF)==0x000000 || (rgb & 0x0000FF)==0x0000FF) &&
		 ((rgb & 0x00FF00)==0x000000 || (rgb & 0x00FF00)==0x00FF00) &&
		 ((rgb & 0xFF0000)==0x000000 || (rgb & 0xFF0000)==0xFF0000) )
	{
		pBrush->SetColour(GetRValue(rgb),GetGValue(rgb),GetBValue(rgb));
		return;
	}

	// We assume the GBrush is defined in the GContext belonging to GRenderRegions
	// ARRRGGHHH Who wrote MakeCurrent() without making it Virtual. What a naughty
	// boy I am (Andy, that is! - Jason ;-)
	GDrawContext *GContext = GRenderRegion::GetStaticDrawContext();
	const BYTE *pGBrush = NULL;

	// This was checked in init/start, but we might as well be safe
	if (GContext != NULL)
	{
		// If the current mode is for solid colours (no dithering) then we ask for solid colour.
		// This is used for things like pasteboards which shouldn't dither for aesthetic (eor!) reasons
		if (UseSolidColours)
			GContext->SetSolidColour(rgb);
		else
			GContext->SetColour(rgb);

		pGBrush = GContext->ReturnBrushRGB();
	}
	else
		ERROR3("GBrush got a NULL GContext!\n");

	if (pGBrush == NULL)
		pBrush->SetColour(GetRValue(rgb),GetGValue(rgb),GetBValue(rgb));
	else if ( UseSolidColours )
		pBrush->SetColour(pGBrush[0],pGBrush[1],pGBrush[2]);
	else
	{
#ifdef __WXGTK__
		// The following line shouldn't be necessary, but wxGTK appears to
		// fail if it doesn't have a valid colour when setting the brush
		// into a device context.
		pBrush->SetColour(0,0,0);
#endif
		wxImage image(4,4,(BYTE*)pGBrush,true);
		wxBitmap bitmap(image);
		pBrush->SetStipple(bitmap);		// Also sets style to wxSTIPPLE
	}
}



/********************************************************************************************

>	void GBrush::SetSolidColours(BOOL SetSolid)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Inputs:		TRUE to turn off dithering of colours, FALSE to turn on dithering

	Purpose:	Allows the dithering of colours to be turned off so they look the same
				using GDraw as they do with the GDI.

	Notes:		By default, GBrushes use dithering.

********************************************************************************************/

void GBrush::SetSolidColours(BOOL SetSolid)
{
	UseSolidColours = SetSolid;
}



/********************************************************************************************

>	void GBrush::GetLogBrush( ColourContext *OutputContext, DocColour &Col,
								LPLOGBRUSH lpBrush )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		OutputContext - The output colour context in which the DocColour
				is to be used.
				Col - colour that the brush is required in
	Outputs:	lpBrush is updated to create a suitably brush which can then be passed
				to CreateBrushIndirect.
	Returns:	-
	Purpose:	The whole point of the GBrush class is to get one of these.
	Errors:		Will ENSURE if not Available.
	SeeAlso:	GBrush::Available

********************************************************************************************/

void GBrush::GetLogBrush( ColourContext *OutputContext, DocColour &Col, wxBrush* pBrush )
{
	const COLORREF rgb = ConvertColourToScreenWord(OutputContext, &Col);

	GetLogBrushInternal(rgb,pBrush);
}



/********************************************************************************************

>	void GBrush::GetLogBrush( const COLORREF rgb, LPLOGBRUSH lpBrush )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>/Jason
	Created:	5/4/94 (28/6/94)
	Inputs:		rgb - a GDI/GDraw style RGB word defining the colour
	Outputs:	lpBrush is updated to create a suitably brush which can then be passed
				to CreateBrushIndirect.
	Returns:	-
	Purpose:	The whole point of the GBrush class is to get one of these. This method
				returns a brush given the RGB screen word to use fo it. Probably you really
				want to use the other overloaded GetLogBrush method, as this does all the
				colour conversion for you.
	Errors:		Will ENSURE if not Available.
	SeeAlso:	GBrush::Available

********************************************************************************************/

void GBrush::GetLogBrush( const COLORREF rgb, wxBrush* pBrush )
{
	GetLogBrushInternal(rgb,pBrush);
}



/********************************************************************************************

>	void GBrush::NewBrushState()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the WantBetterBrushes state changes. Goes around and invalidates
				any current GBrushes, else they will be wrong.
				It also frees the global memory block in each GBrush so it will be
				re-allocated.

********************************************************************************************/

void GBrush::NewBrushState()
{
	// find all current GBrushes
	ListItem *Item = BrushList.GetHead();
	while (Item)
	{
		// check type
		#ifdef _DEBUG
		ENSURE( Item->GetRuntimeClass() == CC_RUNTIME_CLASS(GBrush), "Not GBrush in list");
		#endif

		// safe to cast now
		GBrush *const Brush = (GBrush*const)Item;
		Brush->Stop();									// reset Valid & Current

		// get next one in list
		Item = BrushList.GetNext( Item );
	}
}





// --- Operation to enable/disable GBrush plotting. Used only for debug purposes ---

/*******************************************************************

>	void OpGBrush::Do(OpDescriptor*)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/94
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Toggles state of GBrush flag then forces a redraw.
	Errors:		None

*******************************************************************/

void OpGBrush::Do(OpDescriptor*)
{
	WantBetterBrushes ^= TRUE;

	// Iterate over all the documents, forcing a redraw of all the views.
	ListItem* pItem = Camelot.Documents.GetHead();
	while (pItem != NULL)
	{
		((Document*) pItem)->ForceRedraw();
		pItem = Camelot.Documents.GetNext(pItem);
	}

	// invalidate current brushes
	GBrush::NewBrushState();

#ifndef STANDALONE
	// This is not likely to remain an interactively-alterable option,
	// so for now we just poke at the colour bar to make it redraw itself
	// in the new mode.
	PORTNOTETRACE("other","OpGBrush::Do - removed call to RedrawGBrushAreas");
#ifndef EXCLUDE_FROM_XARALX
	CColourBar::RedrawGBrushAreas();
#endif
	// We should also do this for galleries (e.g. colour gallery) but
	// as this can only be done in debug builds it's not worth the effort.
#endif

	End();
}



/*******************************************************************

>	OpGBrush::OpGBrush()

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/94
	Inputs:
	Outputs:
	Returns:
	Purpose:	Constructor for OpGBrush operation. It is not undoable.
	Errors:		None

*******************************************************************/

OpGBrush::OpGBrush()
{
}



/*******************************************************************

>	OpState OpGBrush::GetState(String_256*, OpDescriptor*)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/94
	Inputs:
	Outputs:
	Returns:
	Purpose:	This item is always available.
	Errors:		None

*******************************************************************/

OpState OpGBrush::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	if (WantBetterBrushes)
		OpSt.Ticked = TRUE;

	return OpSt;
}

