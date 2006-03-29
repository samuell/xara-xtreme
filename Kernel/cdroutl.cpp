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
// *********Outlines for the CDR file filter

/*
*/

#include "camtypes.h"
#include "cdrfiltr.h"
#include "nodepath.h"
#include "trans2d.h"
#include "group.h"
#include "impcol.h"

DECLARE_SOURCE("$Revision: 662 $");

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	BOOL CDRFilter::SetLineAttr(cdrfOffsetHeader *Object)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27 03 95
	Inputs:		address of line data *within* the main object data (loda chunk). Can be zero
	Returns:	error flag
	Purpose:	sets the line attributes
	SeeAlso:	Filter

********************************************************************************************/

#define MAX_LINE_WIDTH	(4*72000)

BOOL CDRFilter::SetLineAttr(cdrfOffsetHeader *Object)
{
	if(Version == CDRVERSION_3)
	{
		return SetLineAttr3(Object);
	}
		
	// find the reference...
	DWORD *pReference = (DWORD *)FindDataInObject(Object, cdrfOBJOFFSETTYPE_LINE);

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
				pReference = (DWORD *)FindDataInObject(&pStyle->Header, cdrfSTYLEOFFSETTYPE_OUTLINE);
			}
		}
	}
	
	cdrfOutline *Out;
	INT32 OutSize;
	
	if(pReference == 0 || (Out = (cdrfOutline *)Outlines.Find(*pReference, &OutSize)) == 0
			|| OutSize < sizeof(cdrfOutline))
	{
		// if no outline data, or outline definitions couldn't be found, or outline
		// definition is too small, set up the default line attributes
		if(!SetLineWidth(cdrfOUTLINE_DEFAULT_WIDTH))
			return FALSE;

	PColourCMYK cmyk;
	cmyk.Cyan = cmyk.Magenta = cmyk.Yellow = 0;
	cmyk.Key = 255;

	DocColour Col;
	Col.SetCMYKValue(&cmyk);
	
		if(!SetLineColour(Col))
			return FALSE;

		return TRUE;
	}

	// check that this thingy should have an outline
	if((CDRDATA_WORD(Out->Flags) & cdrfOUTLINEFLAGS_NOOUTLINE) != 0)
	{
		if (!SetLineColour(DocColour(COLOUR_TRANS)))
			return FALSE;

		return TRUE;
	}

	// convert the line colour
	DocColour Col;
	ConvertColour(&Out->Colour, &Col);

	// find the line width
	INT32 LineWidth = CDRDATA_WORD(Out->LineThickness) * CDRCOORDS_TO_MILLIPOINTS;

	if(LineWidth > MAX_LINE_WIDTH)
		LineWidth = MAX_LINE_WIDTH;
	
	// find the join type
	JointType JType;
	switch(CDRDATA_WORD(Out->JoinStyle))
	{
		case cdrfJOINSTYLE_SQUARE:	JType = MitreJoin;		break;
		case cdrfJOINSTYLE_ROUNDED:	JType = RoundJoin;		break;
		default:
		case cdrfJOINSTYLE_BEVEL:	JType = BevelledJoin;	break;
	}
	
	// set the cap style
	LineCapType CType;
	switch(CDRDATA_WORD(Out->EndStyle))
	{
		case cdrfENDSTYLE_BUTT:		CType = LineCapButt;	break;
		case cdrfENDSTYLE_ROUNDED:	CType = LineCapRound;	break;
		default:
		case cdrfENDSTYLE_SQUARE:	CType = LineCapSquare;	break;

	}

	// set the dash pattern
	DashRec Dash;
	DashElement DashArray[cdrfMAX_DASH_ELEMENTS];
	if(CDRDATA_WORD(Out->NDashSegments) > 0 && (CDRDATA_WORD(Out->Flags) & cdrfOUTLINEFLAGS_NODASH) == 0)
	{
		// set a dash pattern
		INT32 NSeg = CDRDATA_WORD(Out->NDashSegments);
		if(NSeg > cdrfMAX_DASH_ELEMENTS)
			NSeg = cdrfMAX_DASH_ELEMENTS;

		INT32 l;
		for(l = 0; l < NSeg; l++)
		{
			DashArray[l] = CDRDATA_WORD(Out->DashSegments[l]) * LineWidth;
		}

		Dash.Elements = NSeg;
		Dash.DashStart = 0;
		Dash.ElementData = DashArray;

		Dash.LineWidth = LineWidth;
		Dash.ScaleWithLineWidth = TRUE;
		Dash.DashID = -1;
	}
	else
	{
		// no dash pattern
		Dash = SD_SOLID;
	}

	// set the attributes
	if(!SetLineWidth(LineWidth) || !SetLineColour(Col) || !SetJoinType(JType)
			|| !SetLineCap(CType) || !SetDashPattern(Dash))
		return FALSE;

	// check for arrowheads
	if(CDRDATA_DWORD(Out->StartArrowReference) != 0 || CDRDATA_DWORD(Out->EndArrowReference) != 0)
	{
		// apply the attributes to the object
		// If not filled, then set the ignore bit on the fill attribute.
		if (ObjFilled == FALSE)
			CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = TRUE;

		// Add attributes to the path, if they are different from the default...
		AttributeManager::ApplyBasedOnDefaults(pMadeNode, CurrentAttrs);

	//	DeleteCurrentAttrs();
 	//	SetUpCurrentAttrs();

		// Enable the fill attribute again
		CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = FALSE;

		// ensure that the attributes won't be applied again
		AttrsAlreadyApplied = TRUE;

		// apply arrow heads to the path
		AddArrowheadsToPath(CDRDATA_DWORD(Out->StartArrowReference), CDRDATA_DWORD(Out->EndArrowReference),
				&Col, LineWidth, CType, JType);
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::SetLineAttr3(cdrfOffsetHeader *Object)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/95
	Inputs:		address of line data *within* the main object data (loda chunk). Can be zero
	Returns:	error flag
	Purpose:	sets the line attributes for version 3 attributes
	SeeAlso:	Filter

********************************************************************************************/

#define MAX_LINE_WIDTH	(4*72000)

BOOL CDRFilter::SetLineAttr3(cdrfOffsetHeader *Object)
{
	cdrfOutlineV3 *Out = (cdrfOutlineV3 *)FindDataInObject(Object, cdrfOBJOFFSETTYPE_LINE);
	
	if(Out == 0)
	{
		// if no outline data, or outline definitions couldn't be found, or outline
		// definition is too small, set up the default line attributes
		if(!SetLineWidth(cdrfOUTLINE_DEFAULT_WIDTH))
			return FALSE;

		PColourCMYK cmyk;
		cmyk.Cyan = cmyk.Magenta = cmyk.Yellow = 0;
		cmyk.Key = 255;

		DocColour Col;
		Col.SetCMYKValue(&cmyk);
	
		if(!SetLineColour(Col))
			return FALSE;

		return TRUE;
	}

	// check that this thingy should have an outline
	if((Out->Flags & cdrfOUTLINEFLAGSV3_STROKED) == 0)
	{
		if (!SetLineColour(DocColour(COLOUR_TRANS)))
			return FALSE;

		return TRUE;
	}

	// convert the line colour
	DocColour Col;
	ConvertColour((cdrfColour *)&Out->Colour, &Col);

	// find the line width
	INT32 LineWidth = CDRDATA_WORD(Out->LineThickness) * CDRCOORDS_TO_MILLIPOINTS;

	if(LineWidth > MAX_LINE_WIDTH)
		LineWidth = MAX_LINE_WIDTH;
	
	// find the join type
	JointType JType;
	switch(CDRDATA_WORD(Out->JoinStyle))
	{
		case cdrfJOINSTYLE_SQUARE:	JType = MitreJoin;		break;
		case cdrfJOINSTYLE_ROUNDED:	JType = RoundJoin;		break;
		default:
		case cdrfJOINSTYLE_BEVEL:	JType = BevelledJoin;	break;
	}
	
	// set the cap style
	LineCapType CType;
	switch(Out->EndStyle)  // byte
	{
		case cdrfENDSTYLE_BUTT:		CType = LineCapButt;	break;
		case cdrfENDSTYLE_ROUNDED:	CType = LineCapRound;	break;
		default:
		case cdrfENDSTYLE_SQUARE:	CType = LineCapSquare;	break;

	}

	// set the dash pattern
/*	DashRec Dash;
	DashElement DashArray[cdrfMAX_DASH_ELEMENTS];
	if((ObjectFlagsV3 & cdrfOBJFLAGSV3_NODASH) == 0 && Out->NDashSegments > 0)		// byte
	{
		// set a dash pattern
		INT32 NSeg = CDRDATA_WORD(Out->NDashSegments);
		if(NSeg > cdrfMAX_DASH_ELEMENTS)
			NSeg = cdrfMAX_DASH_ELEMENTS;

		INT32 l;
		for(l = 0; l < NSeg; l++)
		{
			DashArray[l] = Out->DashSegments[l] * LineWidth;  // byte
		}

		Dash.Elements = NSeg;
		Dash.DashStart = 0;
		Dash.ElementData = DashArray;

		Dash.LineWidth = LineWidth;
		Dash.ScaleWithLineWidth = TRUE;
		Dash.DashID = -1;
	}
	else
	{
		// no dash pattern
		Dash = SD_SOLID;
	}
*/
	// set the attributes
	if(!SetLineWidth(LineWidth) || !SetLineColour(Col) || !SetJoinType(JType)
			|| !SetLineCap(CType)/* || !SetDashPattern(Dash)*/)
		return FALSE;

	// check for arrowheads
	if(CDRDATA_DWORD(Out->StartArrowReference) != 0 || CDRDATA_DWORD(Out->EndArrowReference) != 0)
	{
		// apply the attributes to the object
		// If not filled, then set the ignore bit on the fill attribute.
		if (ObjFilled == FALSE)
			CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = TRUE;

		// Add attributes to the path, if they are different from the default...
		AttributeManager::ApplyBasedOnDefaults(pMadeNode, CurrentAttrs);

		// Enable the fill attribute again
		CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = FALSE;

		// ensure that the attributes won't be applied again
		AttrsAlreadyApplied = TRUE;

		// apply arrow heads to the path
		AddArrowheadsToPath(CDRDATA_DWORD(Out->StartArrowReference), CDRDATA_DWORD(Out->EndArrowReference),
				&Col, LineWidth, CType, JType);
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::AddAttributesToArrowheadNode(NodeRenderableBounded *N, DocColour *Col, INT32 LineWidth,
		LineCapType Cap, JointType Join);

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29 03 95
	Inputs:		node, colour and line width
	Returns:	error flag
	Purpose:	apply attributes to a arrow head node
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::AddAttributesToArrowheadNode(NodeRenderableBounded *N, DocColour *Col, INT32 LineWidth,
		LineCapType Cap, JointType Join)
{
	if(IS_A(N, NodeGroup))
	{
		// go through all the members of the group setting their attributes
		Node *pNode;

		pNode = N->FindFirstChild();

		while(pNode != 0)
		{
			if(IS_A(pNode, NodePath))
				AddAttributesToArrowheadPath((NodePath *)pNode, Col, LineWidth, Cap, Join);
			
			pNode = pNode->FindNext();
		}
	} else if(IS_A(N, NodePath))
	{
		// simply apply the attributes to this path
		AddAttributesToArrowheadPath((NodePath *)N, Col, LineWidth, Cap, Join);
	} else {
		ERROR3("Something unexpected passed to AddAttributesToArrowheadNode");
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::AddAttributesToArrowheadPath(NodePath *P, DocColour *Col, INT32 LineWidth,
		LineCapType Cap, JointType Join);

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29 03 95
	Inputs:		node, colour and line width
	Returns:	error flag
	Purpose:	apply attributes to a arrow head path
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::AddAttributesToArrowheadPath(NodePath *P, DocColour *Col, INT32 LineWidth,
		LineCapType Cap, JointType Join)
{
	if(P->InkPath.IsFilled)
	{
		// it's a filled path, so it need to be filled with the colour and have no line
		// colour
		SetPathFilled(TRUE);
		if(!SetLineColour(DocColour(COLOUR_TRANS)) || !SetFillColour(*Col))
			return FALSE;
	} else {
		// it's not a filled colour, so don't fill it and stroke it with the line width and colour
		if(!SetLineColour(*Col) || !SetLineWidth(LineWidth) || !SetLineCap(Cap) || !SetJoinType(Join))
			return FALSE;
	}

	// apply the attributes to the object
	// If not filled, then set the ignore bit on the fill attribute.
//	if (P->InkPath.IsFilled == FALSE)
//		CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = TRUE;
//	else
		CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = FALSE;

	// Add attributes to the path, if they are different from the default...
	BOOL Result = AttributeManager::ApplyBasedOnDefaults(P, CurrentAttrs);

	//	DeleteCurrentAttrs();
 	//	SetUpCurrentAttrs();

	// Enable the fill attribute again
	CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = FALSE;

	return Result;
}
		
/********************************************************************************************

>	BOOL CDRFilter::DoesPathNeedArrowheads(NodePath *P)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29 03 95
	Inputs:		NodePath to search
	Returns:	whether the path need some arrowheads
	Purpose:	To find out whether a path need attributes
	SeeAlso:	Filter

********************************************************************************************/
		
BOOL CDRFilter::DoesPathNeedArrowheads(NodePath *P)
{
	// if the number of end points is greater than 0, then it needs some arrowheads
	if(P->InkPath.GetNumEndPoints() > 0)
		return TRUE;

	return FALSE;
}


/********************************************************************************************

>	BOOL CDRFilter::TransformArrowhead(NodePath *N, DocCoord *Point, DocCoord *Other,
		BOOL Start, INT32 LineWidth, INT32 Distance)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29 03 95
	Inputs:		The path to transform, the This coordsinate (the coordinate to apply it to) and the 
				previous coordinate (the one before or after it), whether it's a start one or not
				and the line width and the distance to shift it from the arrow definition. 
	Returns:	error flag
	Purpose:	transform an arrowhead path to be the correct size and orientation
	SeeAlso:	Filter

********************************************************************************************/
		
BOOL CDRFilter::TransformArrowhead(NodeRenderableBounded *N, DocCoord *Point, DocCoord *Other, BOOL Start,
		 INT32 LineWidth, INT32 Distance)
{
	// first of all, work out how much we need to scale this thingy by
	double Scale = (double)LineWidth / (double)cdrfARROWHEAD_LINEWIDTH;

	// work out the adj and opp side of the triangle we're going to rotate around
	double Adj = (double)Point->x - (double)Other->x;
	double Opp = (double)Point->y - (double)Other->y;
	
	// work out the hyp of the triangle
	double Hyp = sqrt((Adj * Adj) + (Opp * Opp));

	// how far we need to move the arrowhead by
	double Move = Scale * (double)Distance;

	// calculate a matrix
	double a, b, c, d, e, f;
	if(Hyp == 0)
	{
		// default thing which shouldn't do too much hard - avoid div by zero
		a = Scale;
		b = 0;
		c = 0;
		d = Scale;
		e = Point->x + Move;
		f = Point->y;
	} else {
		double cosTheta = (Adj / Hyp);
		double sinTheta = (Opp / Hyp);

		a = Scale * cosTheta;
		b = Scale * sinTheta;
		c = 0 - (Scale * sinTheta);
		d = Scale * cosTheta;
		e = Point->x + (Move * cosTheta);
		f = Point->y + (Move * sinTheta);
	}

	// knock up a matrix
	Matrix M = Matrix(a, b, c, d, (INT32)e, (INT32)f);

	// need to flip it over?
	if(Start)
	{
		Matrix F = Matrix(1, 0, 0, -1, 0, 0);

		M = F * M;
	}

	// transform the node
	Trans2DMatrix Trans(M);

	N->Transform(Trans);

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::AddArrowheadsToPath(DWORD StartArrow, DWORD EndArrow, DocColour *Col, INT32 LineWidth,
			LineCapType Cap, JointType Join)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29 03 95
	Inputs:		
	Returns:	error flag
	Purpose:	adds arrowheads to a path. Changes pMadeNode from a path to a group
	SeeAlso:	Filter

********************************************************************************************/
		
BOOL CDRFilter::AddArrowheadsToPath(DWORD StartArrow, DWORD EndArrow, DocColour *Col, INT32 LineWidth,
			LineCapType Cap, JointType Join)
{
	if(!IS_A(pMadeNode, NodePath))
		return TRUE;			// don't apply arrowheads to non path things

	NodePath *pPath = (NodePath *)pMadeNode;

	// check that the path actaully needs some arrow heads
	if(!DoesPathNeedArrowheads(pPath) || (StartArrow == 0 && EndArrow == 0))
		return TRUE;
	
	// get pointers to nodes of the arrow heads to copy
	NodeRenderableBounded *StartA = 0;
	NodeRenderableBounded *EndA = 0;
	INT32 StartDistance;
	INT32 EndDistance;
	if(StartArrow != 0)
	{
		BOOL NotPresent;
		StartA = Arrowheads.GetConvertedNode(StartArrow, &StartDistance, &NotPresent);

		if(StartA == 0 && NotPresent == FALSE)
			return FALSE;	// error occurred
	}
	if(EndArrow != 0)
	{
		BOOL NotPresent;
		EndA = Arrowheads.GetConvertedNode(EndArrow, &EndDistance, &NotPresent);

		if(EndA == 0 && NotPresent == FALSE)
			return FALSE;	// error occurred
	}

	// check that enough arrowheads were found to do something with
	if(StartA == 0 && EndA == 0)
		return TRUE;

	// get rid of any dash patterns
	DashRec NoDash;
	NoDash = SD_SOLID;
	SetDashPattern(NoDash);

	// get some info about the path
	DocCoord *Coords = pPath->InkPath.GetCoordArray();
	PathVerb *Verbs = pPath->InkPath.GetVerbArray();
	INT32 NCoords = pPath->InkPath.GetNumCoords();
	INT32 SubPathStart = -1; 		// the number of the coord the sub path starts on
	INT32 SubPathEnd = -1;			// the number of the coord the sub path ends on

	// set up a pointers for the nodes we're about to create - it points to the made
	// at first, but shuffles on as nodes are copied. You see, we have to copy nodes which
	// are attached to something
	NodeRenderableBounded *ThisNode = pPath;

	// work out the trim distances
	double StartTrimDistance;
	double EndTrimDistance;
	if(StartA != 0)
		StartTrimDistance = ((double)LineWidth / (double)cdrfARROWHEAD_LINEWIDTH) * (double)StartDistance;
	if(EndA != 0)
		EndTrimDistance = ((double)LineWidth / (double)cdrfARROWHEAD_LINEWIDTH) * (double)EndDistance;

	// run through all the coords
	INT32 c;
	for(c = 0; c < NCoords; c++)
	{
		UINT32 Verb = Verbs[c] & ~PT_CLOSEFIGURE;
		BOOL CloseHere = ((Verbs[c] & PT_CLOSEFIGURE) != 0)?TRUE:FALSE;
		
		// if the verb is a close figure, this sub path is close, so we can't add arrowheads
		// to it. Invalidate the path start to avoid getting any arrowheads
		if(CloseHere)
			SubPathStart = -1;
		
		// if we get a new moveto and the sub path start is valid then we have a applying arrowheads
		// situation
		if((Verb == PT_MOVETO || ((c == (NCoords - 1)) && !CloseHere)) && c != 0)
		{
			// set the sub path end
			if(Verb == PT_MOVETO)
			{
				// if it's a moveto, then the sub path end was at the previous coord
				SubPathEnd = c - 1;
			} else {
				// if it wasn't, then it's this one (end of path)
				SubPathEnd = c;
			}
		}

		// have we got a valid start path marker?

		if(SubPathStart != -1 && SubPathEnd != -1)
		{
			// OK, now we need to trim the sub path
			if(StartA != 0)
			{
				// trim the start of the path if the next element is a line
				if((Verbs[SubPathStart + 1] & ~PT_CLOSEFIGURE) == PT_LINETO)
				{
					double dx, dy;
					dx = Coords[SubPathStart + 1].x - Coords[SubPathStart].x;
					dy = Coords[SubPathStart + 1].y - Coords[SubPathStart].y;
					double len = sqrt((dx * dx) + (dy * dy));
					// if the resulting path line leaves enough room, trim it
					if((len - StartTrimDistance) >= 32.0)
					{
						double factor = StartTrimDistance / len;
						dx *= factor;
						Coords[SubPathStart].x += (INT32)dx;
						dy *= factor;
						Coords[SubPathStart].y += (INT32)dy;
					}
				}
				
				// now pop an arrowhead on the resulting point

				// first, make a copy of our node
				if(!StartA->CopyNode(ThisNode, NEXT))
					return FALSE;	// error!

				ThisNode = (NodeRenderableBounded *)ThisNode->FindNext();
				ERROR3IF(ThisNode == 0, "CopyNode returned a zero path but didn't complain");
				ERROR3IF(ThisNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)) == FALSE, "copied thing isn't a renderable bounded node");

				// transform it
				if(!TransformArrowhead(ThisNode, &Coords[SubPathStart], &Coords[SubPathStart + 1],
						TRUE, LineWidth, StartDistance))
					return FALSE;

				// and apply attributes to it
				if(!AddAttributesToArrowheadNode(ThisNode, Col, LineWidth, Cap, Join))
					return FALSE;		
			}

			// and the same for the end path
			if(EndA != 0)
			{
				if((Verbs[SubPathEnd] & ~PT_CLOSEFIGURE) == PT_LINETO)
				{
					double dx, dy;
					dx = Coords[SubPathEnd].x - Coords[SubPathEnd - 1].x;
					dy = Coords[SubPathEnd].y - Coords[SubPathEnd - 1].y;
					double len = sqrt((dx * dx) + (dy * dy));
					// if the resulting path line leaves enough room, trim it
					if((len - EndTrimDistance) >= 32.0)
					{
						double factor = EndTrimDistance / len;
						dx *= factor;
						Coords[SubPathEnd].x += (INT32)dx;
						dy *= factor;
						Coords[SubPathEnd].y += (INT32)dy;
					}
				}
				
				// now pop an arrowhead on the resulting point

				// first, make a copy of our node
				if(!EndA->CopyNode(ThisNode, NEXT))
					return FALSE;	// error!

				ThisNode = (NodeRenderableBounded *)ThisNode->FindNext();
				ERROR3IF(ThisNode == 0, "CopyNode returned a zero path but didn't complain");
				ERROR3IF(ThisNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)) == FALSE, "copied thing isn't a renderable bounded node");

				// transform it
				if(!TransformArrowhead(ThisNode, &Coords[SubPathEnd], &Coords[SubPathEnd - 1],
						FALSE, LineWidth, EndDistance))
					return FALSE;

				// and apply attributes to it
				if(!AddAttributesToArrowheadNode(ThisNode, Col, LineWidth, Cap, Join))
					return FALSE;				
			}
		
		}
			
		// set the new sub path start marker and invalidate the end point marker
		if(Verb == PT_MOVETO)
		{
			SubPathStart = c;
			SubPathEnd = -1;
		}
	}


	// ensure that the bounding rectangle is updated
	pPath->InvalidateBoundingRect();

	// get a new group node
	NodeGroup *pGroup = new NodeGroup;
	if(pGroup == 0)
		return FALSE;
	
	// attach everything to it
	pMadeNode->InsertChainSimple(pGroup, FIRSTCHILD);

	// and make the made node the group
	pMadeNode = pGroup;
	
	return TRUE;
}


/********************************************************************************************

>	BOOL CDRArrowheadStore::AddChunkToStore(RIFFFile *RIFF)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22 03 95
	Inputs:		A RIFFFile object
	Returns:	error flag
	Purpose:	Stores arrowhead chunks for the CDRFilter class
	SeeAlso:	CDRFilter, CDRAttributeStore

********************************************************************************************/

BOOL CDRArrowheadStore::AddChunkToStore(RIFFFile *RIFF)
{
	if(RIFF->GetObjType() != RIFFOBJECTTYPE_CHUNK)
	{
		ERROR2(FALSE, "CDRAttributeStore::AddChunkToStore called without a chunk in the RIFFFile");
	}

	// get a new item obect
	CDRArrowheadStoredItem *Item = new CDRArrowheadStoredItem;

	if(Item == 0)
		return FALSE;

	// get the data of the RIFF chunk
	if(!Item->Aquire(RIFF))
	{
		delete Item;
		return FALSE;
	}

	Item->Size = RIFF->GetObjSize();

	// and add the new item to the list
	AddTail(Item);

	return TRUE;
}


/********************************************************************************************

>	NodeRenderableBounded *CDRArrowheadStore::GetConvertedNode(DWORD Reference, INT32 *Distance, BOOL *NotPresent)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29 03 95
	Inputs:		a DWORD reference
	Returns:	node pointer or 0 for an error
	Purpose:	gets a converted node for the arrow head which should be copied and
				transformed. Converted nodes are cached. If *NotPresent is TRUE when 0 is
				returned then the arrowhead couldn't be found rather than an error occuring
	SeeAlso:	CDRFilter, CDRAttributeStore

********************************************************************************************/

#define GCN_LASTSUB_NONE	0
#define GCN_LASTSUB_CLOSED	1
#define GCN_LASTSUB_OPEN	2

NodeRenderableBounded *CDRArrowheadStore::GetConvertedNode(DWORD Reference, INT32 *Distance, BOOL *NotPresent)
{
	// set up the not present thingy
	*NotPresent = FALSE;
	
	// find the correct chunk
	CDRArrowheadStoredItem *Item;
	INT32 Size;

	if(IsEmpty())
		return 0;		// no items in the list
	
	Item = (CDRArrowheadStoredItem *)GetHead();

	// scan though the list looking for the reference
	while(Item != 0)
	{
		if(CDRDATA_DWORD(*((DWORD *)(Item->Block))) == Reference)
		{
			Size = Item->Size;
			break;
		}

		Item = (CDRArrowheadStoredItem *)GetNext(Item);
	}
	
	// did we find a chunk?
	if(Item == 0)
	{
		*NotPresent = TRUE;
		return 0;
	}

	// locate the coordinates
	cdrfArrowhead *Arrow = (cdrfArrowhead *)Item->Block;
	cdrfCoord *Coords = (cdrfCoord *)(Item->Block + CDRDATA_WORD(Arrow->CoordsOffset) + cdrfARROWHEAD_COORDOFF_CORRECT);

	// store the distance from the definitons
	*Distance = CDRDATA_SWORD(Arrow->Distance);

	// check to see if a cached pointer is available
	if(Item->pNode != 0)
		return Item->pNode;

	// OK, convert that arrowhead

	// this is not particularly pleasant. We need to scan though the coords creating a path,
	// each sub path must be a seperate path unless the next subpath has the same closedness
	// as the previous one, in which case it's a sub path.
	// this is because closed sub paths have different attributes to open ones. It's a nasty
	// system, and rather overcomplicated, but never mind.

	INT32 ThisType = GCN_LASTSUB_NONE;		// the type of this subpath
	INT32 LastType = GCN_LASTSUB_NONE;		// the type of the last subpath
	
	NodePath *FirstPath = 0;				// the first path in my set of paths
	NodePath *LastPath = 0;					// the last path in my set of paths
	NodePath *ThisPath = 0;					// the path I'm currently working on

	// check that the first node type is a move to avoid stuffing up my next bit
	if((Arrow->NodeTypes[0] & cdrfPATHCOORDTYPE_MASK) != cdrfPATHCOORDTYPE_MOVE)
	{
		// for now, if the first element isn't a move, pretend that it doesn't exist
		*NotPresent = TRUE;
		return 0;
	}

	INT32 CoordType;

	INT32 l;
	
	UINT32 Control1 = 0;		// number of first control point
	UINT32 Control2 = 0;		// of second
	DocCoord co, cn1, cn2;	// coordinates
	PathFlags Flags;

	INT32 NNodes = CDRDATA_WORD(Arrow->NNodes);
	BOOL NeedMoveTo = TRUE;

	// convert all the coordinates
	for(l = 0; l < NNodes; l++)
	{
		CoordType = Arrow->NodeTypes[l] & cdrfPATHCOORDTYPE_MASK;

		if(CoordType == cdrfPATHCOORDTYPE_MOVE || l == 0)
		{
			// start a new path!
			LastType = ThisType;

			// first of all, what type of path is this?
			if((Arrow->NodeTypes[l] & cdrfPATHCOORDATTR_CLOSE) != 0)
				ThisType = GCN_LASTSUB_CLOSED;
			else
				ThisType = GCN_LASTSUB_OPEN;

			// OK, do we need to start a new path?
			if(ThisType != LastType)
			{
				// yep, attach the last one we did
				if(ThisPath != 0)
				{
					// check that the path is OK
					if(!ThisPath->InkPath.EnsureValid())
					{
						// no, it's completely knackered
						delete ThisPath;
						ThisPath = 0;
					} else {	
						// finish off the path
						ThisPath->InvalidateBoundingRect();				

						if(FirstPath == 0)
							FirstPath = ThisPath;

						if(LastPath != 0)
							ThisPath->AttachNode(LastPath, NEXT);

						LastPath = ThisPath;

						ThisPath = 0;
					}
				}

				// get a new path
				ThisPath = new NodePath;

				if(ThisPath == 0)
					return 0;			// no path created. A bit of a pity

				// set it up ready for conversion
				if(!ThisPath->SetUpPath())
					return 0;
		
				ThisPath->InkPath.FindStartOfPath();

				// set whether it's filled or not
				if(ThisType == GCN_LASTSUB_CLOSED)
				{
					ThisPath->InkPath.IsFilled = TRUE;
					ThisPath->InkPath.IsStroked = FALSE;
				} else {
					ThisPath->InkPath.IsFilled = FALSE;
					ThisPath->InkPath.IsStroked = TRUE;
				}
				
			}

		}

		co.x = CDRDATA_SWORD(Coords[l].X);
		co.y = CDRDATA_SWORD(Coords[l].Y);

		// ensure we get all the move tos we need.
		if(NeedMoveTo && CoordType != cdrfPATHCOORDTYPE_MOVE)
		{
			if(!ThisPath->InkPath.InsertMoveTo(co))
				return 0;
		}

		NeedMoveTo = FALSE;

		// convert the coordinates
		switch(CoordType)
		{
			case cdrfPATHCOORDTYPE_MOVE:
				// add a move to this path
				if(!ThisPath->InkPath.InsertMoveTo(co))
					return 0;
				break;
			
			case cdrfPATHCOORDTYPE_LINETO:
				// add a line to this coord to the path
				if(!ThisPath->InkPath.InsertLineTo(co))
					return 0;
				break;
			
			case cdrfPATHCOORDTYPE_CURVE:
				// check we have some control points for this curve
				// a control point cannot be the first coord, so it's OK to check against 0
				if(Control1 == 0 || Control2 == 0)
				{
TRACEUSER( "Ben", _T("No control points for curve element\n"));
					return 0;
				}

				// convert the control points
				cn1.x = CDRDATA_SWORD(Coords[Control1].X);
				cn1.y = CDRDATA_SWORD(Coords[Control1].Y);
				cn2.x = CDRDATA_SWORD(Coords[Control2].X);
				cn2.y = CDRDATA_SWORD(Coords[Control2].Y);
				
				// create the curve
				Flags.IsSelected = FALSE;
				Flags.IsSmooth = Flags.IsRotate = ((Arrow->NodeTypes[l] & cdrfPATHCOORDATTR_SMOOTH) != 0)?TRUE:FALSE;
				Flags.IsEndPoint;
				
				// insert it into the path
				if(!ThisPath->InkPath.InsertCurveTo(cn1, cn2, co, &Flags))
					return 0;
				break;

			case cdrfPATHCOORDTYPE_CONTROL:
				// shuffle the control points we've already got and add the new one
				Control1 = Control2;
				Control2 = l;
				break;
			
			default:
				break;
		}
		
		// is this a close subpath situtation?
		if((Arrow->NodeTypes[l] & cdrfPATHCOORDATTR_CLOSE) != 0)
		{
			// close the sub path
			if(CoordType != cdrfPATHCOORDTYPE_MOVE)
			{
				if(!ThisPath->InkPath.CloseSubPath())
					return 0;

				// check the next coord type
				NeedMoveTo = TRUE;
			}
		}
	}

	// finish off the last path
	ThisPath->InvalidateBoundingRect();				

	// finish off this path we're doing
	if(ThisPath != 0)
	{
		if(FirstPath == 0)
			FirstPath = ThisPath;

		if(LastPath != 0)
			ThisPath->AttachNode(LastPath, NEXT);

		LastPath = ThisPath;

		ThisPath = 0;
	}

	// make up a nice group if there's more than one path
	NodeRenderableBounded *ToInsert = 0;
	if(FirstPath == LastPath)
	{
		ToInsert = FirstPath;
	} else {
		ToInsert = new NodeGroup;

		if(ToInsert == 0)
			return 0;

		FirstPath->InsertChainSimple(ToInsert, FIRSTCHILD);
	}

	Item->pNode = ToInsert;

	return ToInsert;
}

/********************************************************************************************

>	CDRArrowheadStoredItem::CDRArrowheadStoredItem(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29 03 95
	Inputs:		
	Returns:	
	Purpose:	Constructor
	SeeAlso:	CDRAttributeStore, CDRAttributeStoredItem

********************************************************************************************/

CDRArrowheadStoredItem::CDRArrowheadStoredItem(void)
{
	pNode = 0;
}


/********************************************************************************************

>	CDRArrowheadStoredItem::~CDRArrowheadStoredItem(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29 03 95
	Inputs:		
	Returns:	
	Purpose:	Destructor
	SeeAlso:	CDRAttributeStore, CDRAttributeStoredItem

********************************************************************************************/

CDRArrowheadStoredItem::~CDRArrowheadStoredItem(void)
{
	if(pNode != 0)
		pNode->CascadeDelete();

	delete pNode;
}


/*******************************************************************************************/

typedef struct {
	BYTE C, M, Y, K;
	TCHAR *Name;
} StandardCDRColour;

static StandardCDRColour StandardCDRColours[] = {
	{0,	0,	0,	100,	"Black"},
	{0,	0,	0,	90,	"90% Black"},
	{0,	0,	0,	80,	"80% Black"},
	{0,	0,	0,	70,	"70% Black"},
	{0,	0,	0,	60,	"60% Black"},
	{0,	0,	0,	50,	"50% Black"},
	{0,	0,	0,	40,	"40% Black"},
	{0,	0,	0,	30,	"30% Black"},
	{0,	0,	0,	20,	"20% Black"},
	{0,	0,	0,	10,	"10% Black"},
	{0,	0,	0,	0,	"White"},
	{100,	100,	0,	0,	"Blue"},
	{100,	0,	0,	0,	"Cyan"},
	{100,	0,	100,	0,	"Green"},
	{0,	0,	100,	0,	"Yellow"},
	{0,	100,	100,	0,	"Red"},
	{0,	100,	0,	0,	"Magenta"},
	{20,	80,	0,	20,	"Purple"},
	{0,	60,	100,	0,	"Orange"},
	{0,	40,	20,	0,	"Pink"},
	{0,	20,	20,	60,	"Dark Brown"},
	{20,	20,	0,	0,	"Powder Blue"},
	{40,	40,	0,	0,	"Pastel Blue"},
	{60,	40,	0,	0,	"Baby Blue"},
	{60,	60,	0,	0,	"Electric Blue"},
	{40,	40,	0,	20,	"Twilight Blue"},
	{60,	40,	0,	40,	"Navy Blue"},
	{40,	40,	0,	60,	"Deep Navy Blue"},
	{40,	20,	0,	40,	"Desert Blue"},
	{100,	20,	0,	0,	"Sky Blue"},
	{40,	0,	0,	0,	"Ice Blue"},
	{20,	0,	0,	20,	"Light BlueGreen"},
	{20,	0,	0,	40,	"Ocean Green"},
	{20,	0,	0,	60,	"Moss Green"},
	{20,	0,	0,	80,	"Dark Green"},
	{40,	0,	20,	60,	"Forest Green"},
	{60,	0,	40,	40,	"Grass Green"},
	{40,	0,	20,	40,	"Kentucky Green"},
	{60,	0,	40,	20,	"Light Green"},
	{60,	0,	60,	20,	"Spring Green"},
	{60,	0,	20,	0,	"Turquoise"},
	{60,	0,	20,	20,	"Sea Green"},
	{20,	0,	20,	20,	"Faded Green"},
	{20,	0,	20,	0,	"Ghost Green"},
	{40,	0,	40,	0,	"Mint Green"},
	{20,	0,	20,	40,	"Army Green"},
	{20,	0,	40,	40,	"Avocado Green"},
	{20,	0,	60,	20,	"Martian Green"},
	{20,	0,	40,	20,	"Dull Green"},
	{40,	0,	100,	0,	"Chartreuse"},
	{20,	0,	60,	0,	"Moon Green"},
	{0,	0,	20,	80,	"Murky Green"},
	{0,	0,	20,	60,	"Olive Drab"},
	{0,	0,	20,	40,	"Khaki"},
	{0,	0,	40,	40,	"Olive"},
	{0,	0,	60,	20,	"Banana Yellow"},
	{0,	0,	60,	0,	"Light Yellow"},
	{0,	0,	40,	0,	"Chalk"},
	{0,	0,	20,	0,	"Pale Yellow"},
	{0,	20,	40,	40,	"Brown"},
	{0,	40,	60,	20,	"Red Brown"},
	{0,	20,	60,	20,	"Gold"},
	{0,	60,	80,	0,	"Autumn Orange"},
	{0,	40,	80,	0,	"Light Orange"},
	{0,	40,	60,	0,	"Peach"},
	{0,	20,	100,	0,	"Deep Yellow"},
	{0,	20,	40,	0,	"Sand"},
	{0,	20,	40,	60,	"Walnut"},
	{0,	60,	60,	40,	"Ruby Red"},
	{0,	60,	80,	20,	"Brick Red"},
	{0,	60,	60,	0,	"Tropical Pink"},
	{0,	40,	40,	0,	"Soft Pink"},
	{0,	20,	20,	0,	"Faded Pink"},
	{0,	40,	20,	40,	"Crimson"},
	{0,	60,	40,	20,	"Regal Red"},
	{0,	60,	20,	20,	"Deep Rose"},
	{0,	100,	60,	0,	"Neon Red"},
	{0,	60,	40,	0,	"Deep Pink"},
	{0,	80,	40,	0,	"Hot Pink"},
	{0,	40,	20,	20,	"Dusty Rose"},
	{0,	40,	0,	60,	"Plum"},
	{0,	60,	0,	40,	"Deep Violet"},
	{0,	40,	0,	0,	"Light Violet"},
	{0,	40,	0,	20,	"Violet"},
	{0,	20,	0,	40,	"Dusty Plum"},
	{0,	20,	0,	20,	"Pale Purple"},
	{20,	60,	0,	20,	"Majestic Purple"},
	{20,	80,	0,	0,	"Neon Purple"},
	{20,	60,	0,	0,	"Light Purple"},
	{20,	40,	0,	20,	"Twilight Violet"},
	{20,	40,	0,	0,	"Easter Purple"},
	{20,	40,	0,	60,	"Deep Purple"},
	{20,	40,	0,	40,	"Grape"},
	{40,	60,	0,	0,	"Blue Violet"},
	{40,	100,	0,	0,	"Blue Purple"},
	{40,	80,	0,	20,	"Deep River"},
	{60,	80,	0,	0,	"Deep Azure"},
	{40,	60,	0,	40,	"Storm Blue"},
	{60,	80,	0,	20,	"Deep Blue"},
	{100,	100,	100,	100,	"100C100M100Y100K"}
};

/********************************************************************************************

>	BOOL CDRFilter::AddStandardColourSet(Document *pDocument)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/06/95
	Inputs:		
	Returns:	
	Purpose:	Adds the standard set of colours to a Corel file

********************************************************************************************/

BOOL CDRFilter::AddStandardColourSet(Document *pDocument)
{
	ColourListComponent *pColours = 0;

	DocComponent *pComponent = pDocument->EnumerateDocComponents(NULL);

	while (pComponent != NULL)
	{
		// If this is the colour component, remember it
		if (pComponent->GetRuntimeClass() == CC_RUNTIME_CLASS(ColourListComponent))
		{
			pColours = (ColourListComponent *) pComponent;
			break;
		}

		// Look for next doc component
		pComponent = pDocument->EnumerateDocComponents(pComponent);
	}

	if(pColours == 0)
		return TRUE;			// can't be bothered to complain nicely about this very unlikely error

	// get an importedcolours object
	ImportedColours *pNewColours = new ImportedColours(pColours, FALSE);
	if(pNewColours == 0 || !pNewColours->Init())
		return FALSE;

	BOOL ok = TRUE;

	// add colours to it...
	ColourCMYK NewColour;
	for(INT32 l = 0; l < (sizeof(StandardCDRColours) / sizeof(StandardCDRColour)); l++)
	{
		NewColour.Cyan = ((double)StandardCDRColours[l].C) / 100;
		NewColour.Magenta = ((double)StandardCDRColours[l].M) / 100;
		NewColour.Yellow = ((double)StandardCDRColours[l].Y) / 100;
		NewColour.Key = ((double)StandardCDRColours[l].K) / 100;
		
		String_64 ColNameS(StandardCDRColours[l].Name);
		if(!pNewColours->AddColour(&ColNameS, &NewColour))
		{
			ok = FALSE;
			break;
		}
	}

	// add the colours to the document
	if(ok)
	{
		pNewColours->AddColoursToDocument();
	}
	else
	{
		pNewColours->DestroyColours();
	}

	// delete the imported colours object
	delete pNewColours;

	return ok;
}

