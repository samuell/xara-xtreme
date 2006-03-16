// $Id: offattr.cpp 662 2006-03-14 21:31:49Z alex $
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

#include "camtypes.h"

#include "docview.h"
#include "offattr.h"

#include "GDrawIntf.h"
#include "offscrn.h"		// GRenderRegionWrapper
#include "grndrgn.h"		// GRenderRegion

// Bitmap manipulation
#include "bitmap.h"
#include "oilbitmap.h"

//#include "winrect.h"		// CalculateWinRect


CC_IMPLEMENT_DYNAMIC( OffscreenAttrValue, GeometryLinkedAttrValue )

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW
#define BOGUS_DEFAULT_ATTR_NODE_POINTER (NodeRenderableBounded*) 0x1


/////////////////////////////////////////////////////////////////////////////////////////////
//
//								Helper functions
//
/////////////////////////////////////////////////////////////////////////////////////////////
// Measurement constants are defined in offscrn.h, and
// view Scale determines dpi (ie pix per inch) of screen.

// if there is no view, then returns zero.
FIXED16 GetCurrentViewScale()
{
	View* pView = View::GetCurrent();
	if (pView == NULL)
		return FIXED16(0);

	return pView->GetViewScale();
}

MILLIPOINT ConvertMeasurementToMillipointsAtCurrentViewScale(INT32 Width, UnitType type)
{
//#pragma message( __LOCMSG__ "ConvertMeasurementToMillipointsAtCurrentViewScale - do nothing" )
//	TRACE( _T("Warning - ConvertMeasurementToMillipointsAtCurrentViewScale called\n") );
//	return MILLIPOINT(0);
	const double dpi = (double)GRenderRegion::GetDefaultDPI();
	const double MillipointsPerPixel = (dpi <= 0) ? 750.0 : MILLIPOINTS_PER_INCH / dpi;

	double ScaledWidth = 0;

	switch(type)
	{
		case MILLIPOINTS:
			ScaledWidth = Width;
			break;

		case PIXELS:
			ScaledWidth =	MillipointsPerPixel * Width;
			ScaledWidth /=	GetCurrentViewScale().MakeDouble();
			break;

		 // unsupported unit type.
		default:
			ScaledWidth = -1;
			break;
	}

	return (MILLIPOINT)ScaledWidth;
}

UINT32 ConvertMillipointsToPixelsAtCurrentViewScale(MILLIPOINT mp)
{
//#pragma message( __LOCMSG__ "ConvertMillipointsToPixelsAtCurrentViewScale - do nothing" )
//	TRACE( _T("Warning - ConvertMillipointsToPixelsAtCurrentViewScale called\n") );
//	return 0;
	const double dpi = (double)GRenderRegion::GetDefaultDPI();
	const double MillipointsPerPixel = (dpi <= 0) ? 750.0 : MILLIPOINTS_PER_INCH / dpi;

	// NB	this routine rounds to the nearest pixel, so if you convert
	//		from mp to pixels and back again, you are highly unlikely
	//		to get the value you started off with!
	double	Pixels	=	(mp / MillipointsPerPixel);
			Pixels	*=	GetCurrentViewScale().MakeDouble();

	return (UINT32)(Pixels + 0.5);
}

INT32 PixelsPerMeterAtCurrentViewScale()
{
	// NB	return value is given to the nearest pixel.
	double	ppm =	PIXELS_PER_INCH * INCHES_PER_METRE;
			ppm *=	GetCurrentViewScale().MakeDouble();
	return (INT32)(ppm + 0.5);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttributeValue classes
//
/////////////////////////////////////////////////////////////////////////////////////////////
/********************************************************************************************
>	OffscreenAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	This constructor is only called when instantiating a derived class.
	SeeAlso:	
 ********************************************************************************************/
OffscreenAttrValue::OffscreenAttrValue()
{
	m_fForceTransparency = FALSE;
#ifdef DEBUGOFFSCREENATTRS
	m_fHasBeenRendered = FALSE;
#endif
}

/********************************************************************************************
>	OffscreenAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	Nothing special to do. All memory used for bitmap structures in offscreen
				rendering is freed in OffscreenRenderingCompleted()
	SeeAlso:	
 ********************************************************************************************/
OffscreenAttrValue::~OffscreenAttrValue()
{
}

/********************************************************************************************
>	OffscreenAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	This funciton is used in context of attribute optimisation.
				I'm implementing this function to re-iterate that all OffscreenAttrValues are
				unique, even though it may never get called (ie because they are never optimised)
	SeeAlso:	
 ********************************************************************************************/
BOOL OffscreenAttrValue::IsDifferent(AttributeValue*)
{
	return TRUE;
}


/********************************************************************************************
>	OffscreenAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	Determined how large an area we need to draw ofscreen in this rendering 
				iterartion. This is calculated in DocRect coords and it is up to the
				GRenderRegion Offscreen rendering system to alloca a large enough chunk
				of memory to hold the bitmap
	SeeAlso:	
 ********************************************************************************************/
void OffscreenAttrValue::Render(RenderRegion *pRender, BOOL Temp)
{
//#pragma message( __LOCMSG__ "OffscreenAttrValue::Render - do nothing" )
//	TRACE( _T("Warning - OffscreenAttrValue::Render called\n") );
	// Debug test for an offscreen attribute being Render()'ed multiple times without
	// OffscreenRenderingCompleted() being called. This is easily caused by:
	//	1.	rendering code which isn't bracketed by calls to SaveContext() / RestoreContext()
	//	2.	rendering an attribute map which includes an offscreen attr - you must be careful
	//		that you don't inadvertently render the attr twice.
	//
	// Reason:	offscreen attrs initialise and divert subsequent rendering into an offscreen
	//			bitmap, which is retrieved and processed when the attribute goes out of scope
	//			after a RestoreContext() call. Render the attr twice => you'll be creating
	//			and maybe rendering into an offscreen bitmap which is never used + may also
	//			cause a memory leak of that bitmap.
#ifdef DEBUGOFFSCREENATTRS
	if (!IsDefaultFlagSet())
	{
		if (m_fHasBeenRendered)
		{
			char msg[120];
			sprintf(msg, "OffscreenAttrValue::Render; Attr at 0x%x rendered more than once! See code for details!", (DWORD)this);
			ERROR3(msg);
			return;
		}
		else
		{
			m_fHasBeenRendered = TRUE;
		}
	}
#endif

	pRender->SetOffscreen(this);
}



/********************************************************************************************
>	OffscreenAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	
	SeeAlso:	GRenderRegion::RestoreGDrawBitmap
 ********************************************************************************************/
void OffscreenAttrValue::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "OffscreenAttrValue::Restore - do nothing" )
//	TRACE( _T("Warning - OffscreenAttrValue::Restore called\n") );
	pRegion->RestoreOffscreen(this);
}

/********************************************************************************************
>	OffscreenAttrValue::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Purpose:	return docrect which needs to be drawn offscreen in this rendering iteration
	SeeAlso:	GRenderRegion::SetGDrawBitmap & OffscreenAttrValue::Render (calcualtes m_OffBB)
 ********************************************************************************************/
DocRect	OffscreenAttrValue::GetOffscreenBoundingRect()
{ 
	return m_OffBB;
}



/********************************************************************************************

>	BOOL OffscreenAttrValue::DoesOffscreenBmpRequireTransp(GRenderRegion* pGRR = NULL)

	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>, rewritten by Karim MacDonald
	Created:	24/01/2000, rewritten 19/11/2000

	Inputs:		pGRR	ptr to the GRenderRegion into which we'll be rendered.

	Returns:	TRUE if we need a transparency-capable bitmap,
				FALSE otherwise.

	Purpose:	As an optimisation, you can make this call on an offscreen attribute,
				and only allocate + use a transparency-capable bitmap if it is necessary.
				eg, this results in about a 10% performance increase for feathering.

	Notes:		pGRR is unused in this method, so may be NULL.
				However, at least one offscreen-attr - FeatherAttrValue - requires that
				this parameter be non-NULL.

	See also:	TestNodeForTransparency()

********************************************************************************************/
BOOL OffscreenAttrValue::DoesOffscreenBmpRequireTransp(GRenderRegion* pGRR)
{
	// if we've been told to always use transparency, then always return TRUE here.
	if (m_fForceTransparency)
		return TRUE;

	// our normal behaviour - test our linked node to see whether it needs transparency.
	return TestNodeForTransparency(GetLinkedNode());
}



/********************************************************************************************

>	BOOL TestNodeForTransparency(Node* pNode)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/08/2000
	Inputs:		pNode	the node to test for transparency.
	Purpose:	Test the given node to see whether it or any of its children requires
				transparency.
	Returns:	TRUE if pNode requires transparency, or is NULL.
	See Also:	NodeRenderable::IsSeeThrough()

********************************************************************************************/
BOOL OffscreenAttrValue::TestNodeForTransparency(Node* pNode)
{
	// return TRUE if the node is NULL, just in case transparency is later required.
	if (pNode == NULL || !pNode->IsNodeRenderableClass())
		return TRUE;

	// check the node to see whether it is see-through, remembering to test
	// its indirectly applied attributes.
	return ((NodeRenderable*)pNode)->IsSeeThrough(TRUE);
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//								Debuging functions
//
/////////////////////////////////////////////////////////////////////////////////////////////
/*
NB this is purely for debugging. Remove once you are happy that your offscreen bitmap
is being created correctly

Notes:-
1) only performed when viewscale = 1.0; ie 100% zoom ie 750 mp per pix

2) only comes into play if more than _DEBUG_OFFSCREEN_PIXEL_WIDTH pix wide region of 
	node is invalid (ie only when intersection of node bounds and current clip rect
	is wider than _DEBUG_OFFSCREEN_PIXEL_WIDTH)

3) select a _DEBUG_OFFSCREEN_PIXEL_WIDTH value which make viewing memory using Visual C++
	'memory debug viewer' easier
3.1) Mem Viewer has max 48 bytes wide view (@ 1152*864)
3.2) Use byte view for 8bpp bitmaps
3.3) Use INT32 hex (ie DWORD) view for 32bpp bitmaps
3.4) NB 32bpp scanlines are 4*(pixel width) wide in viewer

4) If used, the invalid region of the node will be changed to so that it is _DEBUG_OFFSCREEN_PIXEL_WIDTH
   pixels wide and covers only the left hand side of this region
   NB pixel alignment of viewable bounds won't affect FillPath rendering on the nodes path
	  - ie bottom left corner of node bounds will always be drawn in the bottom left pixel in
	  the offscreen bitmap

Conditions for use:-
--------------------
1) pGRR->ScalePixelWidth = 750
2) Width of ViewableRect (ie intersection of NodeBounds and current invalid clip rect)
	> _DEBUG_OFFSCREEN_PIXEL_WIDTH

What can be debugged using _DEBUG_OFFSCREEN_PIXEL_WIDTH:-
---------------------------------------------------------
There are 3 bitmaps which we would want to look at in the memory viewer. Each has a corresponding
bounding box measured in MILLIPOINTS. The bitmaps and mp bounds are listed below:-

1 Offscreen bmp				(OffBB)	= the bitmap of the node to which this offscreen attr is attached
2 Cached bmp				(NodeBB)= bitmap of node generated by GRenderBitmap
									  (generated when offscreen attr is first attached to the node)
3 Sliced cached bmp			(OffBB) = left hand slice of cached bitmap sliced so we can compare it to Offscreen bmp
									  (copied from cache bmp)

The bounds are calculated as follows:-
--------------------------------------
NodeBB	= the nodes original bounding box
ClipBB	= current invalid clip region
			NB even in instances where you would expect the ClipBB to be closely related to NodeBB (eg ops applied
			on the node which invalidate NodeBB), ClipBB seems to be a variable increment larger than NodeBB
			eg	for applying col fill increment = ?pix (5 I think)
				for applying line fill increment = ?pix (3 I think)
				for applying feathering increment = 10 pix

ViewBB	= NodeBB.Intersect(ClipBB)		=> vWidth x vHeight 

IF debugging offscreen bitmap:
OffBB	= _DEBUG_OFFSCREEN_PIXEL_WIDTH x vHeight

Values to use:-
---------------
When debugging Offscreen bitmap :-
- use _DEBUG_OFFSCREEN_PIXEL_WIDTH = 48
- compare sliced cached bmp and offscreen bmp in mem viewer
- NB the offscreen bitmap is upside down in memory. ie the pixels comprising the first row
	in memory at m_pOffscreenBmpBits is infact the bottom row when displayed
	(this is how GDraw draws)

Functions affected:-
--------------------
// Setup
Create feather
	node (ie NodeBB), profile, feather size
	if debugging
	KernelBitmap*				md_NodeBmp;
	KernelBitmap*				md_SlicedBmp;

Render
	DocRect						v = NodeBB.Intersect(ClipBB);
	if debugging && Zoom=100% and v.Width > _DEBUG_OFFSCREEN_PIXEL_WIDTH
		OffBB	= _DEBUG_OFFSCREEN_PIXEL_WIDTH x v.Height
	else
		DocRect						m_OffBB = v.Width x v.Height ;


	SetFeathering( m_pOffscreenBmpInfo, m_pOffscreenBmpBits); -> sets up Offscreen Bmp

// Use members
OffscreenRenderingCompleted
	create bitmap fill from OffscreenBmp

	generate feather mask bitmap from OffscreenBmp using Feather32BppBitmap convolution

	create transp bmp fill
	
	GRR::RenderBitmapFillWithTransp	
*/

#ifdef _DEBUG_OFFSCREEN_PIXEL_WIDTH
KernelBitmap* OffscreenAttrValue::CreateIntermediateBitmap(
							GRenderRegion* GRR,
							Node* pNode,
							double ScreenResMultiplier,
							UINT32 bpp,
							BOOL UseInternalFmt	)
{
	GRenderRegionWrapper* pRendWrap = GRenderRegionWrapper::GetAppropriateRenderWrapper(
		GRR, ScreenResMultiplier, ((NodeRenderableBounded*) pNode)->GetBoundingRect(),bpp,UseInternalFmt);

	if(!pRendWrap)
	{
		// failure message already posted
		return NULL;
	}

	pRendWrap->RenderTree(pNode);

	KernelBitmap* kBmp = pRendWrap->GetKernelBitmap();

	pRendWrap->RestorePreviousRendererState();

	return kBmp;
}

BOOL OffscreenAttrValue::RenderDebuggingBitmaps(
							GRenderRegion* GRR,
							Node* pNode,
							double ScreenResMultiplier,
							UINT32 bpp,
							BOOL UseInternalFmt	)
{
	md_NodeBmp = CreateIntermediateBitmap( NULL, pNode, 1.0, 32, FALSE);
	if(md_NodeBmp==NULL)
	{
		ENSURE(FALSE,"Failed to create GRenderBitmap for comparison with offscreen rendered image.");
		return FALSE;
	}

	double zoom = GetCurrentViewScale().MakeDouble();
	if( (zoom==1.0) && (md_NodeBmp->GetWidth() > _DEBUG_OFFSCREEN_PIXEL_WIDTH) )
	{
		// slice to specified dimensions
		LPBITMAPINFO sliceInfo;
		LPBYTE sliceBits;
		UINT32 sliceWidth = _DEBUG_OFFSCREEN_PIXEL_WIDTH;
		sliceInfo = AllocDIB( sliceWidth, md_NodeBmp->GetHeight(),
							  32,&sliceBits,NULL,TRUE );
		LPBYTE pSetSliceBits = sliceBits;
		LPBYTE pNodeBits = md_NodeBmp->GetBitmapBits();
		UINT32 NodeScanline = md_NodeBmp->GetWidth() * 4;
		UINT32 sliceScanline = sliceWidth * 4;
		UINT32 NodeBmpSize = md_NodeBmp->GetHeight() * NodeScanline;
		UINT32 sliceRow=0;

		// copy bits to sliced dimensions
		for(UINT32 row=0; row < NodeBmpSize; row+=NodeScanline)
		{
			for(UINT32 col=0; col < sliceScanline; col++)
			{
				pSetSliceBits[sliceRow + col] = pNodeBits[row + col];
			}
			sliceRow+=sliceScanline;
		}

		Document* pDoc = Document::GetCurrent();
		WinBitmap* wBitmap = new WinBitmap(sliceInfo, sliceBits);
		md_SlicedBmp = new KernelBitmap(wBitmap,TRUE);	//NB Temp = FALSE doesn't add to kernel bmp gallery
		OILBitmap* pOil = md_SlicedBmp->GetActualBitmap();
		pOil->SetName(String_256("SlicedBitmap"));
		GetApplication()->GetGlobalBitmapList()->MakeNameUnique(&(pOil->m_BitmapName));
		md_SlicedBmp->Attach(pDoc->GetBitmapList());
	}
}
#endif
