// $Id: xpfcaps.cpp 662 2006-03-14 21:31:49Z alex $
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
///
/*
*/

#include "camtypes.h"
#include "xpfcaps.h"

#include "xpfrgn.h"

#include "cxfrgshp.h"

#include "strkattr.h"
#include "brshattr.h"
#include "fillramp.h"
#include "fthrattr.h"

// The implements to match the declares in the .h file.
CC_IMPLEMENT_MEMDUMP(XPFCapability, CC_CLASS_MEMDUMP);
CC_IMPLEMENT_MEMDUMP(XPFCComplexClass, XPFCapability);
CC_IMPLEMENT_MEMDUMP(XPFCLayer, XPFCComplexClass);
CC_IMPLEMENT_MEMDUMP(XPFCContour, XPFCComplexClass);
CC_IMPLEMENT_MEMDUMP(XPFCShadow, XPFCComplexClass);
CC_IMPLEMENT_MEMDUMP(XPFCBevel, XPFCComplexClass);
CC_IMPLEMENT_MEMDUMP(XPFCBlend, XPFCComplexClass);
CC_IMPLEMENT_MEMDUMP(XPFCMould, XPFCComplexClass);
CC_IMPLEMENT_MEMDUMP(XPFCRectangle, XPFCComplexClass);
CC_IMPLEMENT_MEMDUMP(XPFCEllipse, XPFCComplexClass);
CC_IMPLEMENT_MEMDUMP(XPFCPolygon, XPFCComplexClass);
CC_IMPLEMENT_MEMDUMP(XPFCBitmap, XPFCComplexClass);
CC_IMPLEMENT_MEMDUMP(XPFCText, XPFCComplexClass);
CC_IMPLEMENT_MEMDUMP(XPFCClipView, XPFCComplexClass);
CC_IMPLEMENT_MEMDUMP(XPFCBitmapEffect, XPFCComplexClass);
CC_IMPLEMENT_MEMDUMP(XPFCFill, XPFCapability);
CC_IMPLEMENT_MEMDUMP(XPFCFillTrans, XPFCapability);
CC_IMPLEMENT_MEMDUMP(XPFCLine, XPFCapability);
CC_IMPLEMENT_MEMDUMP(XPFCLineTrans, XPFCapability);
CC_IMPLEMENT_MEMDUMP(XPFCFeather, XPFCapability);
CC_IMPLEMENT_MEMDUMP(XPFCColour, XPFCapability);

CC_IMPLEMENT_MEMDUMP(CapabilityTree, CC_CLASS_MEMDUMP);

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW


XPFConvertType XPFCapability::GetConvertTypeForNode(Node* pNode)
{
	if (!DoesNodeMatch(pNode))
		return(XPFCONVTYPE_UNKNOWN);
	
	XPFConvertType Type = m_ConvertType;

	// Scan through the children asking any that match this node 
	// to return the convert type

	XPFCapability* pCap = m_pChild;
	while (pCap)
	{
		XPFConvertType ThisType = pCap->GetConvertTypeForNode(pNode);
		if (ThisType != XPFCONVTYPE_UNKNOWN)
			Type = ThisType;
		pCap = pCap->GetNext();
	}

	return(Type);
}

XPFConvertType XPFCapability::GetConvertTypeForAttrs(RenderRegion* pRegion)
{
	if (!DoAttributesMatch(pRegion))
		return(XPFCONVTYPE_UNKNOWN);
	
	XPFConvertType Type = m_ConvertType;

	// Scan through the children asking any that match this node 
	// to return the convert type

	XPFCapability* pCap = m_pChild;
	while (pCap)
	{
		XPFConvertType ThisType = pCap->GetConvertTypeForAttrs(pRegion);
		if (ThisType != XPFCONVTYPE_UNKNOWN)
			Type = ThisType;
		pCap = pCap->GetNext();
	}

	return(Type);
}



BOOL XPFCComplexClass::DoesNodeMatch(Node* pNode)
{
	return(pNode->IsKindOf(m_pClass));
}


BOOL XPFCLayer::DoesNodeMatch(Node* pNode)
{
	// If we aren't the correct type of node then return
	if (!XPFCComplexClass::DoesNodeMatch(pNode))
		return(FALSE);

	Layer* pLayer = (Layer*)pNode;

	// Now test the property attributes
	if (m_bVisible != XPFB_UNKNOWN && pLayer->IsVisible() != m_bVisible)
		return(FALSE);

	if (m_bLocked != XPFB_UNKNOWN && pLayer->IsLocked() != m_bLocked)
		return(FALSE);

	if (m_bPrintable != XPFB_UNKNOWN && pLayer->IsPrintable() != m_bPrintable)
		return(FALSE);
	
	if (m_bActive != XPFB_UNKNOWN && pLayer->IsActive() != m_bActive)
		return(FALSE);

	if (m_bBackground != XPFB_UNKNOWN && pLayer->IsBackground() != m_bBackground)
		return(FALSE);

	if (m_bGuide != XPFB_UNKNOWN && pLayer->IsGuide() != m_bGuide)
		return(FALSE);

	return(TRUE);
}



BOOL XPFCShadow::DoesNodeMatch(Node* pNode)
{
	// If we aren't the correct type of node then return
	if (!XPFCComplexClass::DoesNodeMatch(pNode))
		return(FALSE);

	NodeShadowController* pCont = (NodeShadowController*)pNode;

	// Now test the property attributes
	if (m_Type != XPFP_UNKNOWN && pCont->GetShadowType() != m_Type)
		return(FALSE);

	return(TRUE);
}


BOOL XPFCBevel::DoesNodeMatch(Node* pNode)
{
	// If we aren't the correct type of node then return
	if (!XPFCComplexClass::DoesNodeMatch(pNode))
		return(FALSE);

	NodeBevelController* pCont = (NodeBevelController*)pNode;

	// Now test the property attributes
	if (m_Type != XPFP_UNKNOWN && pCont->m_BevelType != m_Type)
		return(FALSE);

	if (m_Side != XPFP_UNKNOWN)
	{
		XPFProp Side = (pCont->m_bOuter) ? XPFP_BEVELSIDE_OUTER : XPFP_BEVELSIDE_INNER;
		if (m_Side != Side)
			return(FALSE);
	}

	return(TRUE);
}


BOOL XPFCBlend::DoesNodeMatch(Node* pNode)
{
	// If we aren't the correct type of node then return
	if (!XPFCComplexClass::DoesNodeMatch(pNode))
		return(FALSE);

	NodeBlend* pBlend = (NodeBlend*)pNode;

	// Now test the property attributes
	if (m_Effect != XPFP_UNKNOWN && pBlend->GetColourBlendType() != m_Effect)
		return(FALSE);

	if (m_bOnCurve != XPFB_UNKNOWN && pBlend->IsOnACurve() != m_bOnCurve)
		return(FALSE);

	CProfileBiasGain DefaultProfile;
	
	if (m_bObjProfile != XPFB_UNKNOWN)
	{
		BOOL bBlend = (*(pBlend->GetObjectProfile()) == DefaultProfile);
		if (bBlend == m_bObjProfile)
			return(FALSE);
	}

	if (m_bAttrProfile != XPFB_UNKNOWN)
	{
		BOOL bBlend = (*(pBlend->GetAttrProfile()) == DefaultProfile);
		if (bBlend == m_bAttrProfile)
			return(FALSE);
	}

	return(TRUE);
}


BOOL XPFCMould::DoesNodeMatch(Node* pNode)
{
	// If we aren't the correct type of node then return
	if (!XPFCComplexClass::DoesNodeMatch(pNode))
		return(FALSE);

	NodeMould* pMould = (NodeMould*)pNode;

	// Now test the property attributes
	if (m_Type != XPFP_UNKNOWN && pMould->DescribeGeometry() != m_Type)
		return(FALSE);

	if (m_bGradFill != XPFB_UNKNOWN)
	{
		// Scan story for non-plain attributes
		BOOL bGradFill = HasGraduatedFill(pNode);
		TRACEUSER( "Gerry", _T("HasGraduatedFill returned %s\n", bGradFill ? "true" : "false"));
		if (bGradFill != m_bGradFill)
			return(FALSE);
	}

	return(TRUE);
}

/****************************************************************************

>	BOOL XPFCMould::HasGraduatedFill(Node* pNode)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/08/2005

	Returns:	TRUE if the node contains any fills or transparencies that 
				require perspectivised rendering.
	Purpose:	This determines if a mould uses any attributes that will 
				not render correctly if converted to simple shapes

****************************************************************************/

BOOL XPFCMould::HasGraduatedFill(Node* pNode)
{
//	TRACEUSER( "Gerry", _T("HasGraduatedFill(%s)\n"), pNode->GetRuntimeClass()->m_lpszClassName);

	// If the node has children then we must loop through them testing each
	Node* pChild = pNode->FindFirstChild();
	while (pChild)
	{
		if (HasGraduatedFill(pChild))
			return(TRUE);

		pChild = pChild->FindNext();
	}

	if (pNode->IsAnAttribute())
	{
		NodeAttribute* pAttr = (NodeAttribute*)pNode;

		// These don't return a sensible value from GetAttributeIndex so 
		// we have to check the runtime class
		if (pAttr->GetAttributeType() == CC_RUNTIME_CLASS(AttrFillGeometry) ||
			pAttr->GetAttributeType() == CC_RUNTIME_CLASS(AttrTranspFillGeometry))
		{
			if (!pAttr->IsAFlatFill())
				return(TRUE);
		}
	}

	// We've got this far so there are no perspectivisable fills and we can return FALSE
	return(FALSE);
}



BOOL XPFCRectangle::DoesNodeMatch(Node* pNode)
{
	// If we aren't the correct type of node then return
	if (!XPFCComplexClass::DoesNodeMatch(pNode))
		return(FALSE);

	NodeRegularShape* pShape = (NodeRegularShape*)pNode;

	if (!CXaraFileRegularShape::IsRectangle(pShape))
		return(FALSE);

	// Now test the property attributes
	if (m_bComplex != XPFB_UNKNOWN && CXaraFileRegularShape::IsSimple(pShape) == m_bComplex)
		return(FALSE);

	if (m_bRounded != XPFB_UNKNOWN)
	{
		BOOL bVal = (CXaraFileRegularShape::IsRounded(pShape) != 0);
		if (bVal != m_bRounded)
			return(FALSE);
	}

	if (m_bStellated != XPFB_UNKNOWN && CXaraFileRegularShape::IsStellated(pShape) != m_bStellated)
		return(FALSE);

	if (m_bReformed != XPFB_UNKNOWN && CXaraFileRegularShape::IsReformed(pShape) != m_bReformed)
		return(FALSE);

	return(TRUE);
}


BOOL XPFCEllipse::DoesNodeMatch(Node* pNode)
{
	// If we aren't the correct type of node then return
	if (!XPFCComplexClass::DoesNodeMatch(pNode))
		return(FALSE);

	NodeRegularShape* pShape = (NodeRegularShape*)pNode;

	if (!CXaraFileRegularShape::IsEllipse(pShape))
		return(FALSE);

	// Now test the property attributes
	if (m_bComplex != XPFB_UNKNOWN && CXaraFileRegularShape::IsSimple(pShape) == m_bComplex)
		return(FALSE);

	return(TRUE);
}


BOOL XPFCPolygon::DoesNodeMatch(Node* pNode)
{
	// If we aren't the correct type of node then return
	if (!XPFCComplexClass::DoesNodeMatch(pNode))
		return(FALSE);

	NodeRegularShape* pShape = (NodeRegularShape*)pNode;

	if (CXaraFileRegularShape::IsEllipse(pShape) || CXaraFileRegularShape::IsRectangle(pShape))
		return(FALSE);

	// Now test the property attributes
	if (m_bRounded != XPFB_UNKNOWN && CXaraFileRegularShape::IsRounded(pShape) != m_bRounded)
		return(FALSE);

	if (m_bStellated != XPFB_UNKNOWN && CXaraFileRegularShape::IsStellated(pShape) != m_bStellated)
		return(FALSE);

	if (m_bReformed != XPFB_UNKNOWN && CXaraFileRegularShape::IsReformed(pShape) != m_bReformed)
		return(FALSE);

	return(TRUE);
}


BOOL XPFCBitmap::DoesNodeMatch(Node* pNode)
{
	// If we aren't the correct type of node then return
	if (!XPFCComplexClass::DoesNodeMatch(pNode))
		return(FALSE);

	NodeBitmap* pBitmap = (NodeBitmap*)pNode;

	// Now test the property attributes
	if (m_bComplex != XPFB_UNKNOWN)
	{
//		if (CXaraFileRegularShape::IsSimple(pBitmap) == m_bComplex)
			return(FALSE);
	}

	if (m_bContone != XPFB_UNKNOWN)
	{
		BOOL bContone = (pBitmap->GetStartColour() != NULL) || (pBitmap->GetEndColour() != NULL);
		if (m_bContone != bContone)
			return(FALSE);
	}

	return(TRUE);
}


BOOL XPFCText::DoesNodeMatch(Node* pNode)
{
	// If we aren't the correct type of node then return
	if (!XPFCComplexClass::DoesNodeMatch(pNode))
		return(FALSE);

	TextStory* pStory = (TextStory*)pNode;

	// Now test the property attributes
	if (m_bOnPath != XPFB_UNKNOWN)
	{
		// Check if the text is on a path
		BOOL bOnPath = (pStory->GetTextPath() != NULL);
		if (bOnPath != m_bOnPath)
			return(FALSE);
	}
	
	if (m_bComplex != XPFB_UNKNOWN)
	{
		// Check the story matrix for any rotation or shear
		Matrix mStory = pStory->GetStoryMatrix();
		ANGLE Rot = 0;
		ANGLE Shear = 0;
		mStory.Decompose(NULL, NULL, &Rot, &Shear, NULL, NULL);
		BOOL bComplex = !(Rot == 0 && Shear == 0);
		if (bComplex != m_bComplex)
			return(FALSE);
	}

	if (m_bPlain != XPFB_UNKNOWN)
	{
		// Scan story for non-plain attributes
		BOOL bPlain = IsNodePlain(pStory);
		TRACEUSER( "Gerry", _T("IsNodePlain returned %s\n", bPlain ? "true" : "false"));
		if (bPlain != m_bPlain)
			return(FALSE);
	}

	if (m_bAutoKern != XPFB_UNKNOWN)
	{
		// Check if the story is auto-kerned
		if (pStory->IsAutoKerning() != m_bAutoKern)
			return(FALSE);
	}
	
	if (m_bJustified != XPFB_UNKNOWN)
	{
		// Check if the story uses non-left justification
		BOOL bJustified = IsNodeJustified(pNode);
		if (bJustified != m_bJustified)
			return(FALSE);
	}
	
	return(TRUE);
}


/****************************************************************************

>	BOOL XPFCText::IsNodePlain(Node* pNode)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/08/2005

	Returns:	TRUE if the node only contains "plain" text, FALSE if it doesn't
	Purpose:	This determines if a text story only contains simple text.
				This is currently defined as flat fill, flat fill trans, 
				constant line width, no dash patterns etc...

****************************************************************************/

BOOL XPFCText::IsNodePlain(Node* pNode)
{
//	TRACEUSER( "Gerry", _T("IsNodePlain(%s)\n"), pNode->GetRuntimeClass()->m_lpszClassName);

	// If the node has children then we must loop through them testing each
	Node* pChild = pNode->FindFirstChild();
	while (pChild)
	{
		if (!IsNodePlain(pChild))
			return(FALSE);

		pChild = pChild->FindNext();
	}

	if (pNode->IsAnAttribute())
	{
		NodeAttribute* pAttr = (NodeAttribute*)pNode;

		// These don't return a sensible value from GetAttributeIndex so 
		// we have to check the runtime class
		if (pAttr->GetAttributeType() == CC_RUNTIME_CLASS(AttrFillGeometry))
		{
			if (!pAttr->IsAFlatFill())
				return(FALSE);
		}
		else if (pAttr->GetAttributeType() == CC_RUNTIME_CLASS(AttrTranspFillGeometry))
		{
			if (!pAttr->IsAFlatFill())
				return(FALSE);

			TranspFillAttribute* pTrans = (TranspFillAttribute*)(pAttr->GetAttributeValue());
			// Get the type
			UINT32 Type = pTrans->GetTranspType();
			// If we are flat, mix and 0% trans
			if (Type != TT_Mix ||
				*(pTrans->GetStartTransp()) != 0)
			{
				return(FALSE);
			}
		}
		else
		{
			switch (pAttr->GetAttributeIndex())
			{
				case ATTR_FILLGEOMETRY:
				case ATTR_TRANSPFILLGEOMETRY:
				{
					TRACEUSER( "Gerry", _T("FillGeometry attribute not trapped\n"));
				}
				break;

				case ATTR_DASHPATTERN:
				{
					DashPatternAttribute* pDash = (DashPatternAttribute*)(pAttr->GetAttributeValue());
					DashPatternAttribute NoDash;
					// If the attribute is different to the "no dash" then return FALSE
					// then we do not match
					if (NoDash.IsDifferent(pDash))
						return(FALSE);
				}
				break;

				case ATTR_STROKETYPE:
				{
					StrokeTypeAttrValue* pStroke = (StrokeTypeAttrValue*)(pAttr->GetAttributeValue());
					StrokeTypeAttrValue DefStroke;
					if (!((*pStroke) == DefStroke))
						return(FALSE);
				}
				break;

				case ATTR_VARWIDTH:
				{
					VariableWidthAttrValue* pVarWidth = (VariableWidthAttrValue*)(pAttr->GetAttributeValue());
					VariableWidthAttrValue DefVarWidth;
					if (!((*pVarWidth) == DefVarWidth))
						return(FALSE);
				}
				break;

				case ATTR_BRUSHTYPE:
				{
					BrushAttrValue* pBrush = (BrushAttrValue*)(pAttr->GetAttributeValue());
					BrushAttrValue DefBrush;
					if (DefBrush.IsDifferent(pBrush))
						return(FALSE);
				}
				break;
				
				case ATTR_FEATHER:
				{
					FeatherAttrValue* pFeather = (FeatherAttrValue*)(pAttr->GetAttributeValue());
					if (pFeather->GetFeatherSize() != 0)
						return(FALSE);
				}
				break;
			}
		}
	}

	// We've got this far so there are no non-plain attributes and we can return TRUE
	return(TRUE);
}


/****************************************************************************

>	BOOL XPFCText::IsNodeJustified(Node* pNode)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/09/2005

	Returns:	TRUE if the node contains non-"left justified" text, FALSE 
				if it only uses left justify
	Purpose:	This determines if a text story uses non-left justification

****************************************************************************/

BOOL XPFCText::IsNodeJustified(Node* pRootNode)
{
//	TRACEUSER( "Gerry", _T("IsNodeJustified(%s)\n"), pRootNode->GetRuntimeClass()->m_lpszClassName);

	// If this isn't a renderable ink node then get out
	if (!pRootNode->IS_KIND_OF(NodeRenderableInk))
		return(FALSE);

	NodeRenderableInk* pInkNode = (NodeRenderableInk*)pRootNode;

	// We scan through the tree looking for any AttrTxtJustification nodes
	Node* pNode = pRootNode->FindFirstDepthFirst();
	while (pNode)
	{
		// Check this node
		if (pNode->IsAnAttribute() && pNode->IS_KIND_OF(AttrTxtJustification))
		{
			AttrTxtJustification* pAttr = (AttrTxtJustification*)pNode;
			if (pAttr->Value.justification != JLEFT)
				return(TRUE);
		}

		// Move on to the next node
		pNode = pNode->FindNextDepthFirst(pRootNode);
	}

	// No justification in the story so test the applied justification ignoring children
	AttrTxtJustification* pAttr = (AttrTxtJustification*)(pInkNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTxtJustification), TRUE));
	if (pAttr && pAttr->Value.justification != JLEFT)
		return(TRUE);

	// We've got this far so there are no non-left attributes and we can return FALSE
	return(FALSE);
}


BOOL XPFCFill::DoAttributesMatch(RenderRegion* pRegion)
{
	// If this item specifies the shape property
	if (m_Shape != XPFB_UNKNOWN)
	{
		// Get the fill attribute from the render region
		ColourFillAttribute* pFill = (ColourFillAttribute*)(pRegion->GetCurrentAttribute(ATTR_FILLGEOMETRY));
		if (m_Shape != pFill->GetGeometryShape())
			return(FALSE);
	}

	// If this item specifies the repeat property
	if (m_Repeat != XPFB_UNKNOWN)
	{
		// Get the fill repeat attribute from the render region
		INT32 Shape = ((ColourFillAttribute*)(pRegion->GetCurrentAttribute(ATTR_FILLGEOMETRY)))->GetGeometryShape();
		if (Shape != FILLSHAPE_FLAT)
		{
			INT32 Repeat = ((FillMappingAttribute*)(pRegion->GetCurrentAttribute(ATTR_FILLMAPPING)))->Repeat;
			switch (Repeat)
			{
				case XPFP_FILLREPEAT_REPEAT:
					if (Shape >= FILLSHAPE_FLAT && Shape <= FILLSHAPE_DIAMOND)
						Repeat = XPFP_FILLREPEAT_SIMPLE;
					break;

				case XPFP_FILLREPEAT_REPEATINV:
					if (Shape >= FILLSHAPE_FLAT && Shape <= FILLSHAPE_DIAMOND)
						Repeat = XPFP_FILLREPEAT_SIMPLE;
					break;

				case XPFP_FILLREPEAT_REPEATEXTRA:
					if (Shape >= FILLSHAPE_BITMAP && Shape <= FILLSHAPE_PLASMA)
						Repeat = XPFP_FILLREPEAT_SIMPLE;
					break;
			}
			if (m_Repeat != Repeat)
				return(FALSE);
		}
	}

	// If this item specifies the multistage property
	if (m_bMultistage != XPFB_UNKNOWN)
	{
		// Get the fill attribute from the render region
		ColourFillAttribute* pFill = (ColourFillAttribute*)(pRegion->GetCurrentAttribute(ATTR_FILLGEOMETRY));
		BOOL bMulti = (pFill->GetColourRamp() != NULL && pFill->GetColourRamp()->GetCount() > 0);
		if (m_bMultistage != bMulti)
			return(FALSE);
	}
	
	// If this item specifies the effect property
	if (m_Effect != XPFB_UNKNOWN)
	{
		ColourFillAttribute* pFill = ((ColourFillAttribute*)(pRegion->GetCurrentAttribute(ATTR_FILLGEOMETRY)));
		INT32 Shape = pFill->GetGeometryShape();
		if (Shape != FILLSHAPE_FLAT && Shape != FILLSHAPE_3POINT && Shape != FILLSHAPE_4POINT)
		{
			BOOL bCheckEffect = TRUE;
			// If it is a bitmap or fractal fill and has no colours then ignore the effect
			if (Shape == FILLSHAPE_BITMAP || Shape == FILLSHAPE_CLOUDS || Shape == FILLSHAPE_PLASMA)
			{
				if (pFill->GetStartColour() == NULL && pFill->GetEndColour() == NULL)
					bCheckEffect = FALSE;
			}
		
			if (bCheckEffect)
			{
				// Get the fill effect from the render region
				EFFECTTYPE Effect = pRegion->GetFillEffect();
				if (m_Effect != Effect)
					return(FALSE);
			}
		}
	}
	
	// If this item specifies the profile property
	if (m_bProfile != XPFB_UNKNOWN)
	{
		// Get the fill attribute from the render region
		ColourFillAttribute* pFill = (ColourFillAttribute*)(pRegion->GetCurrentAttribute(ATTR_FILLGEOMETRY));

		CProfileBiasGain DefaultProfile;
	
		BOOL bProfile = !(*(pFill->GetProfilePtr()) == DefaultProfile);
		if (m_bProfile != bProfile)
			return(FALSE);
	}

	// If this item specifies the contone property
	if (m_bContone != XPFB_UNKNOWN)
	{
		// Get the fill attribute from the render region
		ColourFillAttribute* pFill = (ColourFillAttribute*)(pRegion->GetCurrentAttribute(ATTR_FILLGEOMETRY));
		if (pFill->GetGeometryShape() == FILLSHAPE_BITMAP)
		{
			BOOL bContone = (pFill->GetStartColour() != NULL) || (pFill->GetEndColour() != NULL);
			if (m_bContone != bContone)
				return(FALSE);
		}
	}

	return(TRUE);
}


BOOL XPFCFillTrans::DoAttributesMatch(RenderRegion* pRegion)
{
	// If this item specifies the shape property
	if (m_Shape != XPFB_UNKNOWN)
	{
		// Get the transp attribute from the render region
		TranspFillAttribute* pTrans = (TranspFillAttribute*)(pRegion->GetCurrentAttribute(ATTR_TRANSPFILLGEOMETRY));
		if (m_Shape != pTrans->GetGeometryShape())
			return(FALSE);
	}

	// If this item specifies the type property
	if (m_Type != XPFB_UNKNOWN)
	{
		// Get the transp attribute from the render region
		TranspFillAttribute* pTrans = (TranspFillAttribute*)(pRegion->GetCurrentAttribute(ATTR_TRANSPFILLGEOMETRY));
		// Get the type
		UINT32 Type = pTrans->GetTranspType();
		// If we are flat, mix and 0% trans
		if (Type == TT_Mix &&
			pTrans->GetGeometryShape() == FILLSHAPE_FLAT &&
			*(pTrans->GetStartTransp()) == 0)
		{
			// We are actually type none
			Type = TT_NoTranspType;
		}
		if (m_Type != Type)
			return(FALSE);
	}

	// If this item specifies the repeat property
	if (m_Repeat != XPFB_UNKNOWN)
	{
		// Get the transp and repeat attributes from the render region
		INT32 Repeat = ((TranspFillMappingAttribute*)(pRegion->GetCurrentAttribute(ATTR_TRANSPFILLMAPPING)))->Repeat;
		INT32 Shape = ((TranspFillAttribute*)(pRegion->GetCurrentAttribute(ATTR_TRANSPFILLGEOMETRY)))->GetGeometryShape();
		switch (Repeat)
		{
			case XPFP_FILLREPEAT_REPEAT:
				if (Shape >= FILLSHAPE_FLAT && Shape <= FILLSHAPE_DIAMOND)
					Repeat = XPFP_FILLREPEAT_SIMPLE;
				break;

			case XPFP_FILLREPEAT_REPEATINV:
				if (Shape >= FILLSHAPE_FLAT && Shape <= FILLSHAPE_DIAMOND)
					Repeat = XPFP_FILLREPEAT_SIMPLE;
				break;

			case XPFP_FILLREPEAT_REPEATEXTRA:
				if (Shape >= FILLSHAPE_BITMAP && Shape <= FILLSHAPE_PLASMA)
					Repeat = XPFP_FILLREPEAT_SIMPLE;
				break;
		}
		if (m_Repeat != Repeat)
			return(FALSE);
	}

	// If this item specifies the profile property
	if (m_bProfile != XPFB_UNKNOWN)
	{
		// Get the fill attribute from the render region
		TranspFillAttribute* pFill = (TranspFillAttribute*)(pRegion->GetCurrentAttribute(ATTR_TRANSPFILLGEOMETRY));
		CProfileBiasGain DefaultProfile;
		BOOL bProfile = !(*(pFill->GetProfilePtr()) == DefaultProfile);
		if (m_bProfile != bProfile)
			return(FALSE);
	}
	
	return(TRUE);
}


BOOL XPFCLine::DoAttributesMatch(RenderRegion* pRegion)
{
	StrokeColourAttribute* pStrokeCol = (StrokeColourAttribute*)(pRegion->GetCurrentAttribute(ATTR_STROKECOLOUR));
	BOOL bNoStroke = pStrokeCol->Colour.IsTransparent();

	// If this item specifies the dash property
	if (m_bDash != XPFB_UNKNOWN)
	{
		DashPatternAttribute NoDash;
		// Get the dash pattern attribute from the render region
		DashPatternAttribute* pDash = (DashPatternAttribute*)(pRegion->GetCurrentAttribute(ATTR_DASHPATTERN));
		if (bNoStroke)
			pDash = &NoDash;
		// If the attribute is the "no dash" and m_bDash is TRUE or
		// the attribute is not "no dash" and m_bDash is FALSE
		// then we do not match
		if (m_bDash != NoDash.IsDifferent(pDash))
			return(FALSE);
	}
	
	// If this item specifies the arrowhead property
	if (m_bArrowhead != XPFB_UNKNOWN)
	{
		StartArrowAttribute DefStart;
		EndArrowAttribute DefEnd;

		// Get the arrow attributes from the render region
		StartArrowAttribute* pStart = (StartArrowAttribute*)(pRegion->GetCurrentAttribute(ATTR_STARTARROW));
		if (bNoStroke)
			pStart = &DefStart;
		EndArrowAttribute* pEnd = (EndArrowAttribute*)(pRegion->GetCurrentAttribute(ATTR_ENDARROW));
		if (bNoStroke)
			pEnd = &DefEnd;

		BOOL bHasStart = DefStart.IsDifferent(pStart);
		BOOL bHasEnd = DefEnd.IsDifferent(pEnd);

		if (m_bArrowhead != (bHasStart | bHasEnd))
			return(FALSE);
	}
	
	// If this item specifies the cap property
	if (m_Cap != XPFB_UNKNOWN)
	{
		// Get the dash pattern attribute from the render region
		StartCapAttribute* pCap = (StartCapAttribute*)(pRegion->GetCurrentAttribute(ATTR_STARTCAP));
		if (m_Cap != pCap->StartCap)
			return(FALSE);
	}
	
	// If this item specifies the join property
	if (m_Join != XPFB_UNKNOWN)
	{
		// Get the dash pattern attribute from the render region
		JoinTypeAttribute* pJoin = (JoinTypeAttribute*)(pRegion->GetCurrentAttribute(ATTR_JOINTYPE));
		if (m_Join != pJoin->JoinType)
			return(FALSE);
	}
	
	// If this item specifies the stroke property
	if (m_bStroke != XPFB_UNKNOWN)
	{
		StrokeTypeAttrValue DefStroke;
		VariableWidthAttrValue DefVarWidth;

		// Get the attributes from the render region
		StrokeTypeAttrValue* pStroke = (StrokeTypeAttrValue*)(pRegion->GetCurrentAttribute(ATTR_STROKETYPE));
		VariableWidthAttrValue* pVarWidth = (VariableWidthAttrValue*)(pRegion->GetCurrentAttribute(ATTR_VARWIDTH));
		if (bNoStroke)
		{
			pStroke = &DefStroke;
			pVarWidth = &DefVarWidth;
		}

		BOOL bStroke = !(((*pStroke) == DefStroke) && ((*pVarWidth) == DefVarWidth));
		if (m_bStroke != bStroke)
			return(FALSE);
	}

	// If this item specifies the brush property
	if (m_bBrush != XPFB_UNKNOWN)
	{
		BrushAttrValue DefBrush;

		// Get the dash pattern attribute from the render region
		BrushAttrValue* pBrush = (BrushAttrValue*)(pRegion->GetCurrentAttribute(ATTR_BRUSHTYPE));
		if (bNoStroke)
			pBrush = &DefBrush;

		if (m_bBrush != DefBrush.IsDifferent(pBrush))
			return(FALSE);
	}

	return(TRUE);
}


BOOL XPFCLineTrans::DoAttributesMatch(RenderRegion* pRegion)
{
	// If this item specifies the type property
	if (m_Type != XPFB_UNKNOWN)
	{
		StrokeColourAttribute* pStrokeCol = (StrokeColourAttribute*)(pRegion->GetCurrentAttribute(ATTR_STROKECOLOUR));
		BOOL bNoStroke = pStrokeCol->Colour.IsTransparent();

		// Get the transp attribute from the render region
		StrokeTranspAttribute* pTrans = (StrokeTranspAttribute*)(pRegion->GetCurrentAttribute(ATTR_STROKETRANSP));
		// Get the type
		UINT32 Type = pTrans->GetTranspType();
		// If we have no stroke colour or are mix and 0% trans
		if (bNoStroke || (Type == TT_Mix && *(pTrans->GetStartTransp()) == 0))
		{
			// We are actually type none
			Type = TT_NoTranspType;
		}
		if (m_Type != Type)
			return(FALSE);
	}

	return(TRUE);
}


BOOL XPFCFeather::DoesNodeMatch(Node* pNode)
{
	AttrFeather* pAttr = (AttrFeather*)(pNode->FindFirstChild(CC_RUNTIME_CLASS(AttrFeather)));
	if (pAttr && (pAttr->Value.GetFeatherSize() != 0))
		return(TRUE);
	
	return(FALSE);
}


BOOL XPFCFeather::DoAttributesMatch(RenderRegion* pRegion)
{
	// Get the feather attribute from the render region
	OffscreenAttrValue* pAttr = pRegion->GetCurrentOffscreenAttr();
	if (!IS_A(pAttr, FeatherAttrValue))
		return(FALSE);

	FeatherAttrValue* pFeather = (FeatherAttrValue*)pAttr;
	if (pFeather->GetFeatherSize() == 0)
		return(FALSE);

	return(TRUE);
}


// Pass 1 can return the following convert types:
// Spread: native, bitmap, simple
// Layer: native, bitmap, simple
// Objects: native, remove, simple
// Attributes: native, remove
// All other types are returned as native for processing in later passes

XPFConvertType CapabilityTree::GetConvertTypePass1(Node* pNode, XPFRenderRegion* pRegion)
{
	if (pNode->IsAnAttribute())
	{
		return(XPFCONVTYPE_NATIVE);
	}

	// First we need to check for spreads and layers, returning the appropriate type
	if (pNode->IsSpread())
		return(m_SpreadType);

	XPFCapability* pItem = GetObjects();
	XPFConvertType Type = GetObjectsType();
		
	while (pItem)
	{
		XPFConvertType ItemType = pItem->GetConvertTypeForNode(pNode);
		if (ItemType != XPFCONVTYPE_UNKNOWN)
			Type = ItemType;

		pItem = pItem->GetNext();
	}

	if (Type != XPFCONVTYPE_NATIVE && 
		Type != XPFCONVTYPE_REMOVE && 
		Type != XPFCONVTYPE_SIMPLE && 
		Type != XPFCONVTYPE_BITMAP &&
		Type != XPFCONVTYPE_REFORMAT)
		Type = XPFCONVTYPE_NATIVE;

	return(Type);
}


// Pass 2 can return the following convert types:
// This pass is mainly concerned with the stroked conversion type
// The current state of attributes in the render region is scanned and if they result 
// in the stroked conversion type then that is returned otherwise native is returned

XPFConvertType CapabilityTree::GetConvertTypePass2(Node* pNode, XPFRenderRegion* pRegion)
{
	XPFConvertType Type = GetAttributesType();
	XPFCapability* pItem = GetAttributes();
	while (pItem)
	{
		XPFConvertType ItemType = pItem->GetConvertTypeForAttrs(pRegion);
		if (ItemType == XPFCONVTYPE_STROKED)
			Type = ItemType;

		pItem = pItem->GetNext();
	}

	if (Type != XPFCONVTYPE_NATIVE && Type != XPFCONVTYPE_STROKED)
		Type = XPFCONVTYPE_NATIVE;

	return(Type);
}


// Pass 3 is concerned with the bitmapfill and bitmaptrans conversion types
// The current state of attributes in the render region is scanned and if they result 
// in the bitmapfill conversion type then that is returned otherwise native is returned

void CapabilityTree::GetConvertTypePass3(Node* pNode, XPFRenderRegion* pRegion, BOOL* pbFill, BOOL* pbTrans, BOOL* pbFillTrans)
{
	XPFConvertType FillType = GetAttributesType();
	XPFConvertType TransType = FillType;
	XPFConvertType BothType = FillType;
	XPFCapability* pItem = GetAttributes();
	while (pItem)
	{
		XPFConvertType ItemType = pItem->GetConvertTypeForAttrs(pRegion);
		if (ItemType == XPFCONVTYPE_BITMAPFILL)
			FillType = ItemType;
		if (ItemType == XPFCONVTYPE_BITMAPTRANS)
			TransType = ItemType;
		if (ItemType == XPFCONVTYPE_BITMAPFILLTRANS)
			BothType = ItemType;

		pItem = pItem->GetNext();
	}

	*pbFill = (FillType == XPFCONVTYPE_BITMAPFILL);
	*pbTrans = (TransType == XPFCONVTYPE_BITMAPTRANS);
	*pbFillTrans = (BothType == XPFCONVTYPE_BITMAPFILLTRANS);
}

// Pass 4 is concerned with the bitmap conversion type
// The object and the current state of attributes in the render region is scanned and if they result 
// in the bitmap conversion type then that is returned otherwise native is returned

XPFConvertType CapabilityTree::GetConvertTypePass4(Node* pNode, XPFRenderRegion* pRegion)
{
	XPFConvertType Type = GetObjectsType();
	XPFCapability* pItem = GetObjects();
	while (pItem)
	{
		XPFConvertType ItemType = pItem->GetConvertTypeForNode(pNode);
		if (ItemType == XPFCONVTYPE_BITMAP)
			Type = ItemType;

		pItem = pItem->GetNext();
	}

	if (Type != XPFCONVTYPE_NATIVE && Type != XPFCONVTYPE_BITMAP)
		Type = XPFCONVTYPE_NATIVE;

	return(Type);
}



// Pass 5 is concerned with the bitmapspan conversion type
// The current state of attributes in the render region is scanned and if they result 
// in the bitmapspan conversion type then that is returned otherwise native is returned

XPFConvertType CapabilityTree::GetConvertTypePass5(Node* pNode, XPFRenderRegion* pRegion)
{
	XPFConvertType Type = GetAttributesType();
	XPFCapability* pItem = GetAttributes();
	while (pItem)
	{
		XPFConvertType ItemType = pItem->GetConvertTypeForAttrs(pRegion);
		if (ItemType == XPFCONVTYPE_BITMAPSPAN)
			Type = ItemType;

		pItem = pItem->GetNext();
	}

	if (Type != XPFCONVTYPE_NATIVE && Type != XPFCONVTYPE_BITMAPSPAN)
		Type = XPFCONVTYPE_NATIVE;

	return(Type);
}
