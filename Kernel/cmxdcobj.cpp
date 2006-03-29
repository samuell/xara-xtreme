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
// CMXExportDC object output routines

#include "camtypes.h"
#include "cmxexdc.h"
#include "cmxrendr.h"
#include "cmxform.h"
#include "docrect.h"
#include "attrmgr.h"
#include "fillval.h"
#include "colcontx.h"
//#include "nev.h"
//#include "cmxres.h"
#include "fillramp.h"


/********************************************************************************************

>	BOOL CMXExportDC::WriteBlankAttributes(INT32 Tag)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/07/96
	Inputs:		render region
	Returns:	success
	Purpose:	writes a blank attribute tag and contents, which specifies nothing but
				keeps everything happy

********************************************************************************************/

BOOL CMXExportDC::WriteBlankAttributes(INT32 Tag)
{
	// start the tag
	if(Tag != -1)
		if(!StartTag(Tag))
			return FALSE;

	// write the attribute mask thingy
	WriteByte(0);

	// end the tag
	if(Tag != -1)
		if(!EndTag())
			return FALSE;
	
	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::WriteAttributes(CMXRenderRegion *pReg, INT32 Tag, DocCoord *Coords, INT32 NumCoords, BOOL ForceNoFill = FALSE)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/07/96
	Inputs:		render region
	Returns:	success
	Purpose:	Writes the attributes to the DC.
				if ForceNoFill == TRUE, the path will not be filled whatever the colour

********************************************************************************************/

BOOL CMXExportDC::WriteAttributes(CMXRenderRegion *pReg, INT32 Tag, DocCoord *Coords, INT32 NumCoords, BOOL ForceNoFill)
{
	// give lenses a chance
	BOOL WasLens = FALSE;
	if(!WriteAttrCheckLens(pReg, Tag, Coords, NumCoords, &WasLens))
		return FALSE;

	// now did the lens checking bit output something?
	if(WasLens)
		return TRUE;		// yep, so we don't want to output them too

	// start the tag
	if(Tag != -1)
		if(!StartTag(Tag))
			return FALSE;

	// write the attribute mask thingy
	WriteByte((ForceNoFill?0:cmxRENDATTRMASK_FILL) | cmxRENDATTRMASK_OUTLINE);

	// write the fill specification
	if(!ForceNoFill)
		if(!WriteFillSpec(pReg, Coords, NumCoords, ForceNoFill))
			return FALSE;

	// write the outline specification
	if(!WriteOutlineSpec(pReg))
		return FALSE;

	// end the tag
	if(Tag != -1)
		if(!EndTag())
			return FALSE;
	
	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::WriteFillType(WORD FillID)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/07/96
	Inputs:		fill type
	Returns:	success
	Purpose:	Writes the fill type to the DC

********************************************************************************************/

BOOL CMXExportDC::WriteFillType(WORD FillID)
{
	ExportFile->write(&FillID, sizeof(FillID));

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::WriteFillSpec(CMXRenderRegion *pReg)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/07/96
	Inputs:		render region
	Returns:	success
	Purpose:	Writes the fill specifcation to the DC

********************************************************************************************/

BOOL CMXExportDC::WriteFillSpec(CMXRenderRegion *pReg, DocCoord *Coords, INT32 NumCoords, BOOL NoFill)
{
	// tag the start
	if(!StartNestedTag(cmxTAG_RenderAttr_FillSpec))
		return FALSE;

	if(NoFill)
	{
		if(!WriteFillType(cmxFILLID_NONE))
			return FALSE;
	}
	else
	{
		// get the attribute
		FillGeometryAttribute *pFillGeometry
				= (FillGeometryAttribute *)pReg->GetCurrentAttribute(ATTR_FILLGEOMETRY);

		// check it out
		ERROR2IF(pFillGeometry == 0, FALSE, "No fill geometry");
		ERROR3IF(!pFillGeometry->IsKindOf(CC_RUNTIME_CLASS(FillGeometryAttribute)), "not one of them there fill geometries");

		// right then, decide what to do with the thingy
		if(pFillGeometry->IsAFlatFill())
		{
			// flat fill -- check to see if it's transparent
			DocColour *pCol = pFillGeometry->GetStartColour();
			ERROR2IF(pCol == NULL, FALSE, "not one of your earth colours");
			if(pCol->IsTransparent())
			{
				// it's transparent, write no fill
				if(!WriteFillType(cmxFILLID_NONE))
					return FALSE;
			}
			else
			{
				// it's OK, it is. Write it.
				if(!WriteFillType(cmxFILLID_UNIFORM)
					|| !WriteFillSpecFlat(pReg, pFillGeometry))
					return FALSE;
			}
		}
		else if(pFillGeometry->IsAKindOfBitmapFill())
		{
			// bitmap type fill -- do nothing for now
			if(!WriteFillType(9) //cmxFILLID_COLPATTERN)
				|| !WriteFillSpecBitmap(pReg, pFillGeometry, Coords, NumCoords))
				return FALSE;
		}
		else if(pFillGeometry->IsAGradFill())
		{
			// grad fill
			if(!WriteFillType(cmxFILLID_FOUNTAIN)
				|| !WriteFillSpecGrad(pReg, pFillGeometry, Coords, NumCoords))
				return FALSE;
		}
		else
		{
			// catch all -- write a no fill thingy
			if(!WriteFillType(cmxFILLID_NONE))
				return FALSE;
		}
	}

	// end the tag
	if(!EndNestedTag())
		return FALSE;
	
	// write the end tag
	if(!WriteMinEndTag())
		return FALSE;

	return TRUE;
}

/********************************************************************************************

>	BOOL CMXExportDC::WriteFillSpecFlat(CMXRenderRegion *pReg, FillGeometryAttribute *pFill)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/07/96
	Inputs:		render region
	Returns:	success
	Purpose:	Writes the flat fill specifcation to the DC

********************************************************************************************/

BOOL CMXExportDC::WriteFillSpecFlat(CMXRenderRegion *pReg, FillGeometryAttribute *pFill)
{
	// get the colour
	DocColour *pFillColour = pFill->GetStartColour();
	ERROR2IF(pFillColour == NULL, FALSE, "Wibble. No fill colour on this here flat fill");

	// find a reference for it
	WORD FillReference = GetColourReference(pFillColour);

	// write some stuff
	struct {
		WORD FillReference;
		WORD ScreenReference;
	} filldef = {FillReference, cmxSCREENREFERENCE};

	if(!WriteNestedTag(cmxTAG_RenderAttr_FillSpec_Uniform, &filldef, sizeof(filldef))
		|| !WriteMinEndTag())
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::WriteFillSpecGrad ( CMXRenderRegion *pReg,
										  FillGeometryAttribute *pFill,
										  DocCoord *Coords,
										  INT32 NumCoords )

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Inputs:		render region
	Returns:	success
	Purpose:	Writes the grad fill specifcation to the DC

********************************************************************************************/

BOOL CMXExportDC::WriteFillSpecGrad ( CMXRenderRegion *pReg,
									  FillGeometryAttribute *pFill,
									  DocCoord *Coords,
									  INT32 NumCoords )
{
	DocColour	StartColour;				// The first colour in the fill.
	DocColour	EndColour;					// The end colour of the fill.
	DocColour	MidColour;					// Only used to approximate four colour fills.
	BOOL		Radial			= FALSE;	// Is it a radial fill?
	BOOL		FourColour		= FALSE;	// Is it a four colour fill?

	// is this a radial type derivative?
	if ( pFill->IsARadialFill () )
	{
		// We need to swap the colours over as Corel has a different idea
		// of start and finish colours to Camelot.
		StartColour	= *( pFill->GetEndColour () );
		EndColour	= *( pFill->GetStartColour () );

		Radial = TRUE;
	}
	else if ( pFill->IsAConicalFill () || pFill->IsASquareFill () )
	{
		// Set up the colour values.
		StartColour	= *( pFill->GetStartColour () );
		EndColour	= *( pFill->GetEndColour () );

		Radial = TRUE;
	}
	else if ( pFill->IsAFourColFill () )
	{
		// Set up the start and end colours as usual, but create a third colour value to
		// help fake the four colour fill.
		StartColour	= *( pFill->GetStartColour () );
		EndColour	= *( pFill->GetEndColour3 () );
		MidColour.Mix ( pFill->GetEndColour (), pFill->GetEndColour2 (), 0.5f, NULL, FALSE, NULL );

		// Reset the four colour flag.
		FourColour	= TRUE;
	}
	else if ( pFill->IsAThreeColFill () )
	{
		// Set up the colour values. I'm blending the colours of the two endpoints
		// to get a better approximation of a three colour fill.
		StartColour	= *( pFill->GetStartColour () );
		EndColour.Mix ( pFill->GetEndColour (), pFill->GetEndColour2 (), 0.5f, NULL, FALSE, NULL );
	}
	else
	{
		// Default behaviour: Store the start and end colours.
		StartColour	= *( pFill->GetStartColour () );
		EndColour	= *( pFill->GetEndColour () );
	}

	// Find references for the color, and sort out the fill effect.
	WORD	StartColReference	= GetColourReference ( &StartColour );
	WORD	EndColReference		= GetColourReference ( &EndColour );
	WORD	MidColReference		= 0;
	WORD	FillMode			= cmxFILLMODE_RGB;

	// Get a colour reference to MidColour if it's been declared. (i.e. the fill is a
	// four colour fill.)
	if ( FourColour )
	{
		MidColReference = GetColourReference ( &MidColour );
	}

	// get the fill effect attribute
	FillEffectAttribute *pFillEff =
		(FillEffectAttribute *)pReg->GetCurrentAttribute(ATTR_FILLEFFECT);
	ERROR2IF(pFillEff == NULL || !pFillEff->IsKindOf(CC_RUNTIME_CLASS(FillEffectAttribute)), FALSE, "not a fill effect");
	BOOL Rainbow = FALSE, Alt = FALSE;
	if(IS_A(pFillEff, FillEffectRainbowAttribute))
		Rainbow = TRUE;
	if(IS_A(pFillEff, FillEffectAltRainbowAttribute))
		Rainbow = Alt = TRUE;

	if(Rainbow)
	{
		// right, we need to work out which direction we want the colour to go
		// around the HSV colour wheel. Convert both colours to HSV.
		ColourContext *Conv = ColourContext::GetGlobalDefault(COLOURMODEL_HSVT);

		ColourHSVT StartC;
		ColourHSVT EndC;

		Conv->ConvertColour( &StartColour, (ColourGeneric *)&StartC);
		Conv->ConvertColour( &EndColour, (ColourGeneric *)&EndC);

		// compare the hues and work out a difference
		double Difference = (EndC.Hue.MakeDouble()) - (StartC.Hue.MakeDouble());

		BOOL ClockWise = (Difference < 0);

		if(fabs(Difference) >= 0.5)
			ClockWise = !ClockWise;

		if(Alt = TRUE)
			ClockWise = !ClockWise;
		
		// set the correct thingy attribute
		if(ClockWise)
			FillMode = cmxFILLMODE_HSB_CW;
		else
			FillMode = cmxFILLMODE_HSB_CCW;
	}

	// find the start and end coords of the fill
	DocCoord StartPoint = *( pFill->GetStartPoint() );
	DocCoord EndPoint;

	// Graeme (18-2-00) - Modifications to handle three and four colour fills better.
	if ( pFill->IsAFourColFill () )
	{
		// Use EndPoint3 to get a diagonal across the bounding box.
		EndPoint = *( pFill->GetEndPoint3 () );
	}
	else if ( pFill->IsAThreeColFill () )
	{
		// Merge the existing two endpoints to get the correct endpoint for the fill.
		EndPoint = *( pFill->GetEndPoint () ) +
				   *( pFill->GetEndPoint2 () ) -
				   *( pFill->GetStartPoint () );
	}
	else
	{
		// Default: Use the standard endpoint.
		EndPoint = *( pFill->GetEndPoint () );
	}


	// find the corel bounding box of the path we're about to fiddle with
	DocRect cBBox;
	CalcCorelBBox(Coords, NumCoords, &cBBox);

	// run the chicken out clause for extreme cases I can't be bothered to handle
	if ( StartPoint == EndPoint || cBBox.Width () == 0 || cBBox.Height () == 0 )
		return WriteFillSpecFlat(pReg, pFill);

	// calc the central coord
	DocCoord Centre;
	Centre.x = cBBox.lo.x + cBBox.Width() / 2;
	Centre.y = cBBox.lo.y + cBBox.Height() / 2;

	// calc the fill parameters

	// first work out the distance between the start and end points
	double dx = EndPoint.x - StartPoint.x;
	double dy = EndPoint.y - StartPoint.y;
	double PointDist = sqrt(((dx * dx) + (dy * dy)));

	// now we need to work out the angle of the fill
	double Angle = atan2(dy, dx);
		// atan2 is defined if dx == 0
		// but not at the origin -- however we've filtered out this case

	INT32 StartPercent;
	INT32 EndPercent;

	cmxFillBase1 fb1;
	fb1.Padding = 0;
	cmxFillBase2 fb2;
	fb2.XOffset = 0;
	fb2.YOffset = 0;

	if(Radial)
	{
		//------------------------------------------------- radial type fill calucations

		// ...and for my first trick, I will calculate the X and Y
		// offsets of the centre of this fill
		DocCoord dCentre = StartPoint - Centre;
		fb2.XOffset = (dCentre.x * 100) / cBBox.Width();
		fb2.YOffset = (dCentre.y * 100) / cBBox.Height();
		// get these values within the required range
		if(fb2.XOffset > 100) fb2.XOffset = 100;
		if(fb2.XOffset < -100) fb2.XOffset = -100;
		if(fb2.YOffset > 100) fb2.YOffset = 100;
		if(fb2.YOffset < -100) fb2.YOffset = -100;

		// now this is the scary bit. Corel use a complex method for specifing
		// their radial type fills. 
		double w = cBBox.Width(), h = cBBox.Height();
		double bboxdiagonal = sqrt(w*w + h*h);
		double cdx = dCentre.x, cdy = dCentre.y;
		double cendist = sqrt(cdx*cdx + cdy*cdy);
		double BodgeFactor = 1 + (cendist / (bboxdiagonal / 2));
		double targetradius = PointDist / BodgeFactor;
		double Pad = 50 * (1 - (2 * targetradius) / bboxdiagonal);
		// munge the Pad value to the allowed range
		if(Pad < 0) Pad = 0;
		if(Pad > 44) Pad = 44;
		// and bung in the structure for export
		fb1.Padding = (WORD)Pad;

		// set the percentages to 0
		StartPercent = 0;
		EndPercent = 0;

		// if it's a square fill, swap the colours over
		if(pFill->IsASquareFill())
		{
			WORD t = StartColReference;
			StartColReference = EndColReference;
			EndColReference = t;
		}
	} // if ( Radial )

	else // if ( !Radial )
	{
		//------------------------------------------------- linear type fill calucations

		// now work out the coords of the intersection of a line through the
		// centre of the bbox and the edges of the bbox

		// right then, we want the angle within 0 and PI/2
		double cAngle = Angle;
		INT32 cAR = 0;
		while(cAngle > (PI/2))
		{
			cAngle -= PI/2;
			cAR++;
		}

		// luckily, everything is symmetrical so all we need do is calc the
		// intersect in the top quadrant.
		INT32 qdx = cBBox.Width() / 2;
		INT32 qdy = cBBox.Height() / 2;
		if((cAR & 1) != 0)
		{
			// we rotated by an odd multiple of PI/2 radians. Rotate the box as well.
			INT32 t = qdy;
			qdy = qdx;
			qdx = t;
		}
		DocCoord i;
		if(qdy == 0)
		{
			// special case to avoid division by tan(0) = 0
			i.x = qdx;
			i.y = 0;
		}
		else
		{
			double predqdy = qdx * tan(cAngle);
			if(predqdy > qdy)
			{
				// intersects on the horizontal
				i.x = (INT32)(((double)qdy) / tan(cAngle));
				i.y = qdy;
			}
			else
			{
				// intersects on the vertical
				i.x = qdx;
				i.y = (INT32)(((double)qdx) * tan(cAngle));
			}
		}

		// i is the offset from the centre of the bbox where the line of the
		// grad fill rotated to the top right quadrent intersects with the
		// rectange of the bounding box.

		// now we need to work out the percentages along this line of the start
		// and stop bit.
		INT32 linelength = (INT32)sqrt(((double)(i.x) * (double)(i.x)) + ((double)(i.y) * (double)(i.y)));
		// chicken out clause #2
		if(linelength == 0)
			return WriteFillSpecFlat(pReg, pFill);

		// work out the start and end points as offsets from the centre point
		DocCoord	dStartPoint	= StartPoint - Centre;
		DocCoord	dEndPoint	= EndPoint - Centre;

		// rotate these points around the origin
		// TODO: do the rotation myself, instead of going through matrices
		Matrix Mat(((- Angle) * 360.0) / (2*PI));	// why can't people use proper measure? 
		Mat.transform(&dStartPoint);
		Mat.transform(&dEndPoint);

		// find out the smallest distance from the end of the line
		INT32 dStart = dStartPoint.x + linelength;
		INT32 dEnd = linelength - dEndPoint.x;
		if(dStart < 0) dStart = 0;
		if(dEnd < 0) dEnd = 0;
		INT32 dPad;		// smallest of dStart and dEnd
		if(dStart < dEnd)
			dPad = dStart;
		else
			dPad = dEnd;

		INT32 padval = (dPad * 100) / (linelength * 2);
		if(padval < 0) padval = 0;
		if(padval > 44) padval = 44;
		fb1.Padding = (WORD)padval;
		
		// right, now munge the line length value to take account of this padding
		linelength -= dPad;

		// right then, we can now simply compare the x coords of the start and end
		// with the linelength above to find the start and end percentages
		StartPercent = (INT32)((dStartPoint.x + linelength) * 100);
		EndPercent = (INT32)((dEndPoint.x + linelength) * 100);
		StartPercent /= (linelength * 2);
		EndPercent /= (linelength * 2);

		// sanity check the numbers so Corel is happy
		if(StartPercent < 0) StartPercent = 0;
		if(StartPercent > 100) StartPercent = 100;
		if(EndPercent < 0) EndPercent = 0;
		if(EndPercent > 100) EndPercent = 100;
		if(EndPercent < StartPercent)
		{
			INT32 t = StartPercent;
			StartPercent = EndPercent;
			EndPercent = t;
		}
	} // else if ( !Radial )

	// I allege we now know enough to get this thing to the file
	// start the tag
	if(!StartNestedTag(cmxTAG_RenderAttr_FillSpec_Fountain_Base))
		return FALSE;

	if(pFill->IsARadialFill())
		fb1.Type = cmxFOUNTAINTYPE_RADIAL;
	else if(pFill->IsASquareFill())
		fb1.Type = cmxFOUNTAINTYPE_SQUARE;
	else if(pFill->IsAConicalFill())
		fb1.Type = cmxFOUNTAINTYPE_CONICAL;
	else
		fb1.Type = cmxFOUNTAINTYPE_LINEAR;
	fb1.Screen = cmxSCREENREFERENCE;
	// padding set earlier

	// offsets set earlier
	fb2.StepCount = 0;
	fb2.FillMode = FillMode;
	fb2.RateMethod = 0;
	fb2.RateValue = 50;			// 50% is the centre, a commonly held belief

	if(!Radial)
	{
		if(FillMode == cmxFILLMODE_RGB)
		{
			// if it's an RGB mix in a linear fill, we can accurately
			// specify the fill by using a custom fill mode, and specifing
			// the end points presicely.
			fb2.FillMode = cmxFILLMODE_CUSTOM;
		}
		else
		{
			// if it's not an RGB mix, we need to approximate the fill using the
			// padding (set already) as we can't specify where we want the
			// colours to start and finish along the line of the fill
			StartPercent = 0;
			EndPercent = 0;
		}
	}

	// write the data
	ExportFile->write(&fb1, sizeof(fb1));

	WriteAngle(Angle);

	if(ThirtyTwoBit)
	{
		ExportFile->write(&fb2, sizeof(fb2));
	}
	else
	{
		// OK, so this way is a bit of a bodge
		cmxFillBase2_16 b = {0, (SWORD)fb2.XOffset, (SWORD)fb2.YOffset,
			fb2.StepCount, fb2.FillMode};

		ExportFile->write(&b, sizeof(b));
	}

	// end the tag
	if(!EndNestedTag())
		return FALSE;

	// write the colours
	if(!StartNestedTag(cmxTAG_RenderAttr_FillSpec_Fountain_Color))
		return FALSE;

	cmxGradFillColour	col;
	ColourRamp			*pRamp	= pFill->GetColourRamp ();
	WORD				NCol	= 2 + ( ( StartPercent != 0 ) ? 1 : 0 ) +
								  ( ( EndPercent != 100 ) ? 1 : 0 ) + ( FourColour ? 1 : 0 );

	// If the fill has a colour ramp, calculate the number of entries within it.
	if ( pRamp != NULL )
	{
		UINT32	FirstIndex	= 0;
		UINT32	LastIndex	= 0;

		// Call a member function to get the indices to the first and last elements within
		// the colour ramp.
		pRamp->GetIndexRange ( &FirstIndex, &LastIndex );

		// Add the difference between these to the number of colours to be exported. The +1
		// in this sum is necessary since the index values include the number of the element
		// that they represent. E.g. for a one element ramp, we have 1024 for both the first
		// and last items. Thus, without the +1, we would get 0 colours in the index.
		NCol += 1 + LastIndex - FirstIndex;
	}

	// Write the number of colours.
	ExportFile->write ( &NCol, sizeof ( NCol ) );

	// Write as many colours as necessary to get accurate fills. First the start colour.
	if(StartPercent != 0)
	{
		col.ColRef = StartColReference;
		col.ColPos = 0;
		ExportFile->write(&col, sizeof(col));
	}

	col.ColRef = StartColReference;
	col.ColPos = (WORD)StartPercent;
	ExportFile->write(&col, sizeof(col));

	// If it's a four colour fill, add the middle colour to the mix.
	if ( FourColour )
	{
		col.ColRef	= MidColReference;
		col.ColPos	= ( WORD ) ( ( StartPercent + EndPercent ) / 2 );
		ExportFile->write ( &col, sizeof ( col ) );
	}

	// Process the colour ramp.
	if ( pRamp != NULL )
	{
		ColRampItem *pColour = pRamp->GetFirstCol ();

		// Write in the ramp's colour values.
		while ( pColour != NULL )
		{
			cmxGradFillColour	RampColour;

			// Get the position of the colour in the fill.
			RampColour.ColPos	= ( WORD ) ( 100.0f * pColour->GetPosition () );
			RampColour.ColRef	= GetColourReference ( &( pColour->GetColour () ) );

			// Write the colour.
			ExportFile->write ( &RampColour, sizeof ( RampColour ) );

			// Increment the pointer onto the next item.
			pColour = pRamp->GetNextCol ( pColour );
		}
	}

	// And finally the end colour.
	col.ColRef = EndColReference;
	col.ColPos = (WORD)EndPercent;
	ExportFile->write(&col, sizeof(col));

	if(EndPercent != 100)
	{
		col.ColPos = 100;
		ExportFile->write(&col, sizeof(col));
	}

	if(!EndNestedTag())
		return FALSE;

	// write the ending bit
	if(!WriteMinEndTag())
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::WriteFillSpecBitmap(CMXRenderRegion *pReg, FillGeometryAttribute *pFill, DocCoord *Coords, INT32 NumCoords)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		render region
	Returns:	success
	Purpose:	Writes the bitmap fill specifcation to the DC (also does fractals)

********************************************************************************************/

BOOL CMXExportDC::WriteFillSpecBitmap(CMXRenderRegion *pReg, FillGeometryAttribute *pFill, DocCoord *Coords, INT32 NumCoords)
{
	// make up a bitmap fill object -- first find the refer bitmap object necessary
	BitmapFillAttribute *pBFillAttr = (BitmapFillAttribute *)pFill;
	ERROR3IF(!pBFillAttr->IsKindOf(CC_RUNTIME_CLASS(BitmapFillAttribute)), "not a bitmap fill attr");

	// colours
	DocColour *pStartColour = 0;
	DocColour *pEndColour = 0;
	EFFECTTYPE Effect = EFFECT_RGB;

	// get the colours we should use, and the fill effect
	pStartColour = pFill->GetStartColour();
	pEndColour = pFill->GetEndColour();
	if(pStartColour == 0 || pEndColour == 0
		|| pStartColour->IsTransparent()
		|| pEndColour->IsTransparent())
	{
		pStartColour = 0;
		pEndColour = 0;
	}
	
	if(pStartColour != 0)
	{
		// get the fill effect
		Effect = pReg->GetFillEffect();
	}

	// get the reference for the bitmap
	KernelBitmap *pBitmap = pBFillAttr->BitmapRef.GetBitmap();
	WORD BitmapRef = GetBitmapReference(pBitmap, pStartColour, pEndColour, Effect);

	// make a new bitmap fill object
	CMXReferBitmapFill *pBFill = new CMXReferBitmapFill(this);
	if(pBFill == 0)
		return FALSE;
	pBFill->Set(BitmapRef);

	// add it to the refer list
	ReferList.AddTail(pBFill);

	// get it's reference
	WORD BitmapFillProcReference = GetProcedureReference(pBFill);

	// ready to write...
	if(!StartNestedTag(cmxTAG_RenderAttr_FillSpec_ColorBM))
		return FALSE;
	
	ExportFile->write(&BitmapFillProcReference, sizeof(BitmapFillProcReference));

	// write tiling
	if(!StartNestedTag(cmxTAG_Tiling))
		return FALSE;

	// get points from the attribute
	DocCoord *Start = pFill->GetStartPoint();
	DocCoord *End1 = pFill->GetEndPoint();
	DocCoord *End2 = pFill->GetEndPoint2();

	// get a tiling structure
	cmxTilingEnd tile;

	// Variables for rendering the texture to the file. I've deferred the scaling of
	// the image until it's written. It's slightly ugly doing it this way, but it
	// removes a few multipications, and so should get rid of some rounding errors
	// along the way.
	DocRect	cBBox;
	double	TileWidth	= Start->Distance ( *End1 );
	double	TileHeight	= Start->Distance ( *End2 );

	// Calculate the untransformed Corel bounding box.
	CalcCorelBBox(Coords, NumCoords, &cBBox);

	// Graeme (9-5-00) - I've simplified Ben's code a little, and removed the
	// repeated bits in both branches of the if statement.
	if ( ThirtyTwoBit )
	{
		cmxTilingBegin32 Tile32;
		Tile32.Width	= static_cast<DWORD> ( Round ( TileWidth  * ScaleFactor ) );
		Tile32.Height	= static_cast<DWORD> ( Round ( TileHeight * ScaleFactor ) );

		WriteData ( &Tile32, sizeof ( Tile32 ) );
	}
	else
	{
		cmxTilingBegin16 Tile16;
		Tile16.Width	= static_cast<WORD> ( Round ( TileWidth  * ScaleFactor ) );
		Tile16.Height	= static_cast<WORD> ( Round ( TileHeight * ScaleFactor ) );

		WriteData ( &Tile16, sizeof ( Tile16 ) );
	}

	// now work out the offsets to the start of the tile
	double	dX	= static_cast<double> ( Start->x - cBBox.lo.x );
	double	dY	= static_cast<double> ( cBBox.hi.y - Start->y );

	// Graeme (9-5-00) - Round the offset up.
	tile.XOffset = ( static_cast<WORD> ( Round ( ( dX * 100.0 ) / TileWidth  ) ) ) % 100;
	tile.YOffset = ( static_cast<WORD> ( Round ( ( dY * 100.0 ) / TileHeight ) ) ) % 100;

	tile.InterTileOffset = tile.TilingFlags = 0;

	// write the structure to the file
	ExportFile->write(&tile, sizeof(tile));
	if(!EndNestedTag() || !WriteMinEndTag())
		return FALSE;

	// bounding box of bit to use
	DocRect bbox = DocRect(0, 0, cmxBITMAPFILLTILESIZE_X, cmxBITMAPFILLTILESIZE_Y);

	if(!WriteBBox(&bbox, FALSE))
		return FALSE;

	if(!EndNestedTag() || !WriteMinEndTag())
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::WritePath ( DocCoord *Coords,
								  PathVerb *Verbs,
								  INT32 NumCoords,
								  BOOL Filled )

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/07/96
	Inputs:		render region, coords, verbs and number of coords
	Returns:	success
	Purpose:	Writes a path instruction -- transformed by matrix

********************************************************************************************/

BOOL CMXExportDC::WritePath ( DocCoord *Coords,
							  PathVerb *Verbs,
							  INT32 NumCoords,
							  BOOL Filled )
{
	// if this is one of those nasty 16 bit files, sort out a transform matrix

	// scan the verbs to see if there's a close path in it
	// as if there isn't, we need to force no fill on the thingy
	BOOL ForceNoFill = TRUE;
	for(INT32 p = 0; p < NumCoords; p++)
	{
		if((Verbs[p] & PT_CLOSEFIGURE) != 0)
			ForceNoFill = FALSE;
	}
	if(!Filled)
		ForceNoFill = TRUE;

	// begin the command
	if(!StartCommand(cmxINSTR_PolyCurve))
		return FALSE;

	// output the rendering attributes
	if(!WriteAttributes(pRenderRegion, cmxTAG_PolyCurve_RenderingAttr, Coords, NumCoords, ForceNoFill))
		return FALSE;

	// write the point list, transformed of course. Start by tagging it
	if(!StartTag(cmxTAG_PolyCurve_PointList))
		return FALSE;

	// we have to collect a bounding box as we go through as Corel use a
	// naff bbox which is simply the union of all points
	DocRect bb;				// which is written as a corel one
	DocCoord fc = Coords[0];
	pMatrix->transform(&fc);
	bb.lo = fc;				// start with sensible coords to begin with
	bb.hi = fc;

	// write the point count
	WORD Count = (WORD)NumCoords;
	ExportFile->write(&Count, sizeof(Count));

	// run through the coords writing and transforming
	INT32 l;
TRACEUSER( "Ben", _T("\nPath\n"));
	for(l = 0; l < NumCoords; l++)
	{
		DocCoord Coord = Coords[l];
		pMatrix->transform(&Coord);

		// write it...
		if(ThirtyTwoBit)
		{
			cmxPoint32 p = {Coord.x, Coord.y};
			ExportFile->write(&p, sizeof(p));
		}
		else
		{
TRACEUSER( "Ben", _T("Coord %d %d\n"), Coord.x, Coord.y);
			cmxPoint16 p = {(SWORD)Coord.x, (SWORD)Coord.y};
			ExportFile->write(&p, sizeof(p));
		}

		// flibble the bounding box with those lovely new coords
		if(Coord.x < bb.lo.x) bb.lo.x = Coord.x;
		if(Coord.y < bb.lo.y) bb.lo.y = Coord.y;
		if(Coord.x > bb.hi.x) bb.hi.x = Coord.x;
		if(Coord.y > bb.hi.y) bb.hi.y = Coord.y;
	}

	// run through the verbs writing point types
	INT32 BezCount = 0;		// for counting Bez
	for(l = 0; l < NumCoords; l++)
	{
		BYTE Type;

		switch(Verbs[l] & (PT_LINETO | PT_MOVETO | PT_BEZIERTO))
		{
		case PT_MOVETO:
			{
				Type = CMXNODETYPE_TYPE_MOVE | CMXNODETYPE_USER;
			
				// wibble forwards through the verbs to see if the correspondingness is closed
				for(INT32 lp = l + 1; lp < NumCoords; lp++)
				{
					// OK, is this a move?
					if((Verbs[lp] & (PT_LINETO | PT_MOVETO | PT_BEZIERTO)) == PT_MOVETO)
						break;

					// is closefigure set?
					if((Verbs[lp] & PT_CLOSEFIGURE) != 0)
					{
						Type |= CMXNODETYPE_CLOSED;
						break;
					}
				}
			}
			BezCount = 0;
			break;

		case PT_LINETO:
			Type = CMXNODETYPE_TYPE_LINE | CMXNODETYPE_USER;
			BezCount = 0;
			break;

		case PT_BEZIERTO:
			// in CMX, the control points have type ARC, and the end points
			// have type CURVE. This code does the trickery.
			if(BezCount == 2)
			{
				Type = CMXNODETYPE_TYPE_CURVE | CMXNODETYPE_USER;
			}
			else
			{
				Type = CMXNODETYPE_TYPE_ARC;
			}
			BezCount++;
			if(BezCount > 2)
				BezCount = 0;		// so that only the end points of beziers get user flag
			break;

		default:
			ERROR3("Unknown node type");
			break;
		}

		if((Verbs[l] & PT_CLOSEFIGURE) != 0)
		{
			Type |= CMXNODETYPE_CLOSED;
		}

		ExportFile->write(&Type, sizeof(Type));
	}

	// end the point list
	if(!EndTag())
		return FALSE;

	// write bbox we've collected
	if(!StartTag(cmxTAG_PolyCurve_BoundingBox)
		|| !WriteBBox(&bb, FALSE)
		|| !EndTag())
		return FALSE;

	// end the command
	if(!WriteMinEndTag()
		|| !EndCommand())
		return FALSE;

	// if necessary, get rid of the horrid transform matrix

	return TRUE;
}


/********************************************************************************************

>	WORD CMXExportDC::GetColourReference(DocColour *pTheLovelyColour)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/07/96
	Inputs:		colour
	Returns:	refernce number of a colour
	Purpose:	to find a reference number for the colour when writing a CMX file

********************************************************************************************/

WORD CMXExportDC::GetColourReference(DocColour *pTheLovelyColour)
{
	// algorithm:
	// scan the refer list to see if the colour is already there.
	// if it is, return it's index.
	// if it isn't add the colour to the refer list, and return the new index.

	// the reference we need -- references count up from 1
	INT32 Ref = 0;

	// scan that list!
	CMXReferListItem *pEn = (CMXReferListItem *)ReferList.GetHead();
	while(pEn != 0)
	{
		ERROR3IF(!pEn->IsKindOf(CC_RUNTIME_CLASS(CMXReferListItem)), "unexpected type of entry in refer list");

		if(pEn->IsInWhichDesc() == cmxDESC_COLOUR)
		{
			Ref++;

			CMXReferColour *pLC = (CMXReferColour *)pEn;
			ERROR3IF(!pLC->IsKindOf(CC_RUNTIME_CLASS(CMXReferColour)), "not a refer colour, when it said it was");
			
			// is it this colour?
			if(pLC->AreYouThisColour(pTheLovelyColour))
			{
				// yep. return the reference number
				return Ref;
			}
		}

		pEn = (CMXReferListItem *)ReferList.GetNext(pEn);
	}

	// we haven't got one... make a new one, set it up and add it to the list
	CMXReferColour *pNewRefCol = new CMXReferColour(this);
	if(pNewRefCol == 0)
		return 0;

	pNewRefCol->SetColour(pTheLovelyColour);

	ReferList.AddTail(pNewRefCol);

	// return the next reference number, Ref is the ref of the last one in the list
	return (WORD)Ref + 1;
}


/********************************************************************************************

>	WORD CMXExportDC::GetArrowReference(ArrowRec *pArrow)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/07/96
	Inputs:		colour
	Returns:	reference number of a colour
	Purpose:	to find a reference number for the arrow when writing a CMX file
				will return 0 (ie no arrowhead) if it isn't one

********************************************************************************************/

WORD CMXExportDC::GetArrowReference(ArrowRec *pArrow)
{
	// check to see if the arrowrec is in fact an arrowhead
	if(pArrow->IsNullArrow())
		return 0;		// ie no arrow on here mate

	// the reference we need -- references count up from 1
	INT32 Ref = 0;

	// scan that list!
	CMXReferListItem *pEn = (CMXReferListItem *)ReferList.GetHead();
	while(pEn != 0)
	{
		ERROR3IF(!pEn->IsKindOf(CC_RUNTIME_CLASS(CMXReferListItem)), "unexpected type of entry in refer list");

		if(pEn->IsInWhichDesc() == cmxDESC_ARROW)
		{
			Ref++;

			CMXReferArrow *pAr = (CMXReferArrow *)pEn;
			ERROR3IF(!pAr->IsKindOf(CC_RUNTIME_CLASS(CMXReferArrow)), "not an arrow, when it said it was");
			
			// is it this colour?
			if(pAr->AreYouThisArrow(pArrow))
			{
				// yep. return the reference number
				return Ref;
			}
		}

		pEn = (CMXReferListItem *)ReferList.GetNext(pEn);
	}

	// we haven't got one... make a new one, set it up and add it to the list
	CMXReferArrow *pNewArrow = new CMXReferArrow(this);
	if(pNewArrow == 0)
		return 0;

	pNewArrow->Set(pArrow);

	ReferList.AddTail(pNewArrow);

	// return the next reference number, Ref is the ref of the last one in the list
	return (WORD)Ref + 1;
}


/********************************************************************************************

>	WORD CMXExportDC::GetBitmapReference(KernelBitmap *pTheLovelyBitmap, DocColour *pStartCol, DocColour *pEndCol, CMXReferBitmap **ppRB = 0)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/07/96
	Inputs:		bitmap
	Returns:	refernce number of a bitmap
	Purpose:	to find a reference number for the bitmap when writing a CMX file.
				*ppRB is set to a pointer to the CMXReferBitmap object if ppRB != 0

********************************************************************************************/

WORD CMXExportDC::GetBitmapReference(KernelBitmap *pTheLovelyBitmap, DocColour *pStartCol,
	DocColour *pEndCol, EFFECTTYPE Effect, CMXReferBitmap **ppRB)
{
	// the reference we need -- references count up from 1
	INT32 Ref = 0;

	// scan that list!
	CMXReferListItem *pEn = (CMXReferListItem *)ReferList.GetHead();
	while(pEn != 0)
	{
		ERROR3IF(!pEn->IsKindOf(CC_RUNTIME_CLASS(CMXReferListItem)), "unexpected type of entry in refer list");

		if(pEn->IsInWhichIndex() == cmxINDEX_EMBEDFILE && IS_A(pEn, CMXReferBitmap))
		{
			Ref++;

			CMXReferBitmap *pBitty = (CMXReferBitmap *)pEn;
			
			// is it this bitmap?
			if(pBitty->AreYouThisBitmap(pTheLovelyBitmap, pStartCol, pEndCol, Effect))
			{
				// yep. set bitmap object addr and return the reference number
				if(ppRB != 0)
					(*ppRB) = pBitty;

				return Ref;
			}
		}

		pEn = (CMXReferListItem *)ReferList.GetNext(pEn);
	}

	// we haven't got one... make a new one, set it up and add it to the list
	CMXReferBitmap *pNewRefBit = new CMXReferBitmap(this);
	if(pNewRefBit == 0)
		return 0;

	pNewRefBit->Set(pTheLovelyBitmap, pStartCol, pEndCol, Effect);

	ReferList.AddTail(pNewRefBit);

	// set bitmap object addr
	if(ppRB != 0)
		(*ppRB) = pNewRefBit;

	// return the next reference number, Ref is the ref of the last one in the list
	return (WORD)Ref + 1;
}


/********************************************************************************************

>	BOOL CMXReferPen::WriteInDesc(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/07/96
	Inputs:		DC
	Returns:	success
	Purpose:	writes the description record for pens

********************************************************************************************/

BOOL CMXReferPen::WriteInDesc(CMXExportDC *pDC)
{
	if(pDC->IsThirtyTwoBit())
	{
		// write the 32 bit description
		if(!pDC->StartTag(cmxTAG_DescrSection_Pen))
			return FALSE;

		cmxPen32 pen;

		double scaledwidth = ((double)Width) * CAMCOORD_SCALEFACTOR32;
		pen.Width = (DWORD)scaledwidth;
		pen.Aspect = 100;		// width is 100% of the height
		pen.Angle = 0;
		pDC->WriteData(&pen, sizeof(pen));

		// write a matrix (use identity)
		if(!pDC->WriteMatrix())
			return FALSE;

		// end the tag
		if(!pDC->EndTag()
			|| !pDC->WriteMinEndTag())
			return FALSE;
	}
	else
	{
		cmxPen16 pen;

		double scaledwidth = ((double)Width) * pDC->GetScaleFactor();
		pen.Width = (WORD)scaledwidth;
		pen.Aspect = 100;		// width is 100% of the height
		pen.Angle = 0;
		pDC->WriteData(&pen, sizeof(pen));

		// write a matrix (use identity)
		if(!pDC->WriteMatrix())
			return FALSE;
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXReferArrowheads::WriteInDesc(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/07/96
	Inputs:		DC
	Returns:	success
	Purpose:	writes the description record for arrowheads

********************************************************************************************/

BOOL CMXReferArrowheads::WriteInDesc(CMXExportDC *pDC)
{
	struct {
		WORD Start,End;
	} en = {Start, End};

	return pDC->WriteData(&en, sizeof(en));
}


/********************************************************************************************

>	BOOL CMXReferLineStyle::WriteInDesc(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/07/96
	Inputs:		DC
	Returns:	success
	Purpose:	writes the description record for line style

********************************************************************************************/

BOOL CMXReferLineStyle::WriteInDesc(CMXExportDC *pDC)
{
	if(!pDC->WriteTag(cmxTAG_DescrSection_LineStyle, &LineStyle, sizeof(LineStyle))
		|| !pDC->WriteMinEndTag())
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXReferLineStyle::AreYouThisStyle(cmxLineStyle *pLineStyle)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/07/96
	Inputs:		pointer to a line style description
	Returns:	boolean
	Purpose:	compares the given line style with the one in the object

********************************************************************************************/

BOOL CMXReferLineStyle::AreYouThisStyle(cmxLineStyle *pLineStyle)
{
	// this assume that the structures are packed nicely, and are initialised properly
	// failure is not a major problem, just results in larger files
	if(memcmp(&LineStyle, pLineStyle, sizeof(cmxLineStyle)) == 0)
	{
		// they're the same
		return TRUE;
	}
	
	// they're not the same
	return FALSE;
}


/********************************************************************************************

>	BOOL CMXReferOutline::WriteInDesc(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/07/96
	Inputs:		DC
	Returns:	success
	Purpose:	writes the description record for the outline

********************************************************************************************/

BOOL CMXReferOutline::WriteInDesc(CMXExportDC *pDC)
{
	if(!pDC->WriteTag(cmxTAG_DescrSection_Outline, &Outline, sizeof(Outline))
		|| !pDC->WriteMinEndTag())
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXReferOutline::AreYouThisStyle(cmxOutline *pOutline)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/07/96
	Inputs:		pointer to a outline description
	Returns:	boolean
	Purpose:	compares the given outline with the one in the object

********************************************************************************************/

BOOL CMXReferOutline::AreYouThisOutline(cmxOutline *pOutline)
{
	// this assume that the structures are packed nicely, and are initialised properly
	// failure is not a major problem, just results in larger files
	if(memcmp(&Outline, pOutline, sizeof(cmxOutline)) == 0)
	{
		// they're the same
		return TRUE;
	}
	
	// they're not the same
	return FALSE;
}


/********************************************************************************************

>	BOOL CMXExportDC::WriteOutlineSpec(CMXRenderRegion *pReg)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/07/96
	Inputs:		render region
	Returns:	sucess
	Purpose:	writes the outline specification to the file

********************************************************************************************/

BOOL CMXExportDC::WriteOutlineSpec(CMXRenderRegion *pReg)
{
	// algorithm:
	// examine the render region and create the structures we want
	// this involves scanning the refer list to find the components of the
	// outline, and possibly adding new bits to it.
	// then, once we've got this structure, we need to scan the list
	// to see if it's already there. However, if we needed to create
	// anything new in the first phase, we can just skip this.
	// but we need to know how many outlines are in there, so
	// keep a count in the first bit.

	// find out the line width
	MILLIPOINT Width;
	// get the attribute
	LineWidthAttribute *pLineWidthA
			= (LineWidthAttribute *)pReg->GetCurrentAttribute(ATTR_LINEWIDTH);

	// check it out
	ERROR2IF(pLineWidthA == 0, FALSE, "No line width attribute");
	ERROR3IF(!pLineWidthA->IsKindOf(CC_RUNTIME_CLASS(LineWidthAttribute)), "not a line width attribute");

	Width = pLineWidthA->LineWidth;

	// find the colour
	DocColour *pColour;
	// get the attribute
	StrokeColourAttribute *pColA
			= (StrokeColourAttribute *)pReg->GetCurrentAttribute(ATTR_STROKECOLOUR);
	// check it out
	ERROR2IF(pColA == 0, FALSE, "No stroke colour attribute");
	ERROR3IF(!pColA->IsKindOf(CC_RUNTIME_CLASS(StrokeColourAttribute)), "not a stroke colour attribute");

	pColour = pColA->GetStartColour();

	// get the reference for the colour -- check to see if it's transparent
	BOOL TransparentLine = FALSE;
	WORD ColRef;
	if(pColour->IsTransparent())
	{
		TransparentLine = TRUE;
		Width = 0;					// Graeme (15-2-00) - Remove line if transparent.
	}
	else
		ColRef = GetColourReference(pColour);

	// sort out the line style
	cmxLineStyle LineStyle;
	LineStyle.Spec = TransparentLine?cmxLINESPEC_NONE:0;
	LineStyle.CapAndJoin = 0;

	// find the join type
	// get the attribute
	JoinTypeAttribute *pJTA
			= (JoinTypeAttribute *)pReg->GetCurrentAttribute(ATTR_JOINTYPE);
	// check it out
	ERROR2IF(pJTA == 0, FALSE, "No join type attribute");
	ERROR3IF(!pJTA->IsKindOf(CC_RUNTIME_CLASS(JoinTypeAttribute)), "not a join type attribute");
	// set
	switch(pJTA->JoinType)
	{
		case RoundJoin: LineStyle.CapAndJoin |= cmxCAPJOIN_ROUNDJOIN; break;
		case BevelledJoin: LineStyle.CapAndJoin |= cmxCAPJOIN_BEVELJOIN; break;
		default: break;
	}

	// find the end cap
	// get the attribute
	StartCapAttribute *pSCA
			= (StartCapAttribute *)pReg->GetCurrentAttribute(ATTR_STARTCAP);
	// check it out
	ERROR2IF(pSCA == 0, FALSE, "No start cap attribute");
	ERROR3IF(!pSCA->IsKindOf(CC_RUNTIME_CLASS(StartCapAttribute)), "not a start cap attribute");
	// set
	switch(pSCA->StartCap)
	{
		case LineCapRound: LineStyle.CapAndJoin |= cmxCAPJOIN_ROUNDCAP; break;
		case LineCapSquare: LineStyle.CapAndJoin |= cmxCAPJOIN_SQUARECAP; break;
		default: break;
	}

	// find the dotdash pattern
	DashRec *pDash;
	DashPatternAttribute *pDas
			= (DashPatternAttribute *)pReg->GetCurrentAttribute(ATTR_DASHPATTERN);
	// check it out
	ERROR2IF(pDas == 0, FALSE, "No dash pattern attribute");
	ERROR3IF(!pDas->IsKindOf(CC_RUNTIME_CLASS(DashPatternAttribute)), "not a dash pattern attribute");
	// get it...
	pDash = &pDas->DashPattern;

	if(pDash->Elements != 0)
		LineStyle.Spec |= cmxLINESPEC_DOTDASH;
	else
		LineStyle.Spec |= cmxLINESPEC_SOLIDOUTLINE;
	
	// now we want to aquire some arrowhead references
	EndArrowAttribute *pEa
			= (EndArrowAttribute *)pReg->GetCurrentAttribute(ATTR_ENDARROW);
	// check it out
	ERROR2IF(pEa == 0, FALSE, "No arrow attribute");
	ERROR3IF(!pEa->IsKindOf(CC_RUNTIME_CLASS(EndArrowAttribute)), "not a arrow attribute");
	StartArrowAttribute *pSa
			= (StartArrowAttribute *)pReg->GetCurrentAttribute(ATTR_STARTARROW);
	// check it out
	ERROR2IF(pSa == 0, FALSE, "No arrow attribute");
	ERROR3IF(!pSa->IsKindOf(CC_RUNTIME_CLASS(StartArrowAttribute)), "not a arrow attribute");

	// get the arrow references
	WORD StartArrow = GetArrowReference(&pSa->StartArrow);
	WORD EndArrow = GetArrowReference(&pEa->EndArrow);

	// right then, we need to run through the table to construct the specification
	BOOL FoundPen = FALSE;			// whether we found the pen
	WORD PenRef = 0;				// reference number for the pen
	BOOL FoundLineStyle = FALSE;
	WORD LineStyleRef = 0;
	BOOL FoundArrowheads = FALSE;
	WORD ArrowheadsRef = 0;
	BOOL FoundDotDash = FALSE;
	WORD DotDashRef = 0;

	WORD LastOutlineRef = 0;		// count up the outlines to find out the ref of the last one

	CMXReferListItem *pEn = (CMXReferListItem *)ReferList.GetHead();
	while(pEn != 0)
	{
		ERROR3IF(!pEn->IsKindOf(CC_RUNTIME_CLASS(CMXReferListItem)), "unexpected type of entry in refer list");

		switch(pEn->IsInWhichDesc())
		{

		case cmxDESC_PEN:
			if(!FoundPen)
			{
				PenRef++;

				CMXReferPen *pPen = (CMXReferPen *)pEn;
				ERROR3IF(!pPen->IsKindOf(CC_RUNTIME_CLASS(CMXReferPen)), "not a refer pen, when it said it was");
				
				// is it this width?
				if(pPen->AreYouThisWidth(Width))
				{
					// yes
					FoundPen = TRUE;
				}
			}
			break;

		case cmxDESC_LINESTYLE:
			if(!FoundLineStyle)
			{
				LineStyleRef++;

				CMXReferLineStyle *pSt = (CMXReferLineStyle *)pEn;
				ERROR3IF(!pSt->IsKindOf(CC_RUNTIME_CLASS(CMXReferLineStyle)), "not a refer LineStyle, when it said it was");
				
				// is it this style?
				if(pSt->AreYouThisStyle(&LineStyle))
				{
					// yes
					FoundLineStyle = TRUE;
				}
			}
			break;

		case cmxDESC_ARROWHEADS:
			if(!FoundArrowheads)
			{
				ArrowheadsRef++;

				CMXReferArrowheads *pAh = (CMXReferArrowheads *)pEn;
				ERROR3IF(!pAh->IsKindOf(CC_RUNTIME_CLASS(CMXReferArrowheads)), "not a arrowheads, when it said it was");
				
				// is it this arrowheads?
				if(pAh->AreYouThisArrowheads(StartArrow, EndArrow))
				{
					// yes
					FoundArrowheads = TRUE;
				}
			}
			break;

		case cmxDESC_DOTDASH:
			if(!FoundDotDash)
			{
				DotDashRef++;

				CMXReferDotDash *pDd = (CMXReferDotDash *)pEn;
				ERROR3IF(!pDd->IsKindOf(CC_RUNTIME_CLASS(CMXReferDotDash)), "not a refer dot dash, when it said it was");
				
				// is it this style?
				if(pDd->AreYouThisDotDash(pDash))
				{
					// yes
					FoundDotDash = TRUE;
				}
			}
			break;

		case cmxDESC_OUTLINE:
			LastOutlineRef++;		// inc the ref for each outline object enountered
			break;

		default:
			// do nothing
			break;
		}

		pEn = (CMXReferListItem *)ReferList.GetNext(pEn);
	}

	// right then, create some new refer objects if necessary
	BOOL NewReferObjectCreated = FALSE;		// whether an refer object was created

	if(!FoundPen)
	{
		NewReferObjectCreated = TRUE;
		// knock up a pen object
		CMXReferPen *pPen = new CMXReferPen(this);
		if(pPen == NULL)
			return FALSE;

		pPen->SetWidth(Width);
		PenRef++;				// the reference

		ReferList.AddTail(pPen);
	}

	if(!FoundLineStyle)
	{
		NewReferObjectCreated = TRUE;
		// knock up a pen object
		CMXReferLineStyle *pLs = new CMXReferLineStyle(this);
		if(pLs == NULL)
			return FALSE;

		pLs->SetLineStyle(&LineStyle);
		LineStyleRef++;				// the reference

		ReferList.AddTail(pLs);
	}

	if(!FoundArrowheads)
	{
		NewReferObjectCreated = TRUE;
		// knock up a pen object
		CMXReferArrowheads *pAh = new CMXReferArrowheads(this);
		if(pAh == NULL)
			return FALSE;

		pAh->SetArrowheads(StartArrow, EndArrow);
		ArrowheadsRef++;				// the reference

		ReferList.AddTail(pAh);
	}

	if(!FoundDotDash)
	{
		NewReferObjectCreated = TRUE;
		// knock up a pen object
		CMXReferDotDash *pDd = new CMXReferDotDash(this);
		if(pDd == NULL)
			return FALSE;

		pDd->Set(pDash);
		DotDashRef++;				// the reference

		ReferList.AddTail(pDd);
	}

	// make up an outline def
	cmxOutline Outline;
	memset(&Outline, 0, sizeof(Outline));

	Outline.LineStyle = LineStyleRef;
	Outline.Screen = cmxSCREENREFERENCE;
	if(TransparentLine)
		Outline.Colour = 1;		// any random number will do as long the colour exists
	else
		Outline.Colour = ColRef;
	Outline.Arrowheads = ArrowheadsRef;
	Outline.Pen = PenRef;
	Outline.DotDash = DotDashRef;

	// run through the table to see if there's a 
	BOOL FoundOutline = FALSE;
	WORD OutlineRef = 0;
	if(NewReferObjectCreated == FALSE) // only bother looking if no new objects have been created
	{
		CMXReferListItem *pEn = (CMXReferListItem *)ReferList.GetHead();
		while(pEn != 0)
		{
			ERROR3IF(!pEn->IsKindOf(CC_RUNTIME_CLASS(CMXReferListItem)), "unexpected type of entry in refer list");

			if(pEn->IsInWhichDesc() == cmxDESC_OUTLINE)
			{
				OutlineRef++;

				CMXReferOutline *pLC = (CMXReferOutline *)pEn;
				ERROR3IF(!pLC->IsKindOf(CC_RUNTIME_CLASS(CMXReferOutline)), "not a refer outline, when it said it was");
				
				// is it this outline?
				if(pLC->AreYouThisOutline(&Outline))
				{
					FoundOutline = TRUE;
					break;
				}
			}

			pEn = (CMXReferListItem *)ReferList.GetNext(pEn);
		}
	}

	// right then, maybe we want to create a new outline object?
	if(FoundOutline == FALSE)
	{
		OutlineRef = LastOutlineRef + 1;

		// make a new object
		CMXReferOutline *pOlt = new CMXReferOutline(this);
		if(pOlt == NULL)
			return FALSE;

		// set it's outline thingy
		pOlt->SetOutline(&Outline);

		// and add it to the list
		ReferList.AddTail(pOlt);
	}

	// finally, write the reference to the file
	if(!WriteNestedTag(cmxTAG_RenderAttr_OutlineSpec, &OutlineRef, sizeof(OutlineRef)))
		return FALSE;

	// and the end tag
	if(!WriteMinEndTag())
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	void CMXExportDC::CalcCorelBBox(DocCoord *Coords, INT32 NumCoords, DocRect &Result)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Inputs:		coords, num coords and output rec
	Returns:	-
	Purpose:	calculates the corel bounding box of a path. This is just a simple union
				of all point. The result is not transformed by the export transform matrix.

********************************************************************************************/

void CMXExportDC::CalcCorelBBox(DocCoord *Coords, INT32 NumCoords, DocRect *Result)
{
	ERROR3IF(Coords == NULL || NumCoords < 1, "dodgy coords");

	// write some sensible starting values
	Result->lo = Coords[0];
	Result->hi = Coords[0];

	// run through the rest
	for(INT32 l = 1; l < NumCoords; l++)
	{
		if(Result->lo.x > Coords[l].x) Result->lo.x = Coords[l].x;
		if(Result->lo.y > Coords[l].y) Result->lo.y = Coords[l].y;
		if(Result->hi.x < Coords[l].x) Result->hi.x = Coords[l].x;
		if(Result->hi.y < Coords[l].y) Result->hi.y = Coords[l].y;
	}

	// sorted
}


/********************************************************************************************

>	BOOL CMXExportDC::WriteAttrCheckLens(CMXRenderRegion *pReg, INT32 Tag, DocCoord *Coords, INT32 NumCoords, BOOL *WasLens)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/96
	Inputs:		render region, tag, pointer to flag
	Returns:	error flag
	Purpose:	checks to see if this object should be outputed as a lens. If so, we need
				to output the attributes for it ourselves (as we can't use the full range
				of possible things) and add things to the list

********************************************************************************************/

WORD CMXExportDC::GetProcedureReference(CMXReferListItem *pProc)
{
	ERROR3IF(!pProc->IsAProcedure(), "item is not a procedure");

	// scan the refer list for this item, counting procedures before it
	WORD Ref = 0;
	CMXReferListItem *pEn = (CMXReferListItem *)ReferList.GetHead();
	while(pEn != 0)
	{
		ERROR3IF(!pEn->IsKindOf(CC_RUNTIME_CLASS(CMXReferListItem)), "unexpected type of entry in refer list");

		if(pEn->IsAProcedure())
		{
			// inc reference number
			Ref++;

			// is it this one?
			if(pEn == pProc)
				return Ref;
		}

		pEn = (CMXReferListItem *)ReferList.GetNext(pEn);
	}

	return 1;		// fairly OK default
}


/********************************************************************************************

>	BOOL CMXExportDC::WriteAttrCheckLens(CMXRenderRegion *pReg, INT32 Tag, DocCoord *Coords, INT32 NumCoords, BOOL *WasLens)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/96
	Inputs:		render region, tag, pointer to flag
	Returns:	error flag
	Purpose:	checks to see if this object should be outputed as a lens. If so, we need
				to output the attributes for it ourselves (as we can't use the full range
				of possible things) and add things to the list

********************************************************************************************/

BOOL CMXExportDC::WriteAttrCheckLens(CMXRenderRegion *pReg, INT32 Tag, DocCoord *Coords, INT32 NumCoords, BOOL *WasLens)
{
	// set the flag for no lens
	*WasLens = FALSE;

	// get the transparency attr
	TranspFillAttribute *pTra =
		(TranspFillAttribute *)pReg->GetCurrentAttribute(ATTR_TRANSPFILLGEOMETRY);
	// check it out
	if(pTra == NULL)
	{
		TRACEUSER( "Ben", _T("No transparent fill attribute -- assuming it's not transparent\n"));
		return TRUE;
	}
	ERROR3IF(!pTra->IsKindOf(CC_RUNTIME_CLASS(TranspFillAttribute)), "not a transparent fill attribute");

	// right then, let's work out what transparency value we've got here
	UINT32 TransparencyValue = 0;
	if(pTra->IsAFlatFill())
	{
		TransparencyValue = *pTra->GetStartTransp();
	}
	else
	{
		// average the start and end values
		TransparencyValue = (*pTra->GetStartTransp() + *pTra->GetEndTransp()) / 2;
	}

	// check to see if we really want to do this thingy
	if (TransparencyValue == 0)
		return TRUE;			// nothing to do, it's not transparent

	TRACEUSER( "Ben", _T("transparent, has value %d\n"), TransparencyValue);

	// OK, do we want to ignore this lens to stop CorelDRAW from dieing when we import the file
	if(WriteAttrCheckIgnoreLens(pReg, Coords, NumCoords))
	{
		// yes, ignore the damn thing
		return TRUE;
	}

	// transparency has the range 0-255 in Cam, but 0-1000 in CMX
	WORD UniformRate = 1000 - ((TransparencyValue * 1000) / 255);	// the value to export

	// now we need to work out what colour the thingy will be
	WORD ColourReference = 1;		// nice default value

	// get the attribute
	FillGeometryAttribute *pFillGeometry
			= (FillGeometryAttribute *)pReg->GetCurrentAttribute(ATTR_FILLGEOMETRY);

	// check it out
	ERROR2IF(pFillGeometry == 0, FALSE, "No fill geometry");
	ERROR3IF(!pFillGeometry->IsKindOf(CC_RUNTIME_CLASS(FillGeometryAttribute)), "not one of them there fill geometries");

	// get the start colour of the attribute
	DocColour *pColour = pFillGeometry->GetStartColour();

	if(pColour != 0 && !pColour->IsTransparent())
	{
		// get a reference for this nice colour
		ColourReference = GetColourReference(pColour);
	}

	// right then, we now need to create one of those lens refer list items
	CMXReferLens *pLens = new CMXReferLens(this);
	if(pLens == NULL)
		return FALSE;

	// get the bounding box
	DocRect bb;
	CalcCorelBBox(Coords, NumCoords, &bb);

	// set up the lens list item
	if(!pLens->Set(this, &bb))
		return FALSE;

	// add it to the list
	ReferList.AddTail(pLens);

	// right then, we now need to write some attributes to the file
	// start the tag
	if(Tag != -1)
		if(!StartTag(Tag))
			return FALSE;

	// write the attribute mask thingy
	WriteByte(cmxRENDATTRMASK_FILL | cmxRENDATTRMASK_OUTLINE | cmxRENDATTRMASK_LENS);

	// write the fill specification
	if(!StartNestedTag(cmxTAG_RenderAttr_FillSpec)
		||!WriteFillType(cmxFILLID_UNIFORM))
		return FALSE;
	struct {
		WORD FillReference;
		WORD ScreenReference;
	} filldef = {ColourReference, cmxSCREENREFERENCE};
	if(!WriteNestedTag(cmxTAG_RenderAttr_FillSpec_Uniform, &filldef, sizeof(filldef))
		|| !WriteMinEndTag()
		|| !EndNestedTag()
		|| !WriteMinEndTag())
		return FALSE;

	// write the outline specification
	if(!WriteOutlineSpec(pReg))
		return FALSE;

	// and write the lens specification
	cmxGlassLensDefn lendef;
	lendef.LensType = cmxLENSTYPE_GLASS;

	switch(pTra->GetTranspType())
	{
		default:			lendef.TintMethod = cmxLENSTINTMETH_AVERAGE;	break;
		case TT_StainGlass:	lendef.TintMethod = cmxLENSTINTMETH_SUBTRACT;	break;
		case TT_Bleach:		lendef.TintMethod = cmxLENSTINTMETH_ADD;		break;
	}
	lendef.UniformRate = UniformRate;
	lendef.ColourReference = ColourReference;
	lendef.RangeProcReference = GetProcedureReference(pLens);

	if(!WriteNestedTag(cmxTAG_RenderAttr_LensSpec_Base, &lendef, sizeof(lendef)))
		return FALSE;

	if(ThirtyTwoBit)
	{
		cmxLensFrozView fv;			// no idea what this is
		fv.FlagFrozen = 0;
		fv.FlagActive = 0;
		fv.ViewPointX = 0;
		fv.ViewPointY = 0;

		if(!WriteNestedTag(cmxTAG_RenderAttr_LensSpec_FrozViewp, &fv, sizeof(fv))
			|| !WriteMinEndTag())
			return FALSE;
	}

	// end the tag
	if(Tag != -1)
		if(!EndTag())
			return FALSE;

	// tell the caller that we did something
	*WasLens = TRUE;

	// set the page flag to say we had a lens
	SetCMXFlag(cmxSTRUCTFLAGS_HASLENS);

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::WriteAttrCheckIgnoreLens(CMXRenderRegion *pReg, DocCoord *Coords, INT32 NumCoords)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/96
	Inputs:		region, coords, num coords
	Returns:	whether to ignore a lens on this object
	Purpose:	checks to see if we should ignore this lens -- it sees if it's going to
				overlap with too many lenses. This is basically as bodge as CorelDRAW will
				just fall over if we export a file with lots of overlapped
				lenses in it.

********************************************************************************************/

BOOL CMXExportDC::WriteAttrCheckIgnoreLens(CMXRenderRegion *pReg, DocCoord *Coords, INT32 NumCoords)
{
	// first thing, find the bbox of the object we're about to lens to
	DocRect BBox;
	CalcCorelBBox(Coords, NumCoords, &BBox);

	// now search the list to see how many existing lenses intersect with it
	INT32 LensesBehindThisOne = 0;
	CMXLensBBox *pEn = (CMXLensBBox *)LensBBoxes.GetHead();
	while(pEn != 0)
	{
		// check for intersection
		if(pEn->DoesIntersect(&BBox))
			LensesBehindThisOne++;

		// next, please
		pEn = (CMXLensBBox *)LensBBoxes.GetNext(pEn);
	}

	// make up a new object to add to the nice list thingy
	CMXLensBBox *pNewLensBBox = new CMXLensBBox(&BBox);
	if(pNewLensBBox != NULL)
		LensBBoxes.AddTail(pNewLensBBox);

	// do we want to ignore this?
	if(LensesBehindThisOne > CMX_MAX_OVERLAP_LENSES)
	{
		TRACEUSER( "Ben", _T("found transparency to ignore, %d lenses behind\n"), LensesBehindThisOne);
		// maybe we could ask the user if it wanted to ignore the overlapped things
		if(!AreIgnoreingOverlappedLenses && !HaveAskedUserAboutLenses)
		{
			HaveAskedUserAboutLenses = TRUE;

			INT32 Result = InformWarning(_R(IDW_CMXOVERLAPPEDLENS), _R(IDB_CMXDOLENSES), _R(IDB_CMXIGNORELENSES), 0, 0, 2, 1);

			switch(Result)
			{
			case 1: //_R(IDB_CMXDOLENSES):
				AreIgnoreingOverlappedLenses = FALSE;
				break;

			case 2: //_R(IDB_CMXIGNORELENSES):
			default:
				AreIgnoreingOverlappedLenses = TRUE;
				break;
			}
		}

		// ignore it?
		if(AreIgnoreingOverlappedLenses)
		{
			// OK, we've got to ignore this -- mark the fact
			OverlappedLensesHaveBeenIgnored = TRUE;

			return TRUE;		// ignore it
		}
	}

	return FALSE;		// don't ignore this one
}


/********************************************************************************************

>	BOOL CMXReferLens::Set(CMXExportDC *pDC, DocRect *tBBox)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/96
	Inputs:		export DC, bounding box of lens object
	Returns:	error flag
	Purpose:	sets up the data this thingy needs

********************************************************************************************/

BOOL CMXReferLens::Set(CMXExportDC *pDC, DocRect *tBBox)
{
	// copy in the bounding box
	BBox = *tBBox;

	// get the position of the command
	CommandFilePosition = pDC->GetCurrentInstrFilePosition();

	// get the layer number
	LayerNumber = pDC->GetLayerNumber();
	
	return TRUE;
}


/********************************************************************************************

>	BOOL CMXReferLens::WriteSection(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/96
	Inputs:		export DC
	Returns:	error flag
	Purpose:	writes the lens descrition section

********************************************************************************************/

BOOL CMXReferLens::WriteSection(CMXExportDC *pDC)
{
	// note the position of this thingy for future reference
	LensDescFilePosition = pDC->GetFilePosition();

	// start the section
	if(!pDC->StartSection(CMXExportDC::CMXSECTION_LENS))
		return FALSE;

	// write the header
	cmxLensDescriptionSection len;
	len.ParentType = cmxREFLISTEN_TYPE_LAYER;
	len.Page = 1;
	len.ParentReference = LayerNumber;
	len.StartAddress = pDC->GetFirstInstrFilePosition();
	len.EndAddress = CommandFilePosition;
	pDC->WriteData(&len, sizeof(len));

	// write the bbox
	Matrix *pMat = pDC->GetTransMatrix();
	DocRect b = BBox;
	pMat->TransformBounds(&b);
	if(!pDC->WriteBBox(&b))
		return FALSE;

	// end the section
	if(!pDC->EndSection())
		return FALSE;

	return TRUE;
}



/********************************************************************************************

>	BOOL CMXReferLens::WriteInReferenceList(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/96
	Inputs:		export dc
	Returns:	success
	Purpose:	writes the relevant entry in the reference list of the file
	SeeAlso:	

********************************************************************************************/

BOOL CMXReferLens::WriteInReferenceList(CMXExportDC *pDC)
{
	ERROR2IF(pDC == 0, FALSE, "no DC");

	// write the entry
	cmxRefListEntryRefOffset en;
	memset(&en, '\0', sizeof(en));

	en.Association = cmxREFLISTEN_DESC_LENS;
	en.Type = cmxREFLISTEN_TYPE_INSTRUCTION;
	en.Offset = CommandFilePosition;

	pDC->WriteData(&en, sizeof(en));

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXReferLens::WriteInIndex(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/96
	Inputs:		export dc
	Returns:	success
	Purpose:	writes the relevant entry in the procedure index of the file
	SeeAlso:	

********************************************************************************************/

BOOL CMXReferLens::WriteInIndex(CMXExportDC *pDC)
{
	// write the index entry
	cmxProcIndexEntry en;
	pDC->WriteSizeInFile(sizeof(en));
	en.RefList = -1;
	en.Procedure = LensDescFilePosition;

	return pDC->WriteData(&en, sizeof(en));
}


/********************************************************************************************

>	BOOL CMXReferBitmapFill::WriteInIndex(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/96
	Inputs:		export dc
	Returns:	success
	Purpose:	writes the relevant entry in the procedure index of the file
	SeeAlso:	

********************************************************************************************/

BOOL CMXReferBitmapFill::WriteInIndex(CMXExportDC *pDC)
{
	// write the index entry
	cmxProcIndexEntry en;
	pDC->WriteSizeInFile(sizeof(en));
	en.RefList = -1;
	en.Procedure = ProcedureFilePosition;

	return pDC->WriteData(&en, sizeof(en));
}


/********************************************************************************************

>	BOOL CMXReferBitmap::WriteInIndex(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/07/96
	Inputs:		export dc
	Returns:	success
	Purpose:	writes the relevant entry in the procedure index of the file
	SeeAlso:	

********************************************************************************************/

BOOL CMXReferBitmap::WriteInIndex(CMXExportDC *pDC)
{
	cmxEmbedFileIndexEntry en;
	pDC->WriteSizeInFile(sizeof(en));
	en.Offset = BitmapFileOffset;
	en.Type = cmxEMBEDFILETYPE_RIMAGE;

	return pDC->WriteData(&en, sizeof(en));
}


/********************************************************************************************

>	BOOL CMXReferBitmap::WriteSection(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/07/96
	Inputs:		export dc
	Returns:	success
	Purpose:	writes the relevant entry in the procedure index of the file
	SeeAlso:	

********************************************************************************************/

BOOL CMXReferBitmap::WriteSection(CMXExportDC *pDC)
{
	// get the winoil bitmap associated with this thingy
	// this is non kernelally stuff, but noone else cares when they do it
	KernelBitmap *pBitmap = Ref.GetBitmap();
	ERROR2IF(pBitmap == NULL, FALSE, "no bitmap in ref");
	OILBitmap *bit = pBitmap->ActualBitmap;
	ERROR2IF(bit == NULL, FALSE, "kernel bitmap didn't have a oil bitmap");

	// note the position of the thingy
	BitmapFileOffset = pDC->GetFilePosition();

	// contone?
	if(Contone)
	{
		// grab a view for us to use in the following call
		View *pView = NULL;
		if (pDC->GetRenderRegion()!=NULL)
			pView=pDC->GetRenderRegion()->GetRenderView();

		// build the contone bitmap
		if (!bit->BuildContonePalette(StartColour, EndColour, Effect, pView))
			return FALSE;

		// tell the render region we're exporting a contone bitmap
		pDC->GetRenderRegion()->SetAreExportingContoneBitmap(TRUE);
	}
	else
	{
		// we're not doing a contoned bitmap
		pDC->GetRenderRegion()->SetAreExportingContoneBitmap(FALSE);
	}

	// export the bitmap
	if(!bit->ExportBitmap(pDC->GetRenderRegion()))
		return FALSE;

	if(Contone)
	{
		// Clean up contone palette
		bit->DestroyContonePalette();
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXReferBitmapFill::WriteSection(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/07/96
	Inputs:		export dc
	Returns:	success
	Purpose:	writes the relevant entry in the procedure index of the file
	SeeAlso:	

********************************************************************************************/

BOOL CMXReferBitmapFill::WriteSection(CMXExportDC *pDC)
{
	// get the file position for our records
	ProcedureFilePosition = pDC->GetFilePosition();

	// first step -- start the procedure section
	if(!pDC->StartSection(CMXExportDC::CMXSECTION_PROCBITFILL))
		return FALSE;

	// bounding box
	DocRect bbox = DocRect(0, 0, cmxBITMAPFILLTILESIZE_X, cmxBITMAPFILLTILESIZE_Y);

	// put in the start the procedure
	if(!pDC->StartPage(&bbox, TRUE))
		return FALSE;

	// write the bitmap object
	DocCoord Paral[4] =				// bouning parallelogram of the bitmap
		{	DocCoord(0, cmxBITMAPFILLTILESIZE_Y),
			DocCoord(cmxBITMAPFILLTILESIZE_X, cmxBITMAPFILLTILESIZE_Y),
			DocCoord(cmxBITMAPFILLTILESIZE_X, 0),
			DocCoord(0, 0)
		};

	if ( !pDC->WriteBitmap ( 0, Paral, CMXExportDC::CMXBITMAPCOLOURSOURCE_NONE,
							 FALSE, BitmapReference ) )
		return FALSE;

	// end the page
	if(!pDC->EndPage())
		return FALSE;

	// end the section
	if(!pDC->EndSection())
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::WriteBitmap ( KernelBitmap			*pBitmap,
									DocCoord				*pParallelogram,
									CMXBitmapColourSource	ColSource,
									BOOL					TransformParallelogram,
									WORD					BitmapReference )

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/07/96
	Inputs:		render region, bitmap, parllelogram for the bitmap
	Returns:	success
	Purpose:	writes a bitmap object to the file
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::WriteBitmap ( KernelBitmap			*pBitmap,
								DocCoord				*pParallelogram,
								CMXBitmapColourSource	ColSource,
								BOOL					TransformParallelogram,
								WORD					BitmapReference )
{
	// sort out some colours for contoning the bitmap
	DocColour *pStartColour = 0;
	DocColour *pEndColour = 0;
	EFFECTTYPE Effect = EFFECT_RGB;

	// we have three possible methods for getting some colours...
	switch(ColSource)
	{
	case CMXBITMAPCOLOURSOURCE_FILLGEOMETRY:
		ERROR2(FALSE, "fill geometry bitmap colour source not implemented");
		break;

	case CMXBITMAPCOLOURSOURCE_LINEANDFILL:
		{
		// right then, we get the start colour from the line effect
		// get the attribute
		StrokeColourAttribute *pColA
				= (StrokeColourAttribute *)pRenderRegion->GetCurrentAttribute(ATTR_STROKECOLOUR);
		// check it out
		ERROR2IF(pColA == 0, FALSE, "No stroke colour attribute");
		ERROR3IF(!pColA->IsKindOf(CC_RUNTIME_CLASS(StrokeColourAttribute)), "not a stroke colour attribute");
		pStartColour = pColA->GetStartColour();
		// check to see if we need to do contoning
		if(pStartColour != 0 && pStartColour->IsTransparent())
			pStartColour = 0;

		// get the end colour from a fill attribute
		if(pStartColour != 0)
		{
			// get the attribute
			FillGeometryAttribute *pFillGeometry
					= (FillGeometryAttribute *)pRenderRegion->GetCurrentAttribute(ATTR_FILLGEOMETRY);
			// check it out
			ERROR2IF(pFillGeometry == 0, FALSE, "No fill geometry");
			ERROR3IF(!pFillGeometry->IsKindOf(CC_RUNTIME_CLASS(FillGeometryAttribute)), "not one of them there fill geometries");

			// get the start colour of the fill attribute
			pEndColour = pFillGeometry->GetStartColour();

			if(pEndColour == NULL || pEndColour->IsTransparent())
			{
				// no end colour -- can't be contoned then
				pStartColour = 0;
				pEndColour = 0;
			}
			else
			{
				// get the fill effect
				Effect = pRenderRegion->GetFillEffect();
			}
		}
		}
		break;

	default:
	case CMXBITMAPCOLOURSOURCE_NONE:
		break;		// do nothing
	}

	// get a reference number for the bitmap
	WORD BitmapRef = 0;
	if(pBitmap == 0)
	{
		BitmapRef = BitmapReference;
	}
	else
	{
		BitmapRef = GetBitmapReference(pBitmap, pStartColour, pEndColour, Effect);
	}
	if(BitmapRef == 0)
		return TRUE;			// if it isn't able to be got, sod it.

	// make a new transformed parallelogram thingy
	DocCoord Para[4];
	ERROR2IF(pMatrix == NULL, FALSE, "no matrix");
	for(INT32 z = 0; z < (sizeof(Para) / sizeof(DocCoord)); z++)
	{
		Para[z] = pParallelogram[z];
		if(TransformParallelogram)
			pMatrix->transform(&(Para[z]));
	}

	// begin the command
	if(!StartCommand(cmxINSTR_DrawImage))
		return FALSE;

	// output the rendering attributes
	if(!WriteBlankAttributes(cmxTAG_DrawImage_RenderingAttr))
		return FALSE;

	// write the image data
	if(!StartTag(cmxTAG_DrawImage_DrawImageSpecification))
		return FALSE;

	// image extent and cropping rectangle, in pixels
	DocRect ImEx;
	if(ThirtyTwoBit)
	{
		ImEx.lo.x = ImEx.lo.y = 0;
		ImEx.hi.x = 2048;
		ImEx.hi.y = 2048;
	}
	else
	{
		ImEx.lo.x = 0;
		ImEx.lo.y = -2048;
		ImEx.hi.x = 2048;
		ImEx.hi.y = 0;
	}
	if(!WriteBBox(&ImEx, FALSE) ||
		!WriteBBox(&ImEx, FALSE))
		return FALSE;

	// construct a transformation matrix -- no point in going through the write matrix fn
	double magX = 2048.0;
	double magY = 2048.0;
	cmxMatrix Matrix = {cmxMATRIXTYPE_GENERAL,
		(double)(Para[2].x - Para[3].x) / magX,
		(double)(Para[2].y - Para[3].y) / magY,
		(double)(Para[0].x - Para[3].x) / magX,
		(double)(Para[0].y - Para[3].y) / magY,
		Para[3].x,
		Para[3].y};

	if(!ThirtyTwoBit)
	{
		// add correction for bitmapnesses in 16 bit CMX files
		Matrix.f += Matrix.d * 2048;
	}

	// save out the matrix
	ExportFile->write(&Matrix, sizeof(Matrix));

	// other bits and pieces
	cmxDrawImageEndBits eb;
	eb.ImageType = cmxDRAWIMAGE_IMAGETYPE_COLOUR;
	eb.FileRef1 = BitmapRef;
	eb.FileRef2 = 0;			// nothing associated with this
	if(!WriteData(&eb, sizeof(eb)))
		return FALSE;

	// end the specification thingy
	if(!EndTag())
		return FALSE;

	// end the command
	if(!WriteMinEndTag()
		|| !EndCommand())
		return FALSE;

	return TRUE;
}
