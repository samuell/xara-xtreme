// $Id: fillval.cpp 662 2006-03-14 21:31:49Z alex $
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

// This file holds all the AttributeValueclasses to do with path
// filling attributes.

/*
*/

#include "camtypes.h"

#include "ink.h"
#include "rndrgn.h"
#include "grndrgn.h"
#include "osrndrgn.h"
#include "macros.h"
#include "app.h"
#include "opgrad.h"
#include "attrmgr.h"

#include "fillval.h"
#include "lineattr.h"
#include "attrmap.h"

#include "fracfill.h"
#include "bitmap.h" 	// For Test Only
#include "bitmpinf.h"
#include "oilbitmap.h" 	// For Test Only
#include "dibutil.h" 	// For Test Only

#include "progress.h"

//#include "devcolor.h"	// For Test Only
//#include "colcontx.h"	// For Test Only

//#include "will.h"		// For bitmap error ID
#include "monotime.h"	// For Fractal Seed

#include "blendatt.h"
//#include "jason.h"		// For Default Colour Text IDs
#include "colormgr.h"
#include "docview.h"

#include "nodemold.h"	// For NodeMould definition
#include "moldpers.h"	// For MouldPerspective definition

//#include "will2.h"

#ifndef WEBSTER
//#include "bfxalu.h"		// For BfxALU::IsGreyscaleBitmap -- see WEBSTER comment below
#endif

//#include "gerry.h"		// for _R(IDS_REDNAME) & _R(IDS_YELLOWNAME)
#include "bmpcomp.h"
#include "noisef.h"

#include "pathstrk.h"	// For PathStrokerVector
#include "fillramp.h"	// For colour/transparency ramps
#include "pathutil.h"

DECLARE_SOURCE("$Revision: 662 $");


// AttributeValue classes
//
CC_IMPLEMENT_DYNAMIC(FillGeometryAttribute,			 	AttributeValue)
CC_IMPLEMENT_DYNAMIC(ColourFillAttribute,			 	FillGeometryAttribute)
CC_IMPLEMENT_DYNAMIC(TranspFillAttribute,			 	FillGeometryAttribute)

CC_IMPLEMENT_DYNCREATE(FlatFillAttribute,    			ColourFillAttribute)
CC_IMPLEMENT_DYNCREATE(GradFillAttribute,    			ColourFillAttribute)
CC_IMPLEMENT_DYNCREATE(LinearFillAttribute,    			GradFillAttribute)
CC_IMPLEMENT_DYNCREATE(RadialFillAttribute,    			GradFillAttribute)
CC_IMPLEMENT_DYNCREATE(ConicalFillAttribute,    		GradFillAttribute)
CC_IMPLEMENT_DYNCREATE(SquareFillAttribute,    			GradFillAttribute)
CC_IMPLEMENT_DYNCREATE(BitmapFillAttribute,    			GradFillAttribute)
CC_IMPLEMENT_DYNCREATE(FractalFillAttribute,    		BitmapFillAttribute)
CC_IMPLEMENT_DYNCREATE(NoiseFillAttribute,				BitmapFillAttribute)
CC_IMPLEMENT_DYNCREATE(ThreeColFillAttribute,    		GradFillAttribute)
CC_IMPLEMENT_DYNCREATE(FourColFillAttribute,    		ThreeColFillAttribute)

CC_IMPLEMENT_DYNCREATE(FlatTranspFillAttribute,    		TranspFillAttribute)
CC_IMPLEMENT_DYNCREATE(GradTranspFillAttribute,    		TranspFillAttribute)
CC_IMPLEMENT_DYNCREATE(LinearTranspFillAttribute,    	GradTranspFillAttribute)
CC_IMPLEMENT_DYNCREATE(RadialTranspFillAttribute,    	GradTranspFillAttribute)
CC_IMPLEMENT_DYNCREATE(ConicalTranspFillAttribute,    	GradTranspFillAttribute)
CC_IMPLEMENT_DYNCREATE(SquareTranspFillAttribute,    	GradTranspFillAttribute)
CC_IMPLEMENT_DYNCREATE(BitmapTranspFillAttribute,    	GradTranspFillAttribute)
CC_IMPLEMENT_DYNCREATE(FractalTranspFillAttribute,    	BitmapTranspFillAttribute)
CC_IMPLEMENT_DYNCREATE(NoiseTranspFillAttribute,		BitmapTranspFillAttribute)
CC_IMPLEMENT_DYNCREATE(ThreeColTranspFillAttribute,		GradTranspFillAttribute)
CC_IMPLEMENT_DYNCREATE(FourColTranspFillAttribute,		ThreeColTranspFillAttribute)

CC_IMPLEMENT_DYNCREATE(FillEffectAttribute,    			AttributeValue)
CC_IMPLEMENT_DYNCREATE(FillEffectFadeAttribute, 		FillEffectAttribute)
CC_IMPLEMENT_DYNCREATE(FillEffectRainbowAttribute, 		FillEffectAttribute)
CC_IMPLEMENT_DYNCREATE(FillEffectAltRainbowAttribute, 	FillEffectAttribute)

CC_IMPLEMENT_DYNCREATE(FillMappingAttribute,    		AttributeValue)
CC_IMPLEMENT_DYNCREATE(FillMappingLinearAttribute, 		FillMappingAttribute)
CC_IMPLEMENT_DYNCREATE(FillMappingSinAttribute, 		FillMappingAttribute)

CC_IMPLEMENT_DYNCREATE(TranspFillMappingAttribute,    	AttributeValue)
CC_IMPLEMENT_DYNCREATE(TranspFillMappingLinearAttribute,TranspFillMappingAttribute)
CC_IMPLEMENT_DYNCREATE(TranspFillMappingSinAttribute, 	TranspFillMappingAttribute)

CC_IMPLEMENT_DYNCREATE(StrokeColourAttribute,  	ColourFillAttribute)
CC_IMPLEMENT_DYNCREATE(StrokeTranspAttribute,  	TranspFillAttribute)

CC_IMPLEMENT_DYNCREATE(MouldAttribute,	AttributeValue)

// Declare smart memory handling in Debug builds
//#define	new	CAM_DEBUG_NEW

// funky functions for use with our profile gadgets ....

// one to set em'
void FillGeometryAttribute::SetProfile (CProfileBiasGain& SetWith)
{
	DiagramMapper = SetWith;
}

// and one to get em'
CProfileBiasGain& FillGeometryAttribute::GetProfile ()
{
	return (DiagramMapper);
}

CProfileBiasGain* FillGeometryAttribute::GetProfilePtr ()
{
	return (&DiagramMapper);
}



/////////////////////////////////////////////////////////////////////////////////////////////
// Stroke Moulding macros - these are used in most of the MouldIntoStroke functions
// to keep the source code readable & tidy
/////////////////////////////////////////////////////////////////////////////////////////////

#define JCW_MOULDPOINT(pAttr, Point)															\
		if ((Get##Point()) != NULL)															\
		{																						\
			DocCoord Moulded = pMoulder->MapCoord(Get##Point());								\
			pAttr->Set##Point(&Moulded);														\
		}

#define JCW_MOULDTRANS(pAttr, Point)															\
			if ((Get##Point()) != NULL)														\
			{																					\
				UINT32 Trans = 255 - (UINT32) ( ((double)(255 - *(Get##Point()))) * TransScale );	\
				pAttr->Set##Point(&Trans);													\
			}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//							FillGeometryAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	FillGeometryAttribute::FillGeometryAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/94
	Purpose:	Default Constuctor for fill attribute values. 
				Sets the default colours to Black and White.
				Sets the Control Points to DocCoord(0,0).
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

FillGeometryAttribute::FillGeometryAttribute()
{
}

/********************************************************************************************

>	FillGeometryAttribute& FillGeometryAttribute::operator=(FillGeometryAttribute& FillAttrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		FillAttrib - the attribute to copy, which must be an AttrFillGeometry

	Returns:	Usual semantics for equality.
	Purpose:	Make the Attribute the same as the other. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.

********************************************************************************************/

FillGeometryAttribute& FillGeometryAttribute::operator=(FillGeometryAttribute& FillAttrib)
{
	SetStartColour(FillAttrib.GetStartColour());
	SetEndColour(FillAttrib.GetEndColour());

	SetStartTransp(FillAttrib.GetStartTransp());
	SetEndTransp(FillAttrib.GetEndTransp());

	// Copy the profile (CGS)
	SetProfile (FillAttrib.GetProfile ());

	// Copy the control points
	SetStartPoint(FillAttrib.GetStartPoint());
	SetEndPoint(FillAttrib.GetEndPoint());
	SetEndPoint2(FillAttrib.GetEndPoint2());

	CopyBitmap(FillAttrib.GetBitmap());
	SetAspectLock(FillAttrib.IsAspectLocked());
	SetColourRamp(FillAttrib.GetColourRamp());

	return *this;
}

/********************************************************************************************

>	INT32 FillGeometryAttribute::operator==(const FillGeometryAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 FillGeometryAttribute::operator==(const FillGeometryAttribute& Attrib)
{
	return FALSE;
}

/********************************************************************************************

>	void FillGeometryAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	FillGeometryAttribute; RenderStack; AttributeValue; NodeAttribute;
				FillGeometryAttribute::Render; FillGeometryAttribute::Restore;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void FillGeometryAttribute::SimpleCopy(AttributeValue *pValue)
{
	// We may as well just use our assignment operator.
	*this = *((FillGeometryAttribute*)pValue);
}


/********************************************************************************************

>	BOOL FillGeometryAttribute::IsDifferent(AttributeValue *pAttr)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL FillGeometryAttribute::IsDifferent(AttributeValue *pAttr)
{
	// This must be at least a FillGeometryAttribute...
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(FillGeometryAttribute)), 
		   		"Different attribute types in AttributeValue::IsDifferent()");

	// Check they are NOT the same.
	return ( !(*((FillGeometryAttribute *)pAttr) == *this) ); 
}

/********************************************************************************************

>	KernelBitmap* FillGeometryAttribute::GenerateFractalBitmap( INT32 Seed, double Graininess, 
															double Gravity, double Squash, 
															UINT32 Dpi)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Inputs:		NewDpi - New DPI to generate the Fractal at.
	Purpose:	Recalculates the Fractal at a new Dpi.
	SeeAlso:	FractalTranspFillAttribute::GetDPI

********************************************************************************************/

KernelBitmap* FillGeometryAttribute::GenerateFractalBitmap( INT32 Seed, double Graininess, 
															double Gravity, double Squash, 
															UINT32 Dpi)
{
	BOOL Tile = GetTileable();

/*
	switch (GetTesselation())
	{
		case RT_Simple:
			Tile = FALSE;
			break;

		case RT_Repeating:
			Tile = TRUE;
			break;

		case RT_RepeatInverted:
			Tile = FALSE;
			break;

		default:
			Tile = TRUE;
			break;
	}

	SetTileable(Tile);
*/

	TRACEUSER( "Mike", _T("Seed          = %d\n"),Seed);
	TRACEUSER( "Mike", _T("Graininess    = %f\n"),Graininess);
	TRACEUSER( "Mike", _T("Gravity       = %f\n"),Gravity);
	TRACEUSER( "Mike", _T("Squash        = %f\n"),Squash);
	TRACEUSER( "Mike", _T("Dpi           = %d\n"),Dpi);

	// First make a fractal from the variables stored in the fill.
	PlasmaFractalFill TheFractal(Seed, Tile, Squash, Graininess, Gravity);

	// Get the fill control points
	DocCoord Start = *GetStartPoint();
	DocCoord End   = *GetEndPoint();
	DocCoord End2  = *GetEndPoint2();

	if (Dpi == 0)
		Dpi = AttrFillGeometry::FractalDPI;

	if (Dpi > 72000)
		Dpi = 72000;

	// Find the size of a pixel.  Note: This assumes pixels are square at the moment.
	MILLIPOINT PixelWidth = 72000/Dpi;

	INT32 FracWidth  = INT32(Start.Distance(End));
	INT32 FracHeight = INT32(Start.Distance(End2));

	TRACEUSER( "Mike", _T("PixelWidth 	= %d\n"),PixelWidth);
	TRACEUSER( "Mike", _T("Frac Width 	= %d\n"),FracWidth);
	TRACEUSER( "Mike", _T("Frac Height 	= %d\n"),FracHeight);

	// Calculate the size of the fractal (in Pixels) from the control points
   	INT32 FracPixWidth 	= FracWidth / PixelWidth;
   	INT32 FracPixHeight 	= FracHeight/ PixelWidth;

	TRACEUSER( "Mike", _T("Pix Width	= %d\n"),FracPixWidth);
	TRACEUSER( "Mike", _T("Pix Height 	= %d\n"),FracPixHeight);

	if (FracPixWidth == 0)
		FracPixWidth = 2;

	if (FracPixHeight == 0)
		FracPixHeight = 2;

	// The 'actual' dimensions of a fractal must be square and a power of 2.
	// 'GetDimension' finds an appropriate value for this dimension.
	// It will be the next power of 2 higher than Width or Height (whichever is biggest).
	INT32 FracPixDim 	= TheFractal.GetDimension(FracPixWidth, FracPixHeight);

	if (FracPixDim > AttrFillGeometry::MaxFractalSize)
		FracPixDim = AttrFillGeometry::MaxFractalSize;

	// Set the fractal dimensions so we can see if a match is available
	// in the fractal cache
	SetFractalDim(FracPixDim);

	TRACEUSER( "Mike", _T("Fractal Dim	= %d\n"),FracPixDim);

	CachedFractal* ExistingFrac = 
			GetApplication()->GetGlobalFractalList()->CheckFractalBitmap(this);

	if (ExistingFrac != NULL)
	{
		// This fractal already exist, so lets re-use it
		OILBitmap* ExistingBmp = ExistingFrac->GetCachedFractal()->GetBitmap()->ActualBitmap;
		KernelBitmap* ExistingFractal = new KernelBitmap(ExistingBmp, TRUE);
		return ExistingFractal;
	}

	String_64 FracText(_R(IDS_K_FILLVAL_GENFRACT));
	BeginSlowJob(-1, FALSE, &FracText);

	Dpi = (FracPixDim * 72000)/(FracWidth >= FracHeight ? FracWidth : FracHeight);

	// Create the Bitmap !!
	KernelBitmap* Bitmap = new KernelBitmap(FracPixDim, FracPixDim, 8, Dpi, TRUE);

	if (Bitmap == NULL)
	{
		EndSlowJob();
		ERROR1(NULL, _R(IDE_FRACTMEM));
	}

	// Make this into a fractal bitmap
	Bitmap->SetAsFractal();

	// Render the Fractal into the bitmap
 	BOOL ok = TheFractal.DoFill(Bitmap, FracPixDim, 0, 0);

	EndSlowJob();

	if (!ok) TRACEUSER( "Mike", _T("DoFill Failed\n"));
	ERROR1IF(!ok, Bitmap, _R(IDE_FRACTFAILED));

	return Bitmap;
}

FIXED16 FillGeometryAttribute::GetGraininess()
{
	return AttrFillGeometry::FractalGraininess;
}

FIXED16 FillGeometryAttribute::GetGravity()
{
	return AttrFillGeometry::FractalGravity;
}



/********************************************************************************************

>	KernelBitmap* FillGeometryAttribute::GenerateNoiseBitmap(FIXED16 grain,
															 INT32   seed)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/97
	Inputs:		Parameters for a new noisy fractal
	Purpose:	Recalculates the Fractal at a new Dpi.
	SeeAlso:	

********************************************************************************************/

KernelBitmap* FillGeometryAttribute::GenerateNoiseBitmap(FIXED16 grain, INT32 seed)
{
	NoiseFractalFill TheFractal;

	// Get the fill control points
	DocCoord Start = *GetStartPoint();
	DocCoord End   = *GetEndPoint();
	DocCoord End2  = *GetEndPoint2();

	INT32 UAxisLength = INT32(Start.Distance(End));
	INT32 VAxisLength = INT32(Start.Distance(End2));

	INT32 Dpi = 0;
	if (Dpi == 0)
		Dpi = AttrFillGeometry::FractalDPI;

/*
	// Find the size of a pixel.  Note: This assumes pixels are square at the moment.
	MILLIPOINT SizeOfPixel = 72000/Dpi;

	// Calculate the size of the fractal (in Pixels) from the control points
	INT32 NumPixelsX = UAxisLength / SizeOfPixel;
	INT32 NumPixelsY = VAxisLength / SizeOfPixel;

	if (NumPixelsX <= 0) NumPixelsX = 2;
	if (NumPixelsY <= 0) NumPixelsY = 2;

	// Make this a square
	INT32 FracPixDim = LMax(NumPixelsX, NumPixelsY);

	if (FracPixDim > AttrFillGeometry::MaxFractalSize)
		FracPixDim = AttrFillGeometry::MaxFractalSize;
*/
	INT32 FracPixDim = 256;

	// save this size, we'll use it for caching!
	SetFractalDim(FracPixDim);

	// try to find a cached fractal
	CachedFractal* ExistingFrac = 
		GetApplication()->GetGlobalFractalList()->CheckFractalBitmap(this);

	if (ExistingFrac != NULL)
	{
		// This fractal already exist, so lets re-use it
		OILBitmap* ExistingBmp = ExistingFrac->GetCachedFractal()->GetBitmap()->ActualBitmap;
		KernelBitmap* ExistingFractal = new KernelBitmap(ExistingBmp, TRUE);
		return ExistingFractal;
	}

	String_64 FracText(_R(IDS_K_FILLVAL_GENFRACT));
	BeginSlowJob(-1, FALSE, &FracText);

	// calc the dpi of the image
	Dpi = (FracPixDim * 72000) / ( LMax( UAxisLength , VAxisLength) );

	// Create the Bitmap
	KernelBitmap* Bitmap = new KernelBitmap(FracPixDim, FracPixDim, 8, Dpi, TRUE);

	if (Bitmap == NULL)
	{
		EndSlowJob();
		ERROR1(NULL, _R(IDE_FRACTMEM));
	}

	// Make this into a fractal bitmap
	Bitmap->SetAsFractal();

	// Render the Fractal into the bitmap
 	double scale = grain.MakeDouble();
	BOOL ok = TheFractal.DoFill(scale, seed, Bitmap);

	EndSlowJob();

	if (!ok) TRACEUSER( "Mike", _T("DoFill Failed\n"));
//	ERROR1IF(!ok, Bitmap, _R(IDE_FRACTFAILED));

	return Bitmap;
}


/********************************************************************************************

>	void FillGeometryAttribute::CacheFractalData(FillGeometryAttribute *pCachedFractal)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Inputs:		pCachedFractal = A pointer to the fractal in the cache
	Returns:	-
	Purpose:	Copies any data from this FractalFillAttribute into the cached fractal
				pointed to on entry. This data is then checked via IsSameAsCachedFractal

********************************************************************************************/

void FillGeometryAttribute::CacheFractalData(FillGeometryAttribute *pCachedFractal)
{	
	ERROR3("FillGeometryAttribute::CacheFractalData() should not have been called");
}

/********************************************************************************************

>	BOOL FillGeometryAttribute::IsSameAsCachedFractal(FillGeometryAttribute *pCachedFractal)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Inputs:		pCachedFractal = A pointer to the fractal in the cache
	Returns:	TRUE if this fractal matches the cached fractal
				FALSE if not
	Purpose:	A virtual comparison operator used by the fractal cache to check for a
				matching fractal.

********************************************************************************************/

BOOL FillGeometryAttribute::IsSameAsCachedFractal(FillGeometryAttribute *pCachedFractal)
{
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//							ColourFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	ColourFillAttribute::ColourFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

ColourFillAttribute::ColourFillAttribute()
{
	Colour = DocColour(BLACK);
}

/********************************************************************************************

>	BOOL ColourFillAttribute::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers fill type attribute, and provides a default attribute to give
				transparent flat fills, i.e. actually not filled at all.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL ColourFillAttribute::Init()
{
	// Default to simple transparent fill colour
	FlatFillAttribute *pAttr = new FlatFillAttribute;
	if (pAttr==NULL)
		// just return FALSE, as the error message was set by new
		return FALSE;

//	pAttr->Colour = AttributeManager::DefaultBlack;
	pAttr->Colour = DocColour(COLOUR_NONE);
	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising ColourFillAttribute");
	ERROR2IF(ID != ATTR_FILLGEOMETRY, FALSE, "Incorrect ID for ColourFillAttribute");

	return TRUE;
}

/********************************************************************************************

>	FillGeometryAttribute& ColourFillAttribute::operator=(FillGeometryAttribute& FillAttrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		FillAttrib - the attribute to copy, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	Make the Attribute the same as the other. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.

********************************************************************************************/

FillGeometryAttribute& ColourFillAttribute::operator=(FillGeometryAttribute& FillAttrib)
{
	if ( !FillAttrib.IsABitmapFill() && this->IsABitmapFill() )
	{
		SetStartColour(NULL);
		SetEndColour(NULL);
	}
	else if ( FillAttrib.IsAFlatFill() && this->IsAGradFill() )
	{
		SetStartColour(NULL);
		SetEndColour(FillAttrib.GetStartColour());
	}
	else if ( FillAttrib.IsAFlatFill() && this->IsAFractalFill() )
	{
		SetStartColour(NULL);
		SetEndColour(FillAttrib.GetStartColour());
	}
	else if ( FillAttrib.IsAGradFill() && this->IsAFlatFill() )
	{
		SetStartColour(FillAttrib.GetEndColour());
		SetEndColour(NULL);
	}
	else
	{
		SetStartColour(FillAttrib.GetStartColour());
		SetEndColour(FillAttrib.GetEndColour());
		SetEndColour2(FillAttrib.GetEndColour2());
		SetEndColour3(FillAttrib.GetEndColour3());
	}

	// Copy the fills profile
	SetProfile (FillAttrib.GetProfile ());

	// Copy the control points
	SetStartPoint(FillAttrib.GetStartPoint());
	SetEndPoint(FillAttrib.GetEndPoint());
	SetEndPoint2(FillAttrib.GetEndPoint2());

	// Bitmap Virtual points
	if ( FillAttrib.IsAKindOfBitmapFill() &&
		 !this->IsAKindOfBitmapFill() )
	{
		DocCoord Start = *FillAttrib.GetStartPoint();
		DocCoord End   = *FillAttrib.GetEndPoint();
		DocCoord End2  = *FillAttrib.GetEndPoint2();

		GetBitmapVirtualPoints( Start, End, End2,
					   		    GetStartPoint(),  GetEndPoint(),  GetEndPoint2());
	}
 	else if ( !FillAttrib.IsAKindOfBitmapFill() && 
 			  this->IsAKindOfBitmapFill() )
	{
		GetBitmapRealPoints(*GetStartPoint(), *GetEndPoint(), *GetEndPoint2(),
					 		 GetStartPoint(),  GetEndPoint(),  GetEndPoint2());
	}

	if (FillAttrib.IsPerspective())
	{
		MakePerspective();
		SetEndPoint3(FillAttrib.GetEndPoint3());
	}
	else
	{
		RemovePerspective();
	}

	// are both these fractal fills?
	if ( FillAttrib.IsAKindOfBitmapFill() && this->IsAKindOfBitmapFill())
	{
		// are these bitmap items the same?
		if (FillAttrib.GetRuntimeClass() == this->GetRuntimeClass())
			CopyBitmap(FillAttrib.GetBitmap());
	}
	else
		CopyBitmap(FillAttrib.GetBitmap());

	SetTesselation(FillAttrib.GetTesselation());
	SetAspectLock(FillAttrib.IsAspectLocked());
	SetColourRamp(FillAttrib.GetColourRamp());

	return *this;
}

/********************************************************************************************

>	INT32 ColourFillAttribute::operator==(const ColourFillAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 ColourFillAttribute::operator==(const FillGeometryAttribute& Attrib)
{
	if (GetRuntimeClass() != Attrib.GetRuntimeClass())
		return FALSE;

	return (Colour == ((ColourFillAttribute*)&Attrib)->Colour);
}

/********************************************************************************************

>	void ColourFillAttribute::Render(RenderRegion *pRegion)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the fill geometry attribute for the given render region. i.e. all
				paths filled will now be filled with this fill geometry.
	SeeAlso:	ColourFillAttribute; RenderStack; AttributeValue; NodeAttribute;
				ColourFillAttribute::Restore; ColourFillAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void ColourFillAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "Removed RenderRegion usage" )
	pRegion->SetFillGeometry(this, Temp);
}

/********************************************************************************************

>	void ColourFillAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the fill type attribute for the given render region. i.e. all
				paths filled will now be filled with this fill attribute.
	SeeAlso:	ColourFillAttribute; RenderStack; AttributeValue; NodeAttribute;
				ColourFillAttribute::Render; ColourFillAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void ColourFillAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "Removed RenderRegion usage" )
	pRegion->RestoreFillGeometry(this, Temp);
}

/********************************************************************************************

>	void ColourFillAttribute::SetStartColour(DocColour* NewCol)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the Start colour of this fill

********************************************************************************************/

void ColourFillAttribute::SetStartColour(DocColour* NewCol)
{
	if (NewCol == NULL)
	{
//#pragma message( __LOCMSG__ "Removed ColorManager usage" )
		AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
											_R(IDS_WHITENAME), &Colour);
	}
	else
		Colour = *NewCol;
}


/********************************************************************************************

>	BOOL ColourFillAttribute::ContainsNamedColour()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Returns:	TRUE if any one of the colours used by this attribute is a named colour of has
				the reserved flag set that says it was created from a named colour.
	Purpose:	

********************************************************************************************/

BOOL ColourFillAttribute::ContainsNamedColour()
{
	// check through all the colours and see if they are named

	DocColour* pColour = GetStartColour();
	if (pColour != NULL)
	{
		if (pColour->IsNamed())
			return TRUE;
	}
	pColour = GetEndColour();
	if (pColour != NULL)
	{
		if (pColour->IsNamed())
			return TRUE;
	}
	pColour = GetEndColour2();
	if (pColour != NULL)
	{
		if (pColour->IsNamed())
			return TRUE;
	}
	pColour = GetEndColour3();
	if (pColour != NULL)
	{
		if (pColour->IsNamed())
			return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//							FlatFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	FlatFillAttribute::FlatFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

FlatFillAttribute::FlatFillAttribute()
{
}

/********************************************************************************************

>	NodeAttribute *FlatFillAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrFlatColourFill node from this flat fill colour attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode; AttrFlatColourFill

********************************************************************************************/

NodeAttribute *FlatFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrFlatColourFill *pAttr = new AttrFlatColourFill;
	if (pAttr==NULL)
		// The error has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	void FlatFillAttribute::SetStartColour(DocColour* NewCol)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the Start colour of this fill

********************************************************************************************/

void FlatFillAttribute::SetStartColour(DocColour* NewCol)
{
//#pragma message( __LOCMSG__ "FlatFillAttribute::SetStartColour - do nothing" )
//	TRACE( _T("Warning - FlatFillAttribute::SetStartColour called\n") );
	if (NewCol == NULL)
	{
		AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
											_R(IDS_BLACKNAME), &Colour);
	}
	else
		Colour = *NewCol;
}

/********************************************************************************************

>	virtual AttributeValue *FlatFillAttribute::MouldIntoStroke(PathStrokerVector *pMoulder,
																			double TransScale);
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/97

	Inputs:		pMoulder	- A PathStrokerVector which knows how to translate points to "mould" them
							  (May be NULL, in which case moulding of points does not occur)

				TransScale	- A fraction between 0.0 and 1.0, by which any transparency
							  values in this geometry will be scaled, allowing the caller to
							  effectively apply a flat transparency level to everything that is
							  moulded. Use 1.0 to leave transparency unaltered.

	Returns:	NULL

	Purpose:	Helper function for the PathStrokerVector class, which "moulds" clipart
				subtrees to lie along an arbitrary path.

				This function is called to mould fill geometries, so that fill endpoints
				are translated to appropriate positions in the destination envelope, and
				allows the caller to effectively apply a flat transparency by scaling
				any transparency values in this geometry by the given fraction.

********************************************************************************************/

AttributeValue *FlatFillAttribute::MouldIntoStroke(PathStrokerVector *pMoulder, double TransScale)
{
	// There is no need to mould flat fills at all
	return(NULL);
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//							GradFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	GradFillAttribute::GradFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

GradFillAttribute::GradFillAttribute()
{
//	AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
//										_R(IDS_WHITENAME), &EndColour);
	AttributeManager::FindDefaultColour(NULL,
										_R(IDS_WHITENAME), &EndColour);

	StartPoint = DocCoord(0,0);
	EndPoint   = DocCoord(0,0);
	
	m_pColourRamp = NULL;
}

/********************************************************************************************

>	GradFillAttribute::~GradFillAttribute()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/3/97
	Purpose:	All new destructor used to delete a colour ramp

********************************************************************************************/

GradFillAttribute::~GradFillAttribute()
{
	DeleteColourRamp();
}

/********************************************************************************************

>	void GradFillAttribute::DeleteColourRamp()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/3/97
	Inputs:		-
	Purpose:	Function to remove the colour ramp from this grad fill.

********************************************************************************************/

void GradFillAttribute::DeleteColourRamp()
{
	if (m_pColourRamp!=NULL)
	{
		delete m_pColourRamp;
		m_pColourRamp=NULL;
	}
}

/********************************************************************************************

>	BOOL GradFillAttribute::SameColourRampAs(ColourRamp *pOtherRamp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/3/97
	Inputs:		pOtherRamp = the colour ramp to match this one agains
	Returns:	TRUE if the colour ramps are the same
				FALSE if not
	Purpose:	Check whether this colour ramp matches that pointed to by pOtherRamp

********************************************************************************************/

BOOL GradFillAttribute::SameColourRampAs(ColourRamp *pOtherRamp)
{
	// Have we got a transparency ramp at the moment?
	if (m_pColourRamp==NULL)
		return (pOtherRamp==NULL);

	if (pOtherRamp==NULL)
		return FALSE;

	return (!(m_pColourRamp->IsDifferentTo(pOtherRamp)));
}

/********************************************************************************************

>	BOOL GradFillAttribute::SetColourRamp(ColourRamp *pRamp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/3/97
	Inputs:		pRamp = a pointer to a colour ramp
	Returns:	TRUE if the colour ramp was set, FALSE if not
	Purpose:	Set this colour ramp to the version passed in. If pRamp is NULL, the colour
				ramp in this object will be emptied

********************************************************************************************/

BOOL GradFillAttribute::SetColourRamp(ColourRamp *pRamp)
{
	DeleteColourRamp();
	if (pRamp==NULL)
		return TRUE;
	
	m_pColourRamp = new ColourRamp();
	if (m_pColourRamp==NULL)
		return FALSE;

	*m_pColourRamp = *pRamp;
	return TRUE;
}


/********************************************************************************************

>	ColourRamp* GradFillAttribute::MakeNewColourRamp()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/3/97
	Inputs:		-
	Returns:	TRUE if the colour ramp was set, FALSE if not
	Purpose:	Set this colour ramp to the version passed in. If pRamp is NULL, the colour
				ramp in this object will be emptied

********************************************************************************************/

ColourRamp* GradFillAttribute::MakeNewColourRamp()
{
	DeleteColourRamp();
	m_pColourRamp = new ColourRamp();
	return m_pColourRamp;
}


/********************************************************************************************

>	DocCoord GradFillAttribute::GetGeometryCoord(float pos) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/97
	Inputs:		pos = a parameter between 0 and 1
	Returns:	A coordinate which relates the input parameter to a coordinate space
				on this geometry. For instance a linear fill would have a linear parameter
				space, 0 at the start coordinate, increasing to 1 at the end coordinate
	Purpose:	Find the absolute position in geometry coordinate space for the given
				parameter.

********************************************************************************************/

DocCoord GradFillAttribute::GetGeometryCoord(float pos) const
{
	// This and all derived classes will inherit a linear coordinate space
	return DocCoord::PositionPointFromRatio(StartPoint,EndPoint,(double)pos);
}


/********************************************************************************************

>	float GradFillAttribute::GetGeometryParam(const DocCoord& c) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/97
	Inputs:		c = a coordinate
	Returns:	A parameter p, 0<=p<=1, such that GetGeometryCoord(p) is the closest
				coordinate in the geometry to the input c.
	Purpose:	Find the parameter for the closest point to c on this geometry.

********************************************************************************************/

float GradFillAttribute::GetGeometryParam(const DocCoord& c) const
{
	// ok we're a linear geometry so find the closest point to a line type of thing.
	// use a handy util written by that other fab bloke called Mike.
	DocCoord Coords[2];
	Coords[0] = StartPoint;
	Coords[1] = EndPoint;
	double p;
	PathUtil::SqrDistanceToLine(Coords, c, &p);
	return (float)p;
}



/********************************************************************************************

>	FillGeometryAttribute& GradFillAttribute::operator=(FillGeometryAttribute& FillAttrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		FillAttrib - the attribute to copy, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	Make the Attribute the same as the other. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.

********************************************************************************************/

FillGeometryAttribute& GradFillAttribute::operator=(FillGeometryAttribute& FillAttrib)
{
	return ColourFillAttribute::operator=(FillAttrib);
}

/********************************************************************************************

>	INT32 GradFillAttribute::operator==(const FillGeometryAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 GradFillAttribute::operator==(const FillGeometryAttribute& Attrib)
{
	if (GetRuntimeClass() != Attrib.GetRuntimeClass())
		return FALSE;

	GradFillAttribute* pAttrib = (GradFillAttribute*)&Attrib;

	if (IsPerspective())
	{ 
		if (!pAttrib->IsPerspective())
			return FALSE;

		if (*GetEndPoint2() != *pAttrib->GetEndPoint2())
			return FALSE;

		if (*GetEndPoint3() != *pAttrib->GetEndPoint3())
			return FALSE;
	}
	
	// check the colour ramps match
	if (!SameColourRampAs(pAttrib->GetColourRamp()))
		return FALSE;
	
	// Are the Colours and Control points all the same ?
	return (
	
		Colour 		== pAttrib->Colour 		&&
		EndColour 	== pAttrib->EndColour 	&&

		StartPoint 	== pAttrib->StartPoint 	&&
		EndPoint  	== pAttrib->EndPoint
	);
}




/********************************************************************************************

>	BOOL GradFillAttribute::RenderFill(RenderRegion *pRegion, Path *pPath)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/96
	Inputs:		pRegion - the render region of interest
				pPath - the path to fill.
	Returns:	FALSE => let the Render region fill the path, TRUE = we have done it.
	Purpose:	Render the fill, if poss. (override to do something different)

********************************************************************************************/

BOOL GradFillAttribute::RenderFill(RenderRegion *pRegion, Path *pPath)
{
//#pragma message( __LOCMSG__ "GradFillAttribute::RenderFill - do nothing" )
//	TRACE( _T("Warning - GradFillAttribute::RenderFill called\n") );
	if ( pRegion->IsKindOf(CC_RUNTIME_CLASS(GRenderRegion)) )
	{
		// Safe to cast now
		GRenderRegion *gRnd = (GRenderRegion*)pRegion;

		// Do the rendering
		return gRnd->RenderGradFillPath(pPath, this);
	}
	else if ( pRegion->IsKindOf(CC_RUNTIME_CLASS(OSRenderRegion)) )
	{
		// Safe to cast now
		OSRenderRegion *oRnd = (OSRenderRegion*)pRegion;

		// Do the rendering
		return oRnd->RenderGradFillPath(pPath, this);
	}
	return FALSE;
}


/********************************************************************************************

>	void GradFillAttribute::SetStartPoint(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the Start Point of this fill

********************************************************************************************/

void GradFillAttribute::SetStartPoint(DocCoord* Pos)
{
	if (Pos == NULL)
		StartPoint = DocCoord(0,0);
	else
		StartPoint = *Pos;
}

/********************************************************************************************

>	void GradFillAttribute::SetEndPoint(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void GradFillAttribute::SetEndPoint(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint = DocCoord(0,0);
	else
		EndPoint = *Pos;
}

/********************************************************************************************

>	void GradFillAttribute::SetEndColour(DocColour* NewCol)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the End Colour of this fill

********************************************************************************************/

void GradFillAttribute::SetEndColour(DocColour* NewCol)
{
//#pragma message( __LOCMSG__ "GradFillAttribute::SetEndColour - do nothing" )
//	TRACE( _T("Warning - GradFillAttribute::SetEndColour called\n") );
	if (NewCol == NULL)
	{
		AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
											_R(IDS_BLACKNAME), &EndColour);
	}
	else
		EndColour = *NewCol;
}

/********************************************************************************************

>	void GradFillAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	GradFillAttribute; RenderStack; AttributeValue; NodeAttribute;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void GradFillAttribute::SimpleCopy(AttributeValue *pValue)
{
	*(this) = *((FillGeometryAttribute*)pValue);
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//							LinearFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	LinearFillAttribute::LinearFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

LinearFillAttribute::LinearFillAttribute()
{
	IsPersp = FALSE;
}

/********************************************************************************************

>	NodeAttribute *LinearFillAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrLinearFill node from this graduated fill attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *LinearFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrLinearFill *pAttr = new AttrLinearColourFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}


/********************************************************************************************

>	void LinearFillAttribute::SetEndPoint2(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void LinearFillAttribute::SetEndPoint2(DocCoord* Pos)
{
	if (Pos == NULL)
	{
		EndPoint2 = MakeLineAtAngle(StartPoint, EndPoint, 90);
	}
	else
	{
		EndPoint2 = *Pos;
	}
}


/********************************************************************************************

>	void LinearFillAttribute::SetEndPoint3(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void LinearFillAttribute::SetEndPoint3(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint3 = MakeLineAtAngle(*GetEndPoint(), *GetEndPoint2(), 90);
	else
		EndPoint3 = *Pos;
}


/********************************************************************************************

>	void LinearFillAttribute::MakePerspective()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		-
	Purpose:	Make the fill perspectived.
				This should be called just before it is transformed by the moulder,
				so it can validate the 4th control point.

********************************************************************************************/

void LinearFillAttribute::MakePerspective()
{
	IsPersp = TRUE;

	INT32 dx1 = EndPoint.x - StartPoint.x;
	INT32 dx2 = EndPoint2.x - StartPoint.x;

	INT32 dy1 = EndPoint.y - StartPoint.y;
	INT32 dy2 = EndPoint2.y - StartPoint.y;

	DocCoord Pos3(StartPoint.x + dx1 + dx2, StartPoint.y + dy1 + dy2);
	SetEndPoint3(&Pos3);
}

/********************************************************************************************

>	void LinearFillAttribute::RemovePerspective()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		-
	Purpose:	Removes perspective from this fill.

********************************************************************************************/

void LinearFillAttribute::RemovePerspective()
{
	IsPersp = FALSE;
}



/********************************************************************************************

>	virtual AttributeValue *LinearFillAttribute::MouldIntoStroke(PathStrokerVector *pMoulder,
																			double TransScale);
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/97

	Inputs:		pMoulder	- A PathStrokerVector which knows how to translate points to "mould" them
							  (May be NULL, in which case moulding of points does not occur)

				TransScale	- A fraction between 0.0 and 1.0, by which any transparency
							  values in this geometry will be scaled, allowing the caller to
							  effectively apply a flat transparency level to everything that is
							  moulded. Use 1.0 to leave transparency unaltered.

	Returns:	NULL if the original attribute can be used, else
				A pointer to a copy of this object, suitably moulded and adjusted for
				the transparency scaling. The caller must delete the copy when finished
				with it.

	Purpose:	Helper function for the PathStrokerVector class, which "moulds" clipart
				subtrees to lie along an arbitrary path.

				This function is called to mould fill geometries, so that fill endpoints
				are translated to appropriate positions in the destination envelope, and
				allows the caller to effectively apply a flat transparency by scaling
				any transparency values in this geometry by the given fraction.

********************************************************************************************/

AttributeValue *LinearFillAttribute::MouldIntoStroke(PathStrokerVector *pMoulder, double TransScale)
{
// Taken out of WEBSTER Neville 6/8/97
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	if (pMoulder == NULL)
		return(NULL);

	FillGeometryAttribute *pCopy = (FillGeometryAttribute *) GetRuntimeClass()->CreateObject();
	if (pCopy != NULL)
	{
		// Copy this object's values across
		*pCopy = *this;

		// And mould all of the geometry points across
		JCW_MOULDPOINT(pCopy, StartPoint);
		JCW_MOULDPOINT(pCopy, EndPoint);

		if (IsPerspective())
		{
			// If it's perspectivised, just map all the points and hope for the best
			JCW_MOULDPOINT(pCopy, EndPoint2);
			JCW_MOULDPOINT(pCopy, EndPoint3);
		}
		else
		{
			// Otherwise, we have mapped the start/end points, and we pass NULL in for
			// the remaining points so that they are re-generated to get the fill going
			// in the start->end direction (because the fill direction is not controlled
			// by the start->end line, but by the end->end2 line)
			pCopy->SetEndPoint2(NULL);
			pCopy->SetEndPoint3(NULL);
		}
	}
	return(pCopy);
#else
	return NULL;
#endif // VECTOR_STROKING
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//							RadialFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	RadialFillAttribute::RadialFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

RadialFillAttribute::RadialFillAttribute()
{
	Circular = FALSE;
	IsPersp = FALSE;

	EndPoint2 = DocCoord(0,0);
}

/********************************************************************************************

>	NodeAttribute *RadialFillAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrRadialFill node from this graduated fill attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *RadialFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrRadialFill *pAttr = new AttrRadialColourFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	void RadialFillAttribute::SetEndPoint2(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void RadialFillAttribute::SetEndPoint2(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint2 = DocCoord(0,0);
	else
		EndPoint2 = *Pos;
}

/********************************************************************************************

>	FillGeometryAttribute& RadialFillAttribute::operator=(FillGeometryAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		Attrib - the attribute to copy, which must be an AttrFillGeometry
	Returns:	A reference to the copy
	Purpose:	A virtual assignment operator. 

********************************************************************************************/

FillGeometryAttribute& RadialFillAttribute::operator=(FillGeometryAttribute& Attrib)
{
	return GradFillAttribute::operator=(Attrib);
}

/********************************************************************************************

>	INT32 RadialFillAttribute::operator==(const FillGeometryAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 RadialFillAttribute::operator==(const FillGeometryAttribute& Attrib)
{
	if (GetRuntimeClass() != Attrib.GetRuntimeClass())
		return FALSE;

	RadialFillAttribute* pAttrib = (RadialFillAttribute*)&Attrib;

	if (IsPerspective())
	{ 
		if (!pAttrib->IsPerspective())
			return FALSE;

		if (EndPoint3 != pAttrib->EndPoint3)
			return FALSE;
	}

	// check the colour ramps match
	if (!SameColourRampAs(pAttrib->GetColourRamp()))
		return FALSE;

	// Are the Colours and Control points all the same ?
	return (
	
		Colour 		== pAttrib->Colour 		&&
		EndColour 	== pAttrib->EndColour 	&&
					   
		StartPoint 	== pAttrib->StartPoint 	&&
		EndPoint  	== pAttrib->EndPoint 	&&
		EndPoint2   == pAttrib->EndPoint2	&&

		Circular 	== pAttrib->Circular
	);
}

/********************************************************************************************

>	void RadialFillAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	RadialFillAttribute; RenderStack; AttributeValue; NodeAttribute;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void RadialFillAttribute::SimpleCopy(AttributeValue *pValue)
{
	// Base class does most of the work...
	GradFillAttribute::SimpleCopy(pValue);
}


/********************************************************************************************

>	void RadialFillAttribute::MakeCircular()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Purpose:	Forces this radial fill into a circular one.

********************************************************************************************/

void RadialFillAttribute::MakeCircular()
{
	// Set a flag so we can tell we're circular
	Circular = TRUE;

	// Make sure the second end point is on the same radius as the other
	EndPoint2 = MakeLineAtAngle(StartPoint, EndPoint, 90);
}

/********************************************************************************************

>	void RadialFillAttribute::MakeElliptical()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Purpose:	Forces this radial fill into an elliptical one.

********************************************************************************************/

void RadialFillAttribute::MakeElliptical()
{
	// Just clear the flag, so we don't try and lock the secondary point
	Circular = FALSE;
}

/********************************************************************************************

>	void RadialFillAttribute::SetEndPoint3(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void RadialFillAttribute::SetEndPoint3(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint3 = MakeLineAtAngle(*GetEndPoint(), *GetEndPoint2(), 90);
	else
		EndPoint3 = *Pos;
}


/********************************************************************************************

>	void RadialFillAttribute::MakePerspective()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		-
	Purpose:	Make the fill perspectived.
				This should be called just before it is transformed by the moulder,
				so it can validate the 4th control point.

********************************************************************************************/

void RadialFillAttribute::MakePerspective()
{
	IsPersp = TRUE;

	INT32 dx1 = EndPoint.x - StartPoint.x;
	INT32 dx2 = EndPoint2.x - StartPoint.x;

	INT32 dy1 = EndPoint.y - StartPoint.y;
	INT32 dy2 = EndPoint2.y - StartPoint.y;

	DocCoord Pos3(StartPoint.x + dx1 + dx2, StartPoint.y + dy1 + dy2);
	SetEndPoint3(&Pos3);
}

/********************************************************************************************

>	void RadialFillAttribute::RemovePerspective()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		-
	Purpose:	Removes perspective from this fill.

********************************************************************************************/

void RadialFillAttribute::RemovePerspective()
{
	IsPersp = FALSE;
}


/********************************************************************************************

>	virtual AttributeValue *RadialFillAttribute::MouldIntoStroke(PathStrokerVector *pMoulder,
																			double TransScale);
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/97

	Inputs:		pMoulder	- A PathStrokerVector which knows how to translate points to "mould" them
							  (May be NULL, in which case moulding of points does not occur)

				TransScale	- A fraction between 0.0 and 1.0, by which any transparency
							  values in this geometry will be scaled, allowing the caller to
							  effectively apply a flat transparency level to everything that is
							  moulded. Use 1.0 to leave transparency unaltered.

	Returns:	NULL if the original attribute can be used, else
				A pointer to a copy of this object, suitably moulded and adjusted for
				the transparency scaling. The caller must delete the copy when finished
				with it.

	Purpose:	Helper function for the PathStrokerVector class, which "moulds" clipart
				subtrees to lie along an arbitrary path.

				This function is called to mould fill geometries, so that fill endpoints
				are translated to appropriate positions in the destination envelope, and
				allows the caller to effectively apply a flat transparency by scaling
				any transparency values in this geometry by the given fraction.

********************************************************************************************/

AttributeValue *RadialFillAttribute::MouldIntoStroke(PathStrokerVector *pMoulder, double TransScale)
{
// Taken out of WEBSTER Neville 6/8/97
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	if (pMoulder == NULL)
		return(NULL);

	RadialFillAttribute *pCopy = (RadialFillAttribute *) GetRuntimeClass()->CreateObject();
	if (pCopy != NULL)
	{
		// Copy this object's values across
		*pCopy = *this;

		// Make sure it's defined as an elliptical fill, to ensure all points are set up correctly
		pCopy->MakeElliptical();

		// And mould all of the geometry points across
		JCW_MOULDPOINT(pCopy, StartPoint);
		JCW_MOULDPOINT(pCopy, EndPoint);
		JCW_MOULDPOINT(pCopy, EndPoint2);
		JCW_MOULDPOINT(pCopy, EndPoint3);
	}
	return(pCopy);
#else
	return NULL;
#endif // VECTOR_STROKING
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//							ConicalFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	ConicalFillAttribute::ConicalFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

ConicalFillAttribute::ConicalFillAttribute()
{
}

/********************************************************************************************

>	NodeAttribute *ConicalFillAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrConicalFill node from this graduated fill attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *ConicalFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrConicalFill *pAttr = new AttrConicalColourFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}


/********************************************************************************************

>	DocCoord ConicalFillAttribute::GetGeometryCoord(float pos) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/97
	Inputs:		pos = a parameter between 0 and 1
	Returns:	A coordinate which relates the input parameter to a coordinate space
				on this geometry. For instance a linear fill would have a linear parameter
				space, 0 at the start coordinate, increasing to 1 at the end coordinate
	Purpose:	Find the absolute position in geometry coordinate space for the given
				parameter.

********************************************************************************************/

DocCoord ConicalFillAttribute::GetGeometryCoord(float pos) const
{
	// Here we calculate a circular coordinate space
	return PathUtil::PointOnSemiCircle(StartPoint,EndPoint,(double)pos);
}



/********************************************************************************************

>	float ConicalFillAttribute::GetGeometryParam(const DocCoord& c) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/97
	Inputs:		c = a coordinate
	Returns:	A parameter p, 0<=p<=1, such that GetGeometryCoord(p) is the closest
				coordinate in the geometry to the input c.
	Purpose:	Find the parameter for the closest point to c on this geometry.

********************************************************************************************/

float ConicalFillAttribute::GetGeometryParam(const DocCoord& c) const
{
	// ok we're a linear geometry so find the closest point to a line type of thing.
	// use a handy util written by that other fab bloke called Mike.
	DocCoord Coords[2];
	Coords[0] = StartPoint;
	Coords[1] = EndPoint;
	double p;
	PathUtil::SqrDistanceToSemiCircle(Coords, c, &p);
	return (float)p;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//							SquareFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	SquareFillAttribute::SquareFillAttribute()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

SquareFillAttribute::SquareFillAttribute()
{
	IsPersp = FALSE;

	EndPoint2 = DocCoord(0,0);
}

/********************************************************************************************

>	NodeAttribute *SquareFillAttribute::MakeNode()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrSquareFill node from this graduated fill attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *SquareFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrSquareFill *pAttr = new AttrSquareColourFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}


/********************************************************************************************

>	void SquareFillAttribute::SetEndPoint2(DocCoord* Pos)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void SquareFillAttribute::SetEndPoint2(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint2 = MakeLineAtAngle(*GetStartPoint(), *GetEndPoint(), 90);
	else
		EndPoint2 = *Pos;
}


/********************************************************************************************

>	void SquareFillAttribute::SetEndPoint3(DocCoord* Pos)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void SquareFillAttribute::SetEndPoint3(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint3 = MakeLineAtAngle(*GetEndPoint(), *GetEndPoint2(), 90);
	else
		EndPoint3 = *Pos;
}


/********************************************************************************************

>	void SquareFillAttribute::MakePerspective()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Purpose:	Make the fill perspectived.
				This should be called just before it is transformed by the moulder,
				so it can validate the 4th control point.

********************************************************************************************/

void SquareFillAttribute::MakePerspective()
{
	IsPersp = TRUE;

	INT32 dx1 = EndPoint.x - StartPoint.x;
	INT32 dx2 = EndPoint2.x - StartPoint.x;

	INT32 dy1 = EndPoint.y - StartPoint.y;
	INT32 dy2 = EndPoint2.y - StartPoint.y;

	DocCoord Pos3(StartPoint.x + dx1 + dx2, StartPoint.y + dy1 + dy2);
	SetEndPoint3(&Pos3);
}

/********************************************************************************************

>	void SquareFillAttribute::RemovePerspective()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Purpose:	Removes perspective from this fill.

********************************************************************************************/

void SquareFillAttribute::RemovePerspective()
{
	IsPersp = FALSE;
}




/////////////////////////////////////////////////////////////////////////////////////////////
//
//							BitmapFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

BOOL BitmapFillAttribute::m_doBitmapSmoothing = TRUE;

/********************************************************************************************

>	BitmapFillAttribute::BitmapFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

BitmapFillAttribute::BitmapFillAttribute()
{
	Colour    = DocColour(COLOUR_NONE);
	EndColour = DocColour(COLOUR_NONE);

	StartPoint = DocCoord(0,0);
	EndPoint   = DocCoord(0,0);
	EndPoint2  = DocCoord(0,0);

	Tesselation = RT_Repeating;

	IsPersp = FALSE;

	m_CanBlurRender = TRUE;
}

/********************************************************************************************

>	BitmapFillAttribute::~BitmapFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Destructor for bitmap fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

BitmapFillAttribute::~BitmapFillAttribute()
{
}

KernelBitmapRef* BitmapFillAttribute::GetBitmapRef()
{
	return &BitmapRef;
}

/********************************************************************************************

>	NodeAttribute *BitmapFillAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrFlatColourFill node from this flat fill colour attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode; AttrFlatColourFill

********************************************************************************************/

NodeAttribute *BitmapFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrBitmapFill *pAttr = new AttrBitmapColourFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}


/********************************************************************************************

>	KernelBitmap* BitmapFillAttribute::GetBitmap()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/12/94
	Purpose:	Get the bitmap referenced by this attribute. 
	SeeAlso:	BitmapFillAttribute::AttachBitmap()

********************************************************************************************/

KernelBitmap* BitmapFillAttribute::GetBitmap()
{
	KernelBitmap* pBitmap = BitmapRef.GetBitmap();
	
	// Check for a deleted bitmap
	if (pBitmap && pBitmap->HasBeenDeleted())
	{
		ERROR2IF(pBitmap->GetParentBitmapList() == NULL, NULL, "Deleted bitmap has no parent list");
		
		// Use the default bitmap instead
		pBitmap = pBitmap->GetParentBitmapList()->FindDefaultBitmap();

		// There should always be a default bitmap in the list
		ERROR2IF(pBitmap == NULL, 0L, "Couldn't find the default bitmap");
	}

	return pBitmap;
}

/********************************************************************************************

>	BOOL BitmapFillAttribute::AttachBitmap(KernelBitmap* pBitmap)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Purpose:	Attaches a bitmap to this fill. 
	SeeAlso:	BitmapFillAttribute::DetachBitmap()

********************************************************************************************/

BOOL BitmapFillAttribute::AttachBitmap(KernelBitmap* NewBitmap)
{
	BitmapRef.Attach(NewBitmap);

	return TRUE;
}

/********************************************************************************************

>	BOOL BitmapFillAttribute::DetachBitmap()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Purpose:	Detaches a bitmap from this fill. 
	SeeAlso:	BitmapFillAttribute::AttachBitmap()

********************************************************************************************/

BOOL BitmapFillAttribute::DetachBitmap()
{
	BitmapRef.Detach();

	return TRUE;
}

/********************************************************************************************

>	BOOL BitmapFillAttribute::CopyBitmap(KernelBitmap* BmpToCopy)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Purpose:	Used to copy a bitmap from one fill to another. 
	SeeAlso:	BitmapFillAttribute::AttachBitmap()

********************************************************************************************/

BOOL BitmapFillAttribute::CopyBitmap(KernelBitmap* BmpToCopy)
{
	if (BmpToCopy != NULL)	// Is there a bitmap to copy ?
	{
		BitmapRef.Detach();

		OILBitmap* Bmp = BmpToCopy->ActualBitmap;
		if (Bmp->IsTemp())
		{
			BitmapRef.SetBitmap(BmpToCopy);
			return TRUE;
		}

 		BitmapRef.Attach(BmpToCopy);
	}

	return TRUE;
}


/********************************************************************************************

>	void BitmapFillAttribute::Render(RenderRegion *pRegion)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the fill geometry attribute for the given render region. i.e. all
				paths filled will now be filled with this fill geometry.
	SeeAlso:	BitmapFillAttribute; RenderStack; AttributeValue; NodeAttribute;
				BitmapFillAttribute::Restore; BitmapFillAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BitmapFillAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "BitmapFillAttribute::Render - do nothing" )
//	TRACE( _T("Warning - BitmapFillAttribute::Render called\n") );
	pRegion->SetFillGeometry(this, Temp);
}

/********************************************************************************************

>	void BitmapFillAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the fill type attribute for the given render region. i.e. all
				paths filled will now be filled with this fill attribute.
	SeeAlso:	BitmapFillAttribute; RenderStack; AttributeValue; NodeAttribute;
				BitmapFillAttribute::Render; BitmapFillAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BitmapFillAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "BitmapFillAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - BitmapFillAttribute::Restore called\n") );
	pRegion->RestoreFillGeometry(this, Temp);
}

/********************************************************************************************

>	void BitmapFillAttribute::SetStartPoint(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the Start Point of this fill

********************************************************************************************/

void BitmapFillAttribute::SetStartPoint(DocCoord* Pos)
{
	if (Pos == NULL)
		StartPoint = DocCoord(0,0);
	else
		StartPoint = *Pos;
}

/********************************************************************************************

>	void BitmapFillAttribute::SetEndPoint(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void BitmapFillAttribute::SetEndPoint(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint = DocCoord(0,0);
	else
		EndPoint = *Pos;
}

/********************************************************************************************

>	void BitmapFillAttribute::SetEndPoint2(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void BitmapFillAttribute::SetEndPoint2(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint2 = MakeLineAtAngle(*GetStartPoint(), *GetEndPoint(), 90);
	else
		EndPoint2 = *Pos;
}

/********************************************************************************************

>	void BitmapFillAttribute::SetEndPoint3(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void BitmapFillAttribute::SetEndPoint3(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint3 = MakeLineAtAngle(*GetEndPoint(), *GetEndPoint2(), 90);
	else
		EndPoint3 = *Pos;
}



/********************************************************************************************

>	void BitmapFillAttribute::MakePerspective()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/94
	Inputs:		-
	Purpose:	Make the fill perspectived.
				This should be called just before it is transformed by the moulder,
				so it can validate the 4th control point.

********************************************************************************************/

void BitmapFillAttribute::MakePerspective()
{
	IsPersp = TRUE;

	INT32 dx1 = EndPoint.x - StartPoint.x;
	INT32 dx2 = EndPoint2.x - StartPoint.x;

	INT32 dy1 = EndPoint.y - StartPoint.y;
	INT32 dy2 = EndPoint2.y - StartPoint.y;

	DocCoord Pos3(StartPoint.x + dx1 + dx2, StartPoint.y + dy1 + dy2);
	SetEndPoint3(&Pos3);
}

/********************************************************************************************

>	void BitmapFillAttribute::RemovePerspective()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/94
	Inputs:		-
	Purpose:	Removes perspective from this fill.

********************************************************************************************/

void BitmapFillAttribute::RemovePerspective()
{
	IsPersp = FALSE;
}

/********************************************************************************************

>	FillGeometryAttribute& BitmapFillAttribute::operator=(FillGeometryAttribute& FillAttrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		FillAttrib - the attribute to copy, which must be an AttrFillGeometry
	Returns:	A reference
	Purpose:	Make the Attribute the same as the other. 

********************************************************************************************/

FillGeometryAttribute& BitmapFillAttribute::operator=(FillGeometryAttribute& FillAttrib)
{
	return ColourFillAttribute::operator=(FillAttrib);
}

/********************************************************************************************

>	INT32 BitmapFillAttribute::operator==(const FillGeometryAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 BitmapFillAttribute::operator==(const FillGeometryAttribute& Attrib)
{
	if (GetRuntimeClass() != Attrib.GetRuntimeClass())
		return FALSE;

	BitmapFillAttribute* pAttrib = (BitmapFillAttribute*)&Attrib;

	if (GetBitmapRef()->GetBitmap() != pAttrib->GetBitmapRef()->GetBitmap())
		return FALSE;

	if (IsPerspective())
	{ 
		if (!pAttrib->IsPerspective())
			return FALSE;

		if (*GetEndPoint3() != *pAttrib->GetEndPoint3())
			return FALSE;
	}

	if (GetStartColour() != NULL)
	{
		if (pAttrib->GetStartColour() == NULL)
			return FALSE;

		if (*GetStartColour() != *pAttrib->GetStartColour())
			return FALSE;
	}
	else if (pAttrib->GetStartColour() != NULL)
	{
	 	return FALSE;
	}

	if (GetEndColour() != NULL)
	{
		if (pAttrib->GetEndColour() == NULL)
			return FALSE;

		if (*GetEndColour() != *pAttrib->GetEndColour())
			return FALSE;
	}
	else if (pAttrib->GetStartColour() != NULL)
	{
	 	return FALSE;
	}

	// check the colour ramps match
	if (!SameColourRampAs(pAttrib->GetColourRamp()))
		return FALSE;

	// Are the Colours and Control points all the same ?
	return (
	
		StartPoint 	== pAttrib->StartPoint 	&&
		EndPoint  	== pAttrib->EndPoint 	&&
		EndPoint2   == pAttrib->EndPoint2

	);
}

/********************************************************************************************

>	void BitmapFillAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	GradFillAttribute; RenderStack; AttributeValue; NodeAttribute;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BitmapFillAttribute::SimpleCopy(AttributeValue *pValue)
{
	// Just use the assignment operator
	*this = *(FillGeometryAttribute*)pValue;
}

/********************************************************************************************

>	BOOL BitmapFillAttribute::RenderFill(RenderRegion *pRegion, Path *pPath)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Inputs:		pRegion - the render region of interest
				pPath - the path to fill.
	Returns:	FALSE => let the Render region fill the path, TRUE = we have done it.
	Purpose:	Render the fill, if possible - it depends on what kind of render region
				we have been given.

********************************************************************************************/

BOOL BitmapFillAttribute::RenderFill(RenderRegion *pRegion, Path *pPath)
{
//#pragma message( __LOCMSG__ "BitmapFillAttribute::RenderFill - do nothing" )
//	TRACE( _T("Warning - BitmapFillAttribute::RenderFill called\n") );
	if ( pRegion->IsKindOf(CC_RUNTIME_CLASS(GRenderRegion)) )
	{
		// Safe to cast now
		GRenderRegion *gRnd = (GRenderRegion*)pRegion;

		// Do the rendering
		return gRnd->RenderBitmapFill(pPath, this);
	}
	else if ( pRegion->IsKindOf(CC_RUNTIME_CLASS(OSRenderRegion)) )
	{
		// Safe to cast now
		OSRenderRegion *oRnd = (OSRenderRegion*)pRegion;

		// Do the rendering
		return oRnd->RenderBitmapFill(pPath, this);
	}

	// Can't render this fill with this render region.
	return FALSE;
}

/********************************************************************************************

>	BOOL BitmapFillAttribute::SetDPI(UINT32 NewDpi)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Inputs:		NewDpi
	Purpose:	Sets the DPI of a bitmap.
	SeeAlso:	FractalFillAttribute::SetDPI

********************************************************************************************/

BOOL BitmapFillAttribute::SetDPI(UINT32 NewDpi)
{
	if (IsPerspective())
		return FALSE;

	if (NewDpi == 0)
		return FALSE;

	if (GetBitmap() == NULL)
		return FALSE;

	OILBitmap *OilBM = GetBitmap()->ActualBitmap;

	if (OilBM == NULL)
		return FALSE;

	BitmapInfo Info;
	OilBM->GetInfo(&Info);

	INT32 PixWidth  = Info.PixelWidth;
	INT32 PixHeight = Info.PixelHeight;

	DocCoord Start = *GetStartPoint();
	DocCoord End   = *GetEndPoint();
	DocCoord End2  = *GetEndPoint2();

	INT32 Width  = INT32(Start.Distance(End));
	INT32 Height = INT32(Start.Distance(End2));

	INT32 HDpi = (Width  == 0) ? 0 : (PixWidth*72000)/Width;
	INT32 VDpi = (Height == 0) ? 0 : (PixHeight*72000)/Height;

	INT32 OldDpi = HDpi;
	if (VDpi < OldDpi)
		OldDpi = VDpi;

	TRACEUSER( "Mike", _T("Bitmap Dpi is currently %d\n"),OldDpi);
	TRACEUSER( "Mike", _T("Setting Bitmap Dpi to %d\n"),NewDpi);

	FIXED16 Ratio 	= FIXED16(double(OldDpi)/double(NewDpi));
	Matrix Scale  	= Matrix(Ratio, Ratio);

	GetBitmapVirtualPoints(Start, End, End2,
							&Start, &End, &End2);

	DocCoord Centre = Start;

	Start.translate(-Centre.x, -Centre.y);	
	Scale.transform(&Start);
	Start.translate(Centre.x, Centre.y);	

	End.translate(-Centre.x, -Centre.y);	
	Scale.transform(&End);
	End.translate(Centre.x, Centre.y);	

	End2.translate(-Centre.x, -Centre.y);	
	Scale.transform(&End2);
	End2.translate(Centre.x, Centre.y);	

	GetBitmapRealPoints(Start, End, End2,
						&Start, &End, &End2);

	INT32 NewWidth  = INT32(Start.Distance(End));
	INT32 NewHeight = INT32(Start.Distance(End2));

	if (NewWidth == 0 || NewHeight == 0)
		return FALSE;

	SetStartPoint(&Start);
	SetEndPoint(&End);
	SetEndPoint2(&End2);

	SetFractalDPI(GetDPI());

	return TRUE;
}

/********************************************************************************************

>	UINT32 BitmapFillAttribute::GetDPI()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Returns:	The current DPI of the bitmap
	Purpose:	Gets the DPI of a bitmap.
	SeeAlso:	FractalFillAttribute::SetDPI

********************************************************************************************/

UINT32 BitmapFillAttribute::GetDPI()
{
	UINT32 Dpi = 0;

	if (GetBitmap() == NULL)
		return 0;

	OILBitmap *OilBM = GetBitmap()->ActualBitmap;

	if (OilBM != NULL)
	{
		BitmapInfo Info;
		OilBM->GetInfo(&Info);

		INT32 PixWidth  = Info.PixelWidth;
		INT32 PixHeight = Info.PixelHeight;

		DocCoord Start = *GetStartPoint();
		DocCoord End   = *GetEndPoint();
		DocCoord End2  = *GetEndPoint2();

		INT32 Width  = INT32(Start.Distance(End));
		INT32 Height = INT32(Start.Distance(End2));

		UINT32 HDpi = 0;
		UINT32 VDpi = 0;

		if (Width > 0)
			HDpi = (PixWidth*72000)/Width;

		if (Height > 0)
			VDpi = (PixHeight*72000)/Height;

		Dpi = HDpi;
		if (VDpi < Dpi)
			Dpi = VDpi;
	}

	return Dpi;
}

/********************************************************************************************

>	void ColourFillAttribute::SetStartColour(DocColour* NewCol)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the start colour of this fill

********************************************************************************************/

void BitmapFillAttribute::SetStartColour(DocColour* NewCol)
{
//#pragma message( __LOCMSG__ "BitmapFillAttribute::SetStartColour - do nothing" )
//	TRACE( _T("Warning - BitmapFillAttribute::SetStartColour called\n") );
	if (NewCol == NULL)
		Colour = COLOUR_NONE;
	else
		Colour = *NewCol;

	if (Colour == COLOUR_NONE)
		EndColour = COLOUR_NONE;

	if (Colour != COLOUR_NONE && GetEndColour() == NULL)
	{
		AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
											_R(IDS_WHITENAME), &EndColour);

		if (Colour == EndColour)
		{
			AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
												_R(IDS_BLACKNAME), &EndColour);
		}
	}
}

/********************************************************************************************

>	void BitmapFillAttribute::SetEndColour(DocColour* NewCol)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the end colour of this fill

********************************************************************************************/

void BitmapFillAttribute::SetEndColour(DocColour* NewCol)
{
//#pragma message( __LOCMSG__ "BitmapFillAttribute::SetEndColour - do nothing" )
//	TRACE( _T("Warning - BitmapFillAttribute::SetEndColour called\n") );
	if (NewCol == NULL)
	{
		EndColour = COLOUR_NONE;
	}
	else
		EndColour = *NewCol;

	if (EndColour == COLOUR_NONE)
		Colour = COLOUR_NONE;

	if (EndColour != COLOUR_NONE && GetStartColour() == NULL)
	{
		AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
											_R(IDS_BLACKNAME), &Colour);

		if (Colour == EndColour)
		{
			AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
												_R(IDS_WHITENAME), &Colour);
		}
	}
}

/********************************************************************************************

>	DocColour* ColourFillAttribute::GetStartColour()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/95
	Inputs:		-
	Purpose:	Gets the Start colour of this fill

********************************************************************************************/

DocColour* BitmapFillAttribute::GetStartColour()
{
	if (Colour == COLOUR_NONE)
		return NULL;
	else
		return &Colour;
}

/********************************************************************************************

>	DocColour* ColourFillAttribute::GetEndColour()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/95
	Inputs:		-
	Purpose:	Gets the End colour of this fill

********************************************************************************************/

DocColour* BitmapFillAttribute::GetEndColour()
{
	if (EndColour == COLOUR_NONE)
		return NULL;
	else
		return &EndColour;
}

void BitmapFillAttribute::SetTesselation(INT32 NewTess)
{
	if (NewTess == RT_NoRepeatType)
		NewTess = RT_Repeating;

	Tesselation = NewTess;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//							FractalFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	FractalFillAttribute::FractalFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

FractalFillAttribute::FractalFillAttribute()
{
	MonotonicTime time;
	Seed = time.Sample();

	Graininess = AttrFillGeometry::FractalGraininess;
	Gravity    = AttrFillGeometry::FractalGravity;
	Dpi		   = AttrFillGeometry::FractalDPI;

	Squash = 0;
	Dim	   = 0;
	Tileable = FALSE;

	Tesselation = RT_RepeatInverted;
}

/********************************************************************************************

>	FractalFillAttribute::~FractalFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Destructor for fractal fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

FractalFillAttribute::~FractalFillAttribute()
{
	DetachBitmap();
}

/********************************************************************************************

>	NodeAttribute *FractalFillAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrFlatColourFill node from this flat fill colour attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode; AttrFlatColourFill

********************************************************************************************/

NodeAttribute *FractalFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrFractalColourFill *pAttr = new AttrFractalColourFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	FillGeometryAttribute& FractalFillAttribute::operator=(FillGeometryAttribute& FillAttrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		FillAttrib - the attribute to copy, which must be an AttrFillGeometry
	Returns:	a reference to this
	Purpose:	Assign this with the value of FillAttrib

********************************************************************************************/

FillGeometryAttribute& FractalFillAttribute::operator=(FillGeometryAttribute& FillAttrib)
{
	if (FillAttrib.IsKindOf(CC_RUNTIME_CLASS(FractalFillAttribute)))
	{
		Seed 		= ((FractalFillAttribute*)&FillAttrib)->Seed;
		Graininess 	= ((FractalFillAttribute*)&FillAttrib)->Graininess;
		Gravity 	= ((FractalFillAttribute*)&FillAttrib)->Gravity;
		Squash 		= ((FractalFillAttribute*)&FillAttrib)->Squash;
		Dpi 		= ((FractalFillAttribute*)&FillAttrib)->Dpi;
		Tileable	= ((FractalFillAttribute*)&FillAttrib)->Tileable;
		Dim			= ((FractalFillAttribute*)&FillAttrib)->Dim;
	}

	return ColourFillAttribute::operator=(FillAttrib);
}

/********************************************************************************************

>	INT32 FractalFillAttribute::operator==(const FillGeometryAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 FractalFillAttribute::operator==(const FillGeometryAttribute& Attrib)
{
	if (GetRuntimeClass() != Attrib.GetRuntimeClass())
		return FALSE;

	FractalFillAttribute* pAttrib = (FractalFillAttribute*)&Attrib;

	if (IsPerspective())
	{ 
		if (!pAttrib->IsPerspective())
			return FALSE;

		if (*GetEndPoint3() != *pAttrib->GetEndPoint3())
			return FALSE;
	}

	// check the colour ramps match
	if (!SameColourRampAs(pAttrib->GetColourRamp()))
		return FALSE;

	// Are the Colours and Control points all the same ?
	
	return (

		*GetStartColour() 	== *pAttrib->GetStartColour() 	&&
		*GetEndColour()   	== *pAttrib->GetEndColour() 	&&
	
		*GetStartPoint() 	== *pAttrib->GetStartPoint() 	&&
		*GetEndPoint()  	== *pAttrib->GetEndPoint() 		&&
		*GetEndPoint2()   	== *pAttrib->GetEndPoint2()		&&

		Seed   		== pAttrib->Seed		&&
		Graininess  == pAttrib->Graininess	&&
		Gravity   	== pAttrib->Gravity		&&
		Squash   	== pAttrib->Squash		&&
		Dpi   		== pAttrib->Dpi			&&
		Tileable   	== pAttrib->Tileable	&&
		Dim 		== pAttrib->Dim
	);
}


/********************************************************************************************

>	void FractalFillAttribute::CacheFractalData(FillGeometryAttribute *pCachedFractal)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Inputs:		pCachedFractal = A pointer to the fractal in the cache
	Returns:	-
	Purpose:	Copies any data from this FractalFillAttribute into the cached fractal
				pointed to on entry. This data is then checked via IsSameAsCachedFractal

********************************************************************************************/

void FractalFillAttribute::CacheFractalData(FillGeometryAttribute *pCachedFractal)
{
	ERROR3IF(pCachedFractal==NULL, "NULL pointer passed to FractalFillAttribute::CacheFractalData");
	ERROR3IF(!IS_A(pCachedFractal,FractalFillAttribute), "Not a FractalFillAttribute during FractalFillAttribute::CacheFractalData");

	pCachedFractal->SetStartPoint(GetStartPoint());
	pCachedFractal->SetEndPoint(GetEndPoint());
	pCachedFractal->SetEndPoint2(GetEndPoint2());

	pCachedFractal->SetSeed(GetSeed());
	pCachedFractal->SetGraininess(GetGraininess());
	pCachedFractal->SetGravity(GetGravity());
	pCachedFractal->SetSquash(GetSquash());
	pCachedFractal->SetTileable(GetTileable());
	pCachedFractal->SetFractalDPI(GetFractalDPI());
	pCachedFractal->SetFractalDim(GetFractalDim());
}


/********************************************************************************************

>	BOOL FractalFillAttribute::IsSameAsCachedFractal(FillGeometryAttribute *pCachedFractal)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Inputs:		pCachedFractal = A pointer to the fractal in the cache
	Returns:	TRUE if this fractal matches the cached fractal
				FALSE if not
	Purpose:	A virtual comparison operator used by the fractal cache to check for a
				matching fractal.

********************************************************************************************/

BOOL FractalFillAttribute::IsSameAsCachedFractal(FillGeometryAttribute *pCachedFractal)
{
	ERROR3IF(pCachedFractal==NULL, "NULL pointer passed to FractalFillAttribute::IsSameAsCachedFractal");
	ERROR3IF(!IS_A(pCachedFractal,FractalFillAttribute), "Not a FractalFillAttribute during FractalFillAttribute::IsSameAsCachedFractal");

	// a long winded check but separate lines help in debugging, i.e. when does ok go FALSE
	BOOL ok =  (Seed   		== pCachedFractal->GetSeed());
	ok = ok && (Graininess  == pCachedFractal->GetGraininess());
	ok = ok && (Gravity   	== pCachedFractal->GetGravity());
	ok = ok && (Squash   	== pCachedFractal->GetSquash());
	ok = ok && (Tileable   	== pCachedFractal->GetTileable());
	ok = ok && ((UINT32)Dim 	== pCachedFractal->GetFractalDim());
	//ok = ok && (Dpi   		== pCachedFractal->GetFractalDpi());		 // not checked in Will's original code
	return ok;
}


/********************************************************************************************

>	BOOL FractalFillAttribute::AttachBitmap(KernelBitmap* pBitmap)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Purpose:	Attaches a bitmap to this fill. 
	SeeAlso:	BitmapFillAttribute::DetachBitmap()

********************************************************************************************/

BOOL FractalFillAttribute::AttachBitmap(KernelBitmap* NewBitmap)
{
	// Sometimes we are asked to attach the same bitmap to the attribute
	// So if we try to detach it and then attach it, during the detach
	// the bitmap can get deleted, so then we will effectivelly attach
	// a deleted bitmap. To avoid this we add this fractal to the list again in
	// order to increment the usage count, then do the normal detach & attach 
	// and finally remove the additional fractal from the list
	BOOL NeedUsageIncrement = FALSE;
	if ((NewBitmap != NULL) && (GetBitmap() != NULL))
	{
		// only if the bitmap we have is the same as the one we are to attach
		if ((GetBitmap()->ActualBitmap != NULL) && 
			(GetBitmap()->ActualBitmap == NewBitmap->ActualBitmap))
			{
				NeedUsageIncrement = TRUE; // set the flag
				GetApplication()->GetGlobalFractalList()->AddFractal(this); // inc the usage count
				TRACEUSER( "Stefan", _T("NeedUsageIncrement = TRUE\n"));
			}
	}

	// get rid of the old bitmap
	DetachBitmap();
	
	if (NewBitmap == NULL)
		return FALSE;

	OILBitmap* Bmp = NewBitmap->ActualBitmap;
	if (Bmp == NULL || !Bmp->IsTemp())
		return FALSE;

	NewBitmap = new KernelBitmap(Bmp, TRUE);
	BitmapRef.SetBitmap(NewBitmap);
	GetApplication()->GetGlobalFractalList()->AddFractal(this);

	// if we added an additional fractal, remove it
	if (NeedUsageIncrement)
		GetApplication()->GetGlobalFractalList()->RemoveFractal(this);
	
	return TRUE;
}

/********************************************************************************************

>	BOOL FractalFillAttribute::DetachBitmap()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Purpose:	Detaches a bitmap from this fill. 
	SeeAlso:	BitmapFillAttribute::AttachBitmap()

********************************************************************************************/

BOOL FractalFillAttribute::DetachBitmap()
{
	if (GetBitmap() == NULL)
		return FALSE;

	if (GetApplication()->GetGlobalFractalList()->RemoveFractal(this) && BitmapRef.GetBitmap())
	{
		// The fractal was deleted, so make sure we NULL our pointer
		BitmapRef.GetBitmap()->ActualBitmap = NULL;
	}

	// now make sure the kernelbitmap is dead too
	BitmapRef.DeleteBmp();

	return TRUE;
}

/********************************************************************************************

>	BOOL FractalFillAttribute::CopyBitmap(KernelBitmap* BmpToCopy)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Purpose:	Used to copy a bitmap from one fill to another. 
	SeeAlso:	BitmapFillAttribute::AttachBitmap()

********************************************************************************************/

BOOL FractalFillAttribute::CopyBitmap(KernelBitmap* BmpToCopy)
{
	if (BmpToCopy != NULL)	// Is there a bitmap to copy ?
	{
		return AttachBitmap(BmpToCopy);
	}

	return TRUE;
}

/********************************************************************************************

>	void FractalFillAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	GradFillAttribute; RenderStack; AttributeValue; NodeAttribute;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void FractalFillAttribute::SimpleCopy(AttributeValue *pValue)
{
	// Just use the assignment operator
	*this = *(FillGeometryAttribute*)pValue;
}



/********************************************************************************************

>	void FractalFillAttribute::SetStartColour(DocColour* NewCol)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the Start colour of this fill

********************************************************************************************/

void FractalFillAttribute::SetStartColour(DocColour* NewCol)
{
//#pragma message( __LOCMSG__ "FractalFillAttribute::SetStartColour - do nothing" )
//	TRACE( _T("Warning - FractalFillAttribute::SetStartColour called\n") );
	if (NewCol == NULL)
	{
		AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
											_R(IDS_WHITENAME), &Colour);

		if (Colour == EndColour)
		{
			AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
												_R(IDS_BLACKNAME), &Colour);
		}
	}
	else
		Colour = *NewCol;
}

/********************************************************************************************

>	void FractalFillAttribute::SetEndColour(DocColour* NewCol)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the Start colour of this fill

********************************************************************************************/

void FractalFillAttribute::SetEndColour(DocColour* NewCol)
{
//#pragma message( __LOCMSG__ "FractalFillAttribute::SetEndColour - do nothing" )
//	TRACE( _T("Warning - FractalFillAttribute::SetEndColour called\n") );
	if (NewCol == NULL)
	{
		AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
											_R(IDS_BLACKNAME), &EndColour);

		if (Colour == EndColour)
		{
			AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
												_R(IDS_WHITENAME), &EndColour);
		}
	}
	else
		EndColour = *NewCol;
}



/********************************************************************************************

>	BOOL FractalFillAttribute::RecalcFractal()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Purpose:	Recalculates the Fractal.
	SeeAlso:	FractalFillAttribute::SetDPI

********************************************************************************************/

BOOL FractalFillAttribute::RecalcFractal()
{
	if (*GetStartPoint() == *GetEndPoint() ||
		*GetStartPoint() == *GetEndPoint2())
	{
		return FALSE;
	}

	UINT32 OldDim = GetFractalDim();

	KernelBitmap* pBitmap = GenerateFractalBitmap(	Seed, 
													Graininess.MakeDouble(),
													Gravity.MakeDouble(),
													Squash.MakeDouble(), 
													Dpi);

	if (pBitmap == NULL)
	{
		TRACEUSER( "Mike", _T("Fractal Failed !!!\n"));
		return FALSE;	// Error set by GenerateFractalBitmap
	}

	// When asking GenerateFractalBitmap above to give us a bitmap, it might modify the Dim value.
	// DetachBitmap then tries to match this fractal with the cached fractals, so it could find a 
	// wrong cached fractal (one with the same parameters as this, but with Dim value as set by
	// GenerateFractalBitmap, rather then the original value). In this case DetachBitmap acts on
	// the wrong fractal and could even cause destroying its bitmap, even though it is used by 
	// some other attribute. To fix this we temporarily set Dim to the old value, call DetachBitmap
	// and then set the new value back.
	UINT32 NewDim = GetFractalDim();
	if (NewDim != OldDim)
	{
		SetFractalDim(OldDim);
		DetachBitmap();

		SetFractalDim(NewDim);
	}

	AttachBitmap(pBitmap);
	delete pBitmap;

	return TRUE;
}



/********************************************************************************************

>	void FractalFillAttribute::SetTesselation(INT32 NewTess)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Returns:	-
	Purpose:	Set the prefered tesselation of this fractal
	SeeAlso:	

********************************************************************************************/

void FractalFillAttribute::SetTesselation(INT32 NewTess)
{
	if (NewTess == RT_NoRepeatType)
		NewTess = RT_RepeatInverted;

	Tesselation = NewTess;
}

void FractalFillAttribute::SetFractalDim(UINT32 NewDim) 
{ 
	Dim = NewDim; 
}

BOOL FractalFillAttribute::SetTileable(BOOL NewTile)
{
	DetachBitmap();		// Ensure the current fractal is removed from cache

	Tileable = NewTile;

	return TRUE;
}

/********************************************************************************************

>	BOOL FractalFillAttribute::SetFractalDPI(UINT32 NewDpi)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Inputs:		NewDpi - New DPI to generate the Fractal at.
	Purpose:	Recalculates the Fractal at a new Dpi.
	SeeAlso:	FractalFillAttribute::GetFractalDPI

********************************************************************************************/

BOOL FractalFillAttribute::SetFractalDPI(UINT32 NewDpi)
{
	Dpi = NewDpi;

	return TRUE;
}


/*******************************************************************************************
  Values local to this fractal type
*******************************************************************************************/

BOOL FractalFillAttribute::SetSeed(INT32 NewSeed)
{
	DetachBitmap();		// Ensure the current fractal is removed from cache

	Seed = NewSeed;

	return TRUE;
}

BOOL FractalFillAttribute::SetGraininess(FIXED16 NewGrain)
{
	DetachBitmap();		// Ensure the current fractal is removed from cache

	Graininess = NewGrain;

	return TRUE;
}

BOOL FractalFillAttribute::SetGravity(FIXED16 NewGrav)
{
	DetachBitmap();		// Ensure the current fractal is removed from cache

	Gravity = NewGrav;

	return TRUE;
}

BOOL FractalFillAttribute::SetSquash(FIXED16 NewSquash)
{
	DetachBitmap();		// Ensure the current fractal is removed from cache

	Squash = NewSquash;

	return TRUE;
}


BOOL FractalFillAttribute::Randomise()
{
	DetachBitmap();		// Ensure the current fractal is removed from cache

	MonotonicTime time;
	Seed = time.Sample();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//							NoiseFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	NoiseFillAttribute::NoiseFillAttribute()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/97
	Inputs:		-
	Purpose:	Implementation of NoiseFillAttribute

********************************************************************************************/

NoiseFillAttribute::NoiseFillAttribute()
{
	MonotonicTime time;
	seed = time.Sample();

	dpi			 = AttrFillGeometry::FractalDPI;
	dim			 = 0;
	tileable	 = TRUE;

	grain        = FIXED16(30.0);
}


NoiseFillAttribute::~NoiseFillAttribute()
{
	DetachBitmap();
}


NodeAttribute* NoiseFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrNoiseColourFill *pAttr = new AttrNoiseColourFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}


FillGeometryAttribute& NoiseFillAttribute::operator=(FillGeometryAttribute& FillAttrib)
{
	if (FillAttrib.IsKindOf(CC_RUNTIME_CLASS(NoiseFillAttribute)))
	{
		tileable	= ((NoiseFillAttribute*)&FillAttrib)->tileable;
		grain		= ((NoiseFillAttribute*)&FillAttrib)->grain;
		seed		= ((NoiseFillAttribute*)&FillAttrib)->seed;
		dim		    = ((NoiseFillAttribute*)&FillAttrib)->dim;
		dpi		    = ((NoiseFillAttribute*)&FillAttrib)->dpi;
	}
	
	return ColourFillAttribute::operator=(FillAttrib);
}


INT32 NoiseFillAttribute::operator==(const FillGeometryAttribute& Attrib)
{
	// is this the same class of object?
	if (GetRuntimeClass() != Attrib.GetRuntimeClass())
		return FALSE;

	NoiseFillAttribute* pAttrib = (NoiseFillAttribute*)&Attrib;

	if (IsPerspective())
	{ 
		if (!pAttrib->IsPerspective())
			return FALSE;

		if (*GetEndPoint3() != *pAttrib->GetEndPoint3())
			return FALSE;
	}

	// check the colour ramps match
	if (!SameColourRampAs(pAttrib->GetColourRamp()))
		return FALSE;

	return 
	(
		*GetStartColour() 	== *pAttrib->GetStartColour() 	&&
		*GetEndColour()   	== *pAttrib->GetEndColour() 	&&
	
		*GetStartPoint() 	== *pAttrib->GetStartPoint() 	&&
		*GetEndPoint()  	== *pAttrib->GetEndPoint() 		&&
		*GetEndPoint2()   	== *pAttrib->GetEndPoint2()		&&

		dim					== pAttrib->dim					&&
		dpi					== pAttrib->dpi					&&
		tileable			== pAttrib->tileable			&&

		grain				== pAttrib->grain				&&
	    seed				== pAttrib->seed
	); 
}


void NoiseFillAttribute::CacheFractalData(FillGeometryAttribute *pCachedFractal)
{
	ERROR3IF(pCachedFractal==NULL, "NULL pointer passed to NoiseFillAttribute::CacheFractalData");
	ERROR3IF(!IS_A(pCachedFractal,NoiseFillAttribute), "Not a NoiseFillAttribute during NoiseFillAttribute::CacheFractalData");

	pCachedFractal->SetStartPoint(GetStartPoint());
	pCachedFractal->SetEndPoint(GetEndPoint());
	pCachedFractal->SetEndPoint2(GetEndPoint2());

	pCachedFractal->SetFractalDPI(GetFractalDPI());
	pCachedFractal->SetFractalDim(GetFractalDim());
	pCachedFractal->SetTileable(GetTileable());
	pCachedFractal->SetSeed(GetSeed());

	pCachedFractal->SetGraininess(GetGraininess());
}


BOOL NoiseFillAttribute::IsSameAsCachedFractal(FillGeometryAttribute *pCachedFractal)
{
	ERROR3IF(pCachedFractal==NULL, "NULL pointer passed to NoiseFillAttribute::IsSameAsCachedFractal");
	ERROR3IF(!IS_A(pCachedFractal,NoiseFillAttribute), "Not a NoiseFillAttribute during NoiseFillAttribute::IsSameAsCachedFractal");

	return
	(	
		dim			 == pCachedFractal->GetFractalDim()		&&
		tileable	 == pCachedFractal->GetTileable()		&&
		seed		 == pCachedFractal->GetSeed()			&&
		grain		 == pCachedFractal->GetGraininess()	
	);
}


BOOL NoiseFillAttribute::AttachBitmap(KernelBitmap* NewBitmap)
{
	DetachBitmap();

	if (NewBitmap == NULL)
		return FALSE;

	OILBitmap* Bmp = NewBitmap->ActualBitmap;
	if (Bmp == NULL || !Bmp->IsTemp())
		return FALSE;

	NewBitmap = new KernelBitmap(Bmp, TRUE);
	BitmapRef.SetBitmap(NewBitmap);
	GetApplication()->GetGlobalFractalList()->AddFractal(this);
	
	return TRUE;
}


BOOL NoiseFillAttribute::DetachBitmap()
{
	if (GetBitmap() == NULL)
		return FALSE;

	if (GetApplication()->GetGlobalFractalList()->RemoveFractal(this) &&
		BitmapRef.GetBitmap())
	{
		// The fractal was deleted, so make sure we NULL our pointer
		BitmapRef.GetBitmap()->ActualBitmap = NULL;
	}

	// now make sure the kernelbitmap is dead too
	BitmapRef.DeleteBmp();

	return TRUE;
}


BOOL NoiseFillAttribute::CopyBitmap(KernelBitmap* BmpToCopy)
{
	if (BmpToCopy != NULL)	// Is there a bitmap to copy ?
	{
		DetachBitmap();

		return AttachBitmap(BmpToCopy);
	}

	return TRUE;
}


void NoiseFillAttribute::SimpleCopy(AttributeValue *pValue)
{
	// Just use the assignment operator
	*this = *(FillGeometryAttribute*)pValue;
}


void NoiseFillAttribute::SetStartColour(DocColour* NewCol)
{
//#pragma message( __LOCMSG__ "NoiseFillAttribute::SetStartColour - do nothing" )
//	TRACE( _T("Warning - NoiseFillAttribute::SetStartColour called\n") );
	if (NewCol == NULL)
	{
		AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
											_R(IDS_WHITENAME), &Colour);

		if (Colour == EndColour)
		{
			AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
												_R(IDS_BLACKNAME), &Colour);
		}
	}
	else
		Colour = *NewCol;
}


void NoiseFillAttribute::SetEndColour(DocColour* NewCol)
{
//#pragma message( __LOCMSG__ "NoiseFillAttribute::SetEndColour - do nothing" )
//	TRACE( _T("Warning - NoiseFillAttribute::SetEndColour called\n") );
	if (NewCol == NULL)
	{
		AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
											_R(IDS_BLACKNAME), &EndColour);

		if (Colour == EndColour)
		{
			AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
												_R(IDS_WHITENAME), &EndColour);
		}
	}
	else
		EndColour = *NewCol;
}



BOOL NoiseFillAttribute::RecalcFractal()
{
	if (*GetStartPoint() == *GetEndPoint() ||
		*GetStartPoint() == *GetEndPoint2())
	{
		return FALSE;
	}

	KernelBitmap* pBitmap = GenerateNoiseBitmap(grain,seed);
	if (pBitmap == NULL)
	{
		TRACEUSER( "Mike", _T("Noisy fractal Failed !!!\n"));
		return FALSE;	// Error set by GenerateFractalBitmap
	}

	AttachBitmap(pBitmap);
	delete pBitmap;

	return TRUE;
}


void NoiseFillAttribute::SetTesselation(INT32 NewTess)
{
	if (NewTess == RT_NoRepeatType)
		NewTess = RT_Repeating;

	Tesselation = NewTess;
}

void NoiseFillAttribute::SetFractalDim(UINT32 NewDim)
{
	dim=NewDim;
}

BOOL NoiseFillAttribute::SetTileable(BOOL NewTile)
{
	DetachBitmap();		// Ensure the current fractal is removed from cache

	tileable = NewTile;

	return TRUE;
}

BOOL NoiseFillAttribute::SetFractalDPI(UINT32 NewDpi)
{
	dpi = NewDpi;

	return TRUE;
}


BOOL NoiseFillAttribute::SetGraininess(FIXED16 NewGrain)
{
	DetachBitmap();		// Ensure the current fractal is removed from cache

	if (NewGrain<0) NewGrain=0;
	if (NewGrain>100) NewGrain=100;
	grain = NewGrain;	

	return TRUE;
}


BOOL NoiseFillAttribute::SetSeed(INT32 NewSeed)
{
	DetachBitmap();		// Ensure the current fractal is removed from cache

	seed = NewSeed;	

	return TRUE;
}
	


/////////////////////////////////////////////////////////////////////////////////////////////
//
//							ThreeColFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	ThreeColFillAttribute::ThreeColFillAttribute()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

ThreeColFillAttribute::ThreeColFillAttribute()
{
//#pragma message( __LOCMSG__ "ThreeColFillAttribute::ThreeColFillAttribute - do nothing" )
//	TRACE( _T("Warning - ThreeColFillAttribute::ThreeColFillAttribute called\n") );
	IsPersp = FALSE;

	EndPoint2 = DocCoord(0,0);
	AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
										_R(IDS_REDNAME), &EndColour2);
}


/********************************************************************************************

>	INT32 ThreeColFillAttribute::operator==(const FillGeometryAttribute& Attrib)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 ThreeColFillAttribute::operator==(const FillGeometryAttribute& Attrib)
{
	ThreeColFillAttribute* pAttrib = (ThreeColFillAttribute*) &Attrib;
	
	return(GradFillAttribute::operator==(Attrib) && 
			(EndColour2 == pAttrib->EndColour2) &&
			(EndPoint2 == pAttrib->EndPoint2) );
}



/********************************************************************************************

>	NodeAttribute *ThreeColFillAttribute::MakeNode()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrThreeColFill node from this graduated fill attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *ThreeColFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrThreeColFill *pAttr = new AttrThreeColColourFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}


/********************************************************************************************

>	void ThreeColFillAttribute::SetEndPoint2(DocCoord* Pos)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void ThreeColFillAttribute::SetEndPoint2(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint2 = MakeLineAtAngle(*GetStartPoint(), *GetEndPoint(), 90);
	else
		EndPoint2 = *Pos;
}


/********************************************************************************************

>	void ThreeColFillAttribute::SetEndPoint3(DocCoord* Pos)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void ThreeColFillAttribute::SetEndPoint3(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint3 = MakeLineAtAngle(*GetEndPoint(), *GetEndPoint2(), 90);
	else
		EndPoint3 = *Pos;
}


/********************************************************************************************

>	void ThreeColFillAttribute::MakePerspective()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Purpose:	Make the fill perspectived.
				This should be called just before it is transformed by the moulder,
				so it can validate the 4th control point.

********************************************************************************************/

void ThreeColFillAttribute::MakePerspective()
{
	IsPersp = TRUE;

	INT32 dx1 = EndPoint.x - StartPoint.x;
	INT32 dx2 = EndPoint2.x - StartPoint.x;

	INT32 dy1 = EndPoint.y - StartPoint.y;
	INT32 dy2 = EndPoint2.y - StartPoint.y;

	DocCoord Pos3(StartPoint.x + dx1 + dx2, StartPoint.y + dy1 + dy2);
	SetEndPoint3(&Pos3);
}

/********************************************************************************************

>	void ThreeColFillAttribute::RemovePerspective()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Purpose:	Removes perspective from this fill.

********************************************************************************************/

void ThreeColFillAttribute::RemovePerspective()
{
	IsPersp = FALSE;
}


/********************************************************************************************

>	void ThreeColFillAttribute::SetEndColour2(DocColour* NewCol)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/96
	Inputs:		-
	Purpose:	Sets the third colour of this fill (defaults to Red)

********************************************************************************************/

void ThreeColFillAttribute::SetEndColour2(DocColour* NewCol)
{
//#pragma message( __LOCMSG__ "ThreeColFillAttribute::SetEndColour2 - do nothing" )
//	TRACE( _T("Warning - ThreeColFillAttribute::SetEndColour2 called\n") );
	if (NewCol == NULL)
	{
		AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
											_R(IDS_REDNAME), &EndColour2);
	}
	else
		EndColour2 = *NewCol;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//							FourColFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	FourColFillAttribute::FourColFillAttribute()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

FourColFillAttribute::FourColFillAttribute()
{
//#pragma message( __LOCMSG__ "FourColFillAttribute::FourColFillAttribute - do nothing" )
//	TRACE( _T("Warning - FourColFillAttribute::FourColFillAttribute called\n") );
	AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
										_R(IDS_YELLOWNAME), &EndColour3);
}


/********************************************************************************************

>	INT32 FourColFillAttribute::operator==(const FillGeometryAttribute& Attrib)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 FourColFillAttribute::operator==(const FillGeometryAttribute& Attrib)
{
	FourColFillAttribute* pAttrib = (FourColFillAttribute*) &Attrib;
	
	return(ThreeColFillAttribute::operator==(Attrib) && (EndColour3 == pAttrib->EndColour3));
}


/********************************************************************************************

>	NodeAttribute *FourColFillAttribute::MakeNode()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrFourColFill node from this graduated fill attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *FourColFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrFourColFill *pAttr = new AttrFourColColourFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	void FourColFillAttribute::SetEndColour3(DocColour* NewCol)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/96
	Inputs:		-
	Purpose:	Sets the third colour of this fill (default to Yellow)

********************************************************************************************/

void FourColFillAttribute::SetEndColour3(DocColour* NewCol)
{
//#pragma message( __LOCMSG__ "FourColFillAttribute::SetEndColour3 - do nothing" )
//	TRACE( _T("Warning - FourColFillAttribute::SetEndColour3 called\n") );
	if (NewCol == NULL)
	{
		AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
											_R(IDS_YELLOWNAME), &EndColour3);
	}
	else
		EndColour3 = *NewCol;
}




/////////////////////////////////////////////////////////////////////////////////////////////
//
//							TranspFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	TranspFillAttribute::TranspFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

TranspFillAttribute::TranspFillAttribute()
{
	Transp = 0;
	TranspType = TT_Mix;
}

/********************************************************************************************

>	void TranspFillAttribute::SetTranspType(UINT32 NewType)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the Transparency type of this fill

********************************************************************************************/

void TranspFillAttribute::SetTranspType(UINT32 NewType)
{
	if (NewType == TT_NoTranspType)
		NewType = TT_Mix;

	TranspType = NewType;
}

/********************************************************************************************

>	void TranspFillAttribute::SetStartTransp(UINT32* NewTransp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the Start colour of this fill

********************************************************************************************/

void TranspFillAttribute::SetStartTransp(UINT32* NewTransp)
{
	if (NewTransp == NULL)
		Transp = 0;
	else
		Transp = *NewTransp;
}

/********************************************************************************************

>	TranspFillAttribute& TranspFillAttribute::operator=(TranspFillAttribute& FillAttrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		FillAttrib - the attribute to copy, which must be an AttrFillGeometry
	Returns:	A refernce to this object
	Purpose:	Make the Attribute the same as the other. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.

********************************************************************************************/

FillGeometryAttribute& TranspFillAttribute::operator=(FillGeometryAttribute& FillAttrib)
{
	SetStartTransp(FillAttrib.GetStartTransp());
	SetEndTransp(FillAttrib.GetEndTransp());
	SetEndTransp2(FillAttrib.GetEndTransp2());
	SetEndTransp3(FillAttrib.GetEndTransp3());

	SetTranspType(FillAttrib.GetTranspType());

	// Copy the fills profile
	SetProfile (FillAttrib.GetProfile ());

	// Copy the control points
	SetStartPoint(FillAttrib.GetStartPoint());
	SetEndPoint(FillAttrib.GetEndPoint());
	SetEndPoint2(FillAttrib.GetEndPoint2());

	// Bitmap Virtual point stuff
	if ( FillAttrib.IsAKindOfBitmapFill() &&
		 !this->IsAKindOfBitmapFill() )
	{
		DocCoord Start = *FillAttrib.GetStartPoint();
		DocCoord End   = *FillAttrib.GetEndPoint();
		DocCoord End2  = *FillAttrib.GetEndPoint2();

		GetBitmapVirtualPoints( Start, End, End2,
					   		    GetStartPoint(),  GetEndPoint(),  GetEndPoint2());
	}
	else if ( !FillAttrib.IsAKindOfBitmapFill() &&
		 this->IsAKindOfBitmapFill() )
	{
		GetBitmapRealPoints(*GetStartPoint(), *GetEndPoint(), *GetEndPoint2(),
					 		 GetStartPoint(),  GetEndPoint(),  GetEndPoint2());
	}

	if (FillAttrib.IsPerspective())
	{
		MakePerspective();
		SetEndPoint3(FillAttrib.GetEndPoint3());
	}
	else
	{
		RemovePerspective();
	}

	CopyBitmap(FillAttrib.GetBitmap());
	SetTesselation(FillAttrib.GetTesselation());
	SetAspectLock(FillAttrib.IsAspectLocked());
	SetTranspRamp(FillAttrib.GetTranspRamp());

	return *this;
}

/********************************************************************************************

>	INT32 TranspFillAttribute::operator==(const TranspFillAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 TranspFillAttribute::operator==(const FillGeometryAttribute& Attrib)
{
	if (GetRuntimeClass() != Attrib.GetRuntimeClass())
		return FALSE;

	return ((Transp == ((TranspFillAttribute*)&Attrib)->Transp) && 
			(TranspType == ((TranspFillAttribute*)&Attrib)->TranspType) );
}

/********************************************************************************************

>	void TranspFillAttribute::Render(RenderRegion *pRegion)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the fill geometry attribute for the given render region. i.e. all
				paths filled will now be filled with this fill geometry.
	SeeAlso:	TranspFillAttribute; RenderStack; AttributeValue; NodeAttribute;
				TranspFillAttribute::Restore; TranspFillAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void TranspFillAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TranspFillAttribute::Render - do nothing" )
//	TRACE( _T("Warning - TranspFillAttribute::Render called\n") );
	pRegion->SetTranspFillGeometry(this, Temp);
}

/********************************************************************************************

>	void TranspFillAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the fill type attribute for the given render region. i.e. all
				paths filled will now be filled with this fill attribute.
	SeeAlso:	TranspFillAttribute; RenderStack; AttributeValue; NodeAttribute;
				TranspFillAttribute::Render; TranspFillAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void TranspFillAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TranspFillAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TranspFillAttribute::Restore called\n") );
	pRegion->RestoreTranspFillGeometry(this, Temp);
}

/********************************************************************************************

>	BOOL TranspFillAttribute::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers fill type attribute, and provides a default attribute to give
				transparent flat fills, i.e. actually not filled at all.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL TranspFillAttribute::Init()
{
	// Default to simple transparent fill colour
	FlatTranspFillAttribute *pAttr = new FlatTranspFillAttribute;
	if (pAttr==NULL)
		// error message has already been set by new
		return FALSE;

	pAttr->Transp    = 0;
	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising TranspFillAttribute");
	ERROR2IF(ID != ATTR_TRANSPFILLGEOMETRY, FALSE, "Incorrect ID for TranspFillAttribute");

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//							FlatTranspFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	FlatTranspFillAttribute::FlatTranspFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

FlatTranspFillAttribute::FlatTranspFillAttribute()
{
}

/********************************************************************************************

>	NodeAttribute *FlatTranspFillAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrFlatColourFill node from this flat fill colour attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode; AttrFlatFill

********************************************************************************************/

NodeAttribute *FlatTranspFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrFlatTranspFill *pAttr = new AttrFlatTranspFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}


/********************************************************************************************

>	virtual ColourFillAttribute *FlatTranspFillAttribute::MakeSimilarNonTranspFillGeometry(
																	 double TransparencyScale)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/02/97
	Inputs:		TransparencyScale	- Value between 0.0 and 1.0 which denotes by how much
									  the function will scale the transparency. 0 means
									  totally transparent.

	Returns:	Pointer to the new attribute, or NULL if out of memory, problems, etc.

	Purpose:	Creates a non-transparent version of this transparent fill attribute.
				For Flat fills we just return a FlatFillAttribute whose start colour is
				set to a greyscale version of the intensity of the transparency.
				(The original use of this was so airbrushes could maintain their fill's
				transparency geometry)

	SeeAlso:	TranspFillAttribute::MakeSimilarNonTranspFillGeometry; AttrFlatFill

********************************************************************************************/

ColourFillAttribute *FlatTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)
{
	UINT32 *pStartTransparency = GetStartTransp();
	if(pStartTransparency == NULL)
		return NULL;

	FlatFillAttribute *pNewAttr = new FlatFillAttribute;
	if (pNewAttr != NULL)
	{
		INT32 Transparency = 255 - (INT32)(((double)(255 - *pStartTransparency)) * TransparencyScale);
		DocColour		color(Transparency, Transparency, Transparency);
		pNewAttr->SetStartColour( &color );
	}

	return(pNewAttr);
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//							GradTranspFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	GradTranspFillAttribute::GradTranspFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

GradTranspFillAttribute::GradTranspFillAttribute()
{					   
	EndTransp = 255;

	StartPoint = DocCoord(0,0);
	EndPoint = DocCoord(0,0);

	m_pTranspRamp = NULL;
}

GradTranspFillAttribute::~GradTranspFillAttribute()
{
	DeleteTranspRamp();
}

void GradTranspFillAttribute::DeleteTranspRamp()
{
	if (m_pTranspRamp!=NULL)
	{
		delete m_pTranspRamp;
		m_pTranspRamp=NULL;
	}
}


/********************************************************************************************

	BOOL GradTranspFillAttribute::SameTransparencyRampAs(TransparencyRamp *pOtherRamp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/03/97
	Purpose:	Check whether this transparency ramp matches that of the attribute

********************************************************************************************/

BOOL GradTranspFillAttribute::SameTransparencyRampAs(TransparencyRamp *pOtherRamp)
{
	// Have we got a transparency ramp at the moment?
	if (m_pTranspRamp==NULL)
		return (pOtherRamp==NULL);

	if (pOtherRamp==NULL)
		return FALSE;

	return (!( m_pTranspRamp->IsDifferentTo(pOtherRamp)));
}


/********************************************************************************************

>	TranspFillAttribute& GradTranspFillAttribute::operator=(TranspFillAttribute& FillAttrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		FillAttrib - the attribute to copy, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	Make the Attribute the same as the other. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	AttrFillGeometry::operator==; NodeAttribute::operator==

********************************************************************************************/

FillGeometryAttribute& GradTranspFillAttribute::operator=(FillGeometryAttribute& FillAttrib)
{
	return TranspFillAttribute::operator=(FillAttrib);
}

/********************************************************************************************

>	INT32 GradTranspFillAttribute::operator==(const FillGeometryAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 GradTranspFillAttribute::operator==(const FillGeometryAttribute& Attrib)
{
	if (GetRuntimeClass() != Attrib.GetRuntimeClass())
		return FALSE;

	GradTranspFillAttribute* pAttrib = (GradTranspFillAttribute*)&Attrib;

	if (IsPerspective())
	{ 
		if (!pAttrib->IsPerspective())
			return FALSE;

		if (*GetEndPoint2() != *pAttrib->GetEndPoint2())
			return FALSE;

		if (*GetEndPoint3() != *pAttrib->GetEndPoint3())
			return FALSE;
	}

	// check the transparency ramp matches
	if (!SameTransparencyRampAs(pAttrib->GetTranspRamp()))
		return FALSE;
	
	// Are the Colours and Control points all the same ?
	return (

		Transp 		== pAttrib->Transp 		&&
		EndTransp 	== pAttrib->EndTransp 	&&

		TranspType	== pAttrib->TranspType	&&

		StartPoint 	== pAttrib->StartPoint 	&&
		EndPoint  	== pAttrib->EndPoint
	);
}


/********************************************************************************************

>	BOOL GradTranspFillAttribute::SetTranspRamp(TransparencyRamp *pRamp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97
	Inputs:		pRamp, a pointer to a transparency ramp or NULL
	Purpose:	Sets this transparency ramp from pRamp passed in. If pRamp is NULL on entry
				this objects ramp will be emptied.

********************************************************************************************/

BOOL GradTranspFillAttribute::SetTranspRamp(TransparencyRamp *pRamp)
{
	DeleteTranspRamp();
	if (pRamp==NULL)
		return TRUE;
	
	m_pTranspRamp = new TransparencyRamp();
	if (m_pTranspRamp==NULL)
		return FALSE;

	*m_pTranspRamp = *pRamp;
	return TRUE;
}

/********************************************************************************************

>	DocCoord GradTranspFillAttribute::GetGeometryCoord(float pos) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/97
	Inputs:		pos = a parameter between 0 and 1
	Returns:	A coordinate which relates the input parameter to a coordinate space
				on this geometry. For instance a linear fill would have a linear parameter
				space, 0 at the start coordinate, increasing to 1 at the end coordinate
	Purpose:	Find the absolute position in geometry coordinate space for the given
				parameter.

********************************************************************************************/

DocCoord GradTranspFillAttribute::GetGeometryCoord(float pos) const
{
	// This and all derived classes will inherit a linear coordinate space
	return DocCoord::PositionPointFromRatio(StartPoint,EndPoint,(double)pos);
}


/********************************************************************************************

>	float GradTranspFillAttribute::GetGeometryParam(const DocCoord& c) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/97
	Inputs:		c = a coordinate
	Returns:	A parameter p, 0<=p<=1, such that GetGeometryCoord(p) is the closest
				coordinate in the geometry to the input c.
	Purpose:	Find the parameter for the closest point to c on this geometry.

********************************************************************************************/

float GradTranspFillAttribute::GetGeometryParam(const DocCoord& c) const
{
	// ok we're a linear geometry so find the closest point to a line type of thing.
	// use a handy util written by that other fab bloke called Mike.
	DocCoord Coords[2];
	Coords[0] = StartPoint;
	Coords[1] = EndPoint;
	double p;
	PathUtil::SqrDistanceToLine(Coords, c, &p);
	return (float)p;
}


/********************************************************************************************

>	TransparencyRamp* GradTranspFillAttribute::MakeNewTranspRamp()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97
	Inputs:		-
	Purpose:	Create a blank transparency ramp

********************************************************************************************/

TransparencyRamp* GradTranspFillAttribute::MakeNewTranspRamp()
{
	DeleteTranspRamp();
	m_pTranspRamp=new TransparencyRamp();
	return m_pTranspRamp;
}


/********************************************************************************************

>	void GradTranspFillAttribute::SetStartPoint(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the Start Point of this fill

********************************************************************************************/

void GradTranspFillAttribute::SetStartPoint(DocCoord* Pos)
{
	if (Pos == NULL)
		StartPoint = DocCoord(0,0);
	else
		StartPoint = *Pos;
}

/********************************************************************************************

>	void GradTranspFillAttribute::SetEndPoint(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void GradTranspFillAttribute::SetEndPoint(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint = DocCoord(0,0);
	else
		EndPoint = *Pos;
}

/********************************************************************************************

>	void GradTranspFillAttribute::SetEndTransp(UINT32* NewTransp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the End Colour of this fill

********************************************************************************************/

void GradTranspFillAttribute::SetEndTransp(UINT32* NewTransp)
{
	if (NewTransp == NULL)
		EndTransp = 255;
	else
		EndTransp = *NewTransp;
}

/********************************************************************************************

>	void GradTranspFillAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	GradFillAttribute; RenderStack; AttributeValue; NodeAttribute;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void GradTranspFillAttribute::SimpleCopy(AttributeValue *pValue)
{
	*(this) = *((FillGeometryAttribute*)pValue);
}



/********************************************************************************************

>	virtual ColourFillAttribute *GradTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/02/97
	Inputs:		TransparencyScale	- Value between 0.0 and 1.0 which denotes by how much
									  the function will scale the transparency

	Returns:	Pointer to the new attribute, or NULL if out of memory, problems, etc.

	Purpose:	Creates a non-transparent version of this transparent fill attribute.
				(The original use of this was so airbrushes could maintain their fill's
				transparency geometry)

	SeeAlso:	TranspFillAttribute::MakeSimilarNonTranspFillGeometry; AttrFlatFill
				FlatTranspFillAttribute::MakeSimilarNonTranspFillGeometry

********************************************************************************************/

ColourFillAttribute *GradTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)
{
	UINT32 *pStartTransp = GetStartTransp();
	UINT32 *pEndTransp = GetEndTransp();

	if(pStartTransp == NULL || pEndTransp == NULL)
		return NULL;

	// This one's a bit dodgy I think	
	GradFillAttribute *pNewAttr = new GradFillAttribute;	
	if (pNewAttr != NULL)
	{
		pNewAttr->SetStartPoint(GetStartPoint());
		pNewAttr->SetEndPoint(GetEndPoint());

		INT32 StartTransparency = 255 - (INT32)(((double)(255 - *pStartTransp)) * TransparencyScale);
		INT32 EndTransparency = 255 - (INT32)(((double)(255 - *pEndTransp)) * TransparencyScale);

		DocColour		colorStart(StartTransparency, StartTransparency, StartTransparency);
		DocColour		colorEnd(EndTransparency, EndTransparency, EndTransparency);
		pNewAttr->SetStartColour(&colorStart);
		pNewAttr->SetEndColour(&colorEnd);
	
		// if this transparency has a transparency ramp then make
		// a colour ramp version in pNewAttr
		if (m_pTranspRamp)
		{
			ColourRamp* pColRamp = pNewAttr->MakeNewColourRamp();
			if (pColRamp)
			{
				INT32 Col;
				TranspRampItem *pItem = m_pTranspRamp->GetFirstTransp();
				while (pItem!=NULL)
				{
					Col = 255 - (INT32)(((double)(255 - pItem->GetTransparency())) * TransparencyScale);
					DocColour colorEntry(Col,Col,Col);
					pColRamp->AddEntry(pItem->GetPosition(), &colorEntry );
					pItem=m_pTranspRamp->GetNextTransp(pItem);
				}
			}
		}
	}

	return(pNewAttr);
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//							LinearTranspFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	LinearTranspFillAttribute::LinearTranspFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

LinearTranspFillAttribute::LinearTranspFillAttribute()
{
	IsPersp = FALSE;
}

/********************************************************************************************

>	NodeAttribute *LinearTranspFillAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrLinearFill node from this graduated fill attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *LinearTranspFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrLinearTranspFill *pAttr = new AttrLinearTranspFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	void LinearTranspFillAttribute::SetEndPoint2(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void LinearTranspFillAttribute::SetEndPoint2(DocCoord* Pos)
{
	if (Pos == NULL)
	{
		EndPoint2 = MakeLineAtAngle(StartPoint, EndPoint, 90);
	}
	else
		EndPoint2 = *Pos;
}


/********************************************************************************************

>	void LinearTranspFillAttribute::SetEndPoint3(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void LinearTranspFillAttribute::SetEndPoint3(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint3 = MakeLineAtAngle(*GetEndPoint(), *GetEndPoint2(), 90);
	else
		EndPoint3 = *Pos;
}


/********************************************************************************************

>	void LinearTranspFillAttribute::MakePerspective()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		-
	Purpose:	Make the fill perspectived.
				This should be called just before it is transformed by the moulder,
				so it can validate the 4th control point.

********************************************************************************************/

void LinearTranspFillAttribute::MakePerspective()
{
	IsPersp = TRUE;

	INT32 dx1 = EndPoint.x - StartPoint.x;
	INT32 dx2 = EndPoint2.x - StartPoint.x;

	INT32 dy1 = EndPoint.y - StartPoint.y;
	INT32 dy2 = EndPoint2.y - StartPoint.y;

	DocCoord Pos3(StartPoint.x + dx1 + dx2, StartPoint.y + dy1 + dy2);
	SetEndPoint3(&Pos3);
}

/********************************************************************************************

>	void LinearTranspFillAttribute::RemovePerspective()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		-
	Purpose:	Removes perspective from this fill.

********************************************************************************************/

void LinearTranspFillAttribute::RemovePerspective()
{
	IsPersp = FALSE;
}

/********************************************************************************************

>	virtual ColourFillAttribute *LinearTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/02/97
	Inputs:		TransparencyScale	- Value between 0.0 and 1.0 which denotes by how much
									  the function will scale the transparency

	Returns:	Pointer to the new attribute, or NULL if out of memory, problems, etc.

	Purpose:	Creates a non-transparent version of this transparent fill attribute.
				(The original use of this was so airbrushes could maintain their fill's
				transparency geometry)

	SeeAlso:	TranspFillAttribute::MakeSimilarNonTranspFillGeometry; AttrFlatFill
				FlatTranspFillAttribute::MakeSimilarNonTranspFillGeometry

********************************************************************************************/

ColourFillAttribute *LinearTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)
{
	UINT32 *pStartTransp = GetStartTransp();
	UINT32 *pEndTransp = GetEndTransp();

	if(pStartTransp == NULL || pEndTransp == NULL)
		return NULL;
	
	LinearFillAttribute *pNewAttr = new LinearFillAttribute;	
	if (pNewAttr != NULL)
	{
		pNewAttr->SetStartPoint(GetStartPoint());
		pNewAttr->SetEndPoint(GetEndPoint());
		pNewAttr->SetEndPoint2(GetEndPoint2());
		pNewAttr->SetEndPoint3(GetEndPoint3());

		if(IsPerspective())
			pNewAttr->MakePerspective();

		INT32 StartTransparency = 255 - (INT32)(((double)(255 - *pStartTransp)) * TransparencyScale);
		INT32 EndTransparency = 255 - (INT32)(((double)(255 - *pEndTransp)) * TransparencyScale);

		DocColour		colorStart(StartTransparency, StartTransparency, StartTransparency);
		DocColour		colorEnd1(EndTransparency, EndTransparency, EndTransparency);
		pNewAttr->SetStartColour(&colorStart);
		pNewAttr->SetEndColour(&colorEnd1);
	}

	return(pNewAttr);
}



/********************************************************************************************

>	virtual AttributeValue *LinearTranspFillAttribute::MouldIntoStroke(PathStrokerVector *pMoulder,
																			double TransScale);
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/97

	Inputs:		pMoulder	- A PathStrokerVector which knows how to translate points to "mould" them
							  (May be NULL, in which case moulding of points does not occur)

				TransScale	- A fraction between 0.0 and 1.0, by which any transparency
							  values in this geometry will be scaled, allowing the caller to
							  effectively apply a flat transparency level to everything that is
							  moulded. Use 1.0 to leave transparency unaltered.

	Returns:	NULL if the original attribute can be used, else
				A pointer to a copy of this object, suitably moulded and adjusted for
				the transparency scaling. The caller must delete the copy when finished
				with it.

	Purpose:	Helper function for the PathStrokerVector class, which "moulds" clipart
				subtrees to lie along an arbitrary path.

				This function is called to mould fill geometries, so that fill endpoints
				are translated to appropriate positions in the destination envelope, and
				allows the caller to effectively apply a flat transparency by scaling
				any transparency values in this geometry by the given fraction.

********************************************************************************************/

AttributeValue *LinearTranspFillAttribute::MouldIntoStroke(PathStrokerVector *pMoulder, double TransScale)
{
// Taken out of WEBSTER Neville 6/8/97
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	if (pMoulder == NULL)
		return(NULL);

	FillGeometryAttribute *pCopy = (FillGeometryAttribute *) GetRuntimeClass()->CreateObject();
	if (pCopy != NULL)
	{
		// Copy this object's values across
		*pCopy = *this;

		// And mould all of the geometry points across
		JCW_MOULDPOINT(pCopy, StartPoint);
		JCW_MOULDPOINT(pCopy, EndPoint);

		if (IsPerspective())
		{
			// If it's perspectivised, just map all the points and hope for the best
			JCW_MOULDPOINT(pCopy, EndPoint2);
			JCW_MOULDPOINT(pCopy, EndPoint3);
		}
		else
		{
			// Otherwise, we have mapped the start/end points, and we pass NULL in for
			// the remaining points so that they are re-generated to get the fill going
			// in the start->end direction (because the fill direction is not controlled
			// by the start->end line, but by the end->end2 line)
			pCopy->SetEndPoint2(NULL);
			pCopy->SetEndPoint3(NULL);
		}

		ERROR3IF(TransScale < 0.0 || TransScale > 1.0, "Out of range TransScale parameter");
		if (TransScale >= 0.0 && TransScale < 1.0)
		{
			JCW_MOULDTRANS(pCopy, StartTransp);
			JCW_MOULDTRANS(pCopy, EndTransp);
		}
	}
	return(pCopy);

#else
	return NULL;
#endif // VECTOR_STROKING
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//							RadialTranspFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	RadialTranspFillAttribute::RadialTranspFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

RadialTranspFillAttribute::RadialTranspFillAttribute()
{
	Circular = FALSE;
	IsPersp = FALSE;

	EndPoint2 = DocCoord(0,0);
}

/********************************************************************************************

>	void RadialTranspFillAttribute::SetEndPoint2(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void RadialTranspFillAttribute::SetEndPoint2(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint2 = DocCoord(0,0);
	else
		EndPoint2 = *Pos;
}

/********************************************************************************************

>	TranspFillAttribute& RadialTranspFillAttribute::operator=(TranspFillAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

FillGeometryAttribute& RadialTranspFillAttribute::operator=(FillGeometryAttribute& Attrib)
{
	return GradTranspFillAttribute::operator=(Attrib);
}

/********************************************************************************************

>	INT32 RadialTranspFillAttribute::operator==(const TranspFillAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 RadialTranspFillAttribute::operator==(const FillGeometryAttribute& Attrib)
{
	if (GetRuntimeClass() != Attrib.GetRuntimeClass())
		return FALSE;

	RadialTranspFillAttribute* pAttrib = (RadialTranspFillAttribute*)&Attrib;

	if (IsPerspective())
	{ 
		if (!pAttrib->IsPerspective())
			return FALSE;

		if (*GetEndPoint3() != *pAttrib->GetEndPoint3())
			return FALSE;
	}

	// check the transparency ramp matches
	if (!SameTransparencyRampAs(pAttrib->GetTranspRamp()))
		return FALSE;

	// Are the Colours and Control points all the same ?
	return (
	
		Transp 		== pAttrib->Transp 		&&
		EndTransp 	== pAttrib->EndTransp 	&&

		TranspType	== pAttrib->TranspType	&&
					   
		StartPoint 	== pAttrib->StartPoint 	&&
		EndPoint  	== pAttrib->EndPoint 	&&
		EndPoint2   == pAttrib->EndPoint2	&&

		Circular 	== pAttrib->Circular
	);
}

/********************************************************************************************

>	NodeAttribute *RadialTranspFillAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrRadialFill node from this graduated fill attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *RadialTranspFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrRadialTranspFill *pAttr = new AttrRadialTranspFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}


/********************************************************************************************

>	void RadialTranspFillAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	RadialTranspFillAttribute; RenderStack; AttributeValue; NodeAttribute;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void RadialTranspFillAttribute::SimpleCopy(AttributeValue *pValue)
{
	// Base class does most of the work...
	GradTranspFillAttribute::SimpleCopy(pValue);
}

/********************************************************************************************

>	void RadialTranspFillAttribute::MakeCircular()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Purpose:	Forces this radial fill into a circular one.

********************************************************************************************/

void RadialTranspFillAttribute::MakeCircular()
{
	// Set a flag so we can tell we're circular
	Circular = TRUE;

	// Make sure the second end point is on the same radius as the other
	EndPoint2 = MakeLineAtAngle(StartPoint, EndPoint, 90);
}

/********************************************************************************************

>	void RadialTranspFillAttribute::MakeElliptical()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Purpose:	Forces this radial fill into an elliptical one.

********************************************************************************************/

void RadialTranspFillAttribute::MakeElliptical()
{
	// Just clear the flag, so we don't try and lock the secondary point
	Circular = FALSE;
}

/********************************************************************************************

>	void RadialTranspFillAttribute::SetEndPoint3(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void RadialTranspFillAttribute::SetEndPoint3(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint3 = MakeLineAtAngle(*GetEndPoint(), *GetEndPoint2(), 90);
	else
		EndPoint3 = *Pos;
}


/********************************************************************************************

>	void RadialTranspFillAttribute::MakePerspective()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		-
	Purpose:	Make the fill perspectived.
				This should be called just before it is transformed by the moulder,
				so it can validate the 4th control point.

********************************************************************************************/

void RadialTranspFillAttribute::MakePerspective()
{
	IsPersp = TRUE;

	INT32 dx1 = EndPoint.x - StartPoint.x;
	INT32 dx2 = EndPoint2.x - StartPoint.x;

	INT32 dy1 = EndPoint.y - StartPoint.y;
	INT32 dy2 = EndPoint2.y - StartPoint.y;

	DocCoord Pos3(StartPoint.x + dx1 + dx2, StartPoint.y + dy1 + dy2);
	SetEndPoint3(&Pos3);
}

/********************************************************************************************

>	void RadialTranspFillAttribute::RemovePerspective()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		-
	Purpose:	Removes perspective from this fill.

********************************************************************************************/

void RadialTranspFillAttribute::RemovePerspective()
{
	IsPersp = FALSE;
}

/********************************************************************************************

>	virtual ColourFillAttribute *RadialTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/02/97
	Inputs:		TransparencyScale	- Value between 0.0 and 1.0 which denotes by how much
									  the function will scale the transparency

	Returns:	Pointer to the new attribute, or NULL if out of memory, problems, etc.

	Purpose:	Creates a non-transparent version of this transparent fill attribute.
				(The original use of this was so airbrushes could maintain their fill's
				transparency geometry)

	SeeAlso:	TranspFillAttribute::MakeSimilarNonTranspFillGeometry; AttrFlatFill
				FlatTranspFillAttribute::MakeSimilarNonTranspFillGeometry

********************************************************************************************/

ColourFillAttribute *RadialTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)
{
	UINT32 *pStartTransp = GetStartTransp();
	UINT32 *pEndTransp = GetEndTransp();

	if(pStartTransp == NULL || pEndTransp == NULL)
		return NULL;
	
	RadialFillAttribute *pNewAttr = new RadialFillAttribute;	
	if (pNewAttr != NULL)
	{
		pNewAttr->SetStartPoint(GetStartPoint());
		pNewAttr->SetEndPoint(GetEndPoint());
		pNewAttr->SetEndPoint2(GetEndPoint2());
		pNewAttr->SetEndPoint3(GetEndPoint3());

		if(IsCircular())
			pNewAttr->MakeCircular();
		else
			pNewAttr->MakeElliptical();

		if(IsPerspective())
			pNewAttr->MakePerspective();

		INT32 StartTransparency = 255 - (INT32)(((double)(255 - *pStartTransp)) * TransparencyScale);
		INT32 EndTransparency = 255 - (INT32)(((double)(255 - *pEndTransp)) * TransparencyScale);

		DocColour		colorStart(StartTransparency, StartTransparency, StartTransparency);
		DocColour		colorEnd1(EndTransparency, EndTransparency, EndTransparency);
		pNewAttr->SetStartColour(&colorStart);
		pNewAttr->SetEndColour(&colorEnd1);
	}

	return(pNewAttr);
}



/********************************************************************************************

>	virtual AttributeValue *RadialTranspFillAttribute::MouldIntoStroke(PathStrokerVector *pMoulder,
																			double TransScale);
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/97

	Inputs:		pMoulder	- A PathStrokerVector which knows how to translate points to "mould" them
							  (May be NULL, in which case moulding of points does not occur)

				TransScale	- A fraction between 0.0 and 1.0, by which any transparency
							  values in this geometry will be scaled, allowing the caller to
							  effectively apply a flat transparency level to everything that is
							  moulded. Use 1.0 to leave transparency unaltered.

	Returns:	NULL if the original attribute can be used, else
				A pointer to a copy of this object, suitably moulded and adjusted for
				the transparency scaling. The caller must delete the copy when finished
				with it.

	Purpose:	Helper function for the PathStrokerVector class, which "moulds" clipart
				subtrees to lie along an arbitrary path.

				This function is called to mould fill geometries, so that fill endpoints
				are translated to appropriate positions in the destination envelope, and
				allows the caller to effectively apply a flat transparency by scaling
				any transparency values in this geometry by the given fraction.

********************************************************************************************/

AttributeValue *RadialTranspFillAttribute::MouldIntoStroke(PathStrokerVector *pMoulder, double TransScale)
{
// Taken out of WEBSTER Neville 6/8/97
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	if (pMoulder == NULL)
		return(NULL);

	RadialFillAttribute *pCopy = (RadialFillAttribute *) GetRuntimeClass()->CreateObject();
	if (pCopy != NULL)
	{
		// Copy this object's values across
		*pCopy = *this;

		// Make sure it's defined as an elliptical fill, to ensure all points are set up correctly
		pCopy->MakeElliptical();

		// And mould all of the geometry points across
		JCW_MOULDPOINT(pCopy, StartPoint);
		JCW_MOULDPOINT(pCopy, EndPoint);
		JCW_MOULDPOINT(pCopy, EndPoint2);
		JCW_MOULDPOINT(pCopy, EndPoint3);

		ERROR3IF(TransScale < 0.0 || TransScale > 1.0, "Out of range TransScale parameter");
		if (TransScale >= 0.0 && TransScale < 1.0)
		{
			JCW_MOULDTRANS(pCopy, StartTransp);
			JCW_MOULDTRANS(pCopy, EndTransp);
		}
	}
	return(pCopy);
#else
	return NULL;
#endif // VECTOR_STROKING
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//							ConicalTranspFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	ConicalTranspFillAttribute::ConicalTranspFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

ConicalTranspFillAttribute::ConicalTranspFillAttribute()
{
}

/********************************************************************************************

>	NodeAttribute *ConicalTranspFillAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrConicalFill node from this graduated fill attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *ConicalTranspFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrConicalTranspFill *pAttr = new AttrConicalTranspFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	DocCoord ConicalTranspFillAttribute::GetGeometryCoord(float pos) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/97
	Inputs:		pos = a parameter between 0 and 1
	Returns:	A coordinate which relates the input parameter to a coordinate space
				on this geometry. For instance a linear fill would have a linear parameter
				space, 0 at the start coordinate, increasing to 1 at the end coordinate
	Purpose:	Find the absolute position in geometry coordinate space for the given
				parameter.

********************************************************************************************/

DocCoord ConicalTranspFillAttribute::GetGeometryCoord(float pos) const
{
	// Here we calculate a circular coordinate space
	return PathUtil::PointOnSemiCircle(StartPoint,EndPoint,(double)pos);
}



/********************************************************************************************

>	float ConicalTranspFillAttribute::GetGeometryParam(const DocCoord& c) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/97
	Inputs:		c = a coordinate
	Returns:	A parameter p, 0<=p<=1, such that GetGeometryCoord(p) is the closest
				coordinate in the geometry to the input c.
	Purpose:	Find the parameter for the closest point to c on this geometry.

********************************************************************************************/

float ConicalTranspFillAttribute::GetGeometryParam(const DocCoord& c) const
{
	// ok we're a linear geometry so find the closest point to a line type of thing.
	// use a handy util written by that other fab bloke called Mike.
	DocCoord Coords[2];
	Coords[0] = StartPoint;
	Coords[1] = EndPoint;
	double p;
	PathUtil::SqrDistanceToSemiCircle(Coords, c, &p);
	return (float)p;
}

/********************************************************************************************

>	virtual ColourFillAttribute *ConicalTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/02/97
	Inputs:		TransparencyScale	- Value between 0.0 and 1.0 which denotes by how much
									  the function will scale the transparency

	Returns:	Pointer to the new attribute, or NULL if out of memory, problems, etc.

	Purpose:	Creates a non-transparent version of this transparent fill attribute.
				(The original use of this was so airbrushes could maintain their fill's
				transparency geometry)

	SeeAlso:	TranspFillAttribute::MakeSimilarNonTranspFillGeometry; AttrFlatFill
				FlatTranspFillAttribute::MakeSimilarNonTranspFillGeometry

********************************************************************************************/

ColourFillAttribute *ConicalTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)
{
	UINT32 *pStartTransp = GetStartTransp();
	UINT32 *pEndTransp = GetEndTransp();

	if(pStartTransp == NULL || pEndTransp == NULL)
		return NULL;
	
	ConicalFillAttribute *pNewAttr = new ConicalFillAttribute;	
	if (pNewAttr != NULL)
	{
		pNewAttr->SetStartPoint(GetStartPoint());
		pNewAttr->SetEndPoint(GetEndPoint());

		INT32 StartTransparency = 255 - (INT32)(((double)(255 - *pStartTransp)) * TransparencyScale);
		INT32 EndTransparency = 255 - (INT32)(((double)(255 - *pEndTransp)) * TransparencyScale);

		DocColour		colorStart(StartTransparency, StartTransparency, StartTransparency);
		DocColour		colorEnd1(EndTransparency, EndTransparency, EndTransparency);
		pNewAttr->SetStartColour(&colorStart);
		pNewAttr->SetEndColour(&colorEnd1);
	}

	return(pNewAttr);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//							BitmapTranspFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

BOOL BitmapTranspFillAttribute::m_doBitmapSmoothing = TRUE;

/********************************************************************************************

>	BitmapTranspFillAttribute::BitmapTranspFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

BitmapTranspFillAttribute::BitmapTranspFillAttribute()
{
	Transp = EndTransp = 0;

	StartPoint = DocCoord(0,0);
	EndPoint   = DocCoord(0,0);
	EndPoint2  = DocCoord(0,0);
	EndPoint3  = DocCoord(0,0);

	Tesselation = RT_Repeating;

	IsPersp = FALSE;
}

/********************************************************************************************

>	BitmapTranspFillAttribute::~BitmapTranspFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Destructor for bitmap fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

BitmapTranspFillAttribute::~BitmapTranspFillAttribute()
{
}

/********************************************************************************************

>	NodeAttribute *BitmapTranspFillAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrFlatColourFill node from this flat fill colour attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode; AttrFlatColourFill

********************************************************************************************/

NodeAttribute *BitmapTranspFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrBitmapFill *pAttr = new AttrBitmapTranspFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	KernelBitmap* BitmapTranspFillAttribute::GetBitmap()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/12/94
	Purpose:	Get the bitmap referenced by this attribute. 
	SeeAlso:	BitmapTranspFillAttribute::AttachBitmap()

********************************************************************************************/

KernelBitmap* BitmapTranspFillAttribute::GetBitmap()
{
	KernelBitmap* pBitmap = BitmapRef.GetBitmap();
	
	// Check for a deleted bitmap
	if (pBitmap && pBitmap->HasBeenDeleted())
	{
		ERROR2IF(pBitmap->GetParentBitmapList() == NULL, NULL, "Deleted bitmap has no parent list");
		
		// Use the default bitmap instead
		pBitmap = pBitmap->GetParentBitmapList()->FindDefaultBitmap();

		// There should always be a default bitmap in the list
		ERROR2IF(pBitmap == NULL, 0L, "Couldn't find the default bitmap");
	}

	return pBitmap;
}

/********************************************************************************************

>	BOOL BitmapTranspFillAttribute::AttachBitmap(KernelBitmap* pBitmap)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Purpose:	Attaches a bitmap to this fill. 
	SeeAlso:	BitmapTranspFillAttribute::DetachBitmap()

********************************************************************************************/

BOOL BitmapTranspFillAttribute::AttachBitmap(KernelBitmap* NewBitmap)
{
	BitmapRef.Detach();
	BitmapRef.Attach(NewBitmap);

	return TRUE;
}

/********************************************************************************************

>	BOOL BitmapTranspFillAttribute::DetachBitmap()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Purpose:	Detaches a bitmap from this fill. 
	SeeAlso:	BitmapTranspFillAttribute::AttachBitmap()

********************************************************************************************/

BOOL BitmapTranspFillAttribute::DetachBitmap()
{
	BitmapRef.Detach();

	return FALSE;
}

/********************************************************************************************

>	BOOL BitmapTranspFillAttribute::CopyBitmap(KernelBitmap* BmpToCopy)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Purpose:	Used to copy a bitmap from one fill to another. 
	SeeAlso:	BitmapTranspFillAttribute::AttachBitmap()

********************************************************************************************/

BOOL BitmapTranspFillAttribute::CopyBitmap(KernelBitmap* BmpToCopy)
{
	if (BmpToCopy != NULL)	// Is there a bitmap to copy ?
	{
		BitmapRef.Detach();

		OILBitmap* Bmp = BmpToCopy->ActualBitmap;
		if (Bmp->IsTemp())
		{
			BitmapRef.SetBitmap(BmpToCopy);
			return FALSE;
		}

		BitmapRef.Attach(BmpToCopy);
	}

	return TRUE;
}

/********************************************************************************************

>	void BitmapTranspFillAttribute::Render(RenderRegion *pRegion)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the fill geometry attribute for the given render region. i.e. all
				paths filled will now be filled with this fill geometry.
	SeeAlso:	BitmapFillAttribute; RenderStack; AttributeValue; NodeAttribute;
				BitmapFillAttribute::Restore; BitmapFillAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BitmapTranspFillAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "BitmapTranspFillAttribute::Render - do nothing" )
//	TRACE( _T("Warning - BitmapTranspFillAttribute::Render called\n") );
	pRegion->SetTranspFillGeometry(this, Temp);
}

/********************************************************************************************

>	void BitmapTranspFillAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the fill type attribute for the given render region. i.e. all
				paths filled will now be filled with this fill attribute.
	SeeAlso:	BitmapFillAttribute; RenderStack; AttributeValue; NodeAttribute;
				BitmapFillAttribute::Render; BitmapFillAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BitmapTranspFillAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "BitmapTranspFillAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - BitmapTranspFillAttribute::Restore called\n") );
	pRegion->RestoreTranspFillGeometry(this, Temp);
}

/********************************************************************************************

>	void BitmapTranspFillAttribute::SetStartPoint(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the Start Point of this fill

********************************************************************************************/

void BitmapTranspFillAttribute::SetStartPoint(DocCoord* Pos)
{
	if (Pos == NULL)
		StartPoint = DocCoord(0,0);
	else
		StartPoint = *Pos;
}

/********************************************************************************************

>	void BitmapTranspFillAttribute::SetEndPoint(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void BitmapTranspFillAttribute::SetEndPoint(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint = DocCoord(0,0);
	else
		EndPoint = *Pos;
}

/********************************************************************************************

>	void BitmapTranspFillAttribute::SetEndPoint2(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void BitmapTranspFillAttribute::SetEndPoint2(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint2 = MakeLineAtAngle(*GetStartPoint(), *GetEndPoint(), 90);
	else
		EndPoint2 = *Pos;
}

/********************************************************************************************

>	void BitmapTranspFillAttribute::SetEndPoint3(DocCoord* Pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void BitmapTranspFillAttribute::SetEndPoint3(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint3 = MakeLineAtAngle(*GetEndPoint(), *GetEndPoint2(), 90);
	else
		EndPoint3 = *Pos;
}



/********************************************************************************************

>	void BitmapTranspFillAttribute::MakePerspective()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/94
	Inputs:		-
	Purpose:	Make the fill perspectived.
				This should be called just before it is transformed by the moulder,
				so it can validate the 4th control point.

********************************************************************************************/

void BitmapTranspFillAttribute::MakePerspective()
{
	IsPersp = TRUE;

	INT32 dx1 = EndPoint.x - StartPoint.x;
	INT32 dx2 = EndPoint2.x - StartPoint.x;

	INT32 dy1 = EndPoint.y - StartPoint.y;
	INT32 dy2 = EndPoint2.y - StartPoint.y;

	DocCoord Pos3(StartPoint.x + dx1 + dx2, StartPoint.y + dy1 + dy2);
	SetEndPoint3(&Pos3);
}

/********************************************************************************************

>	void BitmapTranspFillAttribute::RemovePerspective()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/94
	Inputs:		-
	Purpose:	Removes perspective from this fill.

********************************************************************************************/

void BitmapTranspFillAttribute::RemovePerspective()
{
	IsPersp = FALSE;
}

/********************************************************************************************

>	void BitmapTranspFillAttribute::SetStartTransp(UINT32* NewTransp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the Start transp of this fill

********************************************************************************************/

void BitmapTranspFillAttribute::SetStartTransp(UINT32* NewTransp)
{
	if (NewTransp == NULL)
		Transp = 0;
	else
		Transp = *NewTransp;
}

/********************************************************************************************

>	void BitmapTranspFillAttribute::SetEndTransp(UINT32* NewTransp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Sets the Start transp of this fill

********************************************************************************************/

void BitmapTranspFillAttribute::SetEndTransp(UINT32* NewTransp)
{
	if (NewTransp == NULL)
		EndTransp = 255;
	else
		EndTransp = *NewTransp;
}

/********************************************************************************************

>	UINT32* BitmapTranspFillAttribute::GetStartTransp()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Gets the Start transp of this fill

********************************************************************************************/

UINT32* BitmapTranspFillAttribute::GetStartTransp()
{
	return &Transp;
}

/********************************************************************************************

>	UINT32* BitmapTranspFillAttribute::GetEndTransp()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Purpose:	Gets the Start transp of this fill

********************************************************************************************/

UINT32* BitmapTranspFillAttribute::GetEndTransp()
{
	return &EndTransp;
}

/********************************************************************************************

>	FillGeometryAttribute& BitmapTranspFillAttribute::operator=(FillGeometryAttribute& FillAttrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		FillAttrib - the attribute to copy, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	Make the Attribute the same as the other. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	AttrFillGeometry::operator==; NodeAttribute::operator==

********************************************************************************************/

FillGeometryAttribute& BitmapTranspFillAttribute::operator=(FillGeometryAttribute& FillAttrib)
{
//	Bitmap = FillAttrib.Bitmap;

	return TranspFillAttribute::operator=(FillAttrib);
}

/********************************************************************************************

>	INT32 BitmapTranspFillAttribute::operator==(const FillGeometryAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 BitmapTranspFillAttribute::operator==(const FillGeometryAttribute& Attrib)
{
	if (GetRuntimeClass() != Attrib.GetRuntimeClass())
		return FALSE;

	BitmapTranspFillAttribute* pAttrib = (BitmapTranspFillAttribute*)&Attrib;

	if (GetBitmapRef()->GetBitmap() != pAttrib->GetBitmapRef()->GetBitmap())
		return FALSE;

	if (IsPerspective())
	{ 
		if (!pAttrib->IsPerspective())
			return FALSE;

		if (*GetEndPoint3() != *pAttrib->GetEndPoint3())
			return FALSE;
	}

	// check the transparency ramp matches
	if (!SameTransparencyRampAs(pAttrib->GetTranspRamp()))
		return FALSE;

	// Are the Colours and Control points all the same ?
	return (
		
		Transp      == pAttrib->Transp      &&
		EndTransp   == pAttrib->EndTransp   &&

		TranspType	== pAttrib->TranspType	&&
	
		StartPoint 	== pAttrib->StartPoint 	&&
		EndPoint  	== pAttrib->EndPoint 	&&
		EndPoint2   == pAttrib->EndPoint2

	);
}

/********************************************************************************************

>	void BitmapTranspFillAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	GradFillAttribute; RenderStack; AttributeValue; NodeAttribute;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BitmapTranspFillAttribute::SimpleCopy(AttributeValue *pValue)
{
	// Just use the assignment operator
	*this = *(FillGeometryAttribute*)pValue;
}

BOOL BitmapTranspFillAttribute::SetDPI(UINT32 NewDpi)
{
	if (IsPerspective())
		return FALSE;

	if (NewDpi == 0)
		return FALSE;

	if (GetBitmap() == NULL)
		return FALSE;

	OILBitmap *OilBM = GetBitmap()->ActualBitmap;

	if (OilBM == NULL)
		return FALSE;

	BitmapInfo Info;
	OilBM->GetInfo(&Info);

	INT32 PixWidth  = Info.PixelWidth;
	INT32 PixHeight = Info.PixelHeight;

	DocCoord Start = *GetStartPoint();
	DocCoord End   = *GetEndPoint();
	DocCoord End2  = *GetEndPoint2();

	INT32 Width  = INT32(Start.Distance(End));
	INT32 Height = INT32(Start.Distance(End2));

	INT32 HDpi = (Width  == 0) ? 0 : (PixWidth*72000)/Width;
	INT32 VDpi = (Height == 0) ? 0 : (PixHeight*72000)/Height;

	INT32 OldDpi = HDpi;
	if (VDpi < OldDpi)
		OldDpi = VDpi;

	TRACEUSER( "Mike", _T("Bitmap Dpi is currently %d\n"),OldDpi);
	TRACEUSER( "Mike", _T("Setting Bitmap Dpi to %d\n"),NewDpi);

	FIXED16 Ratio 	= FIXED16(double(OldDpi)/double(NewDpi));
	Matrix Scale  	= Matrix(Ratio, Ratio);

	GetBitmapVirtualPoints(Start, End, End2,
							&Start, &End, &End2);

	DocCoord Centre = Start;

	Start.translate(-Centre.x, -Centre.y);	
	Scale.transform(&Start);
	Start.translate(Centre.x, Centre.y);	

	End.translate(-Centre.x, -Centre.y);	
	Scale.transform(&End);
	End.translate(Centre.x, Centre.y);	

	End2.translate(-Centre.x, -Centre.y);	
	Scale.transform(&End2);
	End2.translate(Centre.x, Centre.y);	

	GetBitmapRealPoints(Start, End, End2,
						&Start, &End, &End2);

	INT32 NewWidth  = INT32(Start.Distance(End));
	INT32 NewHeight = INT32(Start.Distance(End2));

	if (NewWidth == 0 || NewHeight == 0)
		return FALSE;

	SetStartPoint(&Start);
	SetEndPoint(&End);
	SetEndPoint2(&End2);

	SetFractalDPI(GetDPI());

	return TRUE;
}

/********************************************************************************************

>	UINT32 BitmapTranspFillAttribute::GetDPI()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Returns:	The current DPI of the fractal.
	Purpose:	Gets the DPI of a Fractal bitmap.
	SeeAlso:	FractalFillAttribute::SetDPI

********************************************************************************************/

UINT32 BitmapTranspFillAttribute::GetDPI()
{
	UINT32 Dpi = 0;

	KernelBitmap* KerBM = GetBitmap();
	if (KerBM == NULL)
		return 0;

	OILBitmap *OilBM = KerBM->ActualBitmap;

	if (OilBM != NULL)
	{
		BitmapInfo Info;
		OilBM->GetInfo(&Info);

		INT32 PixWidth  = Info.PixelWidth;
		INT32 PixHeight = Info.PixelHeight;

		DocCoord Start = *GetStartPoint();
		DocCoord End   = *GetEndPoint();
		DocCoord End2  = *GetEndPoint2();

		INT32 Width  = INT32(Start.Distance(End));
		INT32 Height = INT32(Start.Distance(End2));

		UINT32 HDpi = 0;
		UINT32 VDpi = 0;

		if (Width > 0)
			HDpi = (PixWidth*72000)/Width;

		if (Height > 0)
			VDpi = (PixHeight*72000)/Height;

		Dpi = HDpi;
		if (VDpi < Dpi)
			Dpi = VDpi;
	}

	return Dpi;
}

void BitmapTranspFillAttribute::SetTesselation(INT32 NewTess)
{
	if (NewTess == RT_NoRepeatType)
		NewTess = RT_Repeating;

	Tesselation = NewTess;
}


/********************************************************************************************

>	virtual ColourFillAttribute *BitmapTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/02/97
	Inputs:		TransparencyScale	- Value between 0.0 and 1.0 which denotes by how much
									  the function will scale the transparency

	Returns:	Pointer to the new attribute, or NULL if out of memory, problems, etc.

	Purpose:	Creates a non-transparent version of this transparent fill attribute.
				(The original use of this was so airbrushes could maintain their fill's
				transparency geometry)

	SeeAlso:	TranspFillAttribute::MakeSimilarNonTranspFillGeometry; AttrFlatFill
				FlatTranspFillAttribute::MakeSimilarNonTranspFillGeometry

********************************************************************************************/

ColourFillAttribute *BitmapTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)
{
	UINT32 *pStartTransp = GetStartTransp();
	UINT32 *pEndTransp = GetEndTransp();

	if(pStartTransp == NULL || pEndTransp == NULL)
		return NULL;
	
	BitmapFillAttribute *pNewAttr = new BitmapFillAttribute;	
	if (pNewAttr != NULL)
	{
		pNewAttr->SetStartPoint(GetStartPoint());
		pNewAttr->SetEndPoint(GetEndPoint());
		pNewAttr->SetEndPoint2(GetEndPoint2());
		pNewAttr->SetEndPoint3(GetEndPoint3());

		pNewAttr->SetTesselation(GetTesselation());
		pNewAttr->SetDPI(GetDPI());
		if(IsPerspective())
			pNewAttr->MakePerspective();

		pNewAttr->AttachBitmap(GetBitmap());

		INT32 StartTransparency = 255 - (INT32)(((double)(255 - *pStartTransp)) * TransparencyScale);
		INT32 EndTransparency = 255 - (INT32)(((double)(255 - *pEndTransp)) * TransparencyScale);

		DocColour		colorStart(StartTransparency, StartTransparency, StartTransparency);
		DocColour		colorEnd1(EndTransparency, EndTransparency, EndTransparency);
		pNewAttr->SetStartColour(&colorStart);
		pNewAttr->SetEndColour(&colorEnd1);
	}

	return(pNewAttr);
}




/////////////////////////////////////////////////////////////////////////////////////////////
//
//							NoiseTranspFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	NoiseTranspFillAttribute::NoiseTranspFillAttribute()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/97
	Inputs:		-
	Purpose:	Implementation of NoiseTranspFillAttribute

********************************************************************************************/

NoiseTranspFillAttribute::NoiseTranspFillAttribute()
{
	MonotonicTime time;
	seed = time.Sample();

	dpi			 = AttrFillGeometry::FractalDPI;
	dim			 = 0;
	tileable	 = TRUE;

	grain        = FIXED16(30);
}


/********************************************************************************************

>	NoiseTranspFillAttribute::~NoiseTranspFillAttribute()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Purpose:	Destructor for NoiseTranspFillAttribute attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

NoiseTranspFillAttribute::~NoiseTranspFillAttribute()
{
	DetachBitmap();
}


/********************************************************************************************

>	NodeAttribute *NoiseTranspFillAttribute::MakeNode()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrNoiseTranspFill node from this flat fill colour attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode; AttrFlatColourFill

********************************************************************************************/

NodeAttribute* NoiseTranspFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrNoiseTranspFill *pAttr = new AttrNoiseTranspFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	FillGeometryAttribute& FractalTranspFillAttribute::operator=(FillGeometryAttribute& FillAttrib)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Inputs:		FillAttrib - the attribute to copy, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	Make this Attribute the same as FillAttrib. 
	Errors:		No copy is made if the attributes are different runtime classes
	SeeAlso:	TranspFillAttribute::operator=;

********************************************************************************************/

FillGeometryAttribute& NoiseTranspFillAttribute::operator=(FillGeometryAttribute& FillAttrib)
{
	if (FillAttrib.IsKindOf(CC_RUNTIME_CLASS(NoiseTranspFillAttribute)))
	{
		dpi			= ((NoiseTranspFillAttribute*)&FillAttrib)->dpi;
		dim		    = ((NoiseTranspFillAttribute*)&FillAttrib)->dim;
		tileable	= ((NoiseTranspFillAttribute*)&FillAttrib)->tileable;
		seed		= ((NoiseTranspFillAttribute*)&FillAttrib)->seed;
		grain		= ((NoiseTranspFillAttribute*)&FillAttrib)->grain;
	}
	
	return TranspFillAttribute::operator=(FillAttrib);
}


/********************************************************************************************

>	INT32 NoiseTranspFillAttribute::operator==(const FillGeometryAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 NoiseTranspFillAttribute::operator==(const FillGeometryAttribute& Attrib)
{
	// is this the same class of object?
	if (GetRuntimeClass() != Attrib.GetRuntimeClass())
		return FALSE;

	NoiseTranspFillAttribute* pAttrib = (NoiseTranspFillAttribute*)&Attrib;

	if (IsPerspective())
	{ 
		if (!pAttrib->IsPerspective())
			return FALSE;

		if (*GetEndPoint3() != *pAttrib->GetEndPoint3())
			return FALSE;
	}

	// check the transparency ramp matches
	if (!SameTransparencyRampAs(pAttrib->GetTranspRamp()))
		return FALSE;

	return 
	(
		*GetStartTransp()	== *pAttrib->GetStartTransp()	&&	
		*GetEndTransp()		== *pAttrib->GetEndTransp() 	&&
	
		*GetStartPoint() 	== *pAttrib->GetStartPoint() 	&&
		*GetEndPoint()  	== *pAttrib->GetEndPoint() 		&&
		*GetEndPoint2()   	== *pAttrib->GetEndPoint2()		&&
		
		GetTranspType()		== pAttrib->GetTranspType()		&&

		dpi					== pAttrib->dpi					&&	
		dim					== pAttrib->dim					&&
		tileable			== pAttrib->tileable			&&
		seed				== pAttrib->seed				&&
		
		grain				== pAttrib->grain
	); 
}

/********************************************************************************************

>	void NoiseTranspFillAttribute::CacheFractalData(FillGeometryAttribute *pCachedFractal)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Inputs:		pCachedFractal = A pointer to the fractal in the cache
	Returns:	-
	Purpose:	Copies any data from this FractalFillAttribute into the cached fractal
				pointed to on entry. This data is then checked via IsSameAsCachedFractal
	SeeAlso:	CachedFractal, FracList.h / .cpp

********************************************************************************************/

void NoiseTranspFillAttribute::CacheFractalData(FillGeometryAttribute *pCachedFractal)
{
	ERROR3IF(pCachedFractal==NULL, "NULL pointer passed to NoiseTranspFillAttribute::CacheFractalData");
	ERROR3IF(!IS_A(pCachedFractal,NoiseTranspFillAttribute), "Not a NoiseTranspFillAttribute during NoiseTranspFillAttribute::CacheFractalData");

	pCachedFractal->SetStartPoint(GetStartPoint());
	pCachedFractal->SetEndPoint(GetEndPoint());
	pCachedFractal->SetEndPoint2(GetEndPoint2());

	pCachedFractal->SetFractalDPI(GetFractalDPI());
	pCachedFractal->SetFractalDim(GetFractalDim());
	pCachedFractal->SetTileable(GetTileable());
	pCachedFractal->SetSeed(GetSeed());

	pCachedFractal->SetGraininess(GetGraininess());
}

/********************************************************************************************

>	BOOL NoiseTranspFillAttribute::IsSameAsCachedFractal(FillGeometryAttribute *pCachedFractal)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Inputs:		pCachedFractal = A pointer to the fractal in the cache
	Returns:	TRUE if this fractal matches the cached fractal
				FALSE if not
	Purpose:	A virtual comparison operator used by the fractal cache to check for a
				matching fractal.

********************************************************************************************/

BOOL NoiseTranspFillAttribute::IsSameAsCachedFractal(FillGeometryAttribute *pCachedFractal)
{
	ERROR3IF(pCachedFractal==NULL, "NULL pointer passed to NoiseTranspFillAttribute::IsSameAsCachedFractal");
	ERROR3IF(!IS_A(pCachedFractal,NoiseTranspFillAttribute), "Not a NoiseTranspFillAttribute during NoiseTranspFillAttribute::IsSameAsCachedFractal");

	BOOL ok =  (dim			 == pCachedFractal->GetFractalDim());
	ok = ok && (tileable	 == pCachedFractal->GetTileable());
	ok = ok && (seed    	 == pCachedFractal->GetSeed());
	ok = ok && (grain		 == pCachedFractal->GetGraininess());
	
	return ok;	
}

/********************************************************************************************

>	BOOL NoiseTranspFillAttribute::AttachBitmap(KernelBitmap* pBitmap)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97
	Purpose:	Attaches a bitmap to this fill. 
	SeeAlso:	NoiseTranspFillAttribute::DetachBitmap()

********************************************************************************************/

BOOL NoiseTranspFillAttribute::AttachBitmap(KernelBitmap* NewBitmap)
{
	DetachBitmap();

	if (NewBitmap == NULL)
		return FALSE;

	OILBitmap* Bmp = NewBitmap->ActualBitmap;
	if (Bmp == NULL || !Bmp->IsTemp())
		return FALSE;

	NewBitmap = new KernelBitmap(Bmp, TRUE);
	BitmapRef.SetBitmap(NewBitmap);
	GetApplication()->GetGlobalFractalList()->AddFractal(this);

	return TRUE;
}

/********************************************************************************************

>	BOOL NoiseTranspFillAttribute::DetachBitmap()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97
	Purpose:	Detaches a bitmap from this fill. 
	SeeAlso:	BitmapFillAttribute::AttachBitmap()

********************************************************************************************/

BOOL NoiseTranspFillAttribute::DetachBitmap()
{
	if (GetBitmap() == NULL)
		return FALSE;

	if (GetApplication()->GetGlobalFractalList()->RemoveFractal(this) && BitmapRef.GetBitmap())
	{
		// The fractal was deleted, so make sure we NULL our pointer
		BitmapRef.GetBitmap()->ActualBitmap = NULL;
	}

	// now make sure the kernelbitmap is dead too
	BitmapRef.DeleteBmp();

	return TRUE;
}

/********************************************************************************************

>	BOOL NoiseTranspFillAttribute::CopyBitmap(KernelBitmap* BmpToCopy)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97
	Purpose:	Used to copy a bitmap from one fill to another. 
	SeeAlso:	NoiseTranspFillAttribute::AttachBitmap()

********************************************************************************************/

BOOL NoiseTranspFillAttribute::CopyBitmap(KernelBitmap* BmpToCopy)
{
	if (BmpToCopy != NULL)	// Is there a bitmap to copy ?
	{
		DetachBitmap();

		return AttachBitmap(BmpToCopy);
	}

	return TRUE;
}


/********************************************************************************************

>	void NoiseTranspFillAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	GradFillAttribute; RenderStack; AttributeValue; NodeAttribute;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void NoiseTranspFillAttribute::SimpleCopy(AttributeValue *pValue)
{
	// Just use the assignment operator
	*this = *(FillGeometryAttribute*)pValue;
}


/********************************************************************************************

>	BOOL NoiseTranspFillAttribute::RecalcFractal()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Purpose:	Recalculates the Fractal.
	SeeAlso:	NoiseTranspFillAttribute::SetDPI

********************************************************************************************/

BOOL NoiseTranspFillAttribute::RecalcFractal()
{
	if (*GetStartPoint() == *GetEndPoint() ||
		*GetStartPoint() == *GetEndPoint2())
	{
		return FALSE;
	}

	KernelBitmap* pBitmap = GenerateNoiseBitmap(grain,seed);
	if (pBitmap == NULL)
	{
		TRACEUSER( "Mike", _T("Noisy fractal Failed !!!\n"));
		return FALSE;	// Error set by GenerateFractalBitmap
	}

	AttachBitmap(pBitmap);
	delete pBitmap;

	return TRUE;
}



void NoiseTranspFillAttribute::SetTesselation(INT32 NewTess)
{
	if (NewTess == RT_NoRepeatType)
		NewTess = RT_Repeating;

	Tesselation = NewTess;
}

void NoiseTranspFillAttribute::SetFractalDim(UINT32 NewDim)
{
	dim = NewDim;
}

BOOL NoiseTranspFillAttribute::SetTileable(BOOL NewTile)
{
	DetachBitmap();		// Ensure the current fractal is removed from cache

	tileable = NewTile;

	return TRUE;
}

BOOL NoiseTranspFillAttribute::SetFractalDPI(UINT32 NewDpi)
{
	dpi = NewDpi;

	return TRUE;
}



BOOL NoiseTranspFillAttribute::SetGraininess(FIXED16 NewGrain)
{
	DetachBitmap();		// Ensure the current fractal is removed from cache

	if (NewGrain<0) NewGrain=0;
	if (NewGrain>100) NewGrain=100;
	grain = NewGrain;

	return TRUE;
}

/********************************************************************************************

>	virtual ColourFillAttribute *NoiseTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/02/97
	Inputs:		TransparencyScale	- Value between 0.0 and 1.0 which denotes by how much
									  the function will scale the transparency

	Returns:	Pointer to the new attribute, or NULL if out of memory, problems, etc.

	Purpose:	Creates a non-transparent version of this transparent fill attribute.
				(The original use of this was so airbrushes could maintain their fill's
				transparency geometry)

	SeeAlso:	TranspFillAttribute::MakeSimilarNonTranspFillGeometry; AttrFlatFill
				FlatTranspFillAttribute::MakeSimilarNonTranspFillGeometry

********************************************************************************************/

ColourFillAttribute *NoiseTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)
{
	UINT32 *pStartTransp = GetStartTransp();
	UINT32 *pEndTransp = GetEndTransp();

	if(pStartTransp == NULL || pEndTransp == NULL)
		return NULL;
	
	NoiseFillAttribute *pNewAttr = new NoiseFillAttribute;	
	if (pNewAttr != NULL)
	{
		pNewAttr->SetStartPoint(GetStartPoint());
		pNewAttr->SetEndPoint(GetEndPoint());
		pNewAttr->SetEndPoint2(GetEndPoint2());
		pNewAttr->SetEndPoint3(GetEndPoint3());

		pNewAttr->SetTesselation(GetTesselation());
		pNewAttr->SetDPI(GetDPI());
		if(IsPerspective())
			pNewAttr->MakePerspective();

		pNewAttr->AttachBitmap(GetBitmap());
		pNewAttr->SetFractalDim(GetFractalDim());
		pNewAttr->SetTileable(GetTileable());
		pNewAttr->SetFractalDPI(GetFractalDPI());
		pNewAttr->SetSeed(GetSeed());
		pNewAttr->SetNoiseScale(GetNoiseScale());

		INT32 StartTransparency = 255 - (INT32)(((double)(255 - *pStartTransp)) * TransparencyScale);
		INT32 EndTransparency = 255 - (INT32)(((double)(255 - *pEndTransp)) * TransparencyScale);

		DocColour		colorStart(StartTransparency, StartTransparency, StartTransparency);
		DocColour		colorEnd1(EndTransparency, EndTransparency, EndTransparency);
		pNewAttr->SetStartColour(&colorStart);
		pNewAttr->SetEndColour(&colorEnd1);

	  	pNewAttr->RecalcFractal();
	}

	return(pNewAttr);
}

/*******************************************************************************************
  Values local to this fractal type
*******************************************************************************************/

BOOL NoiseTranspFillAttribute::SetSeed(INT32 NewSeed)
{
	DetachBitmap();		// Ensure the current fractal is removed from cache

	seed = NewSeed;

	return TRUE;
}






/////////////////////////////////////////////////////////////////////////////////////////////
//
//							FractalTranspFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	FractalTranspFillAttribute::FractalTranspFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

FractalTranspFillAttribute::FractalTranspFillAttribute()
{
	MonotonicTime time;
	Seed = time.Sample();

	Graininess = AttrFillGeometry::FractalGraininess;
	Gravity    = AttrFillGeometry::FractalGravity;
	Dpi        = AttrFillGeometry::FractalDPI;
	
	Squash = 0;
	Dim	   = 0;
	Tileable = FALSE;

	Tesselation = RT_RepeatInverted;
}

/********************************************************************************************

>	FractalTranspFillAttribute::~FractalTranspFillAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Purpose:	Destructor for fractal fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

FractalTranspFillAttribute::~FractalTranspFillAttribute()
{
	DetachBitmap();
}

/********************************************************************************************

>	NodeAttribute *FractalTranspFillAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrFlatColourFill node from this flat fill colour attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode; AttrFlatColourFill

********************************************************************************************/

NodeAttribute *FractalTranspFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrFractalFill *pAttr = new AttrFractalTranspFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	FillGeometryAttribute& FractalTranspFillAttribute::operator=(FillGeometryAttribute& FillAttrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		FillAttrib - the attribute to copy, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	Make the Attribute the same as the other. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	AttrFillGeometry::operator==; NodeAttribute::operator==

********************************************************************************************/

FillGeometryAttribute& FractalTranspFillAttribute::operator=(FillGeometryAttribute& FillAttrib)
{
	if (FillAttrib.IsKindOf(CC_RUNTIME_CLASS(FractalTranspFillAttribute)))
	{
		Seed 		= ((FractalTranspFillAttribute*)&FillAttrib)->Seed;
		Graininess 	= ((FractalTranspFillAttribute*)&FillAttrib)->Graininess;
		Gravity 	= ((FractalTranspFillAttribute*)&FillAttrib)->Gravity;
		Squash 		= ((FractalTranspFillAttribute*)&FillAttrib)->Squash;
		Dpi 		= ((FractalTranspFillAttribute*)&FillAttrib)->Dpi;
		Tileable	= ((FractalTranspFillAttribute*)&FillAttrib)->Tileable;
		Dim			= ((FractalTranspFillAttribute*)&FillAttrib)->Dim;
	}

	return TranspFillAttribute::operator=(FillAttrib);
}

/********************************************************************************************

>	INT32 FractalTranspFillAttribute::operator==(const FillGeometryAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 FractalTranspFillAttribute::operator==(const FillGeometryAttribute& Attrib)
{
	if (GetRuntimeClass() != Attrib.GetRuntimeClass())
		return FALSE;

	FractalTranspFillAttribute* pAttrib = (FractalTranspFillAttribute*)&Attrib;

	if (IsPerspective())
	{ 
		if (!pAttrib->IsPerspective())
			return FALSE;

		if (*GetEndPoint3() != *pAttrib->GetEndPoint3())
			return FALSE;
	}

	// check the transparency ramp matches
	if (!SameTransparencyRampAs(pAttrib->GetTranspRamp()))
		return FALSE;

	// Are the Colours and Control points all the same ?
	return (

		*GetStartTransp()	== *pAttrib->GetStartTransp()	&&	
		*GetEndTransp()		== *pAttrib->GetEndTransp() 	&&
	
		*GetStartPoint() 	== *pAttrib->GetStartPoint() 	&&
		*GetEndPoint()  	== *pAttrib->GetEndPoint() 		&&
		*GetEndPoint2()   	== *pAttrib->GetEndPoint2()		&&

		GetTranspType()		== pAttrib->GetTranspType()		&&

		Seed   		== pAttrib->Seed		&&
		Graininess  == pAttrib->Graininess	&&
		Gravity   	== pAttrib->Gravity		&&
		Squash   	== pAttrib->Squash		&&
		Dpi   		== pAttrib->Dpi			&&
		Tileable   	== pAttrib->Tileable	&&
		Dim 		== pAttrib->Dim
	);
}

/********************************************************************************************

>	void FractalTranspFillAttribute::CacheFractalData(FillGeometryAttribute *pCachedFractal)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Inputs:		pCachedFractal = A pointer to the fractal in the cache
	Returns:	-
	Purpose:	Copies any data from this FractalFillAttribute into the cached fractal
				pointed to on entry. This data is then checked via IsSameAsCachedFractal
	SeeAlso:	CachedFractal, FracList.h / .cpp

********************************************************************************************/

void FractalTranspFillAttribute::CacheFractalData(FillGeometryAttribute *pCachedFractal)
{
	ERROR3IF(pCachedFractal==NULL, "NULL pointer passed to FractalTranspFillAttribute::CacheFractalData");
	ERROR3IF(!IS_A(pCachedFractal,FractalTranspFillAttribute), "Not a FractalTranspFillAttribute during FractalTranspFillAttribute::CacheFractalData");

	pCachedFractal->SetStartPoint(GetStartPoint());
	pCachedFractal->SetEndPoint(GetEndPoint());
	pCachedFractal->SetEndPoint2(GetEndPoint2());

	pCachedFractal->SetFractalDPI(GetFractalDPI());
	pCachedFractal->SetFractalDim(GetFractalDim());
	pCachedFractal->SetTileable(GetTileable());

	pCachedFractal->SetSeed(GetSeed());
	pCachedFractal->SetGraininess(GetGraininess());
	pCachedFractal->SetGravity(GetGravity());
	pCachedFractal->SetSquash(GetSquash());
}


/********************************************************************************************

>	BOOL FractalTranspFillAttribute::IsSameAsCachedFractal(FillGeometryAttribute *pCachedFractal)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Inputs:		pCachedFractal = A pointer to the fractal in the cache
	Returns:	TRUE if this fractal matches the cached fractal
				FALSE if not
	Purpose:	A virtual comparison operator used by the fractal cache to check for a
				matching fractal.

********************************************************************************************/

BOOL FractalTranspFillAttribute::IsSameAsCachedFractal(FillGeometryAttribute *pCachedFractal)
{
	ERROR3IF(pCachedFractal==NULL, "NULL pointer passed to FractalTranspFillAttribute::IsSameAsCachedFractal");
	ERROR3IF(!IS_A(pCachedFractal,FractalTranspFillAttribute), "Not a FractalTranspFillAttribute during FractalTranspFillAttribute::IsSameAsCachedFractal");

	// a long winded check but separate lines help in debugging, i.e. when does ok go FALSE?
	BOOL ok =  ((UINT32)Dim 	== pCachedFractal->GetFractalDim());
//	ok = ok && (Dpi   		== pCachedFractal->GetFractalDpi());		 // not checked in Will's original code
	ok = ok && (Tileable   	== pCachedFractal->GetTileable());

	ok = ok && (Seed   		== pCachedFractal->GetSeed());
	ok = ok && (Graininess  == pCachedFractal->GetGraininess());
	ok = ok && (Gravity   	== pCachedFractal->GetGravity());
	ok = ok && (Squash   	== pCachedFractal->GetSquash());
	
	return ok;
}


/********************************************************************************************

>	BOOL FractalTranspFillAttribute::AttachBitmap(KernelBitmap* pBitmap)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Purpose:	Attaches a bitmap to this fill. 
	SeeAlso:	BitmapFillAttribute::DetachBitmap()

********************************************************************************************/

BOOL FractalTranspFillAttribute::AttachBitmap(KernelBitmap* NewBitmap)
{
	// Sometimes we are asked to attach the same bitmap to the attribute
	// So if we try to detach it and then attach it, during the detach
	// the bitmap can get deleted, so then we will effectivelly attach
	// a deleted bitmap. To avoid this we add this fractal to the list again in
	// order to increment the usage count, then do the normal detach & attach 
	// and finally remove the additional fractal from the list
	BOOL NeedUsageIncrement = FALSE;
	if ((NewBitmap != NULL) && (GetBitmap() != NULL))
	{
		// only if the bitmap we have is the same as the one we are to attach
		if ((GetBitmap()->ActualBitmap != NULL) && 
			(GetBitmap()->ActualBitmap == NewBitmap->ActualBitmap))
			{
				NeedUsageIncrement = TRUE; // set the flag
				GetApplication()->GetGlobalFractalList()->AddFractal(this); // inc the usage count
				TRACEUSER( "Stefan", _T("NeedUsageIncrement = TRUE\n"));
			}
	}

	// get rid of the old bitmap
	DetachBitmap();

	if (NewBitmap == NULL)
		return FALSE;

	OILBitmap* Bmp = NewBitmap->ActualBitmap;
	if (Bmp == NULL || !Bmp->IsTemp())
		return FALSE;

	NewBitmap = new KernelBitmap(Bmp, TRUE);
	BitmapRef.SetBitmap(NewBitmap);
	GetApplication()->GetGlobalFractalList()->AddFractal(this);

	// if we added an additional fractal, remove it
	if (NeedUsageIncrement)
		GetApplication()->GetGlobalFractalList()->RemoveFractal(this);
	
	return TRUE;
}

/********************************************************************************************

>	BOOL FractalTranspFillAttribute::DetachBitmap()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Purpose:	Detaches a bitmap from this fill. 
	SeeAlso:	BitmapFillAttribute::AttachBitmap()

********************************************************************************************/

BOOL FractalTranspFillAttribute::DetachBitmap()
{
	if (GetBitmap() == NULL)
		return FALSE;

	if (GetApplication()->GetGlobalFractalList()->RemoveFractal(this) && BitmapRef.GetBitmap())
	{
		// The fractal was deleted, so make sure we NULL our pointer
		BitmapRef.GetBitmap()->ActualBitmap = NULL;
	}

	BitmapRef.DeleteBmp();

	return TRUE;
}

/********************************************************************************************

>	BOOL FractalTranspFillAttribute::CopyBitmap(KernelBitmap* BmpToCopy)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Purpose:	Used to copy a bitmap from one fill to another. 
	SeeAlso:	BitmapFillAttribute::AttachBitmap()

********************************************************************************************/

BOOL FractalTranspFillAttribute::CopyBitmap(KernelBitmap* BmpToCopy)
{
	if (BmpToCopy != NULL)	// Is there a bitmap to copy ?
	{
		DetachBitmap();

		return AttachBitmap(BmpToCopy);
	}

	return TRUE;
}

/********************************************************************************************

>	void FractalTranspFillAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	GradFillAttribute; RenderStack; AttributeValue; NodeAttribute;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void FractalTranspFillAttribute::SimpleCopy(AttributeValue *pValue)
{
	// Just use the assignment operator
	*this = *(FillGeometryAttribute*)pValue;
}


/********************************************************************************************

>	BOOL FractalTranspFillAttribute::RecalcFractal()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Purpose:	Recalculates the Fractal.
	SeeAlso:	FractalTranspFillAttribute::SetDPI

********************************************************************************************/

BOOL FractalTranspFillAttribute::RecalcFractal()
{
	if (*GetStartPoint() == *GetEndPoint() ||
		*GetStartPoint() == *GetEndPoint2())
	{
		return FALSE;
	}

	UINT32 OldDim = GetFractalDim();

	KernelBitmap* pBitmap = GenerateFractalBitmap(	Seed, 
													Graininess.MakeDouble(),
													Gravity.MakeDouble(),
													Squash.MakeDouble(), 
													Dpi);
	if (pBitmap == NULL)
	{
		TRACEUSER( "Mike", _T("Fractal Failed !!!\n"));
		return FALSE;	// Error set by GenerateFractalBitmap
	}

	// When asking GenerateFractalBitmap above to give us a bitmap, it might modify the Dim value.
	// DetachBitmap then tries to match this fractal with the cached fractals, so it could find a 
	// wrong cached fractal (one with the same parameters as this, but with Dim value as set by
	// GenerateFractalBitmap, rather then the original value). In this case DetachBitmap acts on
	// the wrong fractal and could even cause destroying its bitmap, even though it is used by 
	// some other attribute. To fix this we temporarily set Dim to the old value, call DetachBitmap
	// and then set the new value back.
	UINT32 NewDim = GetFractalDim();
	if (NewDim != OldDim)
	{
		SetFractalDim(OldDim);
		DetachBitmap();

		SetFractalDim(NewDim);
	}

	AttachBitmap(pBitmap);
	delete pBitmap;

	return TRUE;
}



void FractalTranspFillAttribute::SetTesselation(INT32 NewTess)
{
	if (NewTess == RT_NoRepeatType)
		NewTess = RT_RepeatInverted;

	Tesselation = NewTess;
}

void FractalTranspFillAttribute::SetFractalDim(UINT32 NewDim) 
{
	Dim = NewDim; 
}

BOOL FractalTranspFillAttribute::SetTileable(BOOL NewTile)
{
	DetachBitmap();		// Ensure the current fractal is removed from cache

	Tileable = NewTile;

	return TRUE;
}

BOOL FractalTranspFillAttribute::SetFractalDPI(UINT32 NewDpi)
{
	Dpi = NewDpi;

	return TRUE;
}



BOOL FractalTranspFillAttribute::SetSeed(INT32 NewSeed)
{
	DetachBitmap();		// Ensure the current fractal is removed from cache

	Seed = NewSeed;

	return TRUE;
}

BOOL FractalTranspFillAttribute::SetGraininess(FIXED16 NewGrain)
{
	DetachBitmap();		// Ensure the current fractal is removed from cache

	Graininess = NewGrain;

	return TRUE;
}

BOOL FractalTranspFillAttribute::SetGravity(FIXED16 NewGrav)
{
	DetachBitmap();		// Ensure the current fractal is removed from cache

	Gravity = NewGrav;

	return TRUE;
}

BOOL FractalTranspFillAttribute::SetSquash(FIXED16 NewSquash)
{
	DetachBitmap();		// Ensure the current fractal is removed from cache

	Squash = NewSquash;

	return TRUE;
}

BOOL FractalTranspFillAttribute::Randomise()
{
	DetachBitmap();		// Ensure the current fractal is removed from cache

	MonotonicTime time;
	Seed = time.Sample();

	return TRUE;
}

/********************************************************************************************

>	virtual ColourFillAttribute *FractalTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/02/97
	Inputs:		TransparencyScale	- Value between 0.0 and 1.0 which denotes by how much
									  the function will scale the transparency

	Returns:	Pointer to the new attribute, or NULL if out of memory, problems, etc.

	Purpose:	Creates a non-transparent version of this transparent fill attribute.
				(The original use of this was so airbrushes could maintain their fill's
				transparency geometry)

	SeeAlso:	TranspFillAttribute::MakeSimilarNonTranspFillGeometry; AttrFlatFill
				FlatTranspFillAttribute::MakeSimilarNonTranspFillGeometry

********************************************************************************************/

ColourFillAttribute *FractalTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)
{
	UINT32 *pStartTransp = GetStartTransp();
	UINT32 *pEndTransp = GetEndTransp();

	if(pStartTransp == NULL || pEndTransp == NULL)
		return NULL;
	
	FractalFillAttribute *pNewAttr = new FractalFillAttribute;
	if (pNewAttr != NULL)
	{
		pNewAttr->SetStartPoint(GetStartPoint());
		pNewAttr->SetEndPoint(GetEndPoint());
		pNewAttr->SetEndPoint2(GetEndPoint2());
		pNewAttr->SetEndPoint3(GetEndPoint3());

		pNewAttr->SetTesselation(GetTesselation());
		pNewAttr->SetDPI(GetDPI());
		if(IsPerspective())
			pNewAttr->MakePerspective();

		pNewAttr->AttachBitmap(GetBitmap());
		pNewAttr->SetFractalDim(GetFractalDim());
		pNewAttr->SetTileable(GetTileable());
		pNewAttr->SetFractalDPI(GetFractalDPI());
		pNewAttr->SetSeed(GetSeed());
		pNewAttr->SetNoiseScale(GetNoiseScale());

		pNewAttr->SetGraininess(GetGraininess());
		pNewAttr->SetGravity(GetGravity());
		pNewAttr->SetSquash(GetSquash());

		INT32 StartTransparency = 255 - (INT32)(((double)(255 - *pStartTransp)) * TransparencyScale);
		INT32 EndTransparency = 255 - (INT32)(((double)(255 - *pEndTransp)) * TransparencyScale);

		DocColour		colorStart(StartTransparency, StartTransparency, StartTransparency);
		DocColour		colorEnd1(EndTransparency, EndTransparency, EndTransparency);
		pNewAttr->SetStartColour(&colorStart);
		pNewAttr->SetEndColour(&colorEnd1);

	  	pNewAttr->RecalcFractal();
	}

	return(pNewAttr);
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//							SquareTranspFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	SquareTranspFillAttribute::SquareTranspFillAttribute()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

SquareTranspFillAttribute::SquareTranspFillAttribute()
{
	IsPersp = FALSE;

	EndPoint2 = DocCoord(0,0);
}

/********************************************************************************************

>	NodeAttribute *SquareTranspFillAttribute::MakeNode()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrSquareFill node from this graduated fill attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *SquareTranspFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrSquareTranspFill *pAttr = new AttrSquareTranspFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	void SquareTranspFillAttribute::SetEndPoint2(DocCoord* Pos)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void SquareTranspFillAttribute::SetEndPoint2(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint2 = MakeLineAtAngle(*GetStartPoint(), *GetEndPoint(), 90);
	else
		EndPoint2 = *Pos;
}


/********************************************************************************************

>	void SquareTranspFillAttribute::SetEndPoint3(DocCoord* Pos)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void SquareTranspFillAttribute::SetEndPoint3(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint3 = MakeLineAtAngle(*GetEndPoint(), *GetEndPoint2(), 90);
	else
		EndPoint3 = *Pos;
}


/********************************************************************************************

>	void SquareTranspFillAttribute::MakePerspective()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Purpose:	Make the fill perspectived.
				This should be called just before it is transformed by the moulder,
				so it can validate the 4th control point.

********************************************************************************************/

void SquareTranspFillAttribute::MakePerspective()
{
	IsPersp = TRUE;

	INT32 dx1 = EndPoint.x - StartPoint.x;
	INT32 dx2 = EndPoint2.x - StartPoint.x;

	INT32 dy1 = EndPoint.y - StartPoint.y;
	INT32 dy2 = EndPoint2.y - StartPoint.y;

	DocCoord Pos3(StartPoint.x + dx1 + dx2, StartPoint.y + dy1 + dy2);
	SetEndPoint3(&Pos3);
}

/********************************************************************************************

>	void SquareTranspFillAttribute::RemovePerspective()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Purpose:	Removes perspective from this fill.

********************************************************************************************/

void SquareTranspFillAttribute::RemovePerspective()
{
	IsPersp = FALSE;
}

/********************************************************************************************

>	virtual ColourFillAttribute *SquareTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/02/97
	Inputs:		TransparencyScale	- Value between 0.0 and 1.0 which denotes by how much
									  the function will scale the transparency

	Returns:	Pointer to the new attribute, or NULL if out of memory, problems, etc.

	Purpose:	Creates a non-transparent version of this transparent fill attribute.
				(The original use of this was so airbrushes could maintain their fill's
				transparency geometry)

	SeeAlso:	TranspFillAttribute::MakeSimilarNonTranspFillGeometry; AttrFlatFill
				FlatTranspFillAttribute::MakeSimilarNonTranspFillGeometry

********************************************************************************************/

ColourFillAttribute *SquareTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)
{
	UINT32 *pStartTransp = GetStartTransp();
	UINT32 *pEndTransp = GetEndTransp();

	if(pStartTransp == NULL || pEndTransp == NULL)
		return NULL;
	
	SquareFillAttribute *pNewAttr = new SquareFillAttribute;	
	if (pNewAttr != NULL)
	{
		pNewAttr->SetStartPoint(GetStartPoint());
		pNewAttr->SetEndPoint(GetEndPoint());
		pNewAttr->SetEndPoint2(GetEndPoint2());
		pNewAttr->SetEndPoint3(GetEndPoint3());

		if(IsPerspective())
			pNewAttr->MakePerspective();

		pNewAttr->AttachBitmap(GetBitmap());

		INT32 StartTransparency = 255 - (INT32)(((double)(255 - *pStartTransp)) * TransparencyScale);
		INT32 EndTransparency = 255 - (INT32)(((double)(255 - *pEndTransp)) * TransparencyScale);

		DocColour		colorStart(StartTransparency, StartTransparency, StartTransparency);
		DocColour		colorEnd1(EndTransparency, EndTransparency, EndTransparency);
		pNewAttr->SetStartColour(&colorStart);
		pNewAttr->SetEndColour(&colorEnd1);
	}

	return(pNewAttr);
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//							ThreeColTranspFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	ThreeColTranspFillAttribute::ThreeColTranspFillAttribute()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

ThreeColTranspFillAttribute::ThreeColTranspFillAttribute()
{
	IsPersp = FALSE;

	EndPoint2 = DocCoord(0,0);
	EndTransp2 = 128;
}

/********************************************************************************************

>	INT32 ThreeColTranspFillAttribute::operator==(const FillGeometryAttribute& Attrib)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 ThreeColTranspFillAttribute::operator==(const FillGeometryAttribute& Attrib)
{
	ThreeColTranspFillAttribute* pAttrib = (ThreeColTranspFillAttribute*) &Attrib;
	
	return(GradTranspFillAttribute::operator==(Attrib) && (EndTransp2 == pAttrib->EndTransp2));
}


/********************************************************************************************

>	NodeAttribute *ThreeColTranspFillAttribute::MakeNode()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrThreeColFill node from this graduated fill attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *ThreeColTranspFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrThreeColTranspFill *pAttr = new AttrThreeColTranspFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	void ThreeColTranspFillAttribute::SetEndPoint2(DocCoord* Pos)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void ThreeColTranspFillAttribute::SetEndPoint2(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint2 = MakeLineAtAngle(*GetStartPoint(), *GetEndPoint(), 90);
	else
		EndPoint2 = *Pos;
}


/********************************************************************************************

>	void ThreeColTranspFillAttribute::SetEndPoint3(DocCoord* Pos)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Purpose:	Sets the End Point of this fill

********************************************************************************************/

void ThreeColTranspFillAttribute::SetEndPoint3(DocCoord* Pos)
{
	if (Pos == NULL)
		EndPoint3 = MakeLineAtAngle(*GetEndPoint(), *GetEndPoint2(), 90);
	else
		EndPoint3 = *Pos;
}


/********************************************************************************************

>	void ThreeColTranspFillAttribute::SetEndTransp2(DocColour* NewCol)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/96
	Inputs:		-
	Purpose:	Sets the second colour of this fill

********************************************************************************************/

void ThreeColTranspFillAttribute::SetEndTransp2(UINT32* NewTransp)
{
	if (NewTransp == NULL)
		EndTransp2 = 128;
	else
		EndTransp2 = *NewTransp;
}




/********************************************************************************************

>	void ThreeColTranspFillAttribute::MakePerspective()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Purpose:	Make the fill perspectived.
				This should be called just before it is transformed by the moulder,
				so it can validate the 4th control point.

********************************************************************************************/

void ThreeColTranspFillAttribute::MakePerspective()
{
	IsPersp = TRUE;

	INT32 dx1 = EndPoint.x - StartPoint.x;
	INT32 dx2 = EndPoint2.x - StartPoint.x;

	INT32 dy1 = EndPoint.y - StartPoint.y;
	INT32 dy2 = EndPoint2.y - StartPoint.y;

	DocCoord Pos3(StartPoint.x + dx1 + dx2, StartPoint.y + dy1 + dy2);
	SetEndPoint3(&Pos3);
}

/********************************************************************************************

>	void ThreeColTranspFillAttribute::RemovePerspective()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Purpose:	Removes perspective from this fill.

********************************************************************************************/

void ThreeColTranspFillAttribute::RemovePerspective()
{
	IsPersp = FALSE;
}


/********************************************************************************************

>	virtual ColourFillAttribute *ThreeColTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/02/97
	Inputs:		TransparencyScale	- Value between 0.0 and 1.0 which denotes by how much
									  the function will scale the transparency

	Returns:	Pointer to the new attribute, or NULL if out of memory, problems, etc.

	Purpose:	Creates a non-transparent version of this transparent fill attribute.
				(The original use of this was so airbrushes could maintain their fill's
				transparency geometry)

	SeeAlso:	TranspFillAttribute::MakeSimilarNonTranspFillGeometry; AttrFlatFill
				FlatTranspFillAttribute::MakeSimilarNonTranspFillGeometry

********************************************************************************************/

ColourFillAttribute *ThreeColTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)
{
	UINT32 *pStartTransp = GetStartTransp();
	UINT32 *pEndTransp = GetEndTransp();
	UINT32 *pEndTransp2 = GetEndTransp2();

	if(pStartTransp == NULL || pEndTransp == NULL || pEndTransp2 == NULL)
		return NULL;
	
	ThreeColFillAttribute *pNewAttr = new ThreeColFillAttribute;	
	if (pNewAttr != NULL)
	{
		pNewAttr->SetStartPoint(GetStartPoint());
		pNewAttr->SetEndPoint(GetEndPoint());
		pNewAttr->SetEndPoint2(GetEndPoint2());
		pNewAttr->SetEndPoint3(GetEndPoint3());

		if(IsPerspective())
			pNewAttr->MakePerspective();

		INT32 StartTransparency = 255 - (INT32)(((double)(255 - *pStartTransp)) * TransparencyScale);
		INT32 EndTransparency = 255 - (INT32)(((double)(255 - *pEndTransp)) * TransparencyScale);
		INT32 EndTransparency2 = 255 - (INT32)(((double)(255 - *pEndTransp2)) * TransparencyScale);

		DocColour		colorStart(StartTransparency, StartTransparency, StartTransparency);
		DocColour		colorEnd1(EndTransparency, EndTransparency, EndTransparency);
		DocColour		colorEnd2(EndTransparency2, EndTransparency2, EndTransparency2);
		pNewAttr->SetStartColour(&colorStart);
		pNewAttr->SetEndColour(&colorEnd1);
		pNewAttr->SetEndColour2(&colorEnd2);
	}

	return(pNewAttr);
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//							FourColTranspFillAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	FourColTranspFillAttribute::FourColTranspFillAttribute()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	Default Constuctor for fill attribute values. 
	SeeAlso:	AttrFillGeometry::AttrFillGeometry

********************************************************************************************/

FourColTranspFillAttribute::FourColTranspFillAttribute()
{
	EndTransp3 = 192;
}


/********************************************************************************************

>	INT32 FourColTranspFillAttribute::operator==(const FillGeometryAttribute& Attrib)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 FourColTranspFillAttribute::operator==(const FillGeometryAttribute& Attrib)
{
	FourColTranspFillAttribute* pAttrib = (FourColTranspFillAttribute*) &Attrib;
	
	return(ThreeColTranspFillAttribute::operator==(Attrib) && (EndTransp3 == pAttrib->EndTransp3));
}


/********************************************************************************************

>	NodeAttribute *FourColTranspFillAttribute::MakeNode()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrFourColTranspFill node from this graduated fill attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *FourColTranspFillAttribute::MakeNode()
{
	// Create new attribute node
	AttrFourColTranspFill *pAttr = new AttrFourColTranspFill;
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->GetAttributeValue()->SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	void FourColTranspFillAttribute::SetEndTransp3(DocColour* NewCol)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/96
	Inputs:		-
	Purpose:	Sets the third transparency of this fill

********************************************************************************************/

void FourColTranspFillAttribute::SetEndTransp3(UINT32* NewTransp)
{
	if (NewTransp == NULL)
		EndTransp3 = 192;
	else
		EndTransp3 = *NewTransp;
}

/********************************************************************************************

>	virtual ColourFillAttribute *FourColTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/02/97
	Inputs:		TransparencyScale	- Value between 0.0 and 1.0 which denotes by how much
									  the function will scale the transparency

	Returns:	Pointer to the new attribute, or NULL if out of memory, problems, etc.

	Purpose:	Creates a non-transparent version of this transparent fill attribute.
				(The original use of this was so airbrushes could maintain their fill's
				transparency geometry)

	SeeAlso:	TranspFillAttribute::MakeSimilarNonTranspFillGeometry; AttrFlatFill
				FlatTranspFillAttribute::MakeSimilarNonTranspFillGeometry

********************************************************************************************/

ColourFillAttribute *FourColTranspFillAttribute::MakeSimilarNonTranspFillGeometry(double TransparencyScale)
{
	UINT32 *pStartTransp = GetStartTransp();
	UINT32 *pEndTransp = GetEndTransp();
	UINT32 *pEndTransp2 = GetEndTransp2();
	UINT32 *pEndTransp3 = GetEndTransp3();

	if(pStartTransp == NULL || pEndTransp == NULL || pEndTransp2 == NULL || pEndTransp3 == NULL)
		return NULL;
	
	FourColFillAttribute *pNewAttr = new FourColFillAttribute;	
	if (pNewAttr != NULL)
	{
		pNewAttr->SetStartPoint(GetStartPoint());
		pNewAttr->SetEndPoint(GetEndPoint());
		pNewAttr->SetEndPoint2(GetEndPoint2());
		pNewAttr->SetEndPoint3(GetEndPoint3());

		if(IsPerspective())
			pNewAttr->MakePerspective();

		INT32 StartTransparency = 255 - (INT32)(((double)(255 - *pStartTransp)) * TransparencyScale);
		INT32 EndTransparency  = 255 - (INT32)(((double)(255 - *pEndTransp)) * TransparencyScale);
		INT32 EndTransparency2 = 255 - (INT32)(((double)(255 - *pEndTransp2)) * TransparencyScale);
		INT32 EndTransparency3 = 255 - (INT32)(((double)(255 - *pEndTransp3)) * TransparencyScale);

		DocColour		colorStart(StartTransparency, StartTransparency, StartTransparency);
		DocColour		colorEnd1(EndTransparency, EndTransparency, EndTransparency);
		DocColour		colorEnd2(EndTransparency2, EndTransparency2, EndTransparency2);
		DocColour		colorEnd3(EndTransparency3, EndTransparency3, EndTransparency3);
		pNewAttr->SetStartColour(&colorStart);
		pNewAttr->SetEndColour(&colorEnd1);
		pNewAttr->SetEndColour2(&colorEnd2);
		pNewAttr->SetEndColour3(&colorEnd3);
	}

	return(pNewAttr);
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//							FillMappingAttribute class
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	FillMappingAttribute::FillMappingAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Purpose:	Constructor for mapping attributes.
	SeeAlso:	FillMappingAttribute; RenderStack; AttributeValue; NodeAttribute;
				FillMappingAttribute::Restore;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

FillMappingAttribute::FillMappingAttribute()
{
	Repeat = 2;
}

/********************************************************************************************

>	BOOL FillMappingAttribute::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers fill mapping attribute, and provides a default attribute to give
				linear graduations.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL FillMappingAttribute::Init()
{
	// Default to simple linear mapping (ignored for flat fills, obviously)
	FillMappingAttribute *pAttr = new FillMappingLinearAttribute;
	if (pAttr==NULL)
		// error message has already been set by new
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising FillMappingAttribute");
	ERROR2IF(ID != ATTR_FILLMAPPING, FALSE, "Incorrect ID for FillMappingAttribute");

	return TRUE;
}

/********************************************************************************************

>	void FillMappingAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void FillMappingAttribute::SimpleCopy(AttributeValue *pValue)
{
	Repeat = ((FillMappingAttribute *) pValue)->Repeat;
}

/********************************************************************************************

>	void FillMappingAttribute::Render(RenderRegion *pRegion)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the fill type attribute for the given render region. i.e. all
				paths filled will now be filled with this fill attribute.
	SeeAlso:	FillMappingAttribute; RenderStack; AttributeValue; NodeAttribute;
				FillMappingAttribute::Restore;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void FillMappingAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "FillMappingAttribute::Render - do nothing" )
//	TRACE( _T("Warning - FillMappingAttribute::Render called\n") );
	pRegion->SetFillMapping(this, Temp);
}

/********************************************************************************************

>	void FillMappingAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the fill type attribute for the given render region. i.e. all
				paths filled will now be filled with this fill attribute.
	SeeAlso:	FillMappingAttribute; RenderStack; AttributeValue; NodeAttribute;
				FillMappingAttribute::Render;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void FillMappingAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "FillMappingAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - FillMappingAttribute::Restore called\n") );
	pRegion->RestoreFillMapping(this, Temp);
}

BOOL FillMappingAttribute::IsDifferent(AttributeValue* pVal)
{
	if (pVal->GetRuntimeClass() != GetRuntimeClass())
		return TRUE;

	return (Repeat != ((FillMappingAttribute*)pVal)->Repeat);
}

FillMappingAttribute& FillMappingAttribute::operator=(FillMappingAttribute& Attrib)
{
	Repeat = Attrib.Repeat;

	return *this;
}

INT32 FillMappingAttribute::operator==(const FillMappingAttribute& Attrib)
{
	if (Attrib.GetRuntimeClass() != GetRuntimeClass())
		return FALSE;

	return (Repeat == Attrib.Repeat);
}

/********************************************************************************************

>	NodeAttribute *FillMappingLinearAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrFillMappingLinear node from this fill mapping attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *FillMappingLinearAttribute::MakeNode()
{
	// Create new attribute node
	AttrFillMappingLinear *pAttr = new AttrFillMappingLinear();
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}


/********************************************************************************************

>	NodeAttribute *FillMappingSinAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrFillMappingSin node from this fill mapping attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *FillMappingSinAttribute::MakeNode()
{
	// Create new attribute node
	AttrFillMappingSin *pAttr = new AttrFillMappingSin();
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}


/********************************************************************************************

>	BOOL FillEffectAttribute::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers fill effect attribute, and provides a default attribute to give
				standard RGB fade between two colours.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL FillEffectAttribute::Init()
{
	// Default to simple RGB fade effect (ignored for flat fills, obviously)
	FillEffectAttribute *pAttr = new FillEffectFadeAttribute;
	if (pAttr==NULL)
		// error message has already been set by new
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising FillEffectAttribute");
	ERROR2IF(ID != ATTR_FILLEFFECT, FALSE, "Incorrect ID for FillEffectAttribute");

	return TRUE;
}

/********************************************************************************************

>	void FillEffectAttribute::Render(RenderRegion *pRegion)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the fill type attribute for the given render region. i.e. all
				paths filled will now be filled with this fill attribute.
	SeeAlso:	FillEffectAttribute; RenderStack; AttributeValue; NodeAttribute;
				FillEffectAttribute::Restore;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void FillEffectAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "FillEffectAttribute::Render - do nothing" )
//	TRACE( _T("Warning - FillEffectAttribute::Render called\n") );
	pRegion->SetFillEffect(this, Temp);
}

/********************************************************************************************

>	void FillEffectAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the fill type attribute for the given render region. i.e. all
				paths filled will now be filled with this fill attribute.
	SeeAlso:	FillEffectAttribute; RenderStack; AttributeValue; NodeAttribute;
				FillEffectAttribute::Render;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void FillEffectAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "FillEffectAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - FillEffectAttribute::Restore called\n") );
	pRegion->RestoreFillEffect(this, Temp);
}

/********************************************************************************************

>	void FillEffectAttribute::SimpleCopy(AttributeValue* pVal)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/6/95
	Inputs:		pVal - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void FillEffectAttribute::SimpleCopy(AttributeValue* pVal)
{
}

/********************************************************************************************

>	BOOL FillEffectAttribute::IsDifferent(AttributeValue* pVal)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/6/95
	Inputs:		pVal - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::IsDifferent

********************************************************************************************/

BOOL FillEffectAttribute::IsDifferent(AttributeValue* pVal)
{
	return (pVal->GetRuntimeClass() != GetRuntimeClass());
}

/********************************************************************************************

>	FillEffectAttribute& FillEffectAttribute::operator=(FillEffectAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/6/95
	Inputs:		Attrib - the AttributeValue to copy.
	Purpose:	Make this effect the same as another.

********************************************************************************************/

FillEffectAttribute& FillEffectAttribute::operator=(FillEffectAttribute& Attrib)
{
	return (*this);
}

/********************************************************************************************

>	INT32 FillEffectAttribute::operator==(const FillEffectAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/6/95
	Inputs:		Attrib - The AttributeValue to test.
	Purpose:	See if two effects are the same.

********************************************************************************************/

INT32 FillEffectAttribute::operator==(const FillEffectAttribute& Attrib)
{
	return (Attrib.GetRuntimeClass() == GetRuntimeClass());
}

/********************************************************************************************

>	NodeAttribute *FillEffectFadeAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrFillEffectFade node from this fill effect attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *FillEffectFadeAttribute::MakeNode()
{
	// Create new attribute node
	AttrFillEffectFade *pAttr = new AttrFillEffectFade();
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}


/********************************************************************************************

>	NodeAttribute *FillEffectRainbowAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrFillEffectRainbow node from this fill effect attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *FillEffectRainbowAttribute::MakeNode()
{
	// Create new attribute node
	AttrFillEffectRainbow *pAttr = new AttrFillEffectRainbow();
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}


/********************************************************************************************

>	NodeAttribute *FillEffectAltRainbowAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrFillEffectAltRainbow node from this fill effect attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *FillEffectAltRainbowAttribute::MakeNode()
{
	// Create new attribute node
	AttrFillEffectAltRainbow *pAttr = new AttrFillEffectAltRainbow();
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}







/********************************************************************************************

>	TranspFillMappingAttribute::TranspFillMappingAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Purpose:	Constructor for transp mapping attributes.
	SeeAlso:	FillMappingAttribute; RenderStack; AttributeValue; NodeAttribute;
				FillMappingAttribute::Restore;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

TranspFillMappingAttribute::TranspFillMappingAttribute()
{
	Repeat = 2;
}

/********************************************************************************************

>	BOOL TranspFillMappingAttribute::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers fill mapping attribute, and provides a default attribute to give
				linear graduations.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL TranspFillMappingAttribute::Init()
{
	// Default to simple linear mapping (ignored for flat fills, obviously)
	TranspFillMappingAttribute *pAttr = new TranspFillMappingLinearAttribute;
	if (pAttr==NULL)
		// error message has already been set by new
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising TranspFillMappingAttribute");
	ERROR2IF(ID != ATTR_TRANSPFILLMAPPING, FALSE, "Incorrect ID for TranspFillMappingAttribute");

	return TRUE;
}

/********************************************************************************************

>	void TranspFillMappingAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void TranspFillMappingAttribute::SimpleCopy(AttributeValue *pValue)
{
	Repeat = ((TranspFillMappingAttribute *) pValue)->Repeat;
}

/********************************************************************************************

>	void TranspFillMappingAttribute::Render(RenderRegion *pRegion)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the fill type attribute for the given render region. i.e. all
				paths filled will now be filled with this fill attribute.
	SeeAlso:	FillMappingAttribute; RenderStack; AttributeValue; NodeAttribute;
				FillMappingAttribute::Restore;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void TranspFillMappingAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TranspFillMappingAttribute::Render - do nothing" )
//	TRACE( _T("Warning - TranspFillMappingAttribute::Render called\n") );
	pRegion->SetTranspFillMapping(this, Temp);
}

/********************************************************************************************

>	void TranspFillMappingAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the fill type attribute for the given render region. i.e. all
				paths filled will now be filled with this fill attribute.
	SeeAlso:	FillMappingAttribute; RenderStack; AttributeValue; NodeAttribute;
				FillMappingAttribute::Render;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void TranspFillMappingAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TranspFillMappingAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TranspFillMappingAttribute::Restore called\n") );
	pRegion->RestoreTranspFillMapping(this, Temp);
}

BOOL TranspFillMappingAttribute::IsDifferent(AttributeValue* pVal)
{
	if (pVal->GetRuntimeClass() != GetRuntimeClass())
		return TRUE;

	return (Repeat != ((TranspFillMappingAttribute*)pVal)->Repeat);
}

TranspFillMappingAttribute& TranspFillMappingAttribute::operator=(TranspFillMappingAttribute& Attrib)
{
	Repeat = Attrib.Repeat;

	return *this;
}

INT32 TranspFillMappingAttribute::operator==(const TranspFillMappingAttribute& Attrib)
{
	if (Attrib.GetRuntimeClass() != GetRuntimeClass())
		return FALSE;

	return (Repeat == Attrib.Repeat);
}


// These create a non-transparent version of the relevant fillmapping for use by
// the airbrush / stroking stuff

FillMappingAttribute *TranspFillMappingAttribute::MakeSimilarNonTranspFillMapping(void)
{
	FillMappingAttribute *pNewAttr = new FillMappingAttribute;	
	if (pNewAttr != NULL)
	{
		pNewAttr->Repeat = Repeat;
	}

	return pNewAttr;
}

FillMappingAttribute *TranspFillMappingSinAttribute::MakeSimilarNonTranspFillMapping(void)
{
	FillMappingSinAttribute *pNewAttr = new FillMappingSinAttribute;	
	if (pNewAttr != NULL)
	{
		pNewAttr->Repeat = Repeat;
	}

	return pNewAttr;
}

FillMappingAttribute *TranspFillMappingLinearAttribute::MakeSimilarNonTranspFillMapping(void)
{
	FillMappingLinearAttribute *pNewAttr = new FillMappingLinearAttribute;	
	if (pNewAttr != NULL)
	{
		pNewAttr->Repeat = Repeat;
	}

	return pNewAttr;
}

/********************************************************************************************

>	NodeAttribute *TranspFillMappingLinearAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrFillMappingLinear node from this fill mapping attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *TranspFillMappingLinearAttribute::MakeNode()
{
	// Create new attribute node
	AttrTranspFillMappingLinear *pAttr = new AttrTranspFillMappingLinear();
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	NodeAttribute *TranspFillMappingSinAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrFillMappingSin node from this fill mapping attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *TranspFillMappingSinAttribute::MakeNode()
{
	// Create new attribute node
	AttrTranspFillMappingSin *pAttr = new AttrTranspFillMappingSin();
	if (pAttr==NULL)
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}






/********************************************************************************************

>	StrokeColourAttribute::StrokeColourAttribute(DocColour& NewCol)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		NewCol - the new stroke colour
	Purpose:	Sets the line colour for this attribute.

********************************************************************************************/

StrokeColourAttribute::StrokeColourAttribute(DocColour& NewCol)
{
	SetStartColour(&NewCol);
}

/********************************************************************************************

>	void StrokeColourAttribute::Render(RenderRegion *pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the line colour attribute for the given render region. i.e. all
				lines drawn will now be drawn in this colour.
	SeeAlso:	StrokeColourAttribute; RenderStack; AttributeValue; NodeAttribute;
				StrokeColourAttribute::Restore; StrokeColourAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void StrokeColourAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "StrokeColourAttribute::Render - do nothing" )
//	TRACE( _T("Warning - StrokeColourAttribute::Render called\n") );
	pRegion->SetLineColour(this, Temp);
}

/********************************************************************************************

>	void StrokeColourAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the line colour attribute for the given render region. i.e. all
				lines drawn will now be drawn in this colour.
	SeeAlso:	StrokeColourAttribute; RenderStack; AttributeValue; NodeAttribute;
				StrokeColourAttribute::Render; StrokeColourAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void StrokeColourAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "StrokeColourAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - StrokeColourAttribute::Restore called\n") );
	pRegion->RestoreLineColour(this, Temp);
}

/********************************************************************************************

>	NodeAttribute *StrokeColourAttribute::MakeNode()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrStrokeColour node from this fill colour attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *StrokeColourAttribute::MakeNode()
{
	// Create new attribute node
	AttrStrokeColour *pAttr = new AttrStrokeColour();

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	void StrokeColourAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	FillColourAttribute; RenderStack; AttributeValue; NodeAttribute;
				FillColourAttribute::Render; FillColourAttribute::Restore;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void StrokeColourAttribute::SimpleCopy(AttributeValue *pValue)
{
	Colour = ((StrokeColourAttribute *) pValue)->Colour;
}

/********************************************************************************************

>	BOOL StrokeColourAttribute::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers line colour attribute, and provides a default attribute to give
				black lines.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL StrokeColourAttribute::Init()
{
	// Default to black lines
	StrokeColourAttribute *pAttr = new StrokeColourAttribute;
	if (pAttr == NULL)
		return FALSE;

	AttributeManager::FindDefaultColour( ColourManager::GetCurrentColourList(),
										_R(IDS_BLACKNAME), &pAttr->Colour );

//	pAttr->Colour = AttributeManager::DefaultBlack;
	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising StrokeColourAttribute");
	ERROR2IF(ID != ATTR_STROKECOLOUR, FALSE, "Incorrect ID for StrokeColourAttribute");

	return TRUE;
}







/********************************************************************************************

>	StrokeTranspAttribute::StrokeTranspAttribute(UINT32 NewTransp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		NewTransp - the new stroke transparency
	Purpose:	Sets the line transparency for this attribute.

********************************************************************************************/

StrokeTranspAttribute::StrokeTranspAttribute(UINT32 NewTransp)
{
	SetStartTransp(&NewTransp);
}

/********************************************************************************************

>	void StrokeTranspAttribute::Render(RenderRegion *pRegion)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the line colour attribute for the given render region. i.e. all
				lines drawn will now be drawn in this colour.
	SeeAlso:	StrokeTranspAttribute; RenderStack; AttributeValue; NodeAttribute;
				StrokeTranspAttribute::Restore; StrokeTranspAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void StrokeTranspAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "StrokeTranspAttribute::Render - do nothing" )
//	TRACE( _T("Warning - StrokeColourAttribute::Render called\n") );
	pRegion->SetLineTransp(this, Temp);
}

/********************************************************************************************

>	void StrokeTranspAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the line colour attribute for the given render region. i.e. all
				lines drawn will now be drawn in this colour.
	SeeAlso:	StrokeTranspAttribute; RenderStack; AttributeValue; NodeAttribute;
				StrokeTranspAttribute::Render; StrokeTranspAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void StrokeTranspAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "StrokeTranspAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - StrokeColourAttribute::Restore called\n") );
	pRegion->RestoreLineTransp(this, Temp);
}

/********************************************************************************************

>	NodeAttribute *StrokeTranspAttribute::MakeNode()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrStrokeTransp node from this fill colour attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *StrokeTranspAttribute::MakeNode()
{
	// Create new attribute node
	AttrStrokeTransp *pAttr = new AttrStrokeTransp();

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	void StrokeTranspAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	FillColourAttribute; RenderStack; AttributeValue; NodeAttribute;
				FillColourAttribute::Render; FillColourAttribute::Restore;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void StrokeTranspAttribute::SimpleCopy(AttributeValue *pValue)
{
	Transp = ((StrokeTranspAttribute *) pValue)->Transp;
	TranspType = ((StrokeTranspAttribute *) pValue)->TranspType;
}

/********************************************************************************************

>	BOOL StrokeTranspAttribute::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers line colour attribute, and provides a default attribute to give
				black lines.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL StrokeTranspAttribute::Init()
{
	// Default to black lines
	StrokeTranspAttribute *pAttr = new StrokeTranspAttribute;
	if (pAttr == NULL)
		return FALSE;

	pAttr->Transp = 0;
	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising StrokeTranspAttribute");
	ERROR2IF(ID != ATTR_STROKETRANSP, FALSE, "Incorrect ID for StrokeTranspAttribute");

	return TRUE;
}



/********************************************************************************************

		Mouldy bits

********************************************************************************************/

/********************************************************************************************

>	INT32 FillGeometryAttribute::Mould(AttrMould* pMouldAttr, DocCoord* pOutputCoords, INT32 NumCoords)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/03/95
	Purpose:	Moulds a Fill attribute

********************************************************************************************/

INT32 FillGeometryAttribute::Mould(	MouldAttribute* pMouldAttr,
									DocCoord* SourcePoints, INT32 NumCoords,
									DocCoord* OutputCoords	)
{
	if (pMouldAttr == NULL || pMouldAttr->GetMouldShape() == NULL)
		return 0;

	if (pMouldAttr->IsPerspective() && NumCoords >= 4)	// Is it a perspective mould ?
	{
		// Yes, so we'll need to make a fourth control point
		INT32 dx1 = SourcePoints[1].x - SourcePoints[0].x;
		INT32 dx2 = SourcePoints[2].x - SourcePoints[0].x;

		INT32 dy1 = SourcePoints[1].y - SourcePoints[0].y;
		INT32 dy2 = SourcePoints[2].y - SourcePoints[0].y;

		SourcePoints[3] = SourcePoints[2];

		SourcePoints[2].x = SourcePoints[0].x + dx1 + dx2;		
		SourcePoints[2].y = SourcePoints[0].y + dy1 + dy2;
	}

	// Lets get the mould geometry
	MouldGeometry* pMould = pMouldAttr->GetMouldShape();
	if (pMould == NULL)
		return 0;

	// If it's a perspective mould, then we need 4 Control points,
	// otherwise just 3 are needed.
	INT32 MouldPoints = pMouldAttr->IsPerspective() ? 4 : 3;

	// Ensure we don't overrun the output array
	if (MouldPoints > NumCoords)
		MouldPoints = NumCoords;

	// It is possible for a point to fail to Mould,
	// (eg if it's outside the Mould Bounds),
	// so we'll keep an array of BOOL's to say whether
	// each point moulded ok or not.
	BOOL* PointOK = new BOOL[MouldPoints];

	Node* MouldParent = pMouldAttr->GetParentMould();
	while (pMould != NULL)
	{
		BOOL AllPointsOK = TRUE;
		
		for ( INT32 point = 0; point < MouldPoints; point++)
		{
			// Mould each points in turn
			if (pMould->MouldPoint(SourcePoints[point], OutputCoords[point]))
			{
				// The point moulded ok
				PointOK[point] = TRUE;
			}
			else
			{
				// The point failed to mould
				PointOK[point] = FALSE;
				AllPointsOK = FALSE;
			}
		}

		if (!AllPointsOK)
		{
			// One or more of the control points failed to mould,
			// (probably because it is outside the bounds of the mould)
			// so we will make sure it is sensible.
			TidyMouldPoints(SourcePoints, OutputCoords, PointOK, MouldPoints);
		}

		// Now lets have a look at the parent of the Mould
		MouldParent = MouldParent->FindParent();

		while (MouldParent != NULL)
		{
			if ( IS_A(MouldParent,NodeMould) )		// Is the Parent another Mould ?
			{
				// This mould has been moulded itself, so get the parent
				// mould geometry, and transform all the points again
				pMould = ((NodeMould*)MouldParent)->GetGeometry();

				// We're going to transform the transformed points
				for ( INT32 point = 0; point < MouldPoints; point++)
				{
					SourcePoints[point] = OutputCoords[point];
				}
	
				break;	// Get out of this loop
			}			

			if (MouldParent->IsLayer())	// Have we reached the parent Layer yet ?
			{
				// There can't be any more Moulds now !
				pMould = NULL;
				break;
			}			
			else
			{
				// Try the next parent
				MouldParent = MouldParent->FindParent();
			}
		}
	}

	delete PointOK;		// Delete the array of BOOL's

	return MouldPoints;	// Return the number of Points Moulded
}



/********************************************************************************************

>	virtual AttributeValue *FillGeometryAttribute::MouldIntoStroke(PathStrokerVector *pMoulder,
																			double TransScale);
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/97

	Inputs:		pMoulder	- A PathStrokerVector which knows how to translate points to "mould" them
							  (May be NULL, in which case moulding of points does not occur)

				TransScale	- A fraction between 0.0 and 1.0, by which any transparency
							  values in this geometry will be scaled, allowing the caller to
							  effectively apply a flat transparency level to everything that is
							  moulded. Use 1.0 to leave transparency unaltered.

	Returns:	NULL if the original attribute can be used, else
				A pointer to a copy of this object, suitably moulded and adjusted for
				the transparency scaling. The caller must delete the copy when finished
				with it.

	Purpose:	Helper function for the PathStrokerVector class, which "moulds" clipart
				subtrees to lie along an arbitrary path.

				This function is called to mould fill geometries, so that fill endpoints
				are translated to appropriate positions in the destination envelope, and
				allows the caller to effectively apply a flat transparency by scaling
				any transparency values in this geometry by the given fraction.

********************************************************************************************/

AttributeValue *FillGeometryAttribute::MouldIntoStroke(PathStrokerVector *pMoulder, double TransScale)
{
// Taken out of WEBSTER Neville 6/8/97
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	FillGeometryAttribute *pCopy = (FillGeometryAttribute *) GetRuntimeClass()->CreateObject();
	if (pCopy != NULL)
	{
		// Copy this object's values across
		*pCopy = *this;

		// And mould all of the geometry points across
		if (pMoulder != NULL)
		{
			JCW_MOULDPOINT(pCopy, StartPoint);
			JCW_MOULDPOINT(pCopy, EndPoint);
			JCW_MOULDPOINT(pCopy, EndPoint2);
			JCW_MOULDPOINT(pCopy, EndPoint3);
		}

		ERROR3IF(TransScale < 0.0 || TransScale > 1.0, "Out of range TransScale parameter");
		if (TransScale >= 0.0 && TransScale < 1.0)
		{
			JCW_MOULDTRANS(pCopy, StartTransp);
			JCW_MOULDTRANS(pCopy, EndTransp);
			JCW_MOULDTRANS(pCopy, EndTransp2);
			JCW_MOULDTRANS(pCopy, EndTransp3);
		}
	}

	return(pCopy);
#else
	return NULL;
#endif // VECTOR_STROKING
}



/********************************************************************************************

>	void FillGeometryAttribute::TidyMouldPoints(DocCoord* SrcPoints, DocCoord* DestPoints, 
												BOOL* PointOK, INT32 MouldPoints)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/03/95
	Purpose:	Tidies up control points that fail to mould correctly

********************************************************************************************/

void FillGeometryAttribute::TidyMouldPoints(DocCoord* SrcPoints, DocCoord* DestPoints, 
											BOOL* PointOK, INT32 MouldPoints)
{
	if (!PointOK[0])  // Did the Start Point transform ?
	{
		if (PointOK[1])	// So, did the End Point transform ?
		{
			// Work out how the End Point moved
			INT32 dx = DestPoints[1].x - SrcPoints[1].x;			
			INT32 dy = DestPoints[1].y - SrcPoints[1].y;			

			// and move the start point the same amount
			DestPoints[0] = SrcPoints[0];
			DestPoints[0].translate(dx,dy);
		}
		else if (PointOK[2])
		{
			// Work out how the End2 Point moved
			INT32 dx = DestPoints[2].x - SrcPoints[2].x;			
			INT32 dy = DestPoints[2].y - SrcPoints[2].y;			

			// and move the start point the same amount
			DestPoints[0] = SrcPoints[0];
			DestPoints[0].translate(dx,dy);
		}
	}

	if (!PointOK[1])
	{
		if (PointOK[0])	// So, did the Start Point transform ?
		{
			// Work out how the Start Point moved
			INT32 dx = DestPoints[0].x - SrcPoints[0].x;			
			INT32 dy = DestPoints[0].y - SrcPoints[0].y;			

			// and move the end point the same amount
			DestPoints[1] = SrcPoints[1];
			DestPoints[1].translate(dx,dy);
		}
		else if (PointOK[2])
		{
			// Work out how the End2 Point moved
			INT32 dx = DestPoints[2].x - SrcPoints[2].x;			
			INT32 dy = DestPoints[2].y - SrcPoints[2].y;			

			// and move the end point the same amount
			DestPoints[1] = SrcPoints[1];
			DestPoints[1].translate(dx,dy);
		}
	}

	if (!PointOK[2])
	{
		if (PointOK[0])	// So, did the Start Point transform ?
		{
			// Work out how the Start Point moved
			INT32 dx = DestPoints[0].x - SrcPoints[0].x;			
			INT32 dy = DestPoints[0].y - SrcPoints[0].y;			

			// and move the end2 point the same amount
			DestPoints[2] = SrcPoints[2];
			DestPoints[2].translate(dx,dy);
		}
		else if (PointOK[1])
		{
			// Work out how the End Point moved
			INT32 dx = DestPoints[1].x - SrcPoints[1].x;			
			INT32 dy = DestPoints[1].y - SrcPoints[1].y;			

			// and move the end2 point the same amount
			DestPoints[2] = SrcPoints[2];
			DestPoints[2].translate(dx,dy);
		}
	}

	if (MouldPoints>3 && !PointOK[3])
	{
		TRACEUSER( "Mike", _T("Perspective Point failed to mould\n"));
	}
}

/********************************************************************************************

>	MouldAttribute::MouldAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Purpose:	Default constructor for Mould Attribute.

********************************************************************************************/

MouldAttribute::MouldAttribute()
{
	SetParentMould(NULL);
	SetMouldShape(NULL);
}

/********************************************************************************************

>	MouldAttribute::MouldAttribute(NodeMould* pMould, MouldGeometry* pMouldShape)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pMould - the Mould to associate with this attribute
	Purpose:	Constructor for Mould attribute.

********************************************************************************************/

MouldAttribute::MouldAttribute(NodeMould* pMould, MouldGeometry* pMouldShape)
{
	SetParentMould(pMould);
	SetMouldShape(pMouldShape);
}

/********************************************************************************************

>	void MouldAttribute::Render(RenderRegion *pRegion)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the line colour attribute for the given render region. i.e. all
				lines drawn will now be drawn in this colour.
	SeeAlso:	MouldAttribute; RenderStack; AttributeValue; NodeAttribute;
				MouldAttribute::Restore; MouldAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void MouldAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//	pRegion->SetMould(this, Temp);
}

/********************************************************************************************

>	void MouldAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the line colour attribute for the given render region. i.e. all
				lines drawn will now be drawn in this colour.
	SeeAlso:	MouldAttribute; RenderStack; AttributeValue; NodeAttribute;
				MouldAttribute::Render; MouldAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void MouldAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//	pRegion->RestoreMould(this, Temp);
}

/********************************************************************************************

>	NodeAttribute *MouldAttribute::MakeNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrMould node from this fill colour attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *MouldAttribute::MakeNode()
{
	// Create new attribute node
	AttrMould *pAttr = new AttrMould();

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	void MouldAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	FillColourAttribute; RenderStack; AttributeValue; NodeAttribute;
				FillColourAttribute::Render; FillColourAttribute::Restore;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void MouldAttribute::SimpleCopy(AttributeValue *pValue)
{
	ERROR3IF(!pValue->IsKindOf(CC_RUNTIME_CLASS(MouldAttribute)), "MouldAttribute::SimpleCopy can only copy MouldAttributes");
	SetParentMould(((MouldAttribute*)pValue)->GetParentMould());
	SetMouldShape(((MouldAttribute*)pValue)->GetMouldShape());
}

/********************************************************************************************

>	BOOL MouldAttribute::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers line colour attribute, and provides a default attribute to give
				black lines.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL MouldAttribute::Init()
{
	// Default to no Mould
	MouldAttribute *pAttr = new MouldAttribute;
	if (pAttr == NULL)
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising MouldAttribute");
	ERROR2IF(ID != ATTR_MOULD, FALSE, "Incorrect ID for MouldAttribute");

	return TRUE;
}

/********************************************************************************************

>	void MouldAttribute::SetMouldShape(MouldGeometry* pMould)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	Set the mould shape associated with this Attr.

********************************************************************************************/

void MouldAttribute::SetMouldShape(MouldGeometry* pMould)
{
	pMouldShape = pMould;
	IsPerspectiveMould = FALSE;

	if (pMouldShape == NULL)
		return;

	if (pMouldShape->IsKindOf(CC_RUNTIME_CLASS(MouldPerspective)))
	{
		IsPerspectiveMould = TRUE;
	}
}

/********************************************************************************************

>	void MouldAttribute::SetParentMould(NodeMould* pMould)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	Set the parent mould associated with this Attr.

********************************************************************************************/

void MouldAttribute::SetParentMould(NodeMould* pMould)
{
	pParentMould = pMould;
}

/********************************************************************************************

		Blendy bits

********************************************************************************************/

/********************************************************************************************

>	BOOL FillGeometryAttribute::Blend(BlendAttrParam* pBlendParam)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Returns:	TRUE - Blend went ok, the blended attr val is valid; FALSE if not.
	Purpose:	Blends between two fill attributes.
				This is a general blend function for all fill attributes.
				It will blend between any types of fill, but will always use the 'type'
				of the first fill, taking control points, colours, and transparency
				from the end fill if it can.

	Errors:		Out of memory.
	SeeAlso:	FillGeometryAttribute::BlendFillColours;
				FillGeometryAttribute::BlendFillTransp;
				FillGeometryAttribute::BlendControlPoints;
				FlatFillAttribute::Blend;
				GradFillAttribute::Blend

********************************************************************************************/

BOOL FillGeometryAttribute::Blend(BlendAttrParam* pBlendParam)
{
	// First make a new object of the same type as this one
	CCRuntimeClass* ObjectType = GetRuntimeClass();
	FillGeometryAttribute* pNewAttr = (FillGeometryAttribute*)ObjectType->CreateObject();

	if (pNewAttr == NULL)
	{
		// Fail if we couldn't create the new fill
		pBlendParam->SetBlendedAttrVal(NULL);
		return FALSE;
	}

	// Make the new fill an exact copy of this one
	pNewAttr->SimpleCopy(this);

	// Now get the fill that we are blending to
	FillGeometryAttribute* OtherFill = 
				(FillGeometryAttribute*)pBlendParam->GetOtherAttrVal();

	// and what point along the blend we are at
	double Ratio = pBlendParam->GetBlendRatio();
	double objectRatio = pBlendParam->GetObjectRatio();

	// Blend the profiles (if there are any ....)

	CProfileBiasGain Default;
	
	CProfileBiasGain BiasGain = DiagramMapper;						// the first fills profile
	CProfileBiasGain OtherBiasGain = OtherFill->DiagramMapper;		// the other fills profile

	// only blend profiles if we need to ....

	if (!((BiasGain == Default) && (OtherBiasGain == Default)))
	{
		// then we need to blend the profiles ....

		CProfileBiasGain BlendedProfile;							// test for now ....

		double InvRatio = 1 - Ratio;
		double BlendedBias = (BiasGain.GetBias () * InvRatio) + (OtherBiasGain.GetBias () * Ratio);
		double BlendedGain = (BiasGain.GetGain () * InvRatio) + (OtherBiasGain.GetGain () * Ratio);

		BlendedProfile.SetBias (BlendedBias);
		BlendedProfile.SetGain (BlendedGain);

		pNewAttr->SetProfile (BlendedProfile);
	}

	// CGS:  don't worry if pNewAttr->GetColourRamp () is NULL, this is handled internally by pNewAttr->GetColourRamp ()->Blend ()
	// (even if it is NULL, this code is still perfectly valid:  it means blend a no0n-multi-stage fill to something ....)
	pNewAttr->GetColourRamp ()->Blend (pNewAttr, pBlendParam, OtherFill/*->GetColourRamp ()*/);

	// Blend the Start and End Colours
	DocColour BlendColour;

	DocColour* StartCol 		= GetStartColour();
	DocColour* OtherStartCol 	= OtherFill->GetStartColour();
	DocColour* EndCol			= GetEndColour();
	DocColour* OtherEndCol	 	= OtherFill->GetEndColour();
	DocColour* EndCol2			= GetEndColour2();
	DocColour* OtherEndCol2	 	= OtherFill->GetEndColour2();
	DocColour* EndCol3			= GetEndColour3();
	DocColour* OtherEndCol3	 	= OtherFill->GetEndColour3();
	DocColour* NonNullCol		= NULL;
	DocColour* OtherNonNullCol	= NULL;

	DocColour TempStartCol;
	DocColour TempEndCol;
	DocColour TempOtherStartCol;
	DocColour TempOtherEndCol;

	// Check for a Greyscale Bitmap fill with no start or end colours
	// and use Black and White instead of 'No Colour'
	if (IsABitmapFill() && StartCol == NULL && EndCol == NULL)
	{
		if (GetBitmapRef() != NULL)
		{
			if (CheckForGreyscaleBitmapBlend(GetBitmapRef()->GetBitmap(), &TempStartCol, &TempEndCol))
			{
				StartCol 	= &TempStartCol;
				EndCol 		= &TempEndCol;
			}
		}
	}

	// Same as above, but for the 'Other Fill'
	if (OtherFill->IsABitmapFill() && OtherStartCol == NULL && OtherEndCol == NULL)
	{
		if (OtherFill->GetBitmapRef() != NULL)
		{
			if (CheckForGreyscaleBitmapBlend(OtherFill->GetBitmapRef()->GetBitmap(), &TempOtherStartCol, &TempOtherEndCol))
			{
				OtherStartCol 	= &TempOtherStartCol;
				OtherEndCol 	= &TempOtherEndCol;
			}
		}
	}

	// Handle blending of non bitmap fill colours
	if (BlendFillColours(StartCol, OtherStartCol, &BlendColour, Ratio, pBlendParam))
		pNewAttr->SetStartColour(&BlendColour);

	if (EndCol == NULL)
		NonNullCol = StartCol;
	else
		NonNullCol = EndCol;
	
	if (OtherEndCol == NULL)
	{
		OtherNonNullCol = OtherStartCol;

		if (BlendFillColours(EndCol, OtherStartCol, &BlendColour, Ratio, pBlendParam))
			pNewAttr->SetEndColour(&BlendColour);
	}
	else 
	{
		OtherNonNullCol = OtherEndCol;

		if (BlendFillColours(EndCol, OtherEndCol, &BlendColour, Ratio, pBlendParam))
			pNewAttr->SetEndColour(&BlendColour);
	}

	if ((EndCol2 != NULL) || (OtherEndCol2 != NULL) || (EndCol3 != NULL) || (OtherEndCol3 != NULL))
	{
		if (EndCol2 == NULL)
			EndCol2 = NonNullCol;
		if (OtherEndCol2 == NULL)
			OtherEndCol2 = OtherNonNullCol;
		if (EndCol3 == NULL)
			EndCol3 = NonNullCol;
		if (OtherEndCol3 == NULL)
			OtherEndCol3 = OtherNonNullCol;
	
		if (BlendFillColours(EndCol2, OtherEndCol2, &BlendColour, Ratio, pBlendParam))
				pNewAttr->SetEndColour2(&BlendColour);

		if (BlendFillColours(EndCol3, OtherEndCol3, &BlendColour, Ratio, pBlendParam))
				pNewAttr->SetEndColour3(&BlendColour);
	}

	// Blend the Start and End Transparencies
	UINT32 BlendTransp;

	if (BlendFillTransp(GetStartTransp(), OtherFill->GetStartTransp(), &BlendTransp, Ratio, pBlendParam))
		pNewAttr->SetStartTransp(&BlendTransp);

	if (BlendFillTransp(GetEndTransp(), OtherFill->GetEndTransp(), &BlendTransp, Ratio, pBlendParam))
		pNewAttr->SetEndTransp(&BlendTransp);

	if (BlendFillTransp(GetEndTransp2(), OtherFill->GetEndTransp2(), &BlendTransp, Ratio, pBlendParam))
		pNewAttr->SetEndTransp2(&BlendTransp);

	if (BlendFillTransp(GetEndTransp3(), OtherFill->GetEndTransp3(), &BlendTransp, Ratio, pBlendParam))
		pNewAttr->SetEndTransp3(&BlendTransp);

	// Blend the Control Points
	DocCoord BlendPoint;

	if (!pBlendParam->GetObjectProfileProcessing ())
	{
		// were not doing object processing - fill positions are affected by attribute ratio ....
		if (BlendControlPoints(GetStartPoint(), OtherFill->GetStartPoint(), &BlendPoint, Ratio, pBlendParam))
			pNewAttr->SetStartPoint(&BlendPoint);

		if (BlendControlPoints(GetEndPoint(), OtherFill->GetEndPoint(), &BlendPoint, Ratio, pBlendParam))
			pNewAttr->SetEndPoint(&BlendPoint);

		if (BlendControlPoints(GetEndPoint2(), OtherFill->GetEndPoint2(), &BlendPoint, Ratio, pBlendParam))
			pNewAttr->SetEndPoint2(&BlendPoint);
	}
	else
	{
		// were are doing object processing - fill positions are affected by object ratio ....
		if (BlendControlPoints(GetStartPoint(), OtherFill->GetStartPoint(), &BlendPoint, objectRatio, pBlendParam))
			pNewAttr->SetStartPoint(&BlendPoint);

		if (BlendControlPoints(GetEndPoint(), OtherFill->GetEndPoint(), &BlendPoint, objectRatio, pBlendParam))
			pNewAttr->SetEndPoint(&BlendPoint);

		if (BlendControlPoints(GetEndPoint2(), OtherFill->GetEndPoint2(), &BlendPoint, objectRatio, pBlendParam))
			pNewAttr->SetEndPoint2(&BlendPoint);
	}

	// Make sure that any BitmapRefs in the Blended attributes,
	// are Tempory ones, so we don't try to detach the bitmap
	// when the Blend Attrs are destroyed.

// ****************************************************
//
// Temporarily removed to fix problem with saving Blended bitmaps.
// May re-intruduce problems with deleting bitmaps within blends,
// but the saving problem is more important at the moment ....
// .... Will, 15/12/95

//	if (GetBitmapRef() != NULL)
//		GetBitmapRef()->TempRef = TRUE;

//	if (OtherFill->GetBitmapRef() != NULL)
//		OtherFill->GetBitmapRef()->TempRef = TRUE;

// ****************************************************

//	if (pNewAttr->GetBitmapRef() != NULL)
//		pNewAttr->GetBitmapRef()->TempRef = TRUE;

	// Set the new fill as the blended attribute
	pBlendParam->SetBlendedAttrVal(pNewAttr);

	return TRUE;
}


/********************************************************************************************

>	BOOL FlatFillAttribute::Blend(BlendAttrParam* pBlendParam)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Returns:	TRUE - Blend went ok, the blended attr val is valid; FALSE if not.
	Purpose:	Blends between two flat fill attributes.
				This current calls the general fill blend function.
				(See FillGeometryAttribute::Blend).

	Errors:		Out of memory.
	SeeAlso:	FillGeometryAttribute::Blend;
				FillGeometryAttribute::BlendFillColours;
				FillGeometryAttribute::BlendFillTransp;
				FillGeometryAttribute::BlendControlPoints;
				GradFillAttribute::Blend

********************************************************************************************/

BOOL FlatFillAttribute::Blend(BlendAttrParam* pBlendParam)
{
	// Since this is a flat fill, we will use the other fill's geometry
	// (mainly for ArtWorks compatability)

	// First get the fill that we are blending to
	FillGeometryAttribute* OtherFill = 
				(FillGeometryAttribute*)pBlendParam->GetOtherAttrVal();

	// Make a new object of the same type
	CCRuntimeClass* ObjectType = OtherFill->GetRuntimeClass();
	FillGeometryAttribute* pNewAttr = (FillGeometryAttribute*)ObjectType->CreateObject();

	if (pNewAttr == NULL)
	{
		// Fail if we couldn't create the new fill
		pBlendParam->SetBlendedAttrVal(NULL);
		return FALSE;
	}

	// Make the new fill an exact copy of the other one
	pNewAttr->SimpleCopy(OtherFill);

	// and what point along the blend we are at
	double Ratio = pBlendParam->GetBlendRatio();
	double objectRatio = pBlendParam->GetObjectRatio();

	// Blend the Start and End Colours
	DocColour BlendColour;

	DocColour* OtherStartCol 	= OtherFill->GetStartColour();
	DocColour* OtherEndCol	 	= OtherFill->GetEndColour();

	DocColour TempOtherStartCol;
	DocColour TempOtherEndCol;

	// Check for a Greyscale Bitmap fill with no start or end colours
	// and use Black and White instead of 'No Colour'
	if (OtherFill->IsABitmapFill() && OtherStartCol == NULL && OtherEndCol == NULL)
	{
		if (OtherFill->GetBitmapRef() != NULL)
		{
			if (CheckForGreyscaleBitmapBlend(OtherFill->GetBitmapRef()->GetBitmap(), &TempOtherStartCol, &TempOtherEndCol))
			{
				OtherStartCol 	= &TempOtherStartCol;
				OtherEndCol 	= &TempOtherEndCol;
			}
		}
	}

	if (BlendFillColours(GetStartColour(), OtherStartCol, &BlendColour, Ratio, pBlendParam))
		pNewAttr->SetStartColour(&BlendColour);

	if (BlendFillColours(GetStartColour(), OtherEndCol, &BlendColour, Ratio, pBlendParam))
		pNewAttr->SetEndColour(&BlendColour);

	// CGS:  don't worry if pNewAttr->GetColourRamp () is NULL, this is handled internally by pNewAttr->GetColourRamp ()->Blend ()
	// (even if it is NULL, this code is still perfectly valid:  it means blend a no0n-multi-stage fill to something ....)
	pNewAttr->GetColourRamp ()->Blend (pNewAttr, pBlendParam, this/*->GetColourRamp ()*/, TRUE);

	// Blend the Control Points
	DocCoord BlendPoint;

	if (!pBlendParam->GetObjectProfileProcessing ())
	{
		// were not doing object processing - fill positions are affected by attribute ratio ....
		if (BlendControlPoints(GetStartPoint(), OtherFill->GetStartPoint(), &BlendPoint, Ratio, pBlendParam))
			pNewAttr->SetStartPoint(&BlendPoint);

		if (BlendControlPoints(GetEndPoint(), OtherFill->GetEndPoint(), &BlendPoint, Ratio, pBlendParam))
			pNewAttr->SetEndPoint(&BlendPoint);

		if (BlendControlPoints(GetEndPoint2(), OtherFill->GetEndPoint2(), &BlendPoint, Ratio, pBlendParam))
			pNewAttr->SetEndPoint2(&BlendPoint);
	}
	else
	{
		// were are doing object processing - fill positions are affected by object ratio ....
		if (BlendControlPoints(OtherFill->GetStartPoint(), GetStartPoint(), &BlendPoint, objectRatio, pBlendParam, TRUE))
			pNewAttr->SetStartPoint(&BlendPoint);

		if (BlendControlPoints(OtherFill->GetEndPoint(), GetEndPoint(), &BlendPoint, objectRatio, pBlendParam, TRUE))
			pNewAttr->SetEndPoint(&BlendPoint);

		if (BlendControlPoints(OtherFill->GetEndPoint2(), GetEndPoint2(), &BlendPoint, objectRatio, pBlendParam, TRUE))
			pNewAttr->SetEndPoint2(&BlendPoint);
	}

	// Set the new fill as the blended attribute
	pBlendParam->SetBlendedAttrVal(pNewAttr);

	return TRUE;
}

/********************************************************************************************

>	BOOL FlatTranspFillAttribute::Blend(BlendAttrParam* pBlendParam)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Returns:	TRUE - Blend went ok, the blended attr val is valid; FALSE if not.
	Purpose:	Blends between two flat fill attributes.
				This current calls the general fill blend function.
				(See FillGeometryAttribute::Blend).

	Errors:		Out of memory.
	SeeAlso:	FillGeometryAttribute::Blend;
				FillGeometryAttribute::BlendFillColours;
				FillGeometryAttribute::BlendFillTransp;
				FillGeometryAttribute::BlendControlPoints;
				GradFillAttribute::Blend

********************************************************************************************/

BOOL FlatTranspFillAttribute::Blend(BlendAttrParam* pBlendParam)
{
	// Since this is a flat fill, we will use the other fill's geometry
	// (mainly for ArtWorks compatability)

	// First get the fill that we are blending to
	FillGeometryAttribute* OtherFill = 
				(FillGeometryAttribute*)pBlendParam->GetOtherAttrVal();

	// Make a new object of the same type
	CCRuntimeClass* ObjectType = OtherFill->GetRuntimeClass();
	FillGeometryAttribute* pNewAttr = (FillGeometryAttribute*)ObjectType->CreateObject();

	if (pNewAttr == NULL)
	{
		// Fail if we couldn't create the new fill
		pBlendParam->SetBlendedAttrVal(NULL);
		return FALSE;
	}

	// Make the new fill an exact copy of the other one
	pNewAttr->SimpleCopy(OtherFill);

	// and what point along the blend we are at
	double Ratio = pBlendParam->GetBlendRatio();
	double objectRatio = pBlendParam->GetObjectRatio();

	// Blend the Start and End Transparencies
	UINT32 BlendTransp;

	if (BlendFillTransp(GetStartTransp(), OtherFill->GetStartTransp(), &BlendTransp, Ratio, pBlendParam))
		pNewAttr->SetStartTransp(&BlendTransp);

	if (BlendFillTransp(GetStartTransp(), OtherFill->GetEndTransp(), &BlendTransp, Ratio, pBlendParam))
		pNewAttr->SetEndTransp(&BlendTransp);


	// Blend the Control Points
	DocCoord BlendPoint;

	if (!pBlendParam->GetObjectProfileProcessing ())
	{
		// were not doing object processing - fill positions are affected by attribute ratio ....
		if (BlendControlPoints(GetStartPoint(), OtherFill->GetStartPoint(), &BlendPoint, Ratio, pBlendParam))
			pNewAttr->SetStartPoint(&BlendPoint);

		if (BlendControlPoints(GetEndPoint(), OtherFill->GetEndPoint(), &BlendPoint, Ratio, pBlendParam))
			pNewAttr->SetEndPoint(&BlendPoint);

		if (BlendControlPoints(GetEndPoint2(), OtherFill->GetEndPoint2(), &BlendPoint, Ratio, pBlendParam))
			pNewAttr->SetEndPoint2(&BlendPoint);
	}
	else
	{
		// were are doing object processing - fill positions are affected by object ratio ....
		if (BlendControlPoints(GetStartPoint(), OtherFill->GetStartPoint(), &BlendPoint, objectRatio, pBlendParam))
			pNewAttr->SetStartPoint(&BlendPoint);

		if (BlendControlPoints(GetEndPoint(), OtherFill->GetEndPoint(), &BlendPoint, objectRatio, pBlendParam))
			pNewAttr->SetEndPoint(&BlendPoint);

		if (BlendControlPoints(GetEndPoint2(), OtherFill->GetEndPoint2(), &BlendPoint, objectRatio, pBlendParam))
			pNewAttr->SetEndPoint2(&BlendPoint);
	}

	// Set the new fill as the blended attribute
	pBlendParam->SetBlendedAttrVal(pNewAttr);

	return TRUE;
}

/********************************************************************************************

>	BOOL GradFillAttribute::Blend(BlendAttrParam* pBlendParam)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Returns:	TRUE - Blend went ok, the blended attr val is valid; FALSE if not.
	Purpose:	Blends between two graduated fill attributes.
				This current calls the general fill blend function.
				(See FillGeometryAttribute::Blend).

	Errors:		Out of memory.
	SeeAlso:	FillGeometryAttribute::Blend;
				FillGeometryAttribute::BlendFillColours;
				FillGeometryAttribute::BlendFillTransp;
				FillGeometryAttribute::BlendControlPoints;
				FlatFillAttribute::Blend

********************************************************************************************/

BOOL GradFillAttribute::Blend(BlendAttrParam* pBlendParam)
{
	return FillGeometryAttribute::Blend(pBlendParam);
}

/********************************************************************************************

>	BOOL GradTranspFillAttribute::Blend(BlendAttrParam* pBlendParam)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Returns:	TRUE - Blend went ok, the blended attr val is valid; FALSE if not.
	Purpose:	Blends between two graduated fill attributes.
				This current calls the general fill blend function.
				(See FillGeometryAttribute::Blend).

	Errors:		Out of memory.
	SeeAlso:	FillGeometryAttribute::Blend;
				FillGeometryAttribute::BlendFillColours;
				FillGeometryAttribute::BlendFillTransp;
				FillGeometryAttribute::BlendControlPoints;
				FlatFillAttribute::Blend

********************************************************************************************/

BOOL GradTranspFillAttribute::Blend(BlendAttrParam* pBlendParam)
{
	return FillGeometryAttribute::Blend(pBlendParam);
}

/********************************************************************************************

>	BOOL StrokeTranspAttribute::Blend(BlendAttrParam* pBlendParam)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Returns:	TRUE - Blend went ok, the blended attr val is valid; FALSE if not.
	Purpose:	Blends between two fill attributes.
				This is a general blend function for all fill attributes.
				It will blend between any types of fill, but will always use the 'type'
				of the first fill, taking control points, colours, and transparency
				from the end fill if it can.

	Errors:		Out of memory.
	SeeAlso:	FillGeometryAttribute::BlendFillColours;
				FillGeometryAttribute::BlendFillTransp;
				FillGeometryAttribute::BlendControlPoints;
				FlatFillAttribute::Blend;
				GradFillAttribute::Blend

********************************************************************************************/

BOOL StrokeTranspAttribute::Blend(BlendAttrParam* pBlendParam)
{
	// First make a new object of the same type as this one
	CCRuntimeClass* ObjectType = GetRuntimeClass();
	FillGeometryAttribute* pNewAttr = (FillGeometryAttribute*)ObjectType->CreateObject();

	if (pNewAttr == NULL)
	{
		// Fail if we couldn't create the new fill
		pBlendParam->SetBlendedAttrVal(NULL);
		return FALSE;
	}

	// Make the new fill an exact copy of this one
	pNewAttr->SimpleCopy(this);

	// Now get the fill that we are blending to
	FillGeometryAttribute* OtherFill = 
				(FillGeometryAttribute*)pBlendParam->GetOtherAttrVal();

	// and what point along the blend we are at
	double Ratio = pBlendParam->GetBlendRatio();

	// Look at the Start and Attribute maps to see if either end
	// is no colour
	CCAttrMap* pAttrMapStart = pBlendParam->GetStartAttrMap();
	CCAttrMap* pAttrMapEnd = pBlendParam->GetEndAttrMap();

	CCRuntimeClass	   *pType;
	void			   *pVal = NULL;

	// We're looking for Stroke Colour Attributes
	pType = CC_RUNTIME_CLASS(AttrStrokeColour);

	// Lookup the Start Line Colour
	pAttrMapStart->Lookup( pType, pVal );
	if (pVal == NULL)
		return FALSE;

	AttrStrokeColour * pStartColour = (AttrStrokeColour *)pVal;
	StrokeColourAttribute * pColourAttr = &(pStartColour->Value);

//#pragma message( __LOCMSG__ "Removed DocColour usage" )
	BOOL StartIsNoColour = pColourAttr->Colour.IsTransparent();

	pVal = NULL;

	// Lookup the End Line Colour
	pAttrMapEnd->Lookup(pType,pVal);
	if (pVal == NULL)
		return FALSE;

	AttrStrokeColour * pEndColour = (AttrStrokeColour *)pVal;
	pColourAttr = &(pEndColour->Value);

//#pragma message( __LOCMSG__ "Removed DocColour usage" )
	BOOL EndIsNoColour = pColourAttr->Colour.IsTransparent();

	// Blend the Start and End Transparencies
	UINT32 BlendTransp, BlendStartTransp, BlendEndTransp;

	// If either end if 'No Colour' then treat it as 100% transparent
	BlendStartTransp 	= (StartIsNoColour && !EndIsNoColour) ? 255 : *GetStartTransp();
	BlendEndTransp  	= (EndIsNoColour && !StartIsNoColour) ? 255 : *OtherFill->GetStartTransp();

	if (BlendFillTransp(&BlendStartTransp, &BlendEndTransp, &BlendTransp, Ratio, pBlendParam))
		pNewAttr->SetStartTransp(&BlendTransp);

	// Set the new fill as the blended attribute
	pBlendParam->SetBlendedAttrVal(pNewAttr);
	
	return TRUE;
}

/********************************************************************************************

>	BOOL FillGeometryAttribute::BlendFillColours(DocColour* Start, DocColour* End, DocColour* Blend,
 								  				 double& Ratio, BlendAttrParam* pBlendParam)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Returns:	TRUE - Blend went ok, the blended colour is valid; FALSE if not.
	Purpose:	Blends between two fill colours.
	SeeAlso:	FillGeometryAttribute::Blend;
				FillGeometryAttribute::BlendFillTransp;
				FillGeometryAttribute::BlendControlPoints;
				FlatFillAttribute::Blend;
				GradFillAttribute::Blend

********************************************************************************************/

BOOL FillGeometryAttribute::BlendFillColours(DocColour* Start, DocColour* End, DocColour* Blend,
								  				double& Ratio, BlendAttrParam* pBlendParam)
{
//#pragma message( __LOCMSG__ "FillGeometryAttribute::BlendFillColours - do nothing" )
//	TRACE( _T("Warning - FillGeometryAttribute::BlendFillColours called\n") );
	ERROR3IF(Blend == NULL || pBlendParam == NULL, "Illegal NULL param");

	if (Start == NULL)
		return FALSE;

	if (End == NULL)
	{
		*Blend = *Start;
		return TRUE;
	}

	ColourContext *BlendContext;
	ColourContext *OutputContext = NULL;

	// Find the ouptut ColourContext, if there is one available to us
	// This allows the blend to take advantage of the output context to do the best 
	// job it can of colour-separating tints
	if (pBlendParam->GetRenderRegion() != NULL)
	{
		ERROR3IF(pBlendParam->GetRenderRegion()->GetRenderView() == NULL, "No render view");
		if (pBlendParam->GetRenderRegion()->GetRenderView() != NULL)
			OutputContext = pBlendParam->GetRenderRegion()->GetRenderView()->GetColourContext(COLOURMODEL_RGBT);
	}

	switch (pBlendParam->GetColourBlend())
	{
		case COLOURBLEND_FADE:
			BlendContext = ColourManager::GetColourContext(COLOURMODEL_RGBT);
			Blend->Mix(Start, End, Ratio, BlendContext, FALSE, OutputContext);
			break;

		case COLOURBLEND_RAINBOW:
			BlendContext = ColourManager::GetColourContext(COLOURMODEL_HSVT);
			Blend->Mix(Start, End, Ratio, BlendContext, FALSE, OutputContext);
			break;

		case COLOURBLEND_ALTRAINBOW:
			BlendContext = ColourManager::GetColourContext(COLOURMODEL_HSVT);
			Blend->Mix(Start, End, Ratio, BlendContext, TRUE, OutputContext);
			break;

		default:
			ERROR3("Unknown colour blend type");
			return(FALSE);
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL FillGeometryAttribute::BlendFillTransp(UINT32* Start, UINT32* End, UINT32* Blend,
								 				double& Ratio, BlendAttrParam* pBlendParam)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Returns:	TRUE - Blend went ok, the blended transparency is valid; FALSE if not.
	Purpose:	Blends between two fill transparencies.
	SeeAlso:	FillGeometryAttribute::Blend;
				FillGeometryAttribute::BlendFillColours;
				FillGeometryAttribute::BlendControlPoints;
				FlatFillAttribute::Blend;
				GradFillAttribute::Blend

********************************************************************************************/

BOOL FillGeometryAttribute::BlendFillTransp(UINT32* Start, UINT32* End, UINT32* Blend,
								 			double& Ratio, BlendAttrParam* pBlendParam)
{
	if (Start == NULL)
		return FALSE;

	if (End == NULL)
	{
		*Blend = *Start;
		return TRUE;
	}

	INT32 dt = *End - *Start;
	*Blend = UINT32(double(*Start) + (double(dt) * Ratio));
	
	return TRUE;
}

/********************************************************************************************

>	BOOL FillGeometryAttribute::BlendControlPoints(DocCoord* Start, DocCoord* End, DocCoord* Blend,
								    				double& Ratio, BlendAttrParam* pBlendParam)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Returns:	TRUE - Blend went ok, the blended point is valid; FALSE if not.
	Purpose:	Blends between two fill control point.
	SeeAlso:	FillGeometryAttribute::Blend;
				FillGeometryAttribute::BlendFillColours;
				FillGeometryAttribute::BlendFillTransp;
				FlatFillAttribute::Blend;
				GradFillAttribute::Blend

********************************************************************************************/

BOOL FillGeometryAttribute::BlendControlPoints(DocCoord* Start, DocCoord* End, DocCoord* Blend,
								    			double& Ratio, BlendAttrParam* pBlendParam, BOOL swapOrder)
{
	if (Start == NULL)
	{	
		return FALSE;
	}

	if (End == NULL)
	{
		NodeRenderableBounded* pStartNode = NULL;
		NodeRenderableBounded* pEndNode = NULL;

		double origRatio = Ratio;

		if (!swapOrder)
		{
			if ((NodeRenderableBounded*) pBlendParam->GetStartAttrMap ())
			{
				pStartNode = (NodeRenderableBounded*) pBlendParam->GetStartAttrMap ()->attrMapCreator;
			}

			if ((NodeRenderableBounded*) pBlendParam->GetEndAttrMap ())
			{
				pEndNode = (NodeRenderableBounded*) pBlendParam->GetEndAttrMap ()->attrMapCreator;
			}
		}
		else
		{
			Ratio = 1 - Ratio;
			
			if ((NodeRenderableBounded*) pBlendParam->GetEndAttrMap ())
			{
				pStartNode = (NodeRenderableBounded*) pBlendParam->GetEndAttrMap ()->attrMapCreator;
			}
 
			if ((NodeRenderableBounded*) pBlendParam->GetStartAttrMap ())
			{
				pEndNode = (NodeRenderableBounded*) pBlendParam->GetStartAttrMap ()->attrMapCreator;
			}
		}

//		ERROR3IF (((pStartNode == NULL) && (pEndNode == NULL)), "Somethings wrong!");	// this should NOT be so
		
		if (!pEndNode)
		{
			*Blend = *Start;
			if (swapOrder)
			{
				Ratio = origRatio;
			}
			return TRUE;
		}
		else
		{
			// we are blending from something to nothing
			// previously will just made *Blend equal to *start and returned TRUE.
			// this causes problems when blending (e.g.) a linear transparency to a flat fill
			// (i.e.  all intermediate blend steps have EXACTLY the same fill geometry as the start)
			// this has now been changed.  In a case such as this, we create a temporary estimated 'fill
			// handle' point - and blend to that ....  This solution is NOT rotation independant (owing to
			// limitations of bounding boxs, but at least what we render on-screen is of a higher quality.

			if (pStartNode)
			{
				DocRect rectStart (pStartNode->GetBoundingRect (TRUE));
				DocRect rectEnd (pEndNode->GetBoundingRect (TRUE));

				INT32 dx = static_cast<INT32> ( (rectEnd.Centre ().x - rectStart.Centre ().x) * Ratio );
				INT32 dy = static_cast<INT32> ( (rectEnd.Centre ().y - rectStart.Centre ().y) * Ratio );
		
				*Blend = DocCoord((*Start).x + dx, (*Start).y + dy);

				if (swapOrder)
				{
					Ratio = origRatio;
				}

				return TRUE;
			}
			else
			{
				if (swapOrder)
				{
					Ratio = origRatio;
				}
				return (FALSE);
			}
		}
	}

	// process as normal ....

	INT32 dx = INT32(double((*End).x - (*Start).x) * Ratio);
	INT32 dy = INT32(double((*End).y - (*Start).y) * Ratio);
	
	*Blend = DocCoord((*Start).x + dx, (*Start).y + dy);

	return TRUE;
}


/********************************************************************************************

>	BOOL FillGeometryAttribute::CheckForGreyscaleBitmapBlend(KernelBitmap* pBitmap, 
															 DocColour* StartCol, DocColour* EndCol)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/1/95
	Returns:	TRUE if colours where set.
	Purpose:	Checks for a blend using a Greyscale bitmap, and set the Start and End Cols
				provided to suitable default Black and White colours.

********************************************************************************************/

BOOL FillGeometryAttribute::CheckForGreyscaleBitmapBlend(KernelBitmap* pBitmap, 
														 DocColour* StartCol, DocColour* EndCol)
{
	PORTNOTETRACE("other","FillGeometryAttribute::CheckForGreyscaleBitmapBlend - do nothing");
#ifndef EXCLUDE_FROM_XARALX
#ifndef WEBSTER
	if (pBitmap != NULL)
	{
		if (BfxALU::IsGreyscaleBitmap(pBitmap))	// WEBSTER-Martin-14/01/97 we prob do need this
		{
			// The bitmap is Greyscale, with NULL start and end colours,
			// so we will use Black and White colours in the blend
		
			AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
												_R(IDS_BLACKNAME), StartCol);

			AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
												_R(IDS_WHITENAME), EndCol);

			return TRUE;
		}
	}
#endif //WEBSTER */
#endif
	return FALSE;
}
