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
// ppairbsh - Definition of PathProcessor class for rendering Airbrush strokes

#include "camtypes.h"

#include "ppairbsh.h"

#include "attrmgr.h"
#include "cameleps.h"
#include "docview.h"
#include "document.h"
#include "fillval.h"
#include "fixmem.h"
//#include "grndbmp.h"
#include "paths.h"
#include "pathstrk.h"
#include "pathtrap.h"
#include "printctl.h"
#include "qualattr.h"
#include "rndrgn.h"
#include "spread.h"
#include "strkattr.h"
#include "valfunc.h"

CC_IMPLEMENT_DYNAMIC(PathProcessorStrokeAirbrush, PathProcessorStroke);

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

const INT32 MaxAirbrushSteps = 48;	// This is the max number of rings we'll plot
									// The user has to make a very thick stroke before 
									// 48 rings are noticably "steppy".


/********************************************************************************************

>	PathProcessorStrokeAirbrush::~PathProcessorStrokeAirbrush()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/97

	Purpose:	Destructor

********************************************************************************************/

PathProcessorStrokeAirbrush::~PathProcessorStrokeAirbrush()
{
	if (pIntensityFunction != NULL)
		delete pIntensityFunction;
}



/********************************************************************************************

>	virtual BOOL PathProcessorStrokeAirbrush::WillChangeFillAndStrokeSeparately(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/12/96

	Returns:	TRUE

	Purpose:	Called by the RenderRegion to determine if this PathProcessorStrokeAirbrush affects
				the "fill" and "stroke" portions of the path separately. (Generally
				speaking, only fill/stroke providers will cause these two different
				"bits" of a path to be rendered separately). This call is made BEFORE
				this Processor's ProcessPath function will be called.

				If the caller gets a TRUE back, the stroke and fill paths will be
				rendered separately.

	Notes:		Base class implementation returns FALSE. Derived Stroke and Fill
				processors (such as this one) override this method to return TRUE.

********************************************************************************************/

BOOL PathProcessorStrokeAirbrush::WillChangeFillAndStrokeSeparately(void)
{
	return(FALSE);
}



/********************************************************************************************

>	virtual void PathProcessorStrokeAirbrush::ProcessPath(Path *pPath,
														  RenderRegion *pRender,
														  PathShape ShapePath=PATHSHAPE_PATH)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/97

	Purpose:	Called by the RenderRegion to apply the path processing operation to 
				the given path.

				The PathProcessorStrokeAirbrush class changes the stroking (only) of paths
				passed into it.

	Notes:		* When rendering a path, always pass in your 'this' pointer to
				  RenderRegion::DrawPath, so that you don't start an infinite
				  recursion!

				* To stop rendering of the path, simply return without calling the RR

				* To render this path unchanged, simply call directly back to the RR:
					pRender->DrawPath(pPath, this);
				
				* Only affect the fill of this path if pPath->IsFilled

				* Only affect the stroke of this path if pPath->IsStroked

				* If converting a path into a "filled path" for stroking, the output
				  path should be rendered with IsStroked=FALSE or it'll get a line
				  around the outside!				

********************************************************************************************/

void PathProcessorStrokeAirbrush::ProcessPath(Path *pPath,
											  RenderRegion *pRender,
											  PathShape ShapePath)
{
	PORTNOTETRACE("other","PathProcessorStrokeAirbrush::ProcessPath - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR3IF(pPath == NULL || pRender == NULL, "Illegal NULL Params");

	// --- If the provided path is not stroked, then we'll just pass it straight through
	// We also don't touch it if we're doing EOR rendering, or click regions
	// BLOCK
	{
		StrokeColourAttribute *pStrokeColour = (StrokeColourAttribute *) pRender->GetCurrentAttribute(ATTR_STROKECOLOUR);
		if (pRender->DrawingMode != DM_COPYPEN || pRender->IsHitDetect()
			|| !pPath->IsStroked || pStrokeColour == NULL || pStrokeColour->Colour.IsTransparent())
		{
			pRender->DrawPath(pPath, this, ShapePath);
			return;
		}
	}

	// --- If the quality is set low, strokes are just rendered as centrelines
	// BLOCK
	{
		QualityAttribute *pQuality = (QualityAttribute *) pRender->GetCurrentAttribute(ATTR_QUALITY);
		if (pQuality != NULL && pQuality->QualityValue.GetLineQuality() != Quality::FullLine)
		{
			pRender->DrawPath(pPath, this, ShapePath);
			return;
		}
	}

	// --- If the attribute which created us is not the current StrokeType attribute, then
	// we have been overridden by a different stroke type, so we do nothing.
	// BLOCK
	{
		StrokeTypeAttrValue *pTypeAttr = (StrokeTypeAttrValue *) pRender->GetCurrentAttribute(ATTR_STROKETYPE);
		if (pTypeAttr != NULL && pTypeAttr != GetParentAttr())
		{
			pRender->DrawPath(pPath, this, ShapePath);
			return;
		}
	}

	// --- Get the current line width from the render region
	// In case of failure, we initialise with suitable defaults
	INT32 LineWidth = 5000;
	// BLOCK
	{
		LineWidthAttribute *pWidthAttr = (LineWidthAttribute *) pRender->GetCurrentAttribute(ATTR_LINEWIDTH);
		if (pWidthAttr != NULL)
			LineWidth = pWidthAttr->LineWidth;
	}

	// Obtain an optimal number of steps for the line
	// When printing, we do heaps of steps to get top quality out the other end
	View *pView	= pRender->GetRenderView();
	ERROR3IF(pView == NULL, "No render view?!");

	INT32 NumSteps = MaxAirbrushSteps;
	if (!pRender->IsPrinting())
		GetNumSteps(pView, LineWidth);

	// --- Now, create a transparency mask bitmap for the airbrush
	Spread *pSpread	= pRender->GetRenderSpread();
//	ERROR3IF(pSpread == NULL, "No render spread!?");	// This can happen, rendering into a gallery

	// Get the render region's clip rectangle in Spread Coords. We don't need to
	// render anything bigger than this size, so it is the upper limit on our bitmap area.
	DocRect ClipRegion = pRender->GetClipRect();

	// Intersect this with the path bounding rectangle to get the actual region we need to redraw
	// The smaller this is, the faster we go and the less memory we use.
	//DocRect PathRect = pPath->GetBoundingRect();
	DocRect PathRect = pPath->GetBlobRect();
	PathRect.Inflate(LineWidth);

	BOOL Intersects = ClipRegion.IsIntersectedWith(PathRect);
	if(!Intersects)
	{
		// Don't bother drawing anything - it's clipped out
		return;
	}

	ClipRegion = ClipRegion.Intersection(PathRect);

	// Round the ClipRegion edges up so they all lie exactly on screen pixel boundaries.
	// If we don't do this, then there can be a sub-pixel rounding error between the ClipRegion
	// and the actual bitmap size, so that the bitmap is scaled slightly when we plot it.
	// By making sure it's pixelised, we guarantee that the bitmap & clipregion are exactly equal sizes.
	// (It doesn't matter if the bitmap is a bit bigger than necessary)
	ClipRegion.Inflate(pRender->GetScaledPixelWidth());
	ClipRegion.lo.Pixelise(pView);
	ClipRegion.hi.Pixelise(pView);

	// Get the current view's rendering matrix and view scale
	Matrix ConvMatrix = pRender->GetMatrix();//pView->ConstructRenderingMatrix(pSpread);
	FIXED16 ViewScale = pView->GetViewScale();

	// Generate a 256-colour greyscale palette
	LOGPALETTE *pPalette = MakeGreyScalePalette();
	if(pPalette == NULL)
	{
		pRender->DrawPath(pPath, this, ShapePath);
		return;
	}

	// Work out the DPI to use. Rather than just asking for PixelWidth or DPI from the
	// render region, we have to do a load of non-object-oriented stuff instead...
	double dpi = 96.0;
	if (pRender->IsPrinting())
	{
		// we are printing, so ask the print options
		PrintControl *pPrintControl = pView->GetPrintControl();
		if (pPrintControl != NULL)
			dpi = (double) pPrintControl->GetDotsPerInch();
	}
	else if (IS_A(pRender, CamelotEPSRenderRegion))
	{
		// Use DPI as set in EPS export options dialog.
		dpi = (double) EPSFilter::XSEPSExportDPI;
	}
	else
	{
		ERROR3IF(pRender->GetPixelWidth() <= 0, "Stupid (<1 millipoint) Pixel Width!");
		if (pRender->GetPixelWidth() > 0)
			dpi = (double) (72000.0 / (double)pRender->GetPixelWidth());
	}

	GRenderBitmap *pMaskRegion = new GRenderBitmap(ClipRegion, ConvMatrix, ViewScale, 8, dpi,
													pRender->IsPrinting(), XARADITHER_ORDERED_GREY,
													pPalette, FALSE);
	if (pMaskRegion == NULL)
	{
		pRender->DrawPath(pPath, this, ShapePath);
		return;
	}

	BOOL PathIsFilled = FALSE;		// Will be set TRUE if this path should be filled at the bottom of the function

	//BLOCK
	{
		// Change the GDraw context in this render region so as to preserve the state
		// of the main render region. This is because GRenderRegions currently use
		// a single static GDrawContext! This also sets it up with a nice greyscale palette
		// so that we get the output we desire.
		pMaskRegion->UseGreyscaleContextDangerous();

		// Attach our DC to the view and render region...
		if (pMaskRegion->AttachDevice(pView, NULL, pSpread))
		{
			pMaskRegion->StartRender();

			// We must save & restore the attribute state around all our rendering because
			// attributes otherwise stay on the renderstack until we delete the RndRgn, and as our
			// ones are on the program stack, they will have ceased to exist before then, which
			// makes for a wicked explosion.
			pMaskRegion->SaveContext();

/////////////////////////////////////////////////////////////////////////////////////
			// --- Main Airbrush rendering loop

			// Make sure we've got an intensity function to use. This will create a new one if necessary
			ValueFunction *pvValueFunction = GetIntensityFunction();
			if (pvValueFunction == NULL)
			{
				ERROR3("Failed to set an intensity function on an airbrush stroke");
				pRender->DrawPath(pPath, this, ShapePath);
				return;
			}


			if(!RenderAirBrush(pPath, pMaskRegion, LineWidth, NumSteps, pvValueFunction,
							   pRender, ShapePath))
			{
				// Airbrush failed - just render the path without the airbrush effect
				pRender->DrawPath(pPath, this, ShapePath);
				return;
			}

			pMaskRegion->RestoreContext();

			// --- ClipRect test code

/////////////////////////////////////////////////////////////////////////////////////


			// --- We have drawn the airbrushed stroke - now, if the path is filled, we
			// will render the filled area, so that in semi-transparent cases, the
			// stroke will not "show through" from behind the filled area.			
			if (pPath->IsFilled)
			{
				ColourFillAttribute *pCFAttr = (ColourFillAttribute   *) pRender->GetCurrentAttribute(ATTR_FILLGEOMETRY);
				if (pCFAttr != NULL && (!pCFAttr->Colour.IsTransparent() || pCFAttr->IsABitmapFill()))
				{
					PathIsFilled = TRUE;

					pMaskRegion->SaveContext();

					ColourFillAttribute *pFillAttr = NULL;
					FillMappingAttribute *pMapAttr = NULL;

					// Obtain the object's transparent fill geometry
					TranspFillAttribute *pTransAttr = (TranspFillAttribute *) pRender->GetCurrentAttribute(ATTR_TRANSPFILLGEOMETRY);
					if (pTransAttr != NULL)
					{
						// Get a non-transparent version of the fill geometry
						pFillAttr = pTransAttr->MakeSimilarNonTranspFillGeometry(1.0);
						
						// Convert a fill mapping
						TranspFillMappingAttribute *pTransMapAttr = (TranspFillMappingAttribute *) pRender->GetCurrentAttribute(ATTR_TRANSPFILLMAPPING);
						if(pTransMapAttr != NULL)
							pMapAttr = pTransMapAttr->MakeSimilarNonTranspFillMapping();
					}

					// Setup region and draw path into it
					if (pFillAttr != NULL)
					{
						pMaskRegion->SetFillGeometry(pFillAttr, TRUE);

						if(pMapAttr != NULL)
							pMaskRegion->SetFillMapping(pMapAttr, TRUE);
					}
					else
						pMaskRegion->SetFillColour(DocColour(0, 0, 0));

					pMaskRegion->SetLineColour(DocColour(COLOUR_TRANS));
					pMaskRegion->DrawPath(pPath, NULL, ShapePath);

					pMaskRegion->RestoreContext();
				}
			}

			pMaskRegion->StopRender();
		}

		pMaskRegion->StopUsingGreyscaleContextDangerous();
	}

	// Extract the transparency mask bitmap from the render region
	OILBitmap *pOilBmp = pMaskRegion->ExtractBitmap();

	// We no longer need the RenderRegion, so scrap it.
	delete pMaskRegion;
	pMaskRegion = NULL;
	pPalette = NULL;

	// Now, render a rectangle to the output render region, using the transparency mask
	if (pOilBmp == NULL)
		return;

	KernelBitmap *pMask	= new KernelBitmap(pOilBmp, TRUE);

	if (pMask != NULL)
	{
		// Make sure the bitmap knows it's already a greyscale, else it will spend a lot of
		// time "converting" itself to a greyscale, and what's more, corrupting the grey levels
		// so that 255 (invisible) becomes 254 (slightly visible). Arrrrrgh!
		pMask->SetAsGreyscale();

		// Create a transparency attribute from our mask bitmap
		BitmapTranspFillAttribute Trans;

		// We don't call pTrans->AttachBitmap because it seems to be stupid, and causes ructions
		// when we try to attach a temporary bitmap. We thus do the same thing, but avoiding
		// its attempts to automatically screw us about.
		Trans.BitmapRef.Detach();
		Trans.BitmapRef.SetBitmap(pMask);

		Trans.SetStartPoint(&ClipRegion.lo);
		DocCoord EndPoint(ClipRegion.hi.x, ClipRegion.lo.y);
		Trans.SetEndPoint(&EndPoint);
		DocCoord EndPoint2(ClipRegion.lo.x, ClipRegion.hi.y);
		Trans.SetEndPoint2(&EndPoint2);

		UINT32 TValue = 0;
		Trans.SetStartTransp(&TValue);
		TValue = 255;
		Trans.SetEndTransp(&TValue);

		// Use the same transparency type as is set on the object being rendered (if any)
		{
			TranspFillAttribute *pTransAttr = (TranspFillAttribute *) pRender->GetCurrentAttribute(ATTR_TRANSPFILLGEOMETRY);

			if (pTransAttr != NULL)
				Trans.SetTranspType(pTransAttr->GetTranspType());
			else
				Trans.SetTranspType(TT_Mix);		// By default, we'll use Mix transparency
			
		}

		// --- OK, we finally got here! Render the stroke, using the transparency mask we just made
		pRender->SaveContext();

			Trans.Render(pRender);

			// Render the path. If it is filled, then we render the entire thing (fill & stroke) using
			// the current fill geometry (to get a shadow/feather effect)
			if (PathIsFilled)
			{
				// Render the entire thing (fill & stroke) in one go. We render a rectangle over the cliprect
				// so that we do everything in one go (we can't render the fill &7 stroke separately, or
				// the transparency will overlap & it'll look wrong)
				pRender->SetLineColour(DocColour(COLOUR_TRANS));		// Don't render a line

				Path Rect;
				Rect.Initialise();
				Rect.AddMoveTo(ClipRegion.lo);
				Rect.AddLineTo(DocCoord(ClipRegion.hix, ClipRegion.loy));
				Rect.AddLineTo(ClipRegion.hi);
				Rect.AddLineTo(DocCoord(ClipRegion.lox, ClipRegion.hiy));
				Rect.AddLineTo(ClipRegion.lo);
				Rect.IsFilled  = TRUE;
				Rect.IsStroked = FALSE;
				pRender->DrawPath(&Rect, this, ShapePath);
			}
			else
			{
				// Otherwise, create a filled-outline path for the entire stroke, and render it
				// !!!!****ToDo - for now, strokes always render flat-filled with the stroke colour
				StrokeColourAttribute *pStrokeColour = (StrokeColourAttribute *) pRender->GetCurrentAttribute(ATTR_STROKECOLOUR);
				if (pStrokeColour != NULL)
					pRender->SetFillColour(pStrokeColour->Colour);

				// Fill the holes
				pRender->SetWindingRule(NonZeroWinding);

				Path *pOutput = CreateVarWidthStroke(pPath, pRender, LineWidth);
				if (pOutput != NULL)
				{
					pRender->DrawPath(pOutput, NULL, ShapePath);
					delete pOutput;
					pOutput = NULL;
				}
			}

		pRender->RestoreContext();

		// Delete the kernel bitmap. This auto-deletes the OIL bitmap for us
		delete pMask;
	}
#endif
}



/********************************************************************************************

>	virtual BOOL PathProcessorStrokeAirbrush::IsDifferent(PathProcessorStroke *pOther);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/97

	Inputs:		pOther - The other PathProcessorStroke

	Returns:	TRUE if they're considered different, FALSE if they are equivalent

	Purpose:	Equality operator

	Notes:		The base class implementation compares the runtime classes of the
				2 objects to see if they are different classes. If they are the same
				class, it assumes they're cnsidered equal - derived classes should
				override this behaviour if further comparisons are necessary.

********************************************************************************************/

BOOL PathProcessorStrokeAirbrush::IsDifferent(PathProcessorStroke *pOther)
{
	ERROR3IF(pOther == NULL, "Illegal NULL param");

	if (GetRuntimeClass() != pOther->GetRuntimeClass())
		return(TRUE);

	PathProcessorStrokeAirbrush *pOtherAir = (PathProcessorStrokeAirbrush *)pOther;

	ValueFunction *pMyFunc	  = GetIntensityFunction();
	ValueFunction *pOtherFunc = pOtherAir->GetIntensityFunction();
	if (pMyFunc == NULL || pOtherFunc == NULL)
		return(TRUE);

	// We're the same if we share the same intensity function
	return(pMyFunc->IsDifferent(pOtherFunc));
}



/********************************************************************************************

>	virtual PathProcessorStrokeAirbrush *PathProcessorStrokeAirbrush::Clone(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/97

	Returns:	NULL if we're out of memory, else
				a pointer to a clone (exact copy) of this object

	Purpose:	To copy PathProcessorStrokeAirbrush or derived-class object

********************************************************************************************/

PathProcessorStroke *PathProcessorStrokeAirbrush::Clone(void)
{
	// Clone this object - this can be done by just creating a new one
	PathProcessorStrokeAirbrush *pClone = new PathProcessorStrokeAirbrush;

	if (pClone != NULL)
	{
		// And copy the (base class) parent-attribute pointer so we know when our
		// parent attribute is "current" in the render region.
		pClone->SetParentAttr(GetParentAttr());

		// And copy our intensity function, if any
		if (pIntensityFunction != NULL)
		{
			ValueFunction *pNewFunc = pIntensityFunction->Clone();
			pClone->SetIntensityFunction(pNewFunc);		// This is OK if it's NULL
		}
	}

	return(pClone);
}



/********************************************************************************************
>	virtual INT32 GetNumSteps(View *pView, INT32 LineWidth)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/2/97
	Inputs:		pView		- Relevant View (for zoom info)
				LineWidth	- LineWidth in millipoints
	Returns:	Number of steps which will produce a smooth graduated airbrush for this
				line at the current zoom, etc...

	Purpose:	To get the number of steps for an airbrush stroke
********************************************************************************************/

INT32 PathProcessorStrokeAirbrush::GetNumSteps(View *pView, INT32 LineWidth)
{
	if(pView == NULL)
		return 1;

	// Find out how many millipoints 1 pixel is
	const INT32 spw = pView->GetScaledPixelWidth().MakeLong();
	const INT32 sph = pView->GetScaledPixelHeight().MakeLong();
	const MILLIPOINT OnePixel = min(spw, sph);
  	TRACEUSER( "Richard", _T("1 pixel = %d\n"), OnePixel);

	if (OnePixel <= 0)
		return(1);

	// Now work out how many steps to render. The fewer steps, the faster we go, but this
	// trades off against quality. Because we start in the centre and increase in both
	// directions, we need to divide line width by the size of 2 pixels to get a step per pixel.
	// However, 3 is used because it means each ring of the airbrush is 1.5 pixels wider than the
	// last, which, with anti-aliasing, gives a very smooth effect. Going up to 4 makes each one
	// 2 pixels, and you can begin to see tree-ring effects in the airbrush.
	INT32 NumSteps = (INT32)(LineWidth / (OnePixel * 3));

	// Limit it to a sensible range	
	if(NumSteps < 1)
		NumSteps = 1;
	if(NumSteps > MaxAirbrushSteps)
		NumSteps = MaxAirbrushSteps;

  	TRACEUSER( "Richard", _T("NumSteps = %d\n"), NumSteps);

	return NumSteps;
}



/********************************************************************************************
>	LOGPALETTE *PathProcessorStrokeAirbrush::MakeGreyScalePalette(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/2/97
	Returns:	A 256 entry greyscale palette, or NULL if it failed

	Purpose:	Obtain a greyscale palette

	Notes:		A static palette is cached to save building one for every redraw

********************************************************************************************/

LOGPALETTE *PathProcessorStrokeAirbrush::MakeGreyScalePalette(void)
{
	static BYTE Pal[sizeof(LOGPALETTE) + 256*sizeof(PALETTEENTRY)];
	static LOGPALETTE *pPal = NULL;
	
	if (pPal == NULL)		// Only fill in the palette the first time we're called
	{
		pPal = (LOGPALETTE *) Pal;

		pPal->palVersion = 0x300;
		pPal->palNumEntries = 256;
		for (INT32 i = 0; i < 256; i++)
		{
			pPal->palPalEntry[i].peRed		= i;
			pPal->palPalEntry[i].peGreen	= i;
			pPal->palPalEntry[i].peBlue		= i;
			pPal->palPalEntry[i].peFlags	= 0;
		}
	}

	return pPal;
}



/********************************************************************************************

>	virtual BOOL PathProcessorStrokeAirbrush::RenderAirBrush(Path *pPath,
			GRenderBitmap *pRegion, INT32 LineWidth, INT32 NumSteps,
			ValueFunction *pvValueFunction, RenderRegion *pDestRegion,
			PathShape ShapePath)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/2/97
	Inputs:		pPath			- Path to render
				pRegion			- Region to render the airbrush tranparency mask into
				LineWidth		- Maximum linewidth
				NumSteps		- Number of Airbrush graduation steps
				pvValueFunction	- Value function for the edge rampyness
				pDestRegion		- Region we are rendering the airbrush to in the end
								  (needed to get current stroke attributes from)
				ShapePath		- The PathShape of the current shape.
	Returns:	TRUE if things went ok

	Purpose:	Main Airbrush rendering loop
********************************************************************************************/

BOOL PathProcessorStrokeAirbrush::RenderAirBrush(Path *pPath, GRenderBitmap *pRegion,
		INT32 LineWidth, INT32 NumSteps, ValueFunction *pvValueFunction,
		RenderRegion *pDestRegion, PathShape ShapePath)
{
	PORTNOTETRACE("other","PathProcessorStrokeAirbrush::RenderAirBrush - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if(pPath == NULL || pRegion == NULL || NumSteps <= 0)
	{
		ERROR3("PathProcessorStrokeAirbrush::RenderAirBrush given dodgy params");
		return FALSE;
	}

	// Fill the holes
	pRegion->SetWindingRule(NonZeroWinding);

	// Precalculate the trapezoid structure for stroking with
	TrapsList *pTrapezoids = PrepareTrapsForStroke(pPath, pDestRegion, LineWidth);
	if (pTrapezoids == NULL)
		return FALSE;

	INT32 LastIntensity = 255;

	// Use the previously obtained number of steps for the graduation
	for(INT32 i = NumSteps; i > 0; i--)
	{
		INT32 Intensity = 0;
		
		// What intensity should we use ?
		if(pvValueFunction == NULL)
		{
			// A linear ramp
			Intensity = (255 * i) / NumSteps;
		}
		else
		{
			// Use the value function
			double Position = (double)i / (double)NumSteps;			
			Intensity = (INT32)((1.0 - pvValueFunction->GetValue(Position)) * 255.0);
		}

		// Bit dodgy for 1st stroke, but then it shouldn't be the same as the last
		if(LastIntensity != Intensity)
		{							   
			LastIntensity = Intensity;

			if(Intensity != 255)
			{
				// --- Create the stroke outline by calling our helper function
				INT32 ThisWidth = (LineWidth * i) / NumSteps;

				Path *pOutput = CreateVarWidthStroke(pPath, pDestRegion, ThisWidth, pTrapezoids);
				if(pOutput != NULL)
				{
					ColourFillAttribute *pFillAttr = NULL;
					FillMappingAttribute *pMapAttr = NULL;

					if(pDestRegion != NULL)
					{
						// Obtain the object's transparent fill geometry
						TranspFillAttribute *pTransAttr = (TranspFillAttribute *) pDestRegion->GetCurrentAttribute(ATTR_TRANSPFILLGEOMETRY);
						if (pTransAttr == NULL)
						{
							// There is no transparency on this object - just use a flat transparency
							pFillAttr = new FlatFillAttribute;
							if (pFillAttr)
								pFillAttr->SetStartColour(&DocColour(Intensity, Intensity, Intensity));
						}
						else
						{
							// Get a non-transparent version of the fill geometry
							pFillAttr = pTransAttr->MakeSimilarNonTranspFillGeometry(1.0 - ((double)(Intensity) / 255.0));
							
							// Convert a fill mapping
							TranspFillMappingAttribute *pTransMapAttr = (TranspFillMappingAttribute *) pDestRegion->GetCurrentAttribute(ATTR_TRANSPFILLMAPPING);
							if(pTransMapAttr != NULL)
								pMapAttr = pTransMapAttr->MakeSimilarNonTranspFillMapping();
						}
					}

					// Setup region and draw path into it
					if(pFillAttr != NULL)
					{
						pRegion->SetFillGeometry(pFillAttr, TRUE);

						if(pMapAttr != NULL)
							pRegion->SetFillMapping(pMapAttr, TRUE);
					}
					else
						pRegion->SetFillColour(DocColour(Intensity, Intensity, Intensity));
					
					pRegion->DrawPath(pOutput, NULL, ShapePath);
					delete pOutput;
				}
			}
		}
	}

	delete pTrapezoids;

	return TRUE;
#else
	return FALSE;
#endif
}



/********************************************************************************************

>	ValueFunction *PathProcessorStrokeAirbrush::GetIntensityFunction(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/97

	Returns:	NULL (if it fails horribly), else a pointer to the intensity ValueFunction

	Purpose:	Sets the intensity function which will be used when rendering this airbrush.
				The function controls the intensity (opacity) of the airbrush at Position
				values where Position 0.0 represents the airbrush centreline, and 1.0
				represents the maximum width.

	SeeAlso:	PathProcessorStrokeAirbrush::GetIntensityFunction

********************************************************************************************/

ValueFunction *PathProcessorStrokeAirbrush::GetIntensityFunction(void)
{
	if (pIntensityFunction == NULL)
		pIntensityFunction = new ValueFunctionRampS(1.0, 0.0);

	return(pIntensityFunction);
}



/********************************************************************************************

>	void PathProcessorStrokeAirbrush::SetIntensityFunction(ValueFunction *pFunc)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/97

	Inputs:		pFunc - The new ValueFunction to use
				A value of NULL will set the airbrush to the default setting (using
				an ValueFunctionRampS from 1.0 to 0.0)

	Purpose:	Sets the intensity function which will be used when rendering this airbrush.
				The function controls the intensity (opacity) of the airbrush at Position
				values where Position 0.0 represents the airbrush centreline, and 1.0
				represents the maximum width.

	SeeAlso:	PathProcessorStrokeAirbrush::GetIntensityFunction

********************************************************************************************/

void PathProcessorStrokeAirbrush::SetIntensityFunction(ValueFunction *pFunc)
{
	if (pIntensityFunction != NULL)
		delete pIntensityFunction;

	pIntensityFunction = pFunc;
}

