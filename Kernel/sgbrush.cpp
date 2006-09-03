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

// Line Gallery interface for Brush Attributes - implementation 

#include "camtypes.h"
#include "sgbrush.h"
#include "sgline.h"
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "brshcomp.h"
#include "qualattr.h"
//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "dlgcol.h"
#include "lineattr.h"
#include "ppbrush.h"
//#include "msg.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "brushmsg.h"
#include "ndoptmz.h"
//#include "rik.h"

CC_IMPLEMENT_DYNAMIC(BrushAttrItem, LineAttrItem);

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	BrushAttrItem::BrushAttrItem(BrushAttrAttrValue *pBrush, const String &strDescription)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97
	Inputs:		pBrush			- a pointer to an attribute to be used for this
								  brush item. This now belongs to the BrushAttrItem,
								  so you must not delete it
				strDescription	- a text description of this user-interface item
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a user-interface item representing a stroke type

********************************************************************************************/

BrushAttrItem::BrushAttrItem(BrushAttrValue *pBrush, const String &strDescription)
			  : LineAttrItem(strDescription, BELOW)
{
	ERROR3IF(pBrush == NULL, "Illegal NULL param");
	m_pBrushAttrVal = pBrush;
}



/********************************************************************************************

>	BrushAttrItem::~BrushAttrItem()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97

	Purpose:	Destructor

********************************************************************************************/

BrushAttrItem::~BrushAttrItem()
{
	if (m_pBrushAttrVal != NULL)
	{
		m_pBrushAttrVal->FlushCache();
		m_pBrushAttrVal->SetPathProcessor(NULL);
		delete m_pBrushAttrVal;
	}
}


/********************************************************************************************

>	virtual void BrushAttrItem::SetAttributes(RenderRegion* pRegion) const

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97

	Inputs:		pRegion	- the render-region to draw into

	Purpose:	Sets the brush attributes in the given render-region to render this
				item correctly.

********************************************************************************************/

void BrushAttrItem::SetAttributes(RenderRegion* pRegion) const
{
	// code has been moved into DrawItem, as we need to know the bounding box
	// before we can make the scaling adjustment


	// Set a  line width 
	//pRegion->SetLineWidth(1000);

	// Set the line width (this overrides the "default" line width set by the caller).
	//m_pBrushAttrVal->Render(pRegion, FALSE);
}



/********************************************************************************************

>	virtual NodeAttribute *BrushAttrItem::CreateNewAttribute(BOOL) const

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97

	Returns:	A pointer to a NodeAttribute-derivative that this user-interface item
				represents, ie. a line-width attribute that can be passed to the
				attribute manager.

	Purpose:	Create a NodeAttribute representing the attribute this item applies

********************************************************************************************/

NodeAttribute *BrushAttrItem::CreateNewAttribute(BOOL) const
{
	// because we may have changed some of the attribute data to render the brush objects
	// in the line gallery we really want to make sure the new path processor takes all 
	// its data from the original brush definition.  
	// This is a bit long winded but safer than just taking the local data
	AttrBrushType* pAttrBrushType = (AttrBrushType*)m_pBrushAttrVal->MakeNode();
	if (pAttrBrushType == NULL)
	{
		ERROR3("Failed to make node");
		return NULL;
	}
	BrushAttrValue* pNewBrushAttrVal = (BrushAttrValue*)pAttrBrushType->GetAttributeValue();
	if (pNewBrushAttrVal == NULL)
	{
		ERROR3("No BrushAttrVal");
		return NULL;
	}

	// now get the brush definition
	BrushDefinition* pBrush = m_pBrushAttrVal->GetBrushDefinition();
	if (pBrush == NULL)
	{
		// not an error
		return pAttrBrushType;
	}

	PathProcessorBrush* pPathProc =	pNewBrushAttrVal->GetPathProcessor();
	if (pPathProc == NULL)
	{
		// not actually an error, as this could be a default 'normal line' brush
		return pAttrBrushType;
	}


	// copy the data to the new processor
	pBrush->CopyDataToProcessor(pPathProc);
	
	// tell the processor which attribute value it belongs to
	pPathProc->SetParentAttribute(pAttrBrushType);

	
	return pAttrBrushType;
}


/********************************************************************************************

>	virtual BOOL BrushAttrItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/2000
	Inputs:		pOtherAttrib - the attribute value to compare this item to
	Outputs:	-
	Returns:	TRUE if this item represents the same attribute value as the given
				attribute.
	Purpose:	Used when searching for display items representing the selection's current
				attributes.

********************************************************************************************/

BOOL BrushAttrItem::IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const
{
	if (*ppOtherAttribs == NULL)	// When multiple attribs are selected, this will be NULL
		return(FALSE);

	AttrBrushType* pAttr = (AttrBrushType *) *ppOtherAttribs;
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(AttrBrushType)),
				"BrushAttrItem::IsEqualValueToAny - unexpected object type");

	if (pAttr != NULL)
	{
		BrushAttrValue *pOther = (BrushAttrValue *) pAttr->GetAttributeValue();
		//return((*m_pBrushAttrVal) == (*pOther));
	
		// to simplify - just check to see if they have the same brush handle
		BrushHandle Other = pOther->GetBrushHandle();
		BrushHandle This = m_pBrushAttrVal->GetBrushHandle();
		return (Other == This);
	}

	return(FALSE);
}


/********************************************************************************************

>	virtual BrushDefinition *BrushTypeItem::GetBrushDefinition()

	Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:    7/1/2000

	Returns:    The BrushDefinition for this item
	Purpose:    Get the BrushDefinition for this item

********************************************************************************************/

BrushDefinition *BrushAttrItem::GetBrushDefinition()
{
	BrushHandle Handle = GetBrushHandle();
	if(Handle != BrushHandle(-1))
	{
		// get the brush component
		Document* pDoc = Document::GetCurrent();
		ERROR2IF(pDoc == NULL, NULL, "Wheres the document?");
		BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
		ERROR2IF(pBrushComp == NULL, NULL, "No brush component");
		return pBrushComp->FindBrushDefinition(Handle);
	}

	return NULL;
}

/********************************************************************************************

>	virtual BrushHandle BrushTypeItem::GetBrushHandle()

	Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:    7/1/2000

	Returns:    The BrushHandle for this item, or BrushHandle_NoBrush if strife
	Purpose:    Get the BrushHandle for this item

********************************************************************************************/

BrushHandle BrushAttrItem::GetBrushHandle()
{
	BrushAttrValue *pAttrValue = GetBrushAttrValue();
	if(pAttrValue != NULL)
	{
		PathProcessorBrush *pProcessor = pAttrValue->GetPathProcessor();
		if (pProcessor != NULL && pProcessor->IsKindOf(CC_RUNTIME_CLASS(PathProcessorBrush)))
		{
			BrushHandle Handle = ((PathProcessorBrush *)pProcessor)->GetBrushDefinitionHandle();
			return Handle;
		}
	}

	return BrushHandle_NoBrush;
}


/********************************************************************************************

>	virtual void BrushAttrItem::RenderItem(RenderRegion *pRender, DocRect *pDocRect)

	Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:    9/1/2000

	Inputs:		pRender		- RenderRegion to render into
				pDocRect	- Rectangle to render

	Purpose:    Renders a representation of the stroke into the given renderregion. This
				is used by the line properties dialog since all the stuff needed seems to
				be private...

********************************************************************************************/

void BrushAttrItem::RenderItem(RenderRegion *pRender, DocRect *pDocRect, INT32 Width)
{
	if(pRender == NULL || pDocRect == NULL)
	{
		ERROR3("BrushAttrItem::RenderItem given null params");
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


	pRender->SetLineColour(RedrawColours.TextFore());

	SetAttributes(pRender);
	pRender->SetLineWidth((INT32)((double)Width * 0.8));
	DrawItem(pRender, *pDocRect);

	pRender->RestoreContext();
}



/********************************************************************************************

>	virtual void BrushAttrItem::UpdateGalleryItem()

	Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:    9/1/2000

	Purpose:    Updates the gallery item's cached data to represent any changes this 
				Stoke might have had done to it. Also redraw the gallery if necessary.

********************************************************************************************/

void BrushAttrItem::UpdateGalleryItem()
{
	BrushDefinition *pBrush = GetBrushDefinition();
	if(pBrush != NULL)
	{
		// first try and allocate a new PPB
		PathProcessorBrush* pNewPPB = new PathProcessorBrush;
		if (pNewPPB == NULL)
			return;
		// give it the latest data
		pBrush->CopyDataToProcessor(pNewPPB);

		// tell it what handle to use
		pNewPPB->SetBrushDefinition(m_pBrushAttrVal->GetBrushHandle());
		//ask our attr to delete its path processor
		m_pBrushAttrVal->SetPathProcessor(pNewPPB);

		// set the new name
		SetDescription(pBrush->GetLineName());
	}

	ForceRedrawOfMyself();
}

/********************************************************************************************

>	virtual CCRuntimeClass** BrushAttrItem::GetAttribRuntimeClasses() const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/97
	Inputs:		-
	Outputs:	-
	Returns:	The run-time class of the line attribute this type of display item
				represents.
	Purpose:	Used when searching for display items representing the selection's current
				attributes.

********************************************************************************************/

CCRuntimeClass** BrushAttrItem::GetAttribRuntimeClasses() const
{
	static CCRuntimeClass* aprtc[] =
	{
		CC_RUNTIME_CLASS(AttrBrushType),
		NULL
	};

	return aprtc;
}



/********************************************************************************************
>   virtual void BrushAttrItem::DrawItem(RenderRegion* pRegion, const DocRect& drBounds) const

	Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:    10/1/2000
	Inputs:     pRegion             pointer to the RenderRegion to draw into
				drBounds            reference to the bounding box rectangle to draw within
	Outputs:    -
	Returns:    -
	Purpose:    Draws this item within the bounding box. The aim is to work out what scaling we
				need in order to fit the brush into the FormatRect.  Therefore we draw a line
				through the bounding box, get its bounding box with the brush applied and work out
				the scaling that will fit this into the formatrect.
	Errors:     -
	SeeAlso:    LineAttrItem::Render
********************************************************************************************/

void BrushAttrItem::DrawItem(RenderRegion* pRegion, const DocRect& drBounds) const
{
	if (m_pBrushAttrVal == NULL)
	{
		ERROR3("Wheres the AttrVal gone?");
		return;
	}

	// Calculate the mid-point of the vertical sides of the bounding box.
	INT32 yMid = ((drBounds.lo.y + drBounds.hi.y) / 2) - (drBounds.Height() / 3);

	// Draw a line at this height from the left to the right edge.
	Path pthLinePath;
	if (pthLinePath.Initialise())
	{
		pthLinePath.InsertMoveTo(DocCoord(drBounds.lo.x, yMid));
		pthLinePath.InsertLineTo(DocCoord(drBounds.hi.x, yMid));
		pthLinePath.IsFilled  = FALSE;
		pthLinePath.IsStroked = TRUE;
	
		// Get the brush definition and the local path processor
		BrushDefinition* pBrushDef = m_pBrushAttrVal->GetBrushDefinition();
		if (pBrushDef != NULL)
		{
			PathProcessorBrush* pPathProc = m_pBrushAttrVal->GetPathProcessor();
			if (pPathProc == NULL)
			{
				ERROR3("No brush definition");
				return;
			}
			MILLIPOINT LineWidth = 18000;
			
			pPathProc->ScaleToValue(LineWidth);
			pRegion->SetLineWidth(LineWidth);

			// render the attribute, i.e. push the path processor
			m_pBrushAttrVal->Render(pRegion, FALSE);
			// draw the line
			pRegion->DrawPath(&pthLinePath);
		}
		else  // no actual brush - just draw the line
			pRegion->DrawPath(&pthLinePath);
	}
	else
		ERROR3("Failed to initialise path");
	
}


/********************************************************************************************

>	virtual void BrushAttrItem::ItemSelected()

	Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:    9/1/2000
	Inputs;		pNewAttr - the attribute node created from this BrushAttrItem
	Outputs:	-
	Returns:	FALSE, if all went well and we applied the attributes here ok
	Purpose:    Sets the default line width to be that of the brush, or the previous line 
				width if the brush doesn't have one.  Also broadcasts a message saying that
				it has been selected, for the benefit of the freehand tool.
********************************************************************************************/

BOOL BrushAttrItem::ItemSelected(NodeAttribute* pAttrBrush)
{
	if (m_pBrushAttrVal == NULL)
	{
		ERROR3("BrushAttrValue is NULL");
		return TRUE;
	}

	// if we are setting a 'no brush' brush we must restore its line width to the last
	// line width set that had 'no brush'
	BOOL bSettingNoBrush = (m_pBrushAttrVal->GetBrushHandle() == BrushHandle_NoBrush);

	// if there is something in the selection then we do not want to change
	// the line width, as they may be different.  the only exception to this is
	// if we are setting the default 'no brush' 
	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel == NULL)
		return TRUE;
	if (pSel->FindFirst() != NULL)
	{
		if (!bSettingNoBrush)
			return TRUE;
	}

	MILLIPOINT OldLineWidth = 500; // initialise to default

	// first make sure the line gallery has its previous line width member set
	LineGallery* pLineGal = LineGallery::GetInstance();
	if (pLineGal == NULL)
		return TRUE;
	else
	{
		// get the last line width in case we need it
		OldLineWidth = pLineGal->GetPreviousLineWidth();
		
		// if we are going from a 'no brush' to a brush then we need to store the line
		// width of the 'no brush'
		if ((GetCurrentDefaultBrushHandle() == BrushHandle_NoBrush) && (!bSettingNoBrush))
			pLineGal->SetPreviousLineWidth();
	}

	// get the line width from the attribute
	MILLIPOINT LineWidth = m_pBrushAttrVal->GetDefaultLineWidth(TRUE);
	
	// if we receive < 0 from GetDefaultLineWidth it means that we have no
	// ink objects associated with this brush.  In this instance it means we
	// are probably selecting the 'simple line' brush and should set the old line
	// width 
	if (LineWidth <= 0)
	{
		if (OldLineWidth < 0)
		{
			ERROR3("negative line width");
			return TRUE;
		}
		else
			LineWidth = OldLineWidth;
	}
	
	// quick hack until the problems in attrmgr.cpp with applying non-text attributes in the
	// text tool are sorted out
	if  (Tool::GetCurrentID() == TOOLID_TEXT)
		return FALSE;

	// make a new line width node
	NodeAttribute* pNewAttr = new AttrLineWidth;
	if (pNewAttr == NULL)
	{
		ERROR3("Failed to allocate new line width");
		return TRUE;
	}
	((AttrLineWidth*)pNewAttr)->Value.LineWidth = LineWidth;
	
	// in order to apply the attributes together we need to make a list
	List AttribsList;
	NodeAttributePtrItem* pBrushAttrPtr = new NodeAttributePtrItem;
	if (pBrushAttrPtr == NULL)
	{
		delete pNewAttr;
		return TRUE;
	}
	pBrushAttrPtr->NodeAttribPtr = pAttrBrush;
	
	AttribsList.AddHead(pBrushAttrPtr);

	NodeAttributePtrItem* pLineWidthPtr = new NodeAttributePtrItem;
	if (pLineWidthPtr == NULL)
	{
		pBrushAttrPtr->NodeAttribPtr = NULL;
		return TRUE;
	}
	pLineWidthPtr->NodeAttribPtr = pNewAttr;

	AttribsList.AddHead(pLineWidthPtr);

	AttributeManager::AttributesSelected(AttribsList, _R(IDS_APPLYBRUSH_UNDO));


	// We don't need the list of attrs anymore
	NodeAttributePtrItem* pAttrItem = (NodeAttributePtrItem*)AttribsList.GetHead();
	while (pAttrItem)
	{
		delete (pAttrItem->NodeAttribPtr);
		pAttrItem->NodeAttribPtr = NULL;
		pAttrItem = (NodeAttributePtrItem*)AttribsList.GetNext(pAttrItem);
	}
	AttribsList.DeleteAll(); // tidyup	

	BrushHandle NewHandle = GetBrushHandle();
	BROADCAST_TO_ALL(BrushMsg(NewHandle));
	return FALSE;
}
	//AttributeManager::AttributeSelected(pNewAttr);
	/*
	// turn off the warning if it isn't already
	BOOL ResetAttrMgrFlag = AttributeManager::AskBeforeSettingCurrentAttr;
	if (ResetAttrMgrFlag)
		AttributeManager::AskBeforeSettingCurrentAttr = FALSE;

	
	
	// turn it back on if we need to
	if (ResetAttrMgrFlag)
		AttributeManager::AskBeforeSettingCurrentAttr = TRUE;
	*/
	// tell the world we have changed brushes

	




/********************************************************************************************

>	void BrushAttrItem::GetCurrentDefaultBrushHandle()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/99
	Returns:	the handle of the current defalt brush attribute
	Purpose:	as above

********************************************************************************************/

BrushHandle BrushAttrItem::GetCurrentDefaultBrushHandle()
{
	Document* pDoc = Document::GetCurrent();
	if (pDoc == NULL)
	{
		ERROR3("No document");
		return BrushHandle_NoBrush;
	}

	AttributeManager* pAttrMgr = &(pDoc->GetAttributeMgr());
	if (pAttrMgr == NULL)
	{
		ERROR3("No attribute manager");
		return BrushHandle_NoBrush;
	}

	AttrBrushType* pAttr = (AttrBrushType*)(pAttrMgr->GetCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), CC_RUNTIME_CLASS(AttrBrushType)));

	if (pAttr == NULL)
	{
		ERROR3("No current brush attribute");
		return BrushHandle_NoBrush;
	}
	return pAttr->GetBrushHandle();
	
}
