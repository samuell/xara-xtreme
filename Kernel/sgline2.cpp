// $Id: sgline2.cpp 662 2006-03-14 21:31:49Z alex $
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
/*

	sgline2.cpp

	This header contains definitions of the member-functions of various classes derived
	from LineAttrItem, as used in the line-attributes gallery.

*/

#include "camtypes.h"
#include "app.h"
#include "rndrgn.h"
#include "paths.h"
#include "lineattr.h"
#include "sgline2.h"


/********************************************************************************************
>	LineWidthItem::LineWidthItem(double nPointWidth, const String& strDescription)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		nPointWidth			the width of the line, in points
				strDescription		a text description of this user-interface item
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a user-interface item representing a line's width.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

LineWidthItem::LineWidthItem(double nPointWidth, const String& strDescription)
  :	LineAttrItem(strDescription, LEFT),
  	m_nWidth((MILLIPOINT) (nPointWidth * 1000.0))
{
	// Empty.
}



/********************************************************************************************
>	virtual void LineWidthItem::SetAttributes(RenderRegion* pRegion) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		pRegion			the render-region to draw into
	Outputs:	-
	Returns:	-
	Purpose:	Sets the line-width attributes in the given render-region to render this
				item correctly.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void LineWidthItem::SetAttributes(RenderRegion* pRegion) const
{
	// Set the line width (this overrides the "default" line width set by the caller).
	pRegion->SetLineWidth(m_nWidth);

	// If the line is only a pixel thick then draw it in red, to distinguish it from
	// other very thin lines.
	if (m_nWidth == 0) pRegion->SetLineColour(COLOUR_RED);
}



/********************************************************************************************
>	virtual NodeAttribute* LineWidthItem::CreateNewAttribute(BOOL) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to a NodeAttribute-derivative that this user-interface item
				represents, ie. a line-width attribute that can be passed to the
				attribute manager.
	Purpose:	
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

NodeAttribute* LineWidthItem::CreateNewAttribute(BOOL) const
{
	AttrLineWidth* pAttr = new AttrLineWidth;
	if (pAttr != NULL) pAttr->Value.LineWidth = m_nWidth;
	return pAttr;
}



/********************************************************************************************
>	virtual MILLIPOINT LineWidthItem::GetWidth() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95
	Inputs:		-
	Outputs:	-
	Returns:	The width of a line-width item, in millipoints.
	Purpose:	Called by the line gallery formatting code.
	Errors:		-
	SeeAlso:	LineAttrItem::GetSize
********************************************************************************************/

MILLIPOINT LineWidthItem::GetWidth() const
{
	return 2 * LineAttrItem::GetWidth() / 3;
}



/********************************************************************************************
>	virtual CCRuntimeClass** LineWidthItem::GetAttribRuntimeClasses() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		-
	Outputs:	-
	Returns:	The run-time class of the line attribute this type of display item
				represents.
	Purpose:	Used when searching for display items representing the selection's current
				attributes.
	Errors:		-
	SeeAlso:	LineAttrItem::UpdateCurrentAttribStatus
********************************************************************************************/

CCRuntimeClass** LineWidthItem::GetAttribRuntimeClasses() const
{
	static CCRuntimeClass* aprtc[] =
	{
		CC_RUNTIME_CLASS(AttrLineWidth),
		NULL
	};

	return aprtc;
}



/********************************************************************************************
>	virtual BOOL LineWidthItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		pOtherAttrib		the attribute value to compare this item to
	Outputs:	-
	Returns:	TRUE if this item represents the same attribute value as the given
				attribute.
	Purpose:	Used when searching for display items representing the selection's current
				attributes.
	Errors:		-
	SeeAlso:	LineAttrItem::UpdateCurrentAttribStatus
********************************************************************************************/

BOOL LineWidthItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const
{
	AttrLineWidth* pattr = (AttrLineWidth*) *ppOtherAttribs;
	return pattr != NULL && pattr->Value.LineWidth == m_nWidth;
}



/********************************************************************************************
>	LineDashItem::LineDashItem(StockDash nDashType, const String& strDescription)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		nDashType			an enum representing a stock dash pattern
				strDescription		text description of this user-interface item
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a user-interface item representing a line's dash pattern.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

LineDashItem::LineDashItem(StockDash nDashType, const String& strDescription)
  :	LineAttrItem(strDescription, NO_LABEL),
  	m_nDashType(nDashType)
{
	// Empty.
}



/********************************************************************************************
>	virtual void LineDashItem::SetAttributes(RenderRegion* pRegion) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		pRegion				pointer to the render-region to draw into
	Outputs:	-
	Returns:	-
	Purpose:	Sets the dash-pattern attributes in the given render-region to render this
				item correctly.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void LineDashItem::SetAttributes(RenderRegion* pRegion) const
{
	// Set the dash pattern.
	pRegion->SetLineWidth(c_nDefaultLineWidth / 2);
	pRegion->SetDashPattern(m_nDashType);
}



/********************************************************************************************
>	virtual NodeAttribute* LineDashItem::CreateNewAttribute(BOOL) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to a NodeAttribute-derivative that this user-interface item
				represents, ie. a dash-pattern attribute that can be passed to the
				attribute manager.
	Purpose:	
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

NodeAttribute* LineDashItem::CreateNewAttribute(BOOL) const
{
	AttrDashPattern* pAttr = new AttrDashPattern;
	if (pAttr != NULL) pAttr->Value.SetStockDashPattern(m_nDashType);
	return pAttr;
}



/********************************************************************************************
>	virtual MILLIPOINT LineDashItem::GetHeight() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		-
	Outputs:	-
	Returns:	The height of this line gallery item, in millipoints.
	Purpose:	Overrides the default height of items in the line gallery for dash pattern
				items, so they are half the usual height.
	Errors:		-
	SeeAlso:	LineAttrItem::CalculateMyRect; LineAttrItem::GetHeight
********************************************************************************************/

MILLIPOINT LineDashItem::GetHeight() const
{
	return LineAttrItem::GetHeight() / 2;
}



/********************************************************************************************
>	virtual CCRuntimeClass** LineDashItem::GetAttribRuntimeClasses() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		-
	Outputs:	-
	Returns:	The run-time class of the line attribute this type of display item
				represents.
	Purpose:	Used when searching for display items representing the selection's current
				attributes.
	Errors:		-
	SeeAlso:	LineAttrItem::UpdateCurrentAttribStatus
********************************************************************************************/

CCRuntimeClass** LineDashItem::GetAttribRuntimeClasses() const
{
	static CCRuntimeClass* aprtc[] =
	{
		CC_RUNTIME_CLASS(AttrDashPattern),
		NULL
	};

	return aprtc;
}



/********************************************************************************************
>	virtual BOOL LineDashItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		pOtherAttrib		the attribute value to compare this item to
	Outputs:	-
	Returns:	TRUE if this item represents the same attribute value as the given
				attribute.
	Purpose:	Used when searching for display items representing the selection's current
				attributes.
	Errors:		-
	SeeAlso:	LineAttrItem::UpdateCurrentAttribStatus
********************************************************************************************/

BOOL LineDashItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const
{
	DashPatternAttribute dpa;
	dpa.SetStockDashPattern(m_nDashType);
	AttrDashPattern* pattr = (AttrDashPattern*) *ppOtherAttribs;
	return pattr != NULL && pattr->Value.DashPattern == dpa.DashPattern;
}



/********************************************************************************************
>	LineArrowItem::LineArrowItem(StockArrow nArrowType, const String& strDescription)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		nArrowType			an enum representing a stock arrowhead/tail
				strDescription		text description of this user-interface item
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a user-interface item representing an arrowhead/tail.
				NB. if -1 is passed for the nArrowType a blank, "no-arrow" arrowhead
				attribute is represented.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

LineArrowItem::LineArrowItem(StockArrow nArrowType, double fpScale,
							 const String& strDescription)
  :	LineAttrItem(strDescription, NO_LABEL),
  	m_fCentred(nArrowType == SA_SPOT ||
  			   nArrowType == SA_DIAMOND ||
  			   nArrowType == SA_HOLLOWDIAMOND)
{
	// Fill in the blank ArrowRec object and scale it, if we have a valid stock arrow type.
	if (nArrowType != (StockArrow) -1)
	{
		m_arArrow.CreateStockArrow(nArrowType);
		m_arArrow.SetArrowSize((FIXED16) fpScale);
	}
}



/********************************************************************************************
>	virtual MILLIPOINT LineArrowItem::GetWidth() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/95
	Inputs:		-
	Outputs:	-
	Returns:	The width of this display item, in millipoints.
	Purpose:	Overrides the default width of display items in the line gallery, making
				arrowhead items half the normal width.
	Errors:		-
	SeeAlso:	LineAttrItem::CalculateMyRect; LineAttrItem::GetWidth
********************************************************************************************/

MILLIPOINT LineArrowItem::GetWidth() const
{
	return 5 * LineAttrItem::GetWidth() / 8;
}



/********************************************************************************************
>	virtual MILLIPOINT LineArrowItem::GetHeight() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/95
	Inputs:		-
	Outputs:	-
	Returns:	The height of this display item, in millipoints.
	Purpose:	Overrides the default width of display items in the line gallery, making
				arrowhead items one-and-a-half times the normal height.
	Errors:		-
	SeeAlso:	LineAttrItem::CalculateMyRect; LineAttrItem::GetHeight
********************************************************************************************/

MILLIPOINT LineArrowItem::GetHeight() const
{
	return 3 * LineAttrItem::GetHeight() / 2;
}



/********************************************************************************************
>	virtual MILLIPOINT LineArrowItem::GetHorzGap() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/95
	Inputs:		-
	Outputs:	-
	Returns:	The horizontal gap between this type of display item, in millipoints.
	Purpose:	Overrides the default gap of display items in the line gallery, making
				arrowhead items more closely spaced horizontally than normal.
	Errors:		-
	SeeAlso:	LineAttrItem::CalculateMyRect; LineAttrItem::GetHorzGap
********************************************************************************************/

MILLIPOINT LineArrowItem::GetHorzGap() const
{
	return 0;
}



/********************************************************************************************
>	virtual MILLIPOINT LineArrowItem::GetVertGap() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		-
	Outputs:	-
	Returns:	The line-spacing gap between arrowhead items.
	Purpose:	Halves the vertical gap between arrowhead items.
	Errors:		-
	SeeAlso:	LineAttrItem::GetSize; LineAttrItem::GetVertGap
********************************************************************************************/

MILLIPOINT LineArrowItem::GetVertGap() const
{
	return LineAttrItem::GetVertGap() / 4;
}



/********************************************************************************************
>	virtual void LineArrowItem::SetAttributes(RenderRegion* pRegion) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		pRegion				pointer to the render-region to draw into
	Outputs:	-
	Returns:	-
	Purpose:	Sets the arrowhead attributes in the given render-region to render this
				item.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void LineArrowItem::SetAttributes(RenderRegion* pRegion) const
{
	pRegion->SetLineWidth(c_nDefaultLineWidth / 3);
	pRegion->SetStartArrow((ArrowRec&) m_arArrow);
}



/********************************************************************************************
>	virtual void LineArrowItem::DrawItem(RenderRegion* pRegion, const DocRect& drBounds) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		pRegion			pointer to the RenderRegion to draw into
				drBounds		the bounds of the drawing area within the RenderRegion
	Outputs:	-
	Returns:	-
	Purpose:	Formats and draws an arrowhead/tail.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void LineArrowItem::DrawItem(RenderRegion* pRegion, const DocRect& drBounds) const
{
	// Calculate the mid-point of the bounding box.
	MILLIPOINT xMid = (drBounds.lo.x + drBounds.hi.x) / 2;
	MILLIPOINT yMid = (drBounds.lo.y + drBounds.hi.y) / 2;

	// Work out where to draw the line to and from.  This is a bodge worked out empirically
	// to get around the unfortunate Corel-like feature that arrowheads make lines longer
	// in an unknown way.
	const MILLIPOINT nFudge1 = drBounds.Width() / 7;
	const MILLIPOINT nFudge2 = 13 * drBounds.Width() / 84;
	DocCoord dcStart(xMid - nFudge1 + ((m_fCentred) ? (nFudge2 / 2) : nFudge2), yMid),
			 dcEnd(drBounds.hi.x - nFudge1, yMid);

	// Create and draw the path.
	Path pthLinePath;
	pthLinePath.Initialise();
	pthLinePath.InsertMoveTo(dcStart);
	pthLinePath.InsertLineTo(dcEnd);
	pRegion->DrawPath(&pthLinePath);
}



/********************************************************************************************
>	virtual NodeAttribute* LineArrowItem::CreateNewAttribute(BOOL fIsAdjust) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		fIsAdjust		whether this item is being "applied" by a click with
								the Adjust modifier in force or not.  This has the
								effect of applying the attribute to the "other" end
								of the line, eg. applying an "end-arrowhead" to the
								start of a line.
	Outputs:	-
	Returns:	-
	Purpose:	A pointer to a NodeAttribute-derivative that this user-interface item
				represents, ie. an arrowhead attribute that can be passed to the
				attribute manager.  This is either an AttrStartArrow or an AttrEndArrow,
				depending on the particular type of arrowhead and the state of the
				fIsAdjust parameter.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

NodeAttribute* LineArrowItem::CreateNewAttribute(BOOL fIsAdjust) const
{
	// Work out whether to make a start- or end-arrow attribute
	BOOL fMakeStart = IsStartArrow();
	if (fIsAdjust) fMakeStart = !fMakeStart;

	// Make the appropriate attribute.
	NodeAttribute* pNewAttr;
	if (fMakeStart)
	{
		// Make a start-arrow attribute.
		AttrStartArrow* pNewStartAttr = new AttrStartArrow;
		if (pNewStartAttr != NULL) pNewStartAttr->Value.StartArrow = m_arArrow;
		pNewAttr = pNewStartAttr;
	}
	else
	{
		// Make an end-arrow attribute.
		AttrEndArrow* pNewEndAttr = new AttrEndArrow;
		if (pNewEndAttr != NULL) pNewEndAttr->Value.EndArrow = m_arArrow;
		pNewAttr = pNewEndAttr;
	}

	// Return the new attribute.
	return pNewAttr;
}



/********************************************************************************************
>	virtual CCRuntimeClass** LineArrowItem::GetAttribRuntimeClasses() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		-
	Outputs:	-
	Returns:	The run-time class of the line attribute this type of display item
				represents.
	Purpose:	Used when searching for display items representing the selection's current
				attributes.
	Errors:		-
	SeeAlso:	LineAttrItem::UpdateCurrentAttribStatus
********************************************************************************************/

CCRuntimeClass** LineArrowItem::GetAttribRuntimeClasses() const
{
	static CCRuntimeClass* aprtc[] =
	{
		CC_RUNTIME_CLASS(AttrStartArrow),
		CC_RUNTIME_CLASS(AttrEndArrow),
		NULL
	};
	
	return aprtc;
}



/********************************************************************************************
>	virtual BOOL LineArrowItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		pOtherAttrib		the attribute values to compare this item to
	Outputs:	-
	Returns:	TRUE if this item represents the same attribute value as the given
				attribute.
	Purpose:	Used when searching for display items representing the selection's current
				attributes.
	Errors:		-
	SeeAlso:	LineAttrItem::UpdateCurrentAttribStatus
********************************************************************************************/

BOOL LineArrowItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const
{
	AttrStartArrow* psattr = (AttrStartArrow*) *ppOtherAttribs;
	if (psattr != NULL && psattr->Value.StartArrow == m_arArrow) return TRUE;
	AttrEndArrow* peattr = (AttrEndArrow*) ppOtherAttribs[1];
	return peattr != NULL && peattr->Value.EndArrow == m_arArrow;
}



/********************************************************************************************
>	virtual BOOL LineArrowItem::IsStartArrow() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Returns:	TRUE if this line gallery item represents a start arrow, FALSE if it
				represents an end arrow.
	Purpose:	Used to work out what kind of AttrXXXXArrow attribute type to create in
				LineArrowItem::CreateNewAttribute.  Can be overridden.  This version
				asks the item's arrowhead record what kind it is.
********************************************************************************************/

BOOL LineArrowItem::IsStartArrow() const
{
	return ((ArrowRec&) m_arArrow).IsStartArrow();
}



/********************************************************************************************

>	virtual BOOL LineArrowItem::ShouldCloseOnDoubleClick(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/95

	Returns:	FALSE

	Purpose:	When an item is ctrl-double-clicked, the default action is to apply the
				item and close the gallery (for convenience). However, arrowheads do
				a special action for ctrl-double-click (they apply to the other end
				of the line) so they do not want auto-close to occur.

				This function returns TRUE (auto-close) by default, and is overridden
				to return FALSE by this class, to stop the auto-close action.

	SeeAlso:	LineAttrItem::ShouldCloseOnDoubleClick

********************************************************************************************/

BOOL LineArrowItem::ShouldCloseOnDoubleClick(void)
{
	return(FALSE);
}



/********************************************************************************************
>	LineNoArrowItem::LineNoArrowItem(const String& strDescription)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95
	Inputs:		strDescription			text description of this item
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a "no-arrowhead" line gallery item.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

LineNoArrowItem::LineNoArrowItem(const String& strDescription)
  :	LineArrowItem((StockArrow) -1, 0, strDescription)
{
	// Empty.
}



/********************************************************************************************
>	virtual MILLIPOINT LineNoArrowItem::GetWidth() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95
	Inputs:		-
	Outputs:	-
	Returns:	The width of this item, in millipoints.
	Purpose:	Forces a "no-arrow" item to be full-width.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

MILLIPOINT LineNoArrowItem::GetWidth() const
{
	return 2 * LineArrowItem::GetWidth();
}



/********************************************************************************************
>	virtual MILLIPOINT LineNoArrowItem::GetHeight() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95
	Inputs:		-
	Outputs:	-
	Returns:	The height of this item, in millipoints.
	Purpose:	Called by the line gallery formatting code.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

MILLIPOINT LineNoArrowItem::GetHeight() const
{
	return LineAttrItem::GetHeight();
}



/********************************************************************************************
>	virtual MILLIPOINT LineNoArrowItem::GetVertGap() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95
	Inputs:		-
	Outputs:	-
	Returns:	The vertical gap between this "no arrow" item and other "arrowhead" items
				within the line gallery.
	Purpose:	Called by the formatting code of the line gallery.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

MILLIPOINT LineNoArrowItem::GetVertGap() const
{
	return LineArrowItem::GetVertGap() / 3;
}



/********************************************************************************************
>	virtual void LineNoArrowItem::SetAttributes(RenderRegion*) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Overrides attribute setting for arrowheads to set NO attribute.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void LineNoArrowItem::SetAttributes(RenderRegion*) const
{
	// Empty, as we have no attributes required to render this item.
}



/********************************************************************************************
>	virtual void LineNoArrowItem::DrawItem(RenderRegion* pRegion,
										   const DocRect& drBounds) const
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95
	Inputs:		pRegion			the render-region to draw into
				drBounds		the bounding box of this item to draw within
	Outputs:	-
	Returns:	-
	Purpose:	Draws a "no-arrowhead" item.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void LineNoArrowItem::DrawItem(RenderRegion* pRegion, const DocRect& drBounds) const
{
	// Work out the difference in width between the text string and the bounding box.
	MILLIPOINT nWidthDiff = drBounds.Width() - (GetDescription().Length() * c_nAveCharWidth);

	// Draw the item's description centred within the new bounding box, if it will fit.
	DocRect drTextBounds(drBounds);
	if (nWidthDiff > 0)	drTextBounds.Inflate(-nWidthDiff / 2, 0);
	pRegion->DrawFixedSystemText(&((String&) GetDescription()), drTextBounds);
}



/********************************************************************************************
>	LineNoStartArrowItem::LineNoStartArrowItem(const String& strDescription)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95
	Inputs:		strDescription			text description of this item
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a "no-arrowhead" line gallery item.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

LineNoStartArrowItem::LineNoStartArrowItem(const String& strDescription)
  :	LineNoArrowItem(strDescription)
{
	// Empty.
}



/********************************************************************************************
>	virtual BOOL LineNoStartArrowItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		pOtherAttrib		the attribute values to compare this item to
	Outputs:	-
	Returns:	TRUE if this item represents the same attribute value as the given
				attribute.
	Purpose:	Used when searching for display items representing the selection's current
				attributes.
	Errors:		-
	SeeAlso:	LineAttrItem::UpdateCurrentAttribStatus
********************************************************************************************/

BOOL LineNoStartArrowItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const
{
	AttrStartArrow* psattr = (AttrStartArrow*) *ppOtherAttribs;
	return psattr != NULL && psattr->Value.StartArrow == m_arArrow;
}



/********************************************************************************************
>	virtual BOOL LineNoStartArrowItem::IsStartArrow() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Returns:	TRUE, as a LineNoStartArrowItem always represents a start arrow attribute.
	Purpose:	Used to work out what kind of AttrXXXXArrow attribute type to create in
				LineArrowItem::CreateNewAttribute.
********************************************************************************************/

BOOL LineNoStartArrowItem::IsStartArrow() const
{
	return TRUE;
}



/********************************************************************************************
>	LineNoEndArrowItem::LineNoEndArrowItem(const String& strDescription)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95
	Inputs:		strDescription			text description of this item
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a "no-arrowhead" line gallery item.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

LineNoEndArrowItem::LineNoEndArrowItem(const String& strDescription)
  :	LineNoArrowItem(strDescription)
{
	// Empty.
}



/********************************************************************************************
>	virtual BOOL LineNoEndArrowItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		pOtherAttrib		the attribute values to compare this item to
	Outputs:	-
	Returns:	TRUE if this item represents the same attribute value as the given
				attribute.
	Purpose:	Used when searching for display items representing the selection's current
				attributes.
	Errors:		-
	SeeAlso:	LineAttrItem::UpdateCurrentAttribStatus
********************************************************************************************/

BOOL LineNoEndArrowItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const
{
	AttrEndArrow* peattr = (AttrEndArrow*) ppOtherAttribs[1];
	return peattr != NULL && peattr->Value.EndArrow == m_arArrow;
}



/********************************************************************************************
>	virtual BOOL LineNoEndArrowItem::IsStartArrow() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Returns:	FALSE, as a LineNoEndArrowItem always represents an end arrow attribute.
	Purpose:	Used to work out what kind of AttrXXXXArrow attribute type to create in
				LineArrowItem::CreateNewAttribute.
********************************************************************************************/

BOOL LineNoEndArrowItem::IsStartArrow() const
{
	return FALSE;
}



/********************************************************************************************
>	LineCapItem::LineCapItem(LineCapType nCapType, const String& strDescription)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		nCapType			an enum representing a stock line-cap
				strDescription		text description of this user-interface item
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an line-cap user-interface item.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

LineCapItem::LineCapItem(LineCapType nCapType, const String& strDescription)
  :	LineAttrItem(strDescription, NO_LABEL),
  	m_nCapType(nCapType)
{
	// Empty.
}



/********************************************************************************************
>	virtual void LineCapItem::SetAttributes(RenderRegion* pRegion) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		pRegion				pointer to the render-region to draw into
	Outputs:	-
	Returns:	-
	Purpose:	Sets the line-cap attributes in the given render-region to render this
				item correctly.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void LineCapItem::SetAttributes(RenderRegion* pRegion) const
{
	// Call the SetStartCap function (there are no 'end' caps) and make lines thick.
	StartCapAttribute* pAttr = new StartCapAttribute;
	pAttr->StartCap = m_nCapType;
	pRegion->SetStartCap(pAttr, TRUE);
	pRegion->SetLineWidth(c_nHorzSizeOfItem / 2);
}



/********************************************************************************************
>	virtual NodeAttribute* LineCapItem::CreateNewAttribute(BOOL) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	A pointer to a NodeAttribute-derivative that this user-interface item
				represents, ie. a line-cap attribute that can be passed to the
				attribute manager.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

NodeAttribute* LineCapItem::CreateNewAttribute(BOOL) const
{
	AttrStartCap* pAttr = new AttrStartCap;
	if (pAttr != NULL) pAttr->Value.StartCap = m_nCapType;
	return pAttr;
}




/********************************************************************************************
>	virtual CCRuntimeClass** LineCapItem::GetAttribRuntimeClasses() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		-
	Outputs:	-
	Returns:	The run-time class of the line attribute this type of display item
				represents.
	Purpose:	Used when searching for display items representing the selection's current
				attributes.
	Errors:		-
	SeeAlso:	LineAttrItem::UpdateCurrentAttribStatus
********************************************************************************************/

CCRuntimeClass** LineCapItem::GetAttribRuntimeClasses() const
{
	static CCRuntimeClass* aprtc[] =
	{
		CC_RUNTIME_CLASS(AttrStartCap),
		NULL
	};

	return aprtc;
}



/********************************************************************************************
>	virtual BOOL LineCapItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		pOtherAttrib		the attribute value to compare this item to
	Outputs:	-
	Returns:	TRUE if this item represents the same attribute value as the given
				attribute.
	Purpose:	Used when searching for display items representing the selection's current
				attributes.
	Errors:		-
	SeeAlso:	LineAttrItem::UpdateCurrentAttribStatus
********************************************************************************************/

BOOL LineCapItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const
{
	AttrStartCap* pattr = (AttrStartCap*) *ppOtherAttribs;
	return pattr != NULL && pattr->Value.StartCap == m_nCapType;
}



/********************************************************************************************
>	LineJoinTypeItem::LineJoinTypeItem(JointType nJoinType, const String& strDescription)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		nJoinType			an enum representing a stock line-join
				strDescription		text description of this user-interface item
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an line-join user-interface item.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

LineJoinTypeItem::LineJoinTypeItem(JointType nJoinType, const String& strDescription)
  :	LineAttrItem(strDescription, NO_LABEL),
  	m_nJoinType(nJoinType)
{
	// Empty.
}



/********************************************************************************************
>	virtual void LineJoinTypeItem::SetAttributes(RenderRegion* pRegion) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		pRegion				pointer to the render-region to draw into
	Outputs:	-
	Returns:	-
	Purpose:	Sets the line-join attributes in the given render-region to render this
				item correctly.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void LineJoinTypeItem::SetAttributes(RenderRegion* pRegion) const
{
	// Set the join-type and make the lines even wider.
	JoinTypeAttribute* pAttr = new JoinTypeAttribute;
	pAttr->JoinType = m_nJoinType;
	pRegion->SetJoinType(pAttr, TRUE);
	pRegion->SetLineWidth(c_nHorzSizeOfItem / 2);
}



/********************************************************************************************
>	virtual void LineJoinTypeItem::DrawItem(RenderRegion* pRegion,
											const DocRect& drBounds) const
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		pRegion			pointer to the RenderRegion to draw into
				drBounds		the bounds of the drawing area within the RenderRegion
	Outputs:	-
	Returns:	-
	Purpose:	Formats and draws a line-join.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void LineJoinTypeItem::DrawItem(RenderRegion* pRegion, const DocRect& drBounds) const
{
	// Find the mid-points of the horizontal and vertical edges.
	MILLIPOINT xMid = (drBounds.lo.x + drBounds.hi.x) / 2;
	MILLIPOINT yMid = (drBounds.lo.y + drBounds.hi.y) / 2;

	// Create a right-angle path.
	Path pthJoinPath;
	pthJoinPath.Initialise();
	pthJoinPath.InsertMoveTo(DocCoord(xMid, drBounds.lo.y));
	pthJoinPath.InsertLineTo(DocCoord(xMid, yMid));
	pthJoinPath.InsertLineTo(DocCoord(drBounds.lo.x, yMid));

	// Now render the path.  The join-type will be apparent at the "cusp".
	pRegion->DrawPath(&pthJoinPath);
}



/********************************************************************************************
>	virtual NodeAttribute* LineJoinTypeItem::CreateNewAttribute(BOOL) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	A pointer to a NodeAttribute-derivative that this user-interface item
				represents, ie. a line-join attribute that can be passed to the
				attribute manager.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

NodeAttribute* LineJoinTypeItem::CreateNewAttribute(BOOL) const
{
	AttrJoinType* pAttr = new AttrJoinType;
	if (pAttr != NULL) pAttr->Value.JoinType = m_nJoinType;
	return pAttr;
}



/********************************************************************************************
>	virtual CCRuntimeClass** LineJoinTypeItem::GetAttribRuntimeClasses() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		-
	Outputs:	-
	Returns:	The run-time class of the line attribute this type of display item
				represents.
	Purpose:	Used when searching for display items representing the selection's current
				attributes.
	Errors:		-
	SeeAlso:	LineAttrItem::UpdateCurrentAttribStatus
********************************************************************************************/

CCRuntimeClass** LineJoinTypeItem::GetAttribRuntimeClasses() const
{
	static CCRuntimeClass* aprtc[] =
	{
		CC_RUNTIME_CLASS(AttrJoinType),
		NULL
	};

	return aprtc;
}



/********************************************************************************************
>	virtual BOOL LineJoinTypeItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		pOtherAttrib		the attribute value to compare this item to
	Outputs:	-
	Returns:	TRUE if this item represents the same attribute value as the given
				attribute.
	Purpose:	Used when searching for display items representing the selection's current
				attributes.
	Errors:		-
	SeeAlso:	LineAttrItem::UpdateCurrentAttribStatus
********************************************************************************************/

BOOL LineJoinTypeItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const
{
	AttrJoinType* pattr = (AttrJoinType*) *ppOtherAttribs;
	return pattr != NULL && pattr->Value.JoinType == m_nJoinType;
}
