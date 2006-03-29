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

// classes to render into a bitmap for click detection/colour detection

/*
*/

#include "camtypes.h"
#include "grndclik.h"
#include "ccdc.h"
#include "dibutil.h"

CC_IMPLEMENT_DYNAMIC( GRenderClick, GRenderDIB )
CC_IMPLEMENT_DYNAMIC( GRenderClickColour, GRenderClick )
CC_IMPLEMENT_DYNAMIC( GRenderClickColourNoPaper, GRenderClickColour )

#define	new	CAM_DEBUG_NEW

/********************************************************************************************

>	GRenderClick::GRenderClick( DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		A clip region, conversion matrix, scale and bitmap depth.
	Outputs:	None
	Returns:	None
	Purpose:	Constructor. Most important job is to set the VeryMono flag.
				Do not call directly, use OSRenderBitmap::Create
 				INT32 dpi changed to double dpi (12/12/95) to improve the range of values allowed
				at the < 1000dpi settings that we will be using.

********************************************************************************************/

GRenderClick::GRenderClick( DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi)
	: GRenderDIB(ClipRegion, ConvertMatrix, ViewScale, Depth, dpi)
{
	pMiddle = NULL;								// just in case used before set up
	RenderFlags.VeryMono = TRUE;				// simple black&white rendering
	ClearOnStart = FALSE;
}



/********************************************************************************************

>	GRenderClickColour::GRenderClickColour( DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi)

	Author: 	Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/99
	Inputs:		A clip region, conversion matrix, scale and bitmap depth.
	Outputs:	None
	Returns:	None
	Purpose:	Constructor. Most important jobs are to set the VeryMono flag to FALSE, and
				to set m_DoCompression = TRUE to ensure correct handling of transparencies.
				
				Do not call directly, use OSRenderBitmap::Create

********************************************************************************************/

GRenderClickColour::GRenderClickColour( DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi)
	: GRenderClick(ClipRegion, ConvertMatrix, ViewScale, Depth, dpi)
{
	RenderFlags.VeryMono = FALSE;				// simple black&white rendering NOT !!!!!
	RenderFlags.HitDetect = FALSE;
	m_DoCompression = TRUE;
}



/********************************************************************************************

>	GRenderClickColourNoPaper::GRenderClickColourNoPaper( DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/2004
	Inputs:		A clip region, conversion matrix, scale and bitmap depth.
	Outputs:	None
	Returns:	None
	Purpose:	Constructor. Most important jobs are to set the VeryMono flag to FALSE, and
				to set m_DoCompression = TRUE to ensure correct handling of transparencies.
				
				Do not call directly, use OSRenderBitmap::Create

********************************************************************************************/

GRenderClickColourNoPaper::GRenderClickColourNoPaper( DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi)
	: GRenderClickColour(ClipRegion, ConvertMatrix, ViewScale, Depth, dpi)
{
	RenderFlags.VeryMono = FALSE;				// simple black&white rendering NOT !!!!!
	RenderFlags.HitDetect = TRUE;
	m_DoCompression = TRUE;
	ClearOnStart = FALSE;
}



/********************************************************************************************

>	GRenderClick::~GRenderClick()

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Purpose:	Destructor. Simple cleanup, which involves deleting the RenderDC.

********************************************************************************************/

GRenderClick::~GRenderClick()
{
	// Free up the bitmap here, as the call to FreeLPBits in the
	// GRenderDIB will not call the correct version (the virtual-ness will be
	// broken as it is called from a destructor
	if (pBitmapInfo!=NULL)
	{
//		FreeLPBits( lpBitmapInfo, lpBits );
		FreeOffscreenState();
		pBitmapInfo = NULL;
		pBits = NULL;
	}
}



/********************************************************************************************

>	GRenderClickColour::~GRenderClickColour ()

	Author: 	Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/99
	Purpose:	Destructor. Simple cleanup, which involves calling our base classes.

********************************************************************************************/

GRenderClickColour::~GRenderClickColour ()
//	: ~GRenderClick()
{

}



/********************************************************************************************

>	GRenderClickColourNoPaper::~GRenderClickColourNoPaper()

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/2004
	Purpose:	Destructor. Simple cleanup, which involves calling our base classes.

********************************************************************************************/

GRenderClickColourNoPaper::~GRenderClickColourNoPaper()
//	: ~GRenderClick()
{

}




/********************************************************************************************

>	BOOL GRenderClick::ImmediateRender()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/03/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if current clip rect is within view (ie whether to render or not).
	Purpose:	Used to Draw something immediately.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GRenderClick::ImmediateRender()
{
	RenderFlags.bImmediateRender = TRUE;
	if (!StartRender())
		return FALSE;

	// Set no funny attributes - let the hit test rendering do all that

	return TRUE;
}



/********************************************************************************************

>	virtual void GRenderClick::SetClean(BOOL bResetChangedBounds, BOOL FillWhite)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		FillWhite TRUE for fill with white, FALSE to leave contents alone.
	Purpose:	Clears out the RR, both logically and (optionally) physically).	

********************************************************************************************/

void GRenderClick::SetClean(BOOL bResetChangedBounds, BOOL FillWhite)
{
	// Do the normal cleaning process
	GRenderDIB::SetClean(bResetChangedBounds, FillWhite);

	// and if asked, mark the rr as needing clearing
	ClearOnStart = FillWhite;

	// Damn well clear it NOW!
	if (ClearOnStart)
	{
		// Set it to white
		if (pBits!=NULL)
			memset(pBits, 0xFF, pBitmapInfo->bmiHeader.biSizeImage);
		
		// Dont do it next time please
		ClearOnStart = FALSE;
	}
}



/********************************************************************************************

>	BitmapContents GRenderClick::GetContents( BOOL JustMiddle )

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		If justmiddle is TRUE, only checks on the centre pixel will be done (which
				is faster).
	Outputs:	None
	Returns:	BITMAPCONTENTS_CENTRE if centre pixel is set, _ANY if any pixel is set,
				_NONE if none and _ERROR if failed for some reason.
	Purpose:	To investigate whether anything has been rendered into a bitmap-based
				render region. Don't call directly, use OSRenderBitmap::GetContents
	SeeAlso:	OSRenderBitmap::GetContents

********************************************************************************************/

BitmapContents GRenderClick::GetContents( BOOL JustMiddle )
{
	RECT UsedRect;

	if (
		HaveRenderedSomething &&
		(GetDrawContext()->GetChangedBBox( &UsedRect ))
	   )
	{
		if (
			(UsedRect.right > UsedRect.left) ||
			(UsedRect.top < UsedRect.bottom)
		   )
		{
			// something has been drawn - is it the middle?
			if ( (*pMiddle & MiddleMask) == 0)
				return BITMAPCONTENTS_CENTRE;

			// if only interested in centre pixel then we're out of here
			if (JustMiddle)
				return BITMAPCONTENTS_NONE;

			// not in the middle - if any byte is not 0xFF then something must have been
			// rendered
			LPDWORD Search = (LPDWORD)pBits;
			size_t Count = pBitmapInfo->bmiHeader.biSizeImage >> 2;
			while (Count--)
			{
				if (*Search != 0xFFFFFFFF)
					return BITMAPCONTENTS_ANY;
				Search++;
			}
			// else nothing rendered
		}
	}

	return BITMAPCONTENTS_NONE;
}

/********************************************************************************************

>	BitmapContents GRenderClickColour::GetContents( BOOL JustMiddle )

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>		(Phil 16/03/2004)
	Created:	15/11/94
	Inputs:		If justmiddle is TRUE, only checks on the centre pixel will be done (which
				is faster).
	Outputs:	None
	Returns:	BITMAPCONTENTS_CENTRE if centre pixel is set, _ANY if any pixel is set,
				_NONE if none and _ERROR if failed for some reason.
	Purpose:	To investigate whether anything has been rendered into a bitmap-based
				render region. Don't call directly, use OSRenderBitmap::GetContents
	SeeAlso:	OSRenderBitmap::GetContents

********************************************************************************************/

BitmapContents GRenderClickColour::GetContents( BOOL JustMiddle )
{
	RECT UsedRect;

	if (
		HaveRenderedSomething &&
		(GetDrawContext()->GetChangedBBox( &UsedRect ))
	   )
	{
		if (
			(UsedRect.right > UsedRect.left) ||
			(UsedRect.top < UsedRect.bottom)
		   )
		{
			// something has been drawn - is it the middle?
			BYTE middleT = pMiddle[3];				// Read transp channel
			if (middleT < GRenderRegion::ClickTranspLimit)
				return BITMAPCONTENTS_CENTRE;

			// if only interested in centre pixel then we're out of here
			if (JustMiddle)
				return BITMAPCONTENTS_NONE;

			// not in the middle - if any byte is not 0xFF then something must have been
			// rendered
			LPBYTE pSearch = pBits;
			size_t Count = pBitmapInfo->bmiHeader.biSizeImage >> 2;
			while (Count--)
			{
				if (pSearch[3] < GRenderRegion::ClickTranspLimit)
					return BITMAPCONTENTS_ANY;
				pSearch += 4;
			}
			// else nothing rendered
		}
	}

	return BITMAPCONTENTS_NONE;
}

/********************************************************************************************

>	Pixel32bpp GRenderClickColour::GetCentrePixel32bpp()

	Author: 	Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/99
	Inputs:		Pix --- reference to variable that colour information will be returned in.
						This is created internally by the custom colour picker control.
	Outputs:	Pix (via reference)
	Purpose:	This sexy little function is what makes my custom colour picker control
				read the TRUE 32-bit document colour.
	SeeAlso:	NodeRenderableInk::FindSimpleAtPointForColourPicker
				ccolpick.cpp

********************************************************************************************/

Pixel32bpp GRenderClickColour::GetCentrePixel32BPP()
{
	Pixel32bpp Pix;
	Pix.Alpha = 0;				// Generally there is no alpha channel, so set it to a default

	// grab the centre pixels TRUE 32-bit value ....
	
	Pix.Blue = pMiddle [0];		// pointer to byte of centre pixel
	Pix.Green = pMiddle [1];
	Pix.Red = pMiddle [2];
	Pix.Alpha = pMiddle [3];

	return Pix;
}


/********************************************************************************************

>	Pixel32bpp GRenderClickColour::GetAveragePixel32BPP()

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/2004
	Inputs:		-
	Outputs:	Pix - 32bpp value of average colour in GRenderClickColour bitmap
	Returns:	-
	Purpose:	To find the colour under the pointer, taking the differences of pixels
				around the centre pixel into account
	SeeAlso:	OSRenderBitmap::GetContents

********************************************************************************************/

Pixel32bpp GRenderClickColour::GetAveragePixel32BPP()
{
	Pixel32bpp Pix;
	INT32 Blue	= pMiddle [0];		// Give centre pixel more weight by initialising average
	INT32 Green	= pMiddle [1];
	INT32 Red	= pMiddle [2];
	INT32 Alpha	= pMiddle [3];

	// not in the middle - if any byte is not 0xFF then something must have been
	// rendered
	LPBYTE pSearch = pBits;
	UINT32 Count = pBitmapInfo->bmiHeader.biSizeImage >> 2;
	UINT32 size = Count+1;
	while (Count--)
	{
		Blue	+= pSearch[0];
		Green	+= pSearch[1];
		Red		+= pSearch[2];
		Alpha	+= pSearch[3];

		pSearch += 4;
	}

//TRACEUSER( "Phil", _T("AvgPix1 B=%d G=%d R=%d A=%d\n"), Blue, Green, Red, Alpha);
	Pix.Blue	= Blue/size;
	Pix.Green	= Green/size;
	Pix.Red		= Red/size;
	Pix.Alpha	= Alpha/size;
//TRACEUSER( "Phil", _T("AvgPix2 B=%d G=%d R=%d A=%d\n"), Pix.Blue, Pix.Green, Pix.Red, Pix.Alpha);

	return Pix;
}


/********************************************************************************************

>	void GRenderClick::DebugMe( INT32 Stretch )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		How much you want the image stretched by
	Purpose:	Dupms the current bitmap onto the current view window, so we can see what
				is going on. Do not call directly, use OSRenderBitmap::DebugMe
	SeeAlso:	OSRenderBitmap::DebugMe

********************************************************************************************/

void GRenderClick::DebugMe( INT32 Stretch )
{
#if defined(_DEBUG) && 0		// GAT
	DWORD *rgb = (DWORD*)lpBitmapInfo->bmiColors;

	rgb[0] = RGB(0,0,0);
	rgb[1] = RGB(255,255,255);

	const INT32 w = WRect.width;
	const INT32 h = WRect.height;

	::StretchDIBits( RenderDC->m_hDC, 0,0, w*Stretch, h*Stretch,
						0,0,w,h,
						lpBits, lpBitmapInfo,
						DIB_RGB_COLORS,
						SRCCOPY
					);
#endif
}



/********************************************************************************************

>	void GRenderClickColour::DebugMe( INT32 Stretch )

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/99
	Inputs:		How much you want the image stretched by
	Purpose:	Dumps the current bitmap onto the current view window, so we can see what
				is going on. Do not call directly, use OSRenderBitmap::DebugMe
	SeeAlso:	OSRenderBitmap::DebugMe

********************************************************************************************/

void GRenderClickColour::DebugMe( INT32 Stretch )
{
#if defined(_DEBUG) && 0		// GAT
	const INT32 w = WRect.right - WRect.left;
	const INT32 h = WRect.bottom - WRect.top;

	INT32 Offset = (Stretch/100) * w * (Stretch%100);
	Stretch = Stretch%100;

//	DIBUtil::InvertAlpha(lpBitmapInfo, lpBits);
	DWORD oldComp = lpBitmapInfo->bmiHeader.biCompression;
	lpBitmapInfo->bmiHeader.biCompression = BI_RGB;
	INT32 ret = ::StretchDIBits( RenderDC->m_hDC, 0,0, w*Stretch, h*Stretch,
						0,0,w,h,
						lpBits, lpBitmapInfo,
						DIB_RGB_COLORS,
						SRCCOPY
					);
	lpBitmapInfo->bmiHeader.biCompression = oldComp;
//	DIBUtil::InvertAlpha(lpBitmapInfo, lpBits);
#endif
}



// we don't actually want to display this bitmap thanks

/********************************************************************************************

>	BOOL GRenderClick::DisplayBits(LPBITMAPINFO lpDisplayBitmapInfo, LPBYTE lpDisplayBits)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Returns:	TRUE always (cannot fail).
	Purpose:	Supposed to dump bitmap on screen, but that makes no sense for this bitmap
				so it just does nothing and always returns TRUE.

********************************************************************************************/

BOOL GRenderClick::DisplayBits(LPBITMAPINFO lpDisplayBitmapInfo, LPBYTE lpDisplayBits)
{
	return TRUE;
}



/********************************************************************************************

>	BOOL GRenderClick::AttachDevice(View* pView, CDC* DCToAttach, Spread* SpreadToAttach)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		pView is used for debug purposes only. DCToAttach is used as a 
				reference when making the bitmap. 
	Returns:	FALSE if failed (no mem for bitmap or CCDC), TRUE if OK
	Purpose:	Most work done in base class, except for the creation of a CCDC member.

********************************************************************************************/

BOOL GRenderClick::AttachDevice(View* pView, wxDC* DCToAttach, Spread* SpreadToAttach, bool fOwned /*= false*/ )
{

	// call base class first
	if (!GRenderDIB::AttachDevice(pView, DCToAttach, SpreadToAttach, fOwned))
		return FALSE;
			
	// now replace the usual RenderDC with our new one. This makes the type of the CCDC
	// correct, so Quality rendering works correctly, for example.
/*
	TRY
	{
		// IMPORTANT: we 'borrow' the HDC from DCToAttach. We must take care when
		// deleting this not to delete the HDCs (see the constructor).
		RenderDC = new CCDC( DCToAttach, RENDERTYPE_MONOBITMAP );
	}
	CATCH (CMemoryException, e)
	{
		// Ooer - not enough memory
		TRACEALL( _T("No memory for CCDC in GRenderClick::AttachDevice") );
		return FALSE;
	}
	END_CATCH
*/
	RenderDC = new wxBufferedDC() ;
	m_fOwned = true;

	return TRUE;
}



/********************************************************************************************

>	BOOL GRenderClickColour::AttachDevice(View* pView, CDC* DCToAttach, Spread* SpreadToAttach)

	Author: 	Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/99
	Inputs:		pView is used for debug purposes only. DCToAttach is used as a 
				reference when making the bitmap. 
	Returns:	FALSE if failed (no mem for bitmap or CCDC), TRUE if OK
	Purpose:	Most work done in base class, except for the creation of a CCDC member.

********************************************************************************************/

BOOL GRenderClickColour::AttachDevice(View* pView, wxDC* DCToAttach, Spread* SpreadToAttach, bool fOwned /*= false*/)
{

	// call base class first
	if (!GRenderDIB::AttachDevice(pView, DCToAttach, SpreadToAttach, fOwned))
		return FALSE;
/*
	// now replace the usual RenderDC with our new one. This makes the type of the CCDC
	// correct, so Quality rendering works correctly, for example.
	TRY
	{
		// IMPORTANT: we 'borrow' the HDC from DCToAttach. We must take care when
		// deleting this not to delete the HDCs (see the constructor).
		RenderDC = new CCDC( DCToAttach, RENDERTYPE_COLOURBITMAP );
	}
	CATCH (CMemoryException, e)
	{
		// Ooer - not enough memory
		TRACEALL( _T("No memory for CCDC in GRenderClickColour::AttachDevice") );
		return FALSE;
	}
	END_CATCH
*/
	RenderDC = new wxBufferedDC() ;
	m_fOwned = true;

	return TRUE;
}


/********************************************************************************************

>	BOOL GRenderClickColourNoPaper::AttachDevice(View* pView, CDC* DCToAttach, Spread* SpreadToAttach)

	Author: 	Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/99
	Inputs:		pView is used for debug purposes only. DCToAttach is used as a 
				reference when making the bitmap. 
	Returns:	FALSE if failed (no mem for bitmap or CCDC), TRUE if OK
	Purpose:	Most work done in base class, except for the creation of a CCDC member.

********************************************************************************************/

BOOL GRenderClickColourNoPaper::AttachDevice(View* pView, wxDC* DCToAttach, Spread* SpreadToAttach, bool fOwned /*= false*/)
{

	// call base class first
	if (!GRenderDIB::AttachDevice(pView, DCToAttach, SpreadToAttach, fOwned))
		return FALSE;
/*			
	// now replace the usual RenderDC with our new one. This makes the type of the CCDC
	// correct, so Quality rendering works correctly, for example.
	TRY
	{
		// IMPORTANT: we 'borrow' the HDC from DCToAttach. We must take care when
		// deleting this not to delete the HDCs (see the constructor).
		RenderDC = new CCDC( DCToAttach, RENDERTYPE_HITDETECT );
	}
	CATCH (CMemoryException, e)
	{
		// Ooer - not enough memory
		TRACEALL( _T("No memory for CCDC in GRenderClickColour::AttachDevice") );
		return FALSE;
	}
	END_CATCH
*/
	RenderDC = new wxBufferedDC() ;
	m_fOwned = true;

	return TRUE;
}


/********************************************************************************************

>	BOOL GRenderClick::StartRender()

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Most work done in base class, except for the calculation of the address &
				mask of the centre pixel.

********************************************************************************************/

BOOL GRenderClick::StartRender()
{
	if (!GRenderDIB::StartRender())
		return FALSE;

	// work out the address & mask of the centre pixel
	const INT32 w = WRect.width;
	const INT32 h = WRect.height;

	// address is pretty easy
	pMiddle = pBits + (h/2)*DIBUtil::ScanlineSize( w, 1 );
	pMiddle += ((w/2)/8);

	// bit within byte is also reasonably straight-forward (and should be independent of
	// byte-sex)
	MiddleMask = (BYTE) ( (INT32)0x80 >> ((w/2)&7) );

	// If we have been asked to clear out the contents of the bmp, then do it
	if (ClearOnStart)
	{
		// Set it to white
		if (pBits!=NULL)
			memset(pBits, 0xFF, pBitmapInfo->bmiHeader.biSizeImage);
		
		// Dont do it next time please
		ClearOnStart = FALSE;
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL GRenderClickColour::StartRender()

	Author: 	Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Most work done in base class, except for the calculation of the address &
				mask of the centre pixel.

********************************************************************************************/

BOOL GRenderClickColour::StartRender()
{
	if (!GRenderDIB::StartRender())
		return FALSE;

	// work out the address & mask of the centre pixel
	const INT32 w = WRect.width;
	const INT32 h = WRect.height;

	UINT32 scanlineSize = DIBUtil::ScanlineSize( w, 32 );
	
	// address is pretty easy
	GRenderClick::pMiddle = pBits + (h/2)*scanlineSize;
	GRenderClick::pMiddle += (w/2) * 32/8;					// Bytes per pixel

	// The following is immaterial in 32BPP bitmaps...
	// bit within byte is also reasonably straight-forward (and should be independent of
	// byte-sex) 
	MiddleMask = (BYTE) ( (INT32)0x80 >> ((w/2)&7) );

	// If we have been asked to clear out the contents of the bmp, then do it
	if (ClearOnStart)
	{
		// Set it to white
		if (pBits!=NULL)
			memset(pBits, 0xFF, pBitmapInfo->bmiHeader.biSizeImage);
		
		// Dont do it next time please
		ClearOnStart = FALSE;
	}

	return TRUE;
}



/********************************************************************************************

>	virtual BOOL GRenderClick::SetFirstBand()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/95
	Returns:	TRUE 
	Purpose:	Sets up for banded rendering. This class does not do banded rendering as yet.
				This just sets things up to indicate that everything will be done in one band.

********************************************************************************************/

BOOL GRenderClick::SetFirstBand()
{
	// No Banding needed
	IsRegionBanded = FALSE;
	IsWaitingForRAM = FALSE;
	IsLastBand = TRUE;
	return TRUE;
}



/********************************************************************************************

>	virtual BOOL GRenderClick::GetNextBand()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/95
	Returns:	FALSE
	Purpose:	Since this class does not do banded rendering, there are never any more bands

********************************************************************************************/

BOOL GRenderClick::GetNextBand()
{
	// No banding
	return FALSE;
}



/********************************************************************************************

>	LPBITMAPINFO GRenderClick::GetLPBits( INT32 Width, INT32 Height, INT32 Depth, LPBYTE*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/95
	Inputs:		Width, Height - the width and height of the required bitmap
				Depth - the bpp of the bitmap
	Returns:	Pointer to a bitmap header block
	Purpose:	Allocates a bitmap from the CCMalloc heap

********************************************************************************************/

LPBITMAPINFO GRenderClick::GetLPBits( INT32 Width, INT32 Height, INT32 Depth, LPBYTE* lplpBits)
{
	// Get a DIB out of the CCMalloc heap
	LPBITMAPINFO bmInfo = NULL;
	bmInfo = AllocDIB( Width, Height, Depth, lplpBits, NULL, FALSE);

	// return it.
	return bmInfo;
}




/********************************************************************************************

>	void GRenderClick::FreeLPBits( LPBITMAPINFO, LPBYTE )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/95
	Purpose:	Frees the memory allocated in GetLPBits.

********************************************************************************************/

void GRenderClick::FreeLPBits( LPBITMAPINFO lpBMI, LPBYTE lpB )
{
	// Free up the memory. Not from Limited Heap
	FreeDIB(lpBMI, lpB, NULL, FALSE);
}
