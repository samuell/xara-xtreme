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
// *********Fills for the CDR file filter

/*
*/

#include "camtypes.h"
#include "cdrfiltr.h"
#include "nodepath.h"
#include "cdrbitm.h"
//#include "ben.h"
#include "nodebmp.h"
#include "nodetxts.h"

DECLARE_SOURCE("$Revision: 662 $");

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	BOOL CDRFilter::SetFillAttr(cdrfOffsetHeader *Object)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27 03 95
	Inputs:		address of the object
	Returns:	error flag
	Purpose:	sets the fill attributes
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::SetFillAttr(cdrfOffsetHeader *Object)
{
	if(Version == CDRVERSION_3)
	{
		return SetFillAttr3(Object);
	}

	// find the reference...
	DWORD *pReference = (DWORD *)FindDataInObject(Object, cdrfOBJOFFSETTYPE_FILL);

	// if the pointer to the reference is zero, then the reference is probably
	// within some random style definition
	if(pReference == 0)
	{
		// OK, try and find a style number within the style reference
		WORD *pStyleReference = (WORD *)FindDataInObject(Object, cdrfOBJOFFSETTYPE_STYLE);

		if(pStyleReference != 0)
		{
			// OK, see if we can find a the style...
			cdrfStyle *pStyle;
			INT32 StyleSize;

			if((pStyle = (cdrfStyle *)Styles.Find(*pStyleReference, &StyleSize)) != 0)
			{
				// OK, got a style... now find a fill reference within it
				pReference = (DWORD *)FindDataInObject(&pStyle->Header, cdrfSTYLEOFFSETTYPE_FILL);
			}
		}
	}
	
	// should this be filled?
	if(pReference == 0 || ObjFilled == FALSE)
	{
		// set not filled attribute
		return NoFill();
	}

	cdrfFillHeader *Fill;
	INT32 FillSize;
	
	// set is filled attribute
	if(!SetPathFilled(TRUE))
		return FALSE;

	// try and find the data for a version 4 or 5 fill
	if((Fill = (cdrfFillHeader *)Fills.Find(*pReference, &FillSize)) == 0)
		return SetUKFill();

	// check the fill's size
	if(FillSize < sizeof(cdrfFillHeader))
	{
		// if no fill defn or it's just a tad to small, set the unknown fill colour
		return SetUKFill();
	}

	// OK, what's this fill then?
	switch(CDRDATA_WORD(Fill->FillType))
	{
		case cdrfFILLTYPE_NONE:
			// no fill - set things up to be hoopy about that.
			return NoFill();
			break;

		case cdrfFILLTYPE_FLAT:
			return SetFillAttrFlat(Fill, FillSize);
			break;

		case cdrfFILLTYPE_GRADUATED:
			return SetFillAttrGrad(Fill, FillSize);
			break;

		case cdrfFILLTYPE_PATTERN:
			{
				CDRTiledFillInfo i;
				cdrfFillPattern *PFill = (cdrfFillPattern *)Fill;

				// check the size and set an unknown fill if it's too small
				if(FillSize < sizeof(cdrfFillPattern))
					return SetUKFill();
	
				// set the info structure
				i.SizeX = CDRDATA_SWORD(PFill->SizeX);
				i.SizeY = CDRDATA_SWORD(PFill->SizeY);
				i.OffsetX = CDRDATA_SWORD(PFill->OffsetX);
				i.OffsetY = CDRDATA_SWORD(PFill->OffsetY);
				i.Reference = CDRDATA_DWORD(PFill->Reference);

				// convert the colours
				DocColour Colour1;
				DocColour Colour2;
				ConvertColour(&PFill->Colour1, &Colour1);
				ConvertColour(&PFill->Colour2, &Colour2);

				return SetFillAttrPattern(&i, &Colour1, &Colour2);
			}
			break;
		
		case cdrfFILLTYPE_VECTOR:
			{
				CDRTiledFillInfo i;
				cdrfFillVector *VFill = (cdrfFillVector *)Fill;

				// check the size and set an unknown fill if it's too small
				if(FillSize < sizeof(cdrfFillVector))
					return SetUKFill();
	
				// set the info structure
				i.SizeX = CDRDATA_SWORD(VFill->SizeX);
				i.SizeY = CDRDATA_SWORD(VFill->SizeY);
				i.OffsetX = CDRDATA_SWORD(VFill->OffsetX);
				i.OffsetY = CDRDATA_SWORD(VFill->OffsetY);
				i.Reference = CDRDATA_DWORD(VFill->VectorReference);

				return SetFillAttrVector(&i);
			}
			break;
		
		case cdrfFILLTYPE_TEXTURE:
			return SetFillAttrTexture(Fill, FillSize);
			break;

		default:
			return SetUKFill();
			break;
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::SetFillAttr(cdrfOffsetHeader *Object)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/95
	Inputs:		-
	Returns:	error flag
	Purpose:	makes an object not filled
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::NoFill(void)
{
	if(pMadeNode->IsKindOf(CC_RUNTIME_CLASS(NodePath)))
	{
		NodePath *pPath = (NodePath *)pMadeNode;

		pPath->InkPath.IsFilled = FALSE;
	}

	ObjFilled = FALSE;
	SetFillColour(DocColour(COLOUR_TRANS));
	return SetPathFilled(FALSE);
}

/********************************************************************************************

>	BOOL CDRFilter::SetFillAttr3(cdrfOffsetHeader *Object)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/95
	Inputs:		address of object
	Returns:	error flag
	Purpose:	sets the fill attributes for version 3
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::SetFillAttr3(cdrfOffsetHeader *Object)
{
	cdrfFillV3 *Fill = (cdrfFillV3 *)FindDataInObject(Object, cdrfOBJOFFSETTYPE_FILL);
	
	if(Fill == 0)
	{
		// set not filled attribute
		return NoFill();
	}

	// info about the fill
	CDRFillInfo fi;

	// find the bounding box of this lovely node
	DocRect BBox;
	if(!GetObjCorelBBox((NodeRenderableBounded *)pMadeNode, &BBox))
		return FALSE;

	// convert colours and things as necessary
	DocColour Col1;
	DocColour Col2;

	if(Fill->Type == cdrfFILLTYPEV3_FLAT)
	{
		if(!ConvertColour((cdrfColour *)&Fill->Data.Flat.Colour, &Col1))
			return FALSE;
	}
	else if(Fill->Type == cdrfFILLTYPEV3_LINEAR || Fill->Type == cdrfFILLTYPEV3_RADIAL)
	{
		fi.Angle = CDRDATA_SWORD(Fill->Data.Grad.Angle);
		fi.Pad = CDRDATA_SWORD(Fill->Data.Grad.Pad);
		fi.HorizontalCentreOffset = CDRDATA_SWORD(Fill->Data.Grad.HorizontalCentreOffset);
		fi.VerticalCentreOffset = CDRDATA_SWORD(Fill->Data.Grad.VerticalCentreOffset);
	
		if(!ConvertColour((cdrfColour *)&Fill->Data.Grad.Colour1, &Col1))
			return FALSE;
		if(!ConvertColour((cdrfColour *)&Fill->Data.Grad.Colour2, &Col2))
			return FALSE;
	}

	// make the path filled
	if(!SetPathFilled(TRUE))
		return FALSE;
	
	// set the fill
	switch(Fill->Type)
	{
		case cdrfFILLTYPEV3_NONE:
			return NoFill();
			break;

		case cdrfFILLTYPEV3_FLAT:
			if(!SetFillColour(Col1))
				return FALSE;
			break;

		case cdrfFILLTYPEV3_LINEAR:
			if(!SetLinearGrad(&fi, BBox, &Col1, &Col2))
				return SetUKFill();
			break;

		case cdrfFILLTYPEV3_RADIAL:
			if(!SetRadialGrad(&fi, BBox, &Col1, &Col2))
				return SetUKFill();
			break;


		case cdrfFILLTYPEV3_PATTERN:
			{
				CDRTiledFillInfo i;

				// set the info structure
				i.SizeX = CDRDATA_SWORD(Fill->Data.Pattern.SizeX);
				i.SizeY = CDRDATA_SWORD(Fill->Data.Pattern.SizeY);
				i.OffsetX = CDRDATA_SWORD(Fill->Data.Pattern.OffsetX);
				i.OffsetY = CDRDATA_SWORD(Fill->Data.Pattern.OffsetY);
				i.Reference = CDRDATA_DWORD(Fill->Data.Pattern.Reference);

				// convert the colours
				ConvertColour((cdrfColour *)&Fill->Data.Pattern.Colour1, &Col1);
				ConvertColour((cdrfColour *)&Fill->Data.Pattern.Colour2, &Col2);

				return SetFillAttrPattern(&i, &Col1, &Col2);
			}
			break;
	
		case cdrfFILLTYPEV3_VECTOR:
			{
				CDRTiledFillInfo i;

				// set the info structure
				i.SizeX = CDRDATA_SWORD(Fill->Data.Vector.SizeX);
				i.SizeY = CDRDATA_SWORD(Fill->Data.Vector.SizeY);
				i.OffsetX = CDRDATA_SWORD(Fill->Data.Vector.OffsetX);
				i.OffsetY = CDRDATA_SWORD(Fill->Data.Vector.OffsetY);
				i.Reference = CDRDATA_DWORD(Fill->Data.Vector.Reference);

				return SetFillAttrVector(&i);
			}
			break;
	
		default:
			return SetUKFill();
			break;
	}	

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::SetFillAttrFlat(cdrfFillHeader *Fill, INT32 Size)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 03 95
	Inputs:		pointer to fill header
	Returns:	error flag
	Purpose:	sets flat fill attributes
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::SetFillAttrFlat(cdrfFillHeader *Fill, INT32 Size)
{
	cdrfFillFlat *FFill = (cdrfFillFlat *)Fill;

	// check the size and set an unknown fill if it's too small
	if(Size < sizeof(cdrfFillFlat))
		return SetUKFill();
	
	// convert and set the line colour
	DocColour Col;
	ConvertColour(&FFill->Colour, &Col);

	if(!SetFillColour(Col))
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::SetFillAttrGrad(cdrfFillHeader *Fill, INT32 Size)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 03 95
	Inputs:		pointer to fill header
	Returns:	error flag
	Purpose:	sets graduated fill attributes
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::SetFillAttrGrad(cdrfFillHeader *Fill, INT32 Size)
{
	// check that it's big enought to be a realistic grad fill
	if(Size < (sizeof(cdrfFillGraduated) + sizeof(cdrfFillGradColour)))
	{
TRACEUSER( "Ben", _T("fill too small, size = %d\n"), Size);
		return SetUKFill();
	}

	// make a new pointer
	cdrfFillGraduated *GFill = (cdrfFillGraduated *)Fill;

	// has it got enough colours in it?
	if(CDRDATA_WORD(GFill->NColours) < 1)
		return SetUKFill();

	// find the bounding box of this lovely node
	DocRect BBox;
	if(!GetObjCorelBBox((NodeRenderableBounded *)pMadeNode, &BBox))
		return FALSE;

	// sort out the colours on this fill - ATM we can only go from one colour to
	// another, so we just need to find the last and first ones and use those.
	cdrfFillGradColour *Cols = (cdrfFillGradColour *)(GFill + 1);

	UINT32 min = CDRDATA_WORD(Cols[0].Percentage);
	UINT32 max = min;
	UINT32 minO = 0;
	UINT32 maxO = 0;
	UINT32 l;

	for(l = 1; l < CDRDATA_WORD(GFill->NColours); l++)
	{
		UINT32 p = CDRDATA_WORD(Cols[l].Percentage);

		if(p < min)
		{
			min = p;
			minO = l;
		}
		if(p > max)
		{
			max = p;
			maxO = l;
		}
	}

	// OK, found the minimum and maximim percentages along the grad fill line.
	// get and convert the colours
	DocColour StartColour;
	DocColour EndColour;

	ConvertColour(&Cols[minO].Colour, &StartColour);
	ConvertColour(&Cols[maxO].Colour, &EndColour);

	// now work out what kind of fade we need here
	UINT32 FadeType = CDRDATA_WORD(GFill->ColourType);

	// if it's a rainbow effect, work out whether an anti-clockwise thingy will be a
	// a normal or alt rainbow fill
	BOOL NormalRainbow = TRUE;
	if(FadeType == cdrfGRADFILLCOLOURTYPE_RAINBOW_CLOCKWISE ||
			 FadeType == cdrfGRADFILLCOLOURTYPE_RAINBOW_ANTICLOCKWISE)
	{
		// convert both colours to HSV to find their hue
		ColourContext *Conv = ColourContext::GetGlobalDefault(COLOURMODEL_HSVT);

		ColourHSVT StartC;
		ColourHSVT EndC;

		Conv->ConvertColour(&StartColour, (ColourGeneric *)&StartC);
		Conv->ConvertColour(&EndColour, (ColourGeneric *)&EndC);

		// compare the hues and work out a difference
		ColourValue Difference;

		if(EndC.Hue > StartC.Hue)
		{
			Difference = EndC.Hue - StartC.Hue;
		} else {
			Difference = 1.0 - (StartC.Hue - EndC.Hue);
		}

		// if the difference is more than 0.5 then it's a alt rainbow thingy
		if(Difference > fixed24(0.5))
			NormalRainbow = FALSE;
	}

	// set the fade effect
	switch(FadeType)
	{
		case cdrfGRADFILLCOLOURTYPE_DIRECT:
		case cdrfGRADFILLCOLOURTYPE_CUSTOM:
		default:
			SetFadeFillEffect();
			break;

		case cdrfGRADFILLCOLOURTYPE_RAINBOW_ANTICLOCKWISE:
			if(NormalRainbow)
				SetRainbowFillEffect();
			else
				SetAltRainbowFillEffect();
			break;

		case cdrfGRADFILLCOLOURTYPE_RAINBOW_CLOCKWISE:
			if(NormalRainbow)
				SetAltRainbowFillEffect();
			else
				SetRainbowFillEffect();
			break;
	}

	// get the info about the fill
	CDRFillInfo fi;

	fi.Angle = CDRDATA_SWORD(GFill->Angle);
	fi.Pad = CDRDATA_SWORD(GFill->Pad);
	fi.HorizontalCentreOffset = CDRDATA_SWORD(GFill->HorizontalCentreOffset);
	fi.VerticalCentreOffset = CDRDATA_SWORD(GFill->VerticalCentreOffset);
	
	// and finally, sort out the start and end points of the thingy
	DocCoord StartPoint;
	DocCoord EndPoint;
	switch(CDRDATA_WORD(GFill->GradFillType))
	{
		case cdrfGRADFILLTYPE_LINEAR:
			if(!SetLinearGrad(&fi, BBox, &StartColour, &EndColour))
				return SetUKFill();
			break;

		case cdrfGRADFILLTYPE_RADIAL:
		case cdrfGRADFILLTYPE_SQUARE:		// radial is the nearest Camelot fill to a square fill
			if(!SetRadialGrad(&fi, BBox, &StartColour, &EndColour))
				return SetUKFill();
			break;

		case cdrfGRADFILLTYPE_CONICAL:
			if(!SetConicalGrad(&fi, BBox, &StartColour, &EndColour))
				return SetUKFill();
			break;

		default:			
			return SetUKFill();
			break;
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::SetUKFill(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 03 95
	Inputs:		pointer to fill header
	Returns:	error flag
	Purpose:	sets a fill attribute for unknown fills
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::SetUKFill(void)
{
	PColourCMYK cmyk;
	cmyk.Cyan = cmyk.Magenta = cmyk.Yellow = 0;
	cmyk.Key = 255;

	DocColour Col;
	Col.SetCMYKValue(&cmyk);

	return SetFillColour(Col);
}


/********************************************************************************************

>	BOOL CDRFilter::GetObjCorelBBox(NodeRenderable *pNode, DocRect *BBox)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 03 95
	Inputs:		converted node (after transformation) and a pointer to bbox to fill in
	Returns:	error flag
	Purpose:	gets the corel bbox of a converted object, looking up the bbox of text objects
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::GetObjCorelBBox(NodeRenderableBounded *pNode, DocRect *BBox)
{
	if(IS_A(pNode, TextStory) && TextBBoxes.Find(pNode, BBox) == TRUE)
		return TRUE;

	return GetCorelBBox(pNode, BBox);
}

/********************************************************************************************

>	BOOL CDRFilter::GetCorelBBox(NodeRenderable *pNode, DocRect *BBox)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 03 95
	Inputs:		converted node (after transformation) and a pointer to bbox to fill in
	Returns:	error flag
	Purpose:	gets the Corel simple union bbox of an object
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::GetCorelBBox(NodeRenderableBounded *pNode, DocRect *BBox)
{
	if(pNode->IsKindOf(CC_RUNTIME_CLASS(NodePath)))
	{
		// ok, so it's a path. That means we can run through it to find out the simple
		// union of all points. That's cool. Nice of corel not to do proper bboxes, isn't
		// it?

		NodePath *pNodePath = (NodePath *)pNode;

		// get a pointer to the array of coords in the nice path
		INT32 NCoords = pNodePath->InkPath.GetNumCoords();
		DocCoord *Coords = pNodePath->InkPath.GetCoordArray();

		ERROR3IF(NCoords < 1, "Awooga! Trying to find a corel bbox of a path with less than one coord.");

		INT32 c;

		INT32 x0, y0, x1, y1;

		// initialise my bbox variables
		x0 = x1 = Coords[0].x;
		y0 = y1 = Coords[0].y;
		
		for(c = 1; c < NCoords; c++)
		{
			// update my bbox
			if(Coords[c].x < x0) x0 = Coords[c].x;
			if(Coords[c].y < y0) y0 = Coords[c].y;
			if(Coords[c].x > x1) x1 = Coords[c].x;
			if(Coords[c].y > y1) y1 = Coords[c].y;
		}

		// fill in *BBox
		BBox->lo.x = x0;
		BBox->lo.y = y0;
		BBox->hi.x = x1;
		BBox->hi.y = y1;
	} else {
		// if it's not a path object, ask the node what it's bounding box is - we'll just
		// have to make do with a *correct* bounding box instead of simply corel union.
		*BBox = pNode->GetBoundingRect();
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::SetLinearGrad(CDRFillInfo *GFill, DocRect &BBox, DocColour *StartColour, DocColour *EndColour)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 03 95
	Inputs:		grad fill defn and converted colours for the fill
	Returns:	error flag
	Purpose:	sets a linear graduated fill from fill definition in the file
				Copied from coreleps.cpp and modified
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::SetLinearGrad(CDRFillInfo *GFill, DocRect &BBox, DocColour *StartColour, DocColour *EndColour)
{
	// NB. this function is (hopefully!) over-complex and will be simplified.
	// However, it's like this atm so I can get my head around the weird maths Corel 
	// forces us to use.

	// The desired start and end points of the grad fill 'arrow'.
	DocCoord Start, End;

	// Calculate width and height
	MILLIPOINT Width = BBox.Width();
	MILLIPOINT Height = BBox.Height();

	// bodge just in case of zero height or widths
	if(Width < 16)
		Width = 16;

	if(Height < 16)
		Height = 16;

	// Find centre of box
	DocCoord Centre;
	Centre.x = BBox.lo.x + (Width / 2);
	Centre.y = BBox.lo.y + (Height / 2);

	// Find total area of BBox
	double TotalArea = (double) Width * (double) Height;

	// Cope with angles > 180
	BOOL Mirror = FALSE;
	INT32 Angle = GFill->Angle;		// note angle is * 10
	
	if (Angle >= 1800)
	{
		Angle -= 1800;
		Mirror = TRUE;
	}
	else if (Angle < 00)
	{
		Angle += 1800;
		Mirror = TRUE;
	}

	Angle += 900;

	if (Angle >= 1800)
	{
		Angle -= 1800;
	}

	// get edge pad value
	INT32 EdgePad = GFill->Pad;
	
	// Calculate tan of the angle - convert angle to radians first.
	double Radians = (((double) Angle) / 1800.0) * PI;
	double TanTheta;
	if (Angle == 900)
	{
		// Special case for horizontal grad fill arrow.

		// Make 0% padding first
		Start.x = BBox.lo.x;
		Start.y = Centre.y;
		End.x = BBox.hi.x;
		End.y = Centre.y;

		// Find out width of padding
		INT32 Padding = (Width * EdgePad) / 100;
		Start.x += Padding;
		End.x -= Padding;
	}
	else if (Angle == 0)
	{
		// Special case for vertical grad fill arrow.

		// Make 0% padding first
		Start.x = Centre.x;
		Start.y = BBox.lo.y;
		End.x = Centre.x;
		End.y = BBox.hi.y;

		// Find out width of padding
		INT32 Padding = (Height * EdgePad) / 100;
		Start.y += Padding;
		End.y -= Padding;
	}
	else
	{
		TanTheta = tan(Radians);

		// Find out what the maximum padding is that we can achieve using just triangles:

		// Find the maximum triangle width
		MILLIPOINT TriWidth = (MILLIPOINT) ((double) Height / TanTheta);

		// Limit it to sensible value
		if (TriWidth < 0)
			TriWidth = -TriWidth;
		if (TriWidth > Width)
			TriWidth = Width;

		// Find the maximum triangle width
		MILLIPOINT TriHeight = (MILLIPOINT) ((double) Width * TanTheta);

		// Limit it to sensible value
		if (TriHeight < 0)
			TriHeight = -TriHeight;
		if (TriHeight > Height)
			TriHeight = Height;

		// The 'c' values of the y = mx+c equation.
		MILLIPOINT StartC, EndC;

		// Work out the maximum percentage/edge padding this gives us
		// (50 because it's 100 / 2 because we want area of triangle, not rectangle).
		double Percentage = (50.0 * (double) TriWidth * (double) TriHeight) / TotalArea;

		INT32 Diff = 0;

		// Is this enough?
		if (((INT32) Percentage) >= EdgePad)
		{
			// Yes - calculate exactly how big the triangle needs to be.
			TriHeight = (MILLIPOINT) sqrt(ABS(((double) EdgePad * TotalArea * TanTheta) / 100.0));

			TriWidth = (MILLIPOINT) ((double) TriHeight / TanTheta);
			if (TriWidth < 0)
				TriWidth = -TriWidth;

			ENSURE(TriWidth < Width, "Error in Corel Grad fill decoding logic");
		}
		else
		{
			// How much percentage do we need to add with each rectangle?
			Percentage = (EdgePad - Percentage) / 2;

			// Handle the rectangle stuff.
			if (TriWidth == Width)
			{
				// Need to add rectangles to the top and bottom.
				Diff = (MILLIPOINT) ((Percentage * Height) / 100.0);
			}
			else
			{
				// Need to add rectangles to left and right
				Diff = (MILLIPOINT) ((Percentage * Width) / 100.0);
				Diff = (MILLIPOINT) (Diff / tan(PI - Radians));
				Diff = ABS(Diff);
			}
		}

		// Work out the C value for the start line (c = y - mx)
		// (m = tan(angle) )
		if (Angle == 900)
		{
			//ENSURE(FALSE, "90 degree angle found!");
		}
		else if (Angle < 900)
		{
			StartC = (MILLIPOINT) (BBox.lo.y - ((BBox.hi.x - TriWidth) * TanTheta));
			EndC = (MILLIPOINT) (BBox.hi.y - ((BBox.lo.x + TriWidth) * TanTheta));
		}
		else
		{
			StartC = (MILLIPOINT) (BBox.lo.y - ((BBox.lo.x + TriWidth) * TanTheta));
			EndC = (MILLIPOINT) (BBox.hi.y - ((BBox.hi.x - TriWidth) * TanTheta));
		}

		// Add on difference for rectangles, if any.
		StartC += Diff;
		EndC -= Diff;


		// Work out m and c for the grad fill line.
		// We know m is -1/m of the triangle's hypotenuse.
		// c = roy - (rox/m)
		double FillM = -1.00 / TanTheta;
		MILLIPOINT FillC = (MILLIPOINT) (Centre.y - (Centre.x * FillM));

		// Work out intersections:  x = (c2 - c1) / (2m)

		Start.x = (MILLIPOINT) ( (FillC - StartC) / (TanTheta + (1.00 / TanTheta)) );
		Start.y = (MILLIPOINT) ((FillM * Start.x) + FillC);

		End.x = (MILLIPOINT) ( (FillC - EndC) / (TanTheta + (1.00 / TanTheta)) );
		End.y = (MILLIPOINT) ((FillM * End.x) + FillC);
	}

	if (Mirror)
	{
		// Swap the grid fill end-points over.
		DocCoord Tmp = Start;
		Start = End;
		End = Tmp;
	}

	// Set the fill type according to these calculations.
	return SetLinearFill(*StartColour, *EndColour, Start, End);	
}


/********************************************************************************************

>	BOOL CDRFilter::SetConicalGrad(CDRFillInfo *GFill, DocRect &BBox, DocColour *StartColour, DocColour *EndColour)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 03 95
	Inputs:		grad fill defn and converted colours for the fill
	Returns:	error flag
	Purpose:	sets a conical graduated fill from fill definition in the file
				Copied from coreleps.cpp and modified
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::SetConicalGrad(CDRFillInfo *GFill, DocRect &BBox, DocColour *StartColour, DocColour *EndColour)
{
	// The desired start and end points of the grad fill 'arrow'.
	DocCoord Start, End;

	// Calculate width and height
	MILLIPOINT Width = BBox.Width();
	MILLIPOINT Height = BBox.Height();

	// Start point is the centre given by Corel.
	// This centre is percentage offsets from the centre of the object, i.e. (0,0) is
	// the centre of the bounding box.
	Start.x = BBox.lo.x + (Width  / 2);
	Start.y = BBox.lo.y + (Height / 2);
 	Start.x += ((GFill->HorizontalCentreOffset * Width) / 100);
	Start.y += ((GFill->VerticalCentreOffset * Height) / 100);

	// End point is start point + radius but takes into account the angle
	double Radius = Width / 2;
	// angle is * 10, and needs to be in radians
	double Theta = ((((double)(GFill->Angle)) / 10.0) / 360.0) * (2 * PI);

	// make the angle go anti-clockwise
	Theta = 0 - Theta;

	// rotate by PI / 2
	Theta -= PI / 2;
	
	// angle can be negative, ensure it's positive
	while(Theta < 0)
		Theta += (2 * PI);

	// calculate the triangle
	double dx, dy;

	dx = Radius * sin(Theta);
	dy = Radius * cos(Theta);

	End.x = Start.x + (INT32)dx;
	End.y = Start.y + (INT32)dy;

	// Seems that we need to swap start and end colours...

	// Set the fill type according to these calculations.
	return SetConicalFill(*EndColour, *StartColour, Start, End);	
}


/********************************************************************************************

>	BOOL CDRFilter::SetRadialGrad(CDRFillInfo *GFill, DocRect &BBox, DocColour *StartColour, DocColour *EndColour)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/04/95
	Inputs:		grad fill defn and converted colours for the fill
	Returns:	error flag
	Purpose:	sets a radial graduated fill from fill definition in the file
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::SetRadialGrad(CDRFillInfo *GFill, DocRect &BBox, DocColour *StartColour, DocColour *EndColour)
{
	// The desired start and end points of the grad fill 'arrow'.
	DocCoord Start, End;

	// Calculate width and height
	MILLIPOINT Width = BBox.Width();
	MILLIPOINT Height = BBox.Height();

	// caluculate the source area
	// first, what's the diagonal length
	double dWidth = Width;
	double dHeight = Height;
	INT32 Diagonal = (INT32)sqrt(dWidth*dWidth + dHeight*dHeight);

	// and from that calculate area of the box containing the bit of the
	// bit of the circle in the bbox
	INT32 Edge = (Diagonal * (100 - (GFill->Pad * 2))) / 100;

	// Start point is the centre given by Corel.
	// This centre is percentage offsets from the centre of the object, i.e. (0,0) is
	// the centre of the bounding box.
	DocCoord Centre = DocCoord(BBox.lo.x + (Width / 2), BBox.lo.y + (Height / 2));
	INT32 OffX = (GFill->HorizontalCentreOffset * Width) / 100;
	INT32 OffY = (GFill->VerticalCentreOffset * Height) / 100;
	Start.x = Centre.x + OffX;
	Start.y = Centre.y + OffY;

	// Find required radius of circle.
	double Radius = Edge / 2;

	// how far away is the centre of the fill from the centre of the bbox?
	double dOffX = OffX;
	double dOffY = OffY;
	double Dist = (INT32)sqrt(dOffX*dOffX + dOffY*dOffY);

	// and increase the radius by a bodge factor
	double BodgeFactor = 1 + (Dist / (double)(Diagonal / 2));
	Radius *= BodgeFactor;
	
	// End point is start point + radius
	End.x = Start.x + ((MILLIPOINT) Radius);
	End.y = Start.y;

	// Seems that we need to swap start and end colours...

	// Set the fill type according to these calculations.
	return SetRadialFill(*EndColour, *StartColour, Start, End);	
}


/********************************************************************************************

>	BOOL CDRBitmapStore::AddChunkToStore(RIFFFile *RIFF, BOOL Pattern, CDRFilter *C)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/04/95
	Inputs:		RIFF file to get the chunk from, and wether it's pattern or not (Pattern are
				1bpp and have a different format)
	Returns:	error flag
	Purpose:	Adds a bitmap to the store, converting it now rather than storing it up
				for later
	SeeAlso:	CDRFilter

********************************************************************************************/


BOOL CDRBitmapStore::AddChunkToStore(RIFFFile *RIFF, BOOL Pattern, CDRFilter *C)
{
	CDRBitmapStoredItem *Item = new CDRBitmapStoredItem;

	if(Item == 0)
		return FALSE;
	
	// convert the bitmap
	FOURCC Reference = 0;		// the reference number of this thingy

	// convert depending on the type of thing
	if(Pattern)
	{
		Item->pBitmap = CDRBitmap::ConvertPattern(RIFF, &Reference, C);
	}
	else
	{
		switch(C->Version)
		{
			case CDRVERSION_5:
				Item->pBitmap = CDRBitmap::ConvertBitmap5(RIFF, &Reference, C);
				break;

			default:
				Item->pBitmap = CDRBitmap::ConvertBitmap4(RIFF, &Reference, C);
				break;
		}
	}

	// check that we got one
	if(Item->pBitmap == 0)
		return FALSE;
	
	// make a nice name for it
	String_256 Name;
	if(Pattern)
		Name.MakeMsg(_R(IDT_CDRPATTERNNAME), CDRFilter::PatternNumber++);
	else
 		Name.MakeMsg(_R(IDT_CDRBITMAPNAME), CDRFilter::BitmapNumber++);

	// and set the name
	Item->pBitmap->ActualBitmap->SetName(Name);

	// store the reference
	Item->Reference = Reference;
	
	// add the item to this list
	AddTail(Item);
	
	return TRUE;
}


/********************************************************************************************

>	KernelBitmap *CDRBitmapStore::FindBitmap(DWORD Reference)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/04/95
	Inputs:		reference of the bitmap to find (a CDRDATA_ should already have been done
				on it)
	Returns:	a pointer to a KernelBitmap or 0 for not found
	Purpose:	Finds a bitmap within the store. Finding a bitmap sets it's used flag to
				TRUE to avoid it being deleted.
	SeeAlso:	CDRFilter

********************************************************************************************/

KernelBitmap *CDRBitmapStore::FindBitmap(DWORD Reference)
{
	CDRBitmapStoredItem *Item = (CDRBitmapStoredItem *)GetHead();
	
	// go through the list of items searching for the reference number given
	while(Item != 0)
	{
		if(Item->Reference == Reference)
		{
			// set the used flag
			Item->Used = TRUE;

			return Item->pBitmap;
		}

		Item = (CDRBitmapStoredItem *)GetNext(Item);
	}

	return 0;
}


/********************************************************************************************

>	CDRBitmapStoredItem::CDRBitmapStoredItem()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/04/95
	Inputs:		-
	Returns:	-
	Purpose:	Constructor for a bitmap store entry
	SeeAlso:	CDRBitmapStore

********************************************************************************************/

CDRBitmapStoredItem::CDRBitmapStoredItem()
{
	pBitmap = 0;
	Used = FALSE;
}


/********************************************************************************************

>	CDRBitmapStoredItem::~CDRBitmapStoredItem()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/04/95
	Inputs:		-
	Returns:	-
	Purpose:	Destructor for a bitmap store entry
	SeeAlso:	CDRBitmapStore

********************************************************************************************/

CDRBitmapStoredItem::~CDRBitmapStoredItem()
{
	if(pBitmap != 0 && Used == FALSE)
		delete pBitmap;
}


/********************************************************************************************

>	BOOL CDRFilter::SetFillAttrPattern(CDRTiledFillInfo *Info, DocColour *Colour1, DocColour *Colour2)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/04/95
	Inputs:		pointer to fill header
	Returns:	error flag
	Purpose:	set pattern fill attributes
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::SetFillAttrPattern(CDRTiledFillInfo *Info, DocColour *Colour1, DocColour *Colour2)
{
	// find the bitmap we're going to use
	KernelBitmap *pBitmap = Patterns.FindBitmap(Info->Reference);

	if(pBitmap == 0)
		return SetUKFill();

	// work out where it's going to go.

	// find the size of the tile
	INT32 SizeX = Info->SizeX * CDRCOORDS_TO_MILLIPOINTS;
	INT32 SizeY = Info->SizeY * CDRCOORDS_TO_MILLIPOINTS;

	// find the bounding box of this lovely node
	DocRect BBox;
	if(!GetObjCorelBBox((NodeRenderableBounded *)pMadeNode, &BBox))
		return FALSE;

	// set to the top left + the first tile offset
	INT32 XPos = BBox.lo.x + ((SizeX * Info->OffsetX) / 100);
	INT32 YPos = BBox.hi.y - SizeY - ((SizeY * Info->OffsetY) / 100);

	// can't do anything with the row/column staggering thingy

	// set the bitmap fill
	DocCoord Start 	= DocCoord(XPos, 			YPos);
	DocCoord End1 	= DocCoord(XPos + SizeX, 	YPos);
	DocCoord End2 	= DocCoord(XPos, 			YPos + SizeY);
	if(!SetBitmapFill(pBitmap, Start, End1, End2, Colour1, Colour2))
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::SetFillAttrVector(CDRTiledFillInfo *Info)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/04/95
	Inputs:		pointer to fill header
	Returns:	error flag
	Purpose:	set vector fill attributes
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::SetFillAttrVector(CDRTiledFillInfo *Info)
{
	// find the bitmap we're going to use - at the moment, we can't only do a vector fill
	// if it actually referes to a bitmap

	// first things first - find the entry in the vectors list
	CDRVectorStoredItem *Item = Vectors.Find(Info->Reference);

	if(Item->Objects == 0)
		return SetUKFill();

	// now, does this just refer to a bitmap?
	KernelBitmap *pBitmap = BitmapFromNodeList(Item->Objects);

	if(pBitmap == 0)
		return SetUKFill();		// no bitmap generation possible

	// work out where it's going to go.

	// find the size of the tile
	INT32 SizeX = Info->SizeX * CDRCOORDS_TO_MILLIPOINTS;
	INT32 SizeY = Info->SizeY * CDRCOORDS_TO_MILLIPOINTS;

	// find the bounding box of this lovely node
	DocRect BBox;
	if(!GetObjCorelBBox((NodeRenderableBounded *)pMadeNode, &BBox))
		return FALSE;

	// set to the top left + the first tile offset
	INT32 XPos = BBox.lo.x + ((SizeX * Info->OffsetX) / 100);
	INT32 YPos = BBox.hi.y - SizeY - ((SizeY * Info->OffsetY) / 100);

	// can't do anything with the row/column staggering thingy

	// set the bitmap fill
	DocCoord Start 	= DocCoord(XPos, 			YPos);
	DocCoord End1 	= DocCoord(XPos + SizeX, 	YPos);
	DocCoord End2 	= DocCoord(XPos, 			YPos + SizeY);
	if(!SetBitmapFill(pBitmap, Start, End1, End2))
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::SetFillAttrTexture(cdrfFillHeader *Fill, INT32 Size)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/04/95
	Inputs:		pointer to fill header
	Returns:	error flag
	Purpose:	sets texture fill attributes
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::SetFillAttrTexture(cdrfFillHeader *Fill, INT32 Size)
{
	cdrfFillTexture *TFill = (cdrfFillTexture *)Fill;

	// check the size and set an unknown fill if it's too small
	if(Size < sizeof(cdrfFillTexture))
		return SetUKFill();

	// OK, let's try and find the vector thing it represents
	DocRect fBBox;
	Node *FillNodes;
	
	FillNodes = Vectors.Find(CDRDATA_WORD(TFill->VectorReference), &fBBox);

	// did we get any?
	if(FillNodes == 0)
		return SetUKFill();

	// find the associated bitmap
	KernelBitmap *pBitmap;

	pBitmap = BitmapFromNodeList(FillNodes);

	// got one?
	if(pBitmap == 0)
		return SetUKFill();

	// right then, set the fill to cover the object
	// what's it's bounding box?
	DocRect BBox;
	if(!GetObjCorelBBox((NodeRenderableBounded *)pMadeNode, &BBox))
		return FALSE;

 	// set the bitmap fill
	DocCoord Start 	= DocCoord(BBox.lo.x, 	BBox.lo.y);
	DocCoord End1 	= DocCoord(BBox.hi.x,	BBox.lo.y);
	DocCoord End2 	= DocCoord(BBox.lo.x,	BBox.hi.y);
	if(!SetBitmapFill(pBitmap, Start, End1, End2))
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	CDRVectorStoredItem::CDRVectorStoredItem(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/04/95
	Inputs:		-
	Returns:	-
	Purpose:	constructor
	SeeAlso:	CDRFilter

********************************************************************************************/

CDRVectorStoredItem::CDRVectorStoredItem(void)
	: BBox(0, 0, 0, 0)
{
	Objects = 0;
	Reference = 0;
	pBitmap = 0;
}


/********************************************************************************************

>	BOOL CDRVectorStoredItem::GenerateBitmap(Document *pDocument)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/05/95
	Inputs:		a document to use the default attributes from
	Returns:	pointer to a bitmap or zero for an error
	Purpose:	generates a bitmap representatation of the vectors stored in this entry
	SeeAlso:	CDRFilter

********************************************************************************************/

KernelBitmap *CDRVectorStoredItem::GetBitmap(Document *pDocument)
{
	ERROR2IF(Objects == 0, FALSE, "not objects in VectorStoredItem!");

	// has a bitmap already been made?
	if(pBitmap != 0)
		return pBitmap;

	// generate one...

	return 0;			// nothing could be generated
}


/********************************************************************************************

>	CDRVectorStoredItem::~CDRVectorStoredItem(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/04/95
	Inputs:		-
	Returns:	-
	Purpose:	destructor
	SeeAlso:	CDRFilter

********************************************************************************************/

CDRVectorStoredItem::~CDRVectorStoredItem(void)
{
	// the objects should always be deleted as copies are made when necessary
	if(Objects != 0)
		CDRFilter::DeleteNodeList(Objects);
}



/********************************************************************************************

>	BOOL CDRFilter::ProcessVectorList(ADDR Block, INT32 Size)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/04/95
	Inputs:		vect list contents and size
	Returns:	error flag
	Purpose:	processes the vector list at the beginning of CDR files. This contains
				a list of objects which are used for things such as full colour fills,
				texture fills, mould shapes, that sort of thing.
				
				They are referenced by the objects serial number.
				
				This function takes a block containing the vect list contents and it's
				size as Corel store all these vectors *before* the fill and outline
				definitions. Are these people really crazy?  
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::ProcessVectorList(ADDR Block, INT32 Size)
{
	ERROR3IF(pLevelNodeList != 0, "ProcessVectorList called when LevelNodeList was in use");

	// first of all, make a CCMemFile out of the block so we can do RIFF stuff on it
	CCMemFile MFile;

	MFile.open(Block, Size);

	// make a RIFF file out of it - but we don't have a header
	RIFFFile RFile;

	RFile.Init(&MFile, TRUE);

	// OK, change a few variables over...
	INT32 oTranslateByX = TranslateByX;
	INT32 oTranslateByY = TranslateByY;
	RIFFFile *oRIFF = RIFF;

	TranslateByX = 0;
	TranslateByY = 0;
	RIFF = &RFile;

	// get on and process the block
	// what we've got it a load of 'grp' lists containing the objects we're interested in
	// they contain a serial number which is used to identify the vector and a bbox,
	// which we need to find and store in the list
	do
	{
		// is it a group list?
		if(RIFF->GetObjType() == RIFFOBJECTTYPE_LISTSTART && RIFF->GetObjChunkType() == cdrT_grp)
		{
			if(!ProcessVectorListEntry())
				return FALSE;
		}

		if(!RIFF->NextObject())
			goto Failure;

	} while(RIFF->GetObjType() != RIFFOBJECTTYPE_FILEEND);

	// close everything down and put the variables back
	MFile.close();

	TranslateByX = oTranslateByX;
	TranslateByY = oTranslateByY;
	RIFF = oRIFF;

	return TRUE;

Failure:
	// clear everything up...
	MFile.close();

	TranslateByX = oTranslateByX;
	TranslateByY = oTranslateByY;
	RIFF = oRIFF;

	return FALSE;
}


/********************************************************************************************

>	BOOL CDRFilter::ProcessVectorListEntry()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/04/95
	Inputs:		-
	Returns:	error flag
	Purpose:	processes an entry in the vector list
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::ProcessVectorListEntry()
{
	UINT32 StartLevel = RIFF->GetObjLevel();

	ERROR3IF(RIFF->GetObjType() != RIFFOBJECTTYPE_LISTSTART && RIFF->GetObjChunkType() != cdrT_grp,
		"ProcessVectorListEntry called without RIFF set up correctly");

	// OK, wander though to find the serial number and the bbox
	WORD Reference;		// the serial number
	DocRect BBox;
	BOOL GotReference = FALSE;
	BOOL GotBBox = FALSE;

	do
	{
		if(RIFF->GetObjType() == RIFFOBJECTTYPE_CHUNK)
		{
			switch(RIFF->GetObjChunkType())
			{
				case cdrT_spnd:
					{
						// got the serial number block - get it...
						if(!RIFF->AquireChunkData())
							return FALSE;
					
						WORD *Data = (WORD *)RIFF->GetAquiredData();
					
						if(Data == 0)
							return FALSE;

						// store it...
						Reference = CDRDATA_WORD(*Data);

						GotReference = TRUE;
					}
					break;

				case cdrT_bbox:
					{
						// is it large enought?
						if(RIFF->GetObjSize() < sizeof(cdrfBBox))
						{
							// no - ignore this entry
							RIFF->SkipToListEnd(StartLevel);
							return TRUE;
						}

						// found the bbox... get the data
						if(!RIFF->AquireChunkData())
							return FALSE;
					
						cdrfBBox *Data = (cdrfBBox *)RIFF->GetAquiredData();
					
						if(Data == 0)
							return FALSE;

						// get the bounding box
						BBox.lo.x = CDRDATA_SWORD(Data->x0) * CDRCOORDS_TO_MILLIPOINTS;
						BBox.lo.y = CDRDATA_SWORD(Data->y0) * CDRCOORDS_TO_MILLIPOINTS;
						BBox.hi.x = CDRDATA_SWORD(Data->x1) * CDRCOORDS_TO_MILLIPOINTS;
						BBox.hi.y = CDRDATA_SWORD(Data->y1) * CDRCOORDS_TO_MILLIPOINTS;
					
						GotBBox = TRUE;
					}
					break;
				
				default:
					break;
			}
		}
	
		if(!RIFF->NextObject())
			return FALSE;

	} while((GotReference == FALSE || GotBBox == FALSE)	&&
		(RIFF->GetObjType() != RIFFOBJECTTYPE_LISTEND || RIFF->GetObjLevel() > StartLevel));

	// OK, now get the object tree
	if(!ProcessObjectTree(StartLevel))
		return FALSE;

	// right then, the converted nodes are in pLevelNodeList
	// create a nice list entry

	CDRVectorStoredItem *Item = new CDRVectorStoredItem;

	if(Item == 0)
		return FALSE;

	// set the object data
	Item->Objects = pLevelNodeList;
	Item->Reference = Reference;
	Item->BBox = BBox;

	// make the node list unused
	pLevelNodeList = 0;
	
	// add the list entry
	Vectors.AddTail(Item);

	return TRUE;
}

/********************************************************************************************

>	Node *CDRVectorStore::Find(DWORD Reference, DocRect *BBox)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/04/95
	Inputs:		reference to find, bounding box of found stuff
	Returns:	pointer to a node list, or zero for not found
	Purpose:	find a vector from the vectors list
	SeeAlso:	Filter

********************************************************************************************/

Node *CDRVectorStore::Find(DWORD Reference, DocRect *BBox)
{
	CDRVectorStoredItem *Item = (CDRVectorStoredItem *)GetHead();
	
	// go through the list of items searching for the reference number given
	while(Item != 0)
	{
		if(Item->Reference == Reference)
		{
			// OK, we found it...
			if(BBox != 0)
				*BBox = Item->BBox;
			
			return Item->Objects;
		}

		Item = (CDRVectorStoredItem *)GetNext(Item);
	}

	return 0;
}


/********************************************************************************************

>	KernelBitmap *CDRVectorStore::FindAsBitmap(DWORD Reference, DocRect *BBox)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/04/95
	Inputs:		reference to find, bounding box of found stuff
	Returns:	pointer to a node list, or zero for not found
	Purpose:	find a vector from the vectors list
	SeeAlso:	Filter

********************************************************************************************/

CDRVectorStoredItem *CDRVectorStore::Find(DWORD Reference)
{
	CDRVectorStoredItem *Item = (CDRVectorStoredItem *)GetHead();
	
	// go through the list of items searching for the reference number given
	while(Item != 0)
	{
		if(Item->Reference == Reference)
		{
			// OK, we found it...
			return Item;
		}

		Item = (CDRVectorStoredItem *)GetNext(Item);
	}

	return 0;
}


/********************************************************************************************

>	KernelBitmap *CDRFilter::BitmapFromNodeList(Node *List)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/04/95
	Inputs:		pointer to first node in a list
	Returns:	a pointer to the relevant bitmap, or zero if one couldn't be found
	Purpose:	given a list of nodes, if it only contains one bitmap then return a pointer
				to that bitmap
	SeeAlso:	Filter

********************************************************************************************/

KernelBitmap *CDRFilter::BitmapFromNodeList(Node *List)
{
	// make sure we've got something
	if(List == 0)
		return 0;

	// is there's more than one item?
	if(List->FindNext() != 0)
		return 0;

	// is it a bitmap?
	if(IS_A(List, NodeBitmap))
	{
		// right then, woz it's bitmap den?
		NodeBitmap *NB = (NodeBitmap *)List;

		// return the bitmap...
		return NB->GetBitmap();
	}

	return 0;
}


