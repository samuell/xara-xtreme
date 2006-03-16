// $Id: sgstroke.cpp 662 2006-03-14 21:31:49Z alex $
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
// SGStroke.cpp - definitions for line gallery classes for handling new-style Stroke attributes

#include "camtypes.h"

#include "sgstroke.h"

#include "app.h"
#include "lineattr.h"
#include "paths.h"
#include "ppstroke.h"
#include "rndrgn.h"
#include "strkattr.h"

#include "dlgcol.h"

#include "qualattr.h"
#include "ppvecstr.h"

#include "ndoptmz.h"
//#include "rik.h"
#include "brushmsg.h"

CC_IMPLEMENT_DYNAMIC(StrokeTypeItem, LineAttrItem);
CC_IMPLEMENT_DYNAMIC(VarWidthItem,	 LineAttrItem);


#define new CAM_DEBUG_NEW


/********************************************************************************************

>	StrokeTypeItem::StrokeTypeItem(StrokeTypeAttrValue *pStroke, const String &strDescription)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97
	Inputs:		pStroke			- a pointer to an attribute to be used for this
								  stroke type. This now belongs to the StrokeTypeItem,
								  so you must not delete it
				strDescription	- a text description of this user-interface item
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a user-interface item representing a stroke type

********************************************************************************************/

StrokeTypeItem::StrokeTypeItem(StrokeTypeAttrValue *pStroke, const String &strDescription)
			  : LineAttrItem(strDescription, BELOW)
{
	ERROR3IF(pStroke == NULL, "Illegal NULL param");
	pStrokeDef = pStroke;
}



/********************************************************************************************

>	StrokeTypeItem::~StrokeTypeItem()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97

	Purpose:	Destructor

********************************************************************************************/

StrokeTypeItem::~StrokeTypeItem()
{
	if (pStrokeDef != NULL)
		delete pStrokeDef;
}



/********************************************************************************************

>	virtual void StrokeTypeItem::SetAttributes(RenderRegion* pRegion) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97

	Inputs:		pRegion	- the render-region to draw into

	Purpose:	Sets the stroke typeattributes in the given render-region to render this
				item correctly.

********************************************************************************************/

void StrokeTypeItem::SetAttributes(RenderRegion* pRegion) const
{
	// Set a 16pt line width - nice & thick
	pRegion->SetLineWidth(16000);

	// Set the line width (this overrides the "default" line width set by the caller).
	pStrokeDef->Render(pRegion, FALSE);
}



/********************************************************************************************

>	virtual NodeAttribute *StrokeTypeItem::CreateNewAttribute(BOOL) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97

	Returns:	A pointer to a NodeAttribute-derivative that this user-interface item
				represents, ie. a line-width attribute that can be passed to the
				attribute manager.

	Purpose:	Create a NodeAttribute representing the attribute this item applies

********************************************************************************************/

NodeAttribute *StrokeTypeItem::CreateNewAttribute(BOOL) const
{
	return(pStrokeDef->MakeNode());
}



/********************************************************************************************

>	virtual MILLIPOINT StrokeTypeItem::GetWidth() const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97
	Inputs:		-
	Outputs:	-
	Returns:	The width of a line-width item, in millipoints.
	Purpose:	Called by the line gallery formatting code.

********************************************************************************************/

MILLIPOINT StrokeTypeItem::GetWidth() const
{
	return 2 * LineAttrItem::GetWidth() / 3;
}



/********************************************************************************************

>	virtual CCRuntimeClass** StrokeTypeItem::GetAttribRuntimeClasses() const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97
	Inputs:		-
	Outputs:	-
	Returns:	The run-time class of the line attribute this type of display item
				represents.
	Purpose:	Used when searching for display items representing the selection's current
				attributes.

********************************************************************************************/

CCRuntimeClass** StrokeTypeItem::GetAttribRuntimeClasses() const
{
	static CCRuntimeClass* aprtc[] =
	{
		CC_RUNTIME_CLASS(AttrStrokeType),
		NULL
	};

	return aprtc;
}



/********************************************************************************************

>	virtual BOOL StrokeTypeItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97
	Inputs:		pOtherAttrib		the attribute value to compare this item to
	Outputs:	-
	Returns:	TRUE if this item represents the same attribute value as the given
				attribute.
	Purpose:	Used when searching for display items representing the selection's current
				attributes.

********************************************************************************************/

BOOL StrokeTypeItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const
{
	if (*ppOtherAttribs == NULL)	// When multiple attribs are selected, this will be NULL
		return(FALSE);

	AttrStrokeType* pAttr = (AttrStrokeType *) *ppOtherAttribs;
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(AttrStrokeType)),
				"StrokeTypeItem::IsEqualValueToAny - unexpected object type");

	if (pAttr != NULL)
	{
		StrokeTypeAttrValue *pOther = (StrokeTypeAttrValue *) pAttr->GetAttributeValue();
		return((*pStrokeDef) == (*pOther));
	}

	return(FALSE);
}



/********************************************************************************************

>   virtual MILLIPOINT StrokeTypeItem::GetHorzGap() const

	Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:    3/3/97

	Returns:    The horizontal gap between this item and another item of the same type, in
				millipoints.
	Purpose:    Called by the formatting code to determine the gap between items of this type

********************************************************************************************/

MILLIPOINT StrokeTypeItem::GetHorzGap() const
{
	// I would like the gap to be the same all the way around
	return(GetVertGap());
}


/********************************************************************************************

>	virtual BOOL StrokeTypeItem::IsRepeating()

	Author:     Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:    6/3/97

	Returns:    TRUE if the stroke is a repeating stroke.
	Purpose:    Determins if the stroke repeats

********************************************************************************************/

BOOL StrokeTypeItem::IsRepeating()
{
	BOOL Repeating = FALSE;
	StrokeDefinition *pStroke = GetStrokeDefinition();
	if (pStroke != NULL)
	{
		Repeating = pStroke->IsRepeating();
	}

	return Repeating;
}

/********************************************************************************************

>	virtual INT32 StrokeTypeItem::NumRepeats()

	Author:     Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:    6/3/97

	Returns:    Number of times the brush should be repeated along the line. If 0, then we
				work out the best based on the aspect ratio and line thickness.
	Purpose:    See Returns.

********************************************************************************************/

INT32 StrokeTypeItem::NumRepeats()
{
	INT32 Repeats = 0;
	StrokeDefinition *pStroke = GetStrokeDefinition();
	if (pStroke != NULL)
	{
		Repeats = pStroke->NumRepeats();
	}

	return Repeats;
}

/********************************************************************************************

>	virtual BOOL StrokeTypeItem::OverridesFill()

	Author:     Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:    6/3/97

	Returns:    TRUE if the stroke's overrides fill flag is set
	Purpose:    Determins if the stroke overrides the fill or not

********************************************************************************************/

BOOL StrokeTypeItem::OverridesFill()
{
	BOOL OverridesFill = FALSE;
	StrokeDefinition *pStroke = GetStrokeDefinition();
	if (pStroke != NULL)
	{
		OverridesFill = pStroke->OverrideFill();
	}

	return OverridesFill;
}

/********************************************************************************************

>	virtual BOOL StrokeTypeItem::OverridesTrans()

	Author:     Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:    6/3/97

	Returns:    TRUE if the stroke's overrides transparency flag is set
	Purpose:    Determins if the stroke overrides transparency or not

********************************************************************************************/

BOOL StrokeTypeItem::OverridesTrans()
{
	BOOL OverridesTrans = FALSE;
	StrokeDefinition *pStroke = GetStrokeDefinition();
	if (pStroke != NULL)
	{
		OverridesTrans = pStroke->OverrideTrans();
	}

	return OverridesTrans;
}

/********************************************************************************************

>	virtual StrokeDefinition *StrokeTypeItem::GetStrokeDefinition()

	Author:     Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:    6/3/97

	Returns:    The StrokeDefinition for this item
	Purpose:    Get the StrokeDefinition for this item

********************************************************************************************/

StrokeDefinition *StrokeTypeItem::GetStrokeDefinition()
{
	StrokeHandle Handle = GetStrokeHandle();
	if(Handle != -1)
		return StrokeComponent::FindStroke(Handle);

	return NULL;
}

/********************************************************************************************

>	virtual StrokeHandle StrokeTypeItem::GetStrokeHandle()

	Author:     Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:    6/3/97

	Returns:    The StrokeHandle for this item, or StrokeHandle_NoStroke if strife
	Purpose:    Get the StrokeHandle for this item

********************************************************************************************/

StrokeHandle StrokeTypeItem::GetStrokeHandle()
{
	StrokeTypeAttrValue *pAttrValue = GetStrokeTypeAttrValue();
	if(pAttrValue != NULL)
	{
		PathProcessorStroke *pProcessor = pAttrValue->GetPathProcessor();
		if (pProcessor != NULL && pProcessor->IsKindOf(CC_RUNTIME_CLASS(PathProcessorStrokeVector)))
		{
			StrokeHandle Handle = ((PathProcessorStrokeVector *)pProcessor)->GetStrokeDefinition();
			return Handle;
		}
	}

	return StrokeHandle_NoStroke;
}


/********************************************************************************************

>	virtual void StrokeTypeItem::RenderItem(RenderRegion *pRender, DocRect *pDocRect)

	Author:     Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:    6/3/97

	Inputs:		pRender		- RenderRegion to render into
				pDocRect	- Rectangle to render

	Purpose:    Renders a representation of the stroke into the given renderregion. This
				is used by the line properties dialog since all the stuff needed seems to
				be private...

********************************************************************************************/

void StrokeTypeItem::RenderItem(RenderRegion *pRender, DocRect *pDocRect, INT32 Width)
{
	if(pRender == NULL || pDocRect == NULL)
	{
		ERROR3("StrokeTypeItem::RenderItem given null params");
		return;
	}

	pRender->SaveContext();

	// Make sure we're anti-aliasing
	Quality             AntiAliasQuality(Quality::QualityMax);
	QualityAttribute    AntiAliasQualityAttr(AntiAliasQuality);
	pRender->SetQuality(&AntiAliasQualityAttr,FALSE);

	// Render the background rectangle
	DialogColourInfo RedrawColours;
	pRender->SetLineColour(RedrawColours.DialogBack());
	pRender->SetFillColour(RedrawColours.DialogBack());
	pRender->DrawRect(pDocRect);

	// Draw the Saturation vs. Value square
	Path SquarePath;
	LinearFillAttribute ValueGradFill;
	LinearTranspFillAttribute TransFill;

	ValueGradFill.Colour	= DocColour(255L, 0L, 0L);
	ValueGradFill.EndColour	= DocColour(255L, 255L, 255L);

	DocCoord ThePoint(pDocRect->lo.x, pDocRect->hi.y);
	ValueGradFill.SetStartPoint(&ThePoint);
	ThePoint = DocCoord(pDocRect->hi.x, pDocRect->hi.y);
	ValueGradFill.SetEndPoint(&ThePoint);
	ValueGradFill.SetEndPoint2(NULL);

	pRender->SetFillGeometry(&ValueGradFill, FALSE);			// Set Grad-filled

	//pRender->DrawRect(pDocRect);

/*		SquarePath.Initialise(12, 12);
		SquarePath.FindStartOfPath();

		SquarePath.InsertMoveTo(ValSatSquare.lo);
		SquarePath.InsertLineTo(DocCoord(ValSatSquare.hi.x, ValSatSquare.lo.y));
		SquarePath.InsertLineTo(ValSatSquare.hi);
		SquarePath.InsertLineTo(DocCoord(ValSatSquare.lo.x, ValSatSquare.hi.y));
		SquarePath.IsFilled = TRUE;

		pRender->DrawPath(&SquarePath);		// Render the value square

		// Render a white linear-grad-transparency square over the top to get the
		// effect of the Saturation gradient
		pRender->SetFillColour(DocColour(0L, 0L, 0L));

		// Set transparency to circular 0% at center, 100% at radius, && plot it
		TransFill.SetStartPoint(&ValSatSquare.hi);
		ThePoint = DocCoord(ValSatSquare.hi.x, ValSatSquare.lo.y);
		TransFill.SetEndPoint(&ThePoint);
		TransFill.SetEndPoint2(NULL);
		TransFill.Transp		= 255;
		TransFill.EndTransp		= 0;
		TransFill.TranspType	= 2;		// 'Stained glass' transparency

		pRender->SetTranspFillGeometry(&TransFill, FALSE);
  */


	pRender->SetLineColour(RedrawColours.TextFore());

	SetAttributes(pRender);
	pRender->SetLineWidth((INT32)((double)Width * 0.8));
	DrawItem(pRender, *pDocRect);

	pRender->RestoreContext();
}



/********************************************************************************************

>	virtual void StrokeTypeItem::UpdateGalleryItem()

	Author:     Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:    6/3/97

	Purpose:    Updates the gallery item's cached data to represent any changes this 
				Stoke might have had done to it. Also redraw the gallery if necessary.

********************************************************************************************/

void StrokeTypeItem::UpdateGalleryItem()
{
	StrokeDefinition *pStroke = GetStrokeDefinition();
	if(pStroke != NULL)
	{
		SetDescription(pStroke->GetStrokeName());
	}

	ForceRedrawOfMyself();
}






/********************************************************************************************

>	VarWidthItem::VarWidthItem(VariableWidthAttrValue *pAttr, const String &strDescription)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97
	Inputs:		pAttr			- a pointer to an attribute to be used for this
								  stroke type. This now belongs to the VarWidthItem,
								  so you must not delete it
				strDescription	- a text description of this user-interface item
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a user-interface item representing a stroke type

********************************************************************************************/

VarWidthItem::VarWidthItem(VariableWidthAttrValue *pAttr, const String &strDescription)
			  : LineAttrItem(strDescription, BELOW)
{
	ERROR3IF(pAttr == NULL, "Illegal NULL param");
	pAttrDef = pAttr;
}



/********************************************************************************************

>	StrokeTypeItem::~StrokeTypeItem()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97

	Purpose:	Destructor

********************************************************************************************/

VarWidthItem::~VarWidthItem()
{
	if (pAttrDef != NULL)
		delete pAttrDef;
}



/********************************************************************************************

>	virtual void VarWidthItem::SetAttributes(RenderRegion* pRegion) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97

	Inputs:		pRegion	- the render-region to draw into

	Purpose:	Sets the stroke typeattributes in the given render-region to render this
				item correctly.

********************************************************************************************/

void VarWidthItem::SetAttributes(RenderRegion* pRegion) const
{
	// Set a 16pt line width - nice & thick
	pRegion->SetLineWidth(16000);

	// Set the line width (this overrides the "default" line width set by the caller).
	pAttrDef->Render(pRegion, FALSE);

	// Set a path processor up so that the variable line width is used when stroking
	StrokeTypeAttrValue *pStroke = new StrokeTypeAttrValue(new PathProcessorStroke);
	if (pStroke != NULL)
		pStroke->Render(pRegion, TRUE);
}



/********************************************************************************************

>	virtual NodeAttribute *VarWidthItem::CreateNewAttribute(BOOL) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97

	Returns:	A pointer to a NodeAttribute-derivative that this user-interface item
				represents, ie. a line-width attribute that can be passed to the
				attribute manager.

	Purpose:	Create a NodeAttribute representing the attribute this item applies

********************************************************************************************/

NodeAttribute *VarWidthItem::CreateNewAttribute(BOOL) const
{
	return(pAttrDef->MakeNode());
}



/********************************************************************************************

>	virtual MILLIPOINT VarWidthItem::GetWidth() const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97
	Inputs:		-
	Outputs:	-
	Returns:	The width of a line-width item, in millipoints.
	Purpose:	Called by the line gallery formatting code.

********************************************************************************************/

MILLIPOINT VarWidthItem::GetWidth() const
{
	return 2 * LineAttrItem::GetWidth() / 3;
}



/********************************************************************************************

>	virtual CCRuntimeClass** VarWidthItem::GetAttribRuntimeClasses() const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97
	Inputs:		-
	Outputs:	-
	Returns:	The run-time class of the line attribute this type of display item
				represents.
	Purpose:	Used when searching for display items representing the selection's current
				attributes.

********************************************************************************************/

CCRuntimeClass** VarWidthItem::GetAttribRuntimeClasses() const
{
	static CCRuntimeClass* aprtc[] =
	{
		CC_RUNTIME_CLASS(AttrVariableWidth),
		NULL
	};

	return aprtc;
}



/********************************************************************************************

>	virtual BOOL VarWidthItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97
	Inputs:		pOtherAttrib		the attribute value to compare this item to
	Outputs:	-
	Returns:	TRUE if this item represents the same attribute value as the given
				attribute.
	Purpose:	Used when searching for display items representing the selection's current
				attributes.

********************************************************************************************/

BOOL VarWidthItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const
{
	if (*ppOtherAttribs == NULL)
		return(FALSE);

	AttrVariableWidth* pAttr = (AttrVariableWidth *) *ppOtherAttribs;
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(AttrVariableWidth)),
				"VarWidthItem::IsEqualValueToAny - unexpected object type");

	if (pAttr != NULL)
	{
		VariableWidthAttrValue *pOther = (VariableWidthAttrValue *) pAttr->GetAttributeValue();
		return((*pAttrDef) == (*pOther));
	}

	return(FALSE);
}



/********************************************************************************************

>   virtual MILLIPOINT VarWidthItem::GetHorzGap() const

	Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:    3/3/97

	Returns:    The horizontal gap between this item and another item of the same type, in
				millipoints.
	Purpose:    Called by the formatting code to determine the gap between items of this type

********************************************************************************************/

MILLIPOINT VarWidthItem::GetHorzGap() const
{
	// I would like the gap to be the same all the way around
	return(GetVertGap());
}


/********************************************************************************************
>	virtual BOOL LineAttrItem::ItemSelected() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/2000
	Inputs:		pNewAttr - the attribute that we are about to apply
	Outputs:	-
	Returns:	TRUE if we want the line gallery to go ahead and apply the attribute, 
				FALSE if we applied it ourselves.
	Purpose:	utility fn called when the item is selected.  As we now wish to apply both the
				stroke type item and the variable width item through one UI action we must 
				do so here.
	Notes:      This was implemented so that the user could apply a variable width stroke with
				just one click.  As we have abaondoned the idea of keeping the different stroke
				types for this release it has been decided that we will only have the basic
				stroke type which will automatically be applied when you select a variable width
********************************************************************************************/

BOOL VarWidthItem::ItemSelected(NodeAttribute* pAttribute)
{
	if (pAttribute == NULL || !pAttribute->IsKindOf(CC_RUNTIME_CLASS(AttrVariableWidth)))
	{
		ERROR3("Invalid attribute in VarWidthItem::ItemSelected");
		return FALSE;
	}

	// if we're applying a default then we should also apply an old-style stroke so...
	PathProcessorStroke* PPS = NULL;
	VariableWidthAttrValue* pVal = (VariableWidthAttrValue*)pAttribute->GetAttributeValue();
	if (pVal->GetWidthFunction() != NULL)
	{
		// If not a default then create the PathProcessor for the stroke attribute
		// The default case will fall through with NULL and create an old-style stroke
		PPS = new PathProcessorStroke;
		if (PPS == NULL)
			return FALSE;
	}

	StrokeTypeAttrValue NewStroke(PPS);
	AttrStrokeType* pAttrStroke = (AttrStrokeType*)NewStroke.MakeNode();

	if (pAttrStroke == NULL)
		return FALSE;

	List AttribsList;
	NodeAttributePtrItem* pStrokeAttrPtr = new NodeAttributePtrItem;
	if (pStrokeAttrPtr == NULL)
	{
		delete pAttrStroke;
		return TRUE;
	}
	pStrokeAttrPtr->NodeAttribPtr = pAttrStroke;

	// in order to apply the attributes simultaneously we need a list
	AttribsList.AddHead(pStrokeAttrPtr);

	NodeAttributePtrItem* pVarWidthPtr = new NodeAttributePtrItem;
	if (pVarWidthPtr == NULL)
	{
		pStrokeAttrPtr->NodeAttribPtr = NULL;
		delete pAttrStroke;
		delete pStrokeAttrPtr;
		return TRUE;
	}
	pVarWidthPtr->NodeAttribPtr = pAttribute;

	AttribsList.AddHead(pVarWidthPtr);

	AttributeManager::AttributesSelected(AttribsList, _R(IDS_STROKEAPPLY_UNDO));

	// We don't need the list of attrs anymore
	NodeAttributePtrItem* pAttrItem = (NodeAttributePtrItem*)AttribsList.GetHead();
	while (pAttrItem)
	{
		delete (pAttrItem->NodeAttribPtr);
		pAttrItem->NodeAttribPtr = NULL;
		pAttrItem = (NodeAttributePtrItem*)AttribsList.GetNext(pAttrItem);
	}
	AttribsList.DeleteAll(); // tidyup	

	BROADCAST_TO_ALL(StrokeMsg());
	return FALSE; // means do not go ahead and apply pAttribute again
}
