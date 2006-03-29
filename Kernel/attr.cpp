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

// The basic Camelot attributes

/*
*/

#include "camtypes.h"


#include "attr.h"
#include "lineattr.h"
#include "userattr.h"
#include "ink.h"
#include "fillattr.h"
#include "attrmgr.h"
#include "blendatt.h"
#include "pathstrk.h"

//#include "will2.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNAMIC(AttributeValue, CCObject);
CC_IMPLEMENT_DYNCREATE(DashRec,	CCObject);

CC_IMPLEMENT_DYNCREATE(LineWidthAttribute,   	AttributeValue);
CC_IMPLEMENT_DYNCREATE(StartArrowAttribute,  	AttributeValue);
CC_IMPLEMENT_DYNCREATE(EndArrowAttribute,    	AttributeValue);
CC_IMPLEMENT_DYNCREATE(StartCapAttribute,    	AttributeValue);
CC_IMPLEMENT_DYNCREATE(JoinTypeAttribute,    	AttributeValue);
CC_IMPLEMENT_DYNCREATE(MitreLimitAttribute,  	AttributeValue);
CC_IMPLEMENT_DYNCREATE(WindingRuleAttribute, 	AttributeValue);
CC_IMPLEMENT_DYNCREATE(DashPatternAttribute, 	AttributeValue);
CC_IMPLEMENT_DYNCREATE(DrawingModeAttribute, 	AttributeValue);
CC_IMPLEMENT_DYNCREATE(UserAttribute,   		AttributeValue);

CC_IMPLEMENT_MEMDUMP(BlendAttrParam,			CC_CLASS_MEMDUMP);

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	virtual void AttributeValue::Restore(RenderRegion *pRegion, BOOL Temporary)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temporary - TRUE if this is a temporary attribute, FALSE if it is
							permanent (e.g. it's in a document tree).
	Purpose:	Used when restoring attributes from the render region's context stack.

				This changes the relevant current rendering attribute to be that described
				by this AttributeValue object.
				
				The Temporary flag controls what is done with this object when it is no
				longer needed by the stack - i.e. the next time an attribute of this type
				is restored from the stack, this attribute value will no longer be 
				referenced, and so if the Temporary flag is TRUE (i.e. this object was
				created to satisfy a direct attribute manipulation request as opposed to
				rendering an attribute from a Camelot document tree), then the object is
				deleted.

				NB. This is a pure virtual function - any attribute value derived from
					this class must implement this function so that it restores the
					render region's attribute variables and objects to the correct state.

	SeeAlso:	RenderStack; AttributeValue; NodeAttribute;
				AttributeValue::Render; AttributeValue::SimpleCopy

********************************************************************************************/

/********************************************************************************************

>	virtual void AttributeValue::Render(RenderRegion *pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Used when a render region needs to change a particular rendering attribute.

				This usually occurs either because a tree is being rendered, and an
				attribute node has been encountered and hence needs to be rendered, or
				because a RenderRegion client (or sometimes the RenderRegion itself) has
				asked to change an attribute directly, by name, as it were.  Examples of the
				latter case might be:

				MonoOn
				pRegion->SetFillColour(COLOUR_BLACK);
				pRegion->SetLineWidth(300);
				pRegion->SetDrawingMode(DM_EORPEN);
				MonoOff

				NB. This is a pure virtual function - any attribute value derived from
					this class must implement this function so that it sets the
					render region's attribute variables and objects to the correct state.
				
	SeeAlso:	RenderStack; AttributeValue; NodeAttribute;
				AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

/********************************************************************************************

>	virtual void AttributeValue::SimpleCopy(AttributeValue *pAttr)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	This function provides a way of copying the contents of an AttributeValue
				derived class to another.  It is assumed that the source object is the
				same class as the destination object.
				The data from the object pointed to by pAttr is copied into this object.
				This is primarily used when cloning render regions - the context stack
				must be copied, and if it points to any temporary objects, then they
				must be copied too, to avoid multiple deletion errors.
	SeeAlso:	RenderStack; AttributeValue; NodeAttribute;
				AttributeValue::Render; AttributeValue::Restore

********************************************************************************************/


/********************************************************************************************

>	NodeAttribute *AttributeValue::MakeNode()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	Pointer to NodeAttribute object, or NULL if out of memory.
	Purpose:	Given an AttributeValue object, construct the appropriate NodeAttribute
				object which can be put into a document tree.
				This should be over-ridden for all derived AttributeValue classes, except
				those that cannot be put into the document tree (e.g. DrawingModeAttribute).
				The base class version (i.e. this one) will always ENSURE because it
				should not be called.
	Errors:		Out of memory.
	SeeAlso:	NodeAttribute

********************************************************************************************/

NodeAttribute* AttributeValue::MakeNode()
{
	TRACE( wxT("Error - Base class MakeNode() function called for a %s\n"),
		  GetRuntimeClass()->GetClassName() );
	return NULL;
}


/********************************************************************************************
>	NodeAttribute* AttributeValue::MakeNode(Node* pContextNode, AttachNodeDirection Direction)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95
	Inputs:		pContextNode - 
				Direction    - 
	Returns:	Pointer to NodeAttribute object, or NULL if out of memory.
	Purpose:	Convert an attribute value into an attribute node and attatch to the context node
	SeeAlso:	AttributeValue::MakeNode()
********************************************************************************************/

NodeAttribute* AttributeValue::MakeNode(Node* pContextNode, AttachNodeDirection Direction)
{
	NodeAttribute* pAttrNode=MakeNode();
	if (pAttrNode!=NULL)
		pAttrNode->AttachNode(pContextNode, Direction, FALSE);
	return pAttrNode;
}


/********************************************************************************************

>	BOOL AttributeValue::IsDifferent(AttributeValue *pAttr)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		pAttr - the attribute to compare against this one.
	Returns:	TRUE if objects represent different attributes (e.g. one is red and the 
				     other is blue);
				FALSE otherwise (i.e. they represent the same attribute, e.g. both 
					  represent 0.25pt lines).
	Purpose:	Compare two AttributeValue objects.
				The base class version always returns TRUE - derived classes should
				override this function to provide functionality.
	SeeAlso:	AttributeManager::ApplyBasedOnDefaults

********************************************************************************************/

BOOL AttributeValue::IsDifferent(AttributeValue*)
{
	return TRUE;
}

/********************************************************************************************

>	BOOL AttributeValue::Blend(BlendAttrParam *pBlendParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94
	Inputs:		pBlendParam = ptr to param holding all data needed for the attr val to blend
	Outputs:	if TRUE  returned, pBlendParam->GetBlendedAttrVal() will get ptr to the blended attr val
				if FALSE returned, pBlendParam->GetBlendedAttrVal() will return NULL
	Returns:	TRUE if successful, FALSE otherwaie
	Purpose:	Blends this attr val with the attr val associated with the other NodeAttribute
				ptr held in pBlendParam
				This base version just returns FALSE, and sets the blended attr val ptr to NULL
	Errors:		Errors in debug builds
	SeeAlso:	-

********************************************************************************************/

BOOL AttributeValue::Blend(BlendAttrParam *pBlendParam)
{
	ERROR3("AttributeValue::Blend() called");
	pBlendParam->SetBlendedAttrVal(NULL);
	return FALSE;
}


/********************************************************************************************

>	virtual AttributeValue *AttributeValue::MouldIntoStroke(PathStrokerVector *pMoulder,
																double TransScale);
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/97

	Inputs:		pMoulder	- A PathStrokerVector which knows how to translate points to "mould" them
							  (May be NULL, in which case moulding of points does not occur)

				TransScale	- A fraction between 0.0 and 1.0, by which any transparency
							  values in this geometry will be scaled, allowing the caller to
							  effectively apply a flat transparency level to everything that is
							  moulded. Use 1.0 to leave transparency unaltered.

	Returns:	NULL if there is no need to mould this attribute (so you should just
				render the original attribute), else
				A pointer to a copy of this object, suitably moulded and adjusted for
				the transparency scaling. The caller must delete the copy when finished
				with it.

	Purpose:	Helper function for the PathStrokerVector class, which "moulds" clipart
				subtrees to lie along an arbitrary path.

				This function is called to mould attributes, so that:
					* Fill/Trans geometries can be moulded
					* Line Widths can be scaled to an appropriate value
					* A global flat transparency can be applied
					etc

	Notes:		The base class does nothing (it returns NULL - by default, attrs assume that
				they will work OK when moulded without being changed)

********************************************************************************************/

AttributeValue *AttributeValue::MouldIntoStroke(PathStrokerVector *pMoulder, double TransScale)
{
	return(NULL);
}


/********************************************************************************************
>	virtual BOOL AttributeValue::CanBeRenderedDirectly()
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/00
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	There is only one case to date where AttributeValue are rendered directly ie 
				without calling the render function of their associated/containing NodeAttribute
				class. This occurs in RenderRegion::InitDevice().
				This function is to determine if the AttributeValue can be rendered independently
				of the NodeAttribute. It is only used in RR::InitDevice() at present but I am
				defining it in order to highlight this new characteristic of nodes which are geometry
				dependent, in order to ensure that this becomes an acknowledged attribute design
				consideration (ie so that these special cases get handled correctly by future code
				which may need to render AttributeValues directly)
	Errors:		
	SeeAlso:	-
********************************************************************************************/
BOOL AttributeValue::CanBeRenderedDirectly()
{
	return TRUE;
}


//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

LineWidthAttribute::LineWidthAttribute()
{
	LineWidth = 500 + 1;  // BODGE The 1 added by Simon 25/10/95, so that the default attr 
						  // is different from the current attr. That way line scaling at 
						  // least works in simple cases.
}

/********************************************************************************************

>	void LineWidthAttribute::Render(RenderRegion *pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the line width attribute for the given render region. i.e. all
				lines drawn will now be drawn with this line width.
	SeeAlso:	LineWidthAttribute; RenderStack; AttributeValue; NodeAttribute;
				LineWidthAttribute::Restore; LineWidthAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void LineWidthAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "LineWidthAttribute::Render - do nothing" )
//	TRACE( wxT("Warning - LineWidthAttribute::Render called\n") );
	pRegion->SetLineWidth(this, Temp);
}

/********************************************************************************************

>	void LineWidthAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the line width attribute for the given render region. i.e. all
				lines drawn will now be drawn with this line width.
	SeeAlso:	LineWidthAttribute; RenderStack; AttributeValue; NodeAttribute;
				LineWidthAttribute::Render; LineWidthAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void LineWidthAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "LineWidthAttribute::Restore - do nothing" )
//	TRACE( wxT("Warning - LineWidthAttribute::Restore called\n") );
	pRegion->RestoreLineWidth(this, Temp);
}

/********************************************************************************************

>	void LineWidthAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	LineWidthAttribute; RenderStack; AttributeValue; NodeAttribute;
				LineWidthAttribute::Render; LineWidthAttribute::Restore;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void LineWidthAttribute::SimpleCopy(AttributeValue *pValue)
{
	LineWidth = ((LineWidthAttribute *) pValue)->LineWidth;
}

/********************************************************************************************

>	BOOL LineWidthAttribute::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers line width attribute, and provides a default attribute to give
				0.25pt lines.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL LineWidthAttribute::Init()
{
	// Default to 0.25pt lines
	LineWidthAttribute *pAttr = new LineWidthAttribute;
	if (pAttr == NULL)
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	if (ID == ATTR_BAD_ID)
		return FALSE;
	ENSURE(ID == ATTR_LINEWIDTH, "Incorrect ID for attribute!");
	return TRUE;
}

/********************************************************************************************

>	NodeAttribute *LineWidthAttribute::MakeNode()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrLineWidth node from this line width attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *LineWidthAttribute::MakeNode()
{
	// Create new attribute node
	AttrLineWidth *pAttr = new AttrLineWidth();

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	BOOL LineWidthAttribute::IsDifferent(AttributeValue *pAttr)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL LineWidthAttribute::IsDifferent(AttributeValue *pAttr)
{
	ENSURE(GetRuntimeClass() == pAttr->GetRuntimeClass(), 
		   "Different attribute types in AttributeValue::IsDifferent()");
	return ((LineWidthAttribute *) pAttr)->LineWidth != LineWidth;
}

/********************************************************************************************

>	BOOL LineWidthAttribute::Blend(BlendAttrParam *pBlendParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94
	Inputs:		pBlendParam = ptr to param holding all data needed for the attr val to blend
	Outputs:	if TRUE  returned, pBlendParam->GetBlendedAttrVal() will get ptr to the blended attr val
				if FALSE returned, pBlendParam->GetBlendedAttrVal() will return NULL
	Returns:	TRUE if successful, FALSE otherwaie
	Purpose:	Blends this attr val with the attr val held in pBlendParam
	SeeAlso:	-

********************************************************************************************/

BOOL LineWidthAttribute::Blend(BlendAttrParam *pBlendParam)
{
	// Check entry param
	ERROR3IF(pBlendParam == NULL,"NULL entry param");
	if (pBlendParam == NULL) return FALSE;

	LineWidthAttribute* pOtherLineAttr = (LineWidthAttribute*) pBlendParam->GetOtherAttrVal();

	// Check that the other line attr val is not NULL, and is a LineWidthAttribute
	ERROR3IF(pOtherLineAttr == NULL,"NULL other attr val");
	ERROR3IF(!IS_A(pOtherLineAttr,LineWidthAttribute),"other attr val not a line width attr");
	if (pOtherLineAttr == NULL || !IS_A(pOtherLineAttr,LineWidthAttribute)) return FALSE;

	// Get a new LineWidthAttribute to hold the blended version, (return FALSE if this fails)
	LineWidthAttribute* pBlendedLineAttr = new LineWidthAttribute;
	if (pBlendedLineAttr == NULL) return FALSE;

	// The blended line width is done by linearly tending the start line width to the
	// end line width, proportionally to the blend ration (0.0 to 1.0)

	// DeltaWidth is the amount to subtract from this LineWidth, and is proportional to BlendRatio
	MILLIPOINT DeltaWidth = INT32((LineWidth - pOtherLineAttr->LineWidth) * pBlendParam->GetBlendRatio());

	pBlendedLineAttr->LineWidth = LineWidth - DeltaWidth;

	// Store the ptr to the new blended line width attr val
	pBlendParam->SetBlendedAttrVal(pBlendedLineAttr);

	return TRUE;
}

/********************************************************************************************

>	virtual AttributeValue *LineWidthAttribute::MouldIntoStroke(PathStrokerVector *pMoulder,
																double TransScale);
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/97

	Inputs:		pMoulder	- A PathStrokerVector which knows how to translate points to "mould" them
							  (May be NULL, in which case moulding of points does not occur)

				TransScale	- A fraction between 0.0 and 1.0, by which any transparency
							  values in this geometry will be scaled, allowing the caller to
							  effectively apply a flat transparency level to everything that is
							  moulded. Use 1.0 to leave transparency unaltered.

	Returns:	NULL if there is no need to mould this attribute (so you should just
				render the original attribute), else
				A pointer to a copy of this object, suitably moulded and adjusted for
				the transparency scaling. The caller must delete the copy when finished
				with it.

	Purpose:	Helper function for the PathStrokerVector class, which "moulds" clipart
				subtrees to lie along an arbitrary path.

				This function is called to mould attributes, so that:
					* Fill/Trans geometries can be moulded
					* Line Widths can be scaled to an appropriate value
					* A global flat transparency can be applied
					etc

	Notes:		The base class does nothing (it returns NULL - by default, attrs assume that
				they will work OK when moulded without being changed)

********************************************************************************************/

AttributeValue *LineWidthAttribute::MouldIntoStroke(PathStrokerVector *pMoulder, double TransScale)
{
#ifdef VECTOR_STROKING // Neville 6/8/97
	LineWidthAttribute* pMouldedAttr = new LineWidthAttribute;
	if (pMouldedAttr != NULL)
	{
		double ScaledWidth = pMoulder->GetScaleFactor() * (double)LineWidth;
		if (ScaledWidth < 1.0)
			ScaledWidth = 1.0;
		else if (pMouldedAttr->LineWidth > 1000000.0)
			ScaledWidth = 1000000.0;
		pMouldedAttr->LineWidth = (INT32)ScaledWidth;
	}

	return(pMouldedAttr);
#else
	return NULL;
#endif
}



//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

/********************************************************************************************

>	DashPatternAttribute::DashPatternAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Purpose:	Default Constructor for dash pattern attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DashPatternAttribute::DashPatternAttribute()
{
	DashPattern.Elements = 0;
	DashPattern.DashStart = 0;

	DashPattern.ElementData = NULL;

	DashPattern.LineWidth = 72000/4;
}

/********************************************************************************************

>	DashPatternAttribute::~DashPatternAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Purpose:	Destructor for dash pattern attribute.
	SeeAlso:	-

********************************************************************************************/

DashPatternAttribute::~DashPatternAttribute()
{
	if (DashPattern.ElementData)
		delete DashPattern.ElementData;
}

/********************************************************************************************

>	void DashPatternAttribute::Render(RenderRegion *pRegion)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the dash pattern attribute for the given render region. i.e. all
				lines drawn will now be drawn with this dash pattern.
	SeeAlso:	-

********************************************************************************************/

void DashPatternAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "DashPatternAttribute::Render - do nothing" )
//	TRACE( wxT("Warning - LineWidthAttribute::Render called\n") );
	pRegion->SetDashPattern(this, Temp);
}

/********************************************************************************************

>	void DashPatternAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the dash pattern attribute for the given render region. i.e. all
				lines drawn will now be drawn with this dash pattern.
	SeeAlso:	-

********************************************************************************************/

void DashPatternAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "DashPatternAttribute::Restore - do nothing" )
//	TRACE( wxT("Warning - LineWidthAttribute::Restore called\n") );
	pRegion->RestoreDashPattern(this, Temp);
}

/********************************************************************************************

>	BOOL DashPatternAttribute::SetDashPattern(DashRec& NewDash)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Purpose:	Sets the dash pattern for this attribute.
	Errors:		No memory.
	SeeAlso:	-

********************************************************************************************/

BOOL DashPatternAttribute::SetDashPattern(DashRec& NewDash)
{
	DashPattern.Elements 			= NewDash.Elements;
	DashPattern.DashStart 			= NewDash.DashStart;
	DashPattern.LineWidth 			= NewDash.LineWidth;
	DashPattern.DashID 				= NewDash.DashID;
	DashPattern.ScaleWithLineWidth 	= NewDash.ScaleWithLineWidth;

	if (NewDash.Elements == 0)
		return TRUE;

	if (DashPattern.ElementData != NULL)
		delete DashPattern.ElementData;

	DashPattern.ElementData = new INT32[NewDash.Elements];

	if (DashPattern.ElementData == NULL)
		return FALSE;

	for (INT32 element = 0; element < NewDash.Elements; element++)
	{
		DashPattern.ElementData[element] = NewDash.ElementData[element];
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL DashPatternAttribute::SetStockDashPattern(StockDash NewDash)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Purpose:	Sets the dash pattern for this attribute.
	Errors:		No memory.
	SeeAlso:	-

********************************************************************************************/

BOOL DashPatternAttribute::SetStockDashPattern(StockDash NewDash)
{
	DashRec StockDash;
	DashElement DashArray[8];

	const INT32 DashUnit = StockDash.LineWidth;

	StockDash.DashID = NewDash;

	switch (NewDash)
	{
		case SD_SOLID:
			StockDash.Elements = 0;
			SetDashPattern(StockDash);
			break;

		case SD_DASH1:
			StockDash.Elements = 2;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*2;
			DashArray[1] = DashUnit*2;

			SetDashPattern(StockDash);
			break;

		case SD_DASH2:
			StockDash.Elements = 2;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*4;
			DashArray[1] = DashUnit*2;

			SetDashPattern(StockDash);
			break;

		case SD_DASH3:
			StockDash.Elements = 2;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*8;
			DashArray[1] = DashUnit*2;

			SetDashPattern(StockDash);
			break;

		case SD_DASH4:
			StockDash.Elements = 2;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*16;
			DashArray[1] = DashUnit*2;

			SetDashPattern(StockDash);
			break;

		case SD_DASH5:
			StockDash.Elements = 2;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*24;
			DashArray[1] = DashUnit*2;

			SetDashPattern(StockDash);
			break;

		case SD_DASH6:
			StockDash.Elements = 2;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*4;
			DashArray[1] = DashUnit*4;

			SetDashPattern(StockDash);
			break;

		case SD_DASH7:
			StockDash.Elements = 2;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*8;
			DashArray[1] = DashUnit*4;

			SetDashPattern(StockDash);
			break;

		case SD_DASH8:
			StockDash.Elements = 2;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*16;
			DashArray[1] = DashUnit*4;

			SetDashPattern(StockDash);
			break;

		case SD_DASH9:
			StockDash.Elements = 2;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*8;
			DashArray[1] = DashUnit*8;

			SetDashPattern(StockDash);
			break;

		case SD_DASH10:
			StockDash.Elements = 2;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*16;
			DashArray[1] = DashUnit*8;

			SetDashPattern(StockDash);
			break;

		case SD_DASH11:
			StockDash.Elements = 4;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*4;
			DashArray[1] = DashUnit*2;
			DashArray[2] = DashUnit*2;
			DashArray[3] = DashUnit*2;

			SetDashPattern(StockDash);
			break;

		case SD_DASH12:
			StockDash.Elements = 4;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*8;
			DashArray[1] = DashUnit*2;
			DashArray[2] = DashUnit*2;
			DashArray[3] = DashUnit*2;

			SetDashPattern(StockDash);
			break;

		case SD_DASH13:
			StockDash.Elements = 4;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*16;
			DashArray[1] = DashUnit*2;
			DashArray[2] = DashUnit*2;
			DashArray[3] = DashUnit*2;

			SetDashPattern(StockDash);
			break;

		case SD_DASH14:
			StockDash.Elements = 4;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*8;
			DashArray[1] = DashUnit*2;
			DashArray[2] = DashUnit*4;
			DashArray[3] = DashUnit*2;

			SetDashPattern(StockDash);
			break;

		case SD_DASH15:
			StockDash.Elements = 4;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*16;
			DashArray[1] = DashUnit*2;
			DashArray[2] = DashUnit*4;
			DashArray[3] = DashUnit*2;

			SetDashPattern(StockDash);
			break;

		case SD_DASH16:
			StockDash.Elements = 6;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*8;
			DashArray[1] = DashUnit*2;
			DashArray[2] = DashUnit*2;
			DashArray[3] = DashUnit*2;
			DashArray[4] = DashUnit*2;
			DashArray[5] = DashUnit*2;

			SetDashPattern(StockDash);
			break;

		case SD_DASH17:
			StockDash.Elements = 6;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*16;
			DashArray[1] = DashUnit*2;
			DashArray[2] = DashUnit*2;
			DashArray[3] = DashUnit*2;
			DashArray[4] = DashUnit*2;
			DashArray[5] = DashUnit*2;

			SetDashPattern(StockDash);
			break;

		case SD_DASH18:
			StockDash.Elements = 8;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*8;
			DashArray[1] = DashUnit*2;
			DashArray[2] = DashUnit*2;
			DashArray[3] = DashUnit*2;
			DashArray[4] = DashUnit*2;
			DashArray[5] = DashUnit*2;
			DashArray[6] = DashUnit*2;
			DashArray[7] = DashUnit*2;

			SetDashPattern(StockDash);
			break;

		case SD_DASH19:
			StockDash.Elements = 8;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*16;
			DashArray[1] = DashUnit*2;
			DashArray[2] = DashUnit*2;
			DashArray[3] = DashUnit*2;
			DashArray[4] = DashUnit*2;
			DashArray[5] = DashUnit*2;
			DashArray[6] = DashUnit*2;
			DashArray[7] = DashUnit*2;

			SetDashPattern(StockDash);
			break;

		case SD_DASH20:
			StockDash.Elements = 8;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			DashArray[0] = DashUnit*8;
			DashArray[1] = DashUnit*2;
			DashArray[2] = DashUnit*2;
			DashArray[3] = DashUnit*2;
			DashArray[4] = DashUnit*4;
			DashArray[5] = DashUnit*2;
			DashArray[6] = DashUnit*2;
			DashArray[7] = DashUnit*2;

			SetDashPattern(StockDash);
			break;

		case SD_DASH_GUIDELAYER:
			// This is the dash pattern used to render objects in guide layers with.
			// It's different to the others in that the dashed aren't scaled relative to the line width
			StockDash.Elements = 2;
			StockDash.DashStart = 0;
			StockDash.ElementData = DashArray;
			StockDash.ScaleWithLineWidth = FALSE;
			DashArray[0] = DashUnit*2;
			DashArray[1] = DashUnit*2;

			SetDashPattern(StockDash);
			break;

		default:
			StockDash.Elements = 0;
			StockDash.DashStart = 0;
			StockDash.ElementData = NULL;

			SetDashPattern(StockDash);
			break;
	}

	return TRUE;
}

DashRec::DashRec()
{
	Elements = 0;
	DashStart = 0;
	ElementData = NULL;
	DashID = 0;
	ScaleWithLineWidth = TRUE;

	LineWidth = 72000/4;
}

String_256 DashRec::GetStockDashName(StockDash DashType)
{
	String_256 DashName;

	switch (DashType)
	{
		case SD_SOLID:
			DashName.Load(_R(IDS_K_ATTR_SOLIDLINE));
			break;

		default:
			DashName.Load(_R(IDS_K_ATTR_DASHLINE));
			break;
	}

	return DashName;
}

/********************************************************************************************

>	void DashRec::CheckAndFix()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/07/95
	Purpose:	Checks a dash pattern, and fixes it if it's not very nice.
	Errors:		No errors.
	SeeAlso:	-

********************************************************************************************/

void DashRec::CheckAndFix()
{
	// check the dash offset
	if(DashStart < 0)
		DashStart = 0;

	if(Elements == 0)
		return;

	// check the dash pattern elements
	INT32 End = 0;
	INT32 l;

	for(l = 0; l < Elements; l++)
	{

		// is this element OK?
		if(ElementData[l] > 0)
		{
			// element is OK...
			if(End != l)
			{
				// move it back if something has been deleted before it
				ElementData[End] = ElementData[l];
			}
			End++;
		}
	}

	// have we got enough elements?
	if(End < 1)
	{
		// no elements - make it a solid outline
		Elements = 0;
		DashID = SD_SOLID;
	}
	else
	{
		Elements = End;
	}
}

/********************************************************************************************

>	void DashRec::CheckIfDefaultPattern()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/96
	Purpose:	Checks a dash pattern to see if it is one of the built in ones and if so
				then sets the ID to be correct.
				Not the best way but a) its quick, b) it works, c) not three million
				big rebuilds of 330 files to get it right

				This is mainly required for the following reason:
				When we save Native files we at present save out a dash pattern ID and then
				save out the full dash pattern definition! This is ok apart from wasting space.
				The problem happens when we load the information back in, we load the id and apply
				the relevent dash pattern and then load the definition and apply it, zapping the
				id.
	Errors:		No errors.
	SeeAlso:	-

********************************************************************************************/

/* Disable the code as completely useless as we scale the line widths when we first save them
   in a old format xar file! So the simple fix becomes a nightmare!

void DashRec::CheckIfDefaultPattern()
{
	// Check to see if the pattern corresponds to one of our built in ones
	// If so then set the DashID accordingly
	// Default is do nothing
	// SD_SOLID == 0 so if this then do nothing

	const INT32 DefLineWidth = 72000/4;
	const INT32 DashUnit = DefLineWidth;

	// All the useful defaults are defined like this using the default constructed
	// values, so if they are different then its not default pattern
	if (LineWidth != DefLineWidth || DashID != 0 || DashStart != 0 || Elements == 0)
		return;
	
	// The SD_DASH_GUIDELAYER has 2 elements and is the only one to have ScaleWithLineWidth = FALSE
	if (ScaleWithLineWidth != TRUE && Elements != 2)
		return;

	switch (Elements)
	{
		case 2:
		{
			switch (ElementData[1])
			{
				case DashUnit*2:
				{
					switch (ElementData[0])
					{
						case DashUnit*2:
							if (ScaleWithLineWidth)
								DashID = SD_DASH1;
							else
								DashID = SD_DASH_GUIDELAYER;
							break;
						case DashUnit*4:
							DashID = SD_DASH2;
							break;
						case DashUnit*8:
							DashID = SD_DASH3;
							break;
						case DashUnit*16:
							DashID = SD_DASH4;
							break;
						case DashUnit*24:
							DashID = SD_DASH5;
							break;
					}
				}
				break;

				case DashUnit*4:
				{
					switch (ElementData[0])
					{
						case DashUnit*4:
							DashID = SD_DASH6;
							break;
						case DashUnit*8:
							DashID = SD_DASH7;
							break;
						case DashUnit*16:
							DashID = SD_DASH8;
							break;
					}
				}
				break;

				case DashUnit*8:
				{
					switch (ElementData[0])
					{
						case DashUnit*8:
							DashID = SD_DASH9;
							break;
						case DashUnit*16:
							DashID = SD_DASH10;
							break;
					}
				}
				break;
			}
			break;
		}

		case 4:
		{ 
			// All our current definitions have the 1st and 3rd elements == DashUnit*2 
			if (
					ElementData[1] != DashUnit*2 ||
					ElementData[3] != DashUnit*2
			   )
				return;
			
			switch (ElementData[2])
			{
				case DashUnit*2:
					switch (ElementData[0])
					{
						case DashUnit*4:
							DashID = SD_DASH11;
							break;
						case DashUnit*8:
							DashID = SD_DASH12;
							break;
						case DashUnit*16:
							DashID = SD_DASH13;
							break;
					}
					break;
				case DashUnit*4:
					switch (ElementData[0])
					{
						case DashUnit*8:
							DashID = SD_DASH14;
							break;
						case DashUnit*16:
							DashID = SD_DASH15;
							break;
					}
					break;
			}

			break;
		}

		case 6: 
		{
			if (
					ElementData[0] == DashUnit*8 &&
					ElementData[1] == DashUnit*2 &&
					ElementData[2] == DashUnit*2 &&
					ElementData[3] == DashUnit*2 &&
					ElementData[4] == DashUnit*2 &&
					ElementData[5] == DashUnit*2
				)
					DashID = SD_DASH16;
			else if (
						ElementData[0] == DashUnit*16 &&
						ElementData[1] == DashUnit*2 &&
						ElementData[2] == DashUnit*2 &&
						ElementData[3] == DashUnit*2 &&
						ElementData[4] == DashUnit*2 &&
						ElementData[5] == DashUnit*2
					)
						DashID = SD_DASH17;
			break;
		}

		case 8:
		{
			if (
				ElementData[0] == DashUnit*8 &&
				ElementData[1] == DashUnit*2 &&
				ElementData[2] == DashUnit*2 &&
				ElementData[3] == DashUnit*2 &&
				ElementData[4] == DashUnit*2 &&
				ElementData[5] == DashUnit*2 &&
				ElementData[6] == DashUnit*2 &&
				ElementData[7] == DashUnit*2
			   )
				DashID == SD_DASH18;
			else if (
						ElementData[0] == DashUnit*16 &&
						ElementData[1] == DashUnit*2 &&
						ElementData[2] == DashUnit*2 &&
						ElementData[3] == DashUnit*2 &&
						ElementData[4] == DashUnit*2 &&
						ElementData[5] == DashUnit*2 &&
						ElementData[6] == DashUnit*2 &&
						ElementData[7] == DashUnit*2
					)
						DashID == SD_DASH19;
			else if (
						ElementData[0] == DashUnit*8 &&
						ElementData[1] == DashUnit*2 &&
						ElementData[2] == DashUnit*2 &&
						ElementData[3] == DashUnit*2 &&
						ElementData[4] == DashUnit*4 &&
						ElementData[5] == DashUnit*2 &&
						ElementData[6] == DashUnit*2 &&
						ElementData[7] == DashUnit*2
					)
						DashID == SD_DASH20;
			break;
		}
	}
} */

/********************************************************************************************

>	BOOL DashPatternAttribute::SetDeviceDashPattern(DashRec& NewDash, INT32 PixelSize)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Purpose:	Sets the dash pattern for this attribute.
	Errors:		No memory.
	SeeAlso:	-

********************************************************************************************/

BOOL DashPatternAttribute::SetDeviceDashPattern(DashRec& NewDash, INT32 PixelSize)
{
	SetDashPattern(NewDash);

	if (DashPattern.Elements == 0)
		return TRUE;

	DashPattern.DashStart *= PixelSize;

	for (INT32 element = 0; element < DashPattern.Elements; element++)
	{
		DashPattern.ElementData[element] *= PixelSize;
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL DashPatternAttribute::SetDeviceStockDashPattern(StockDash NewDash, INT32 PixelSize)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Purpose:	Sets the dash pattern for this attribute.
	Errors:		No memory.
	SeeAlso:	-

********************************************************************************************/

BOOL DashPatternAttribute::SetDeviceStockDashPattern(StockDash NewDash, INT32 PixelSize)
{
	SetStockDashPattern(NewDash);

	if (DashPattern.Elements == 0)
		return TRUE;

	const INT32 DashUnit = DashPattern.LineWidth;

	DashPattern.DashStart = (DashPattern.DashStart/DashUnit) * (PixelSize);

	for (INT32 element = 0; element < DashPattern.Elements; element++)
	{
		DashPattern.ElementData[element] = (DashPattern.ElementData[element]/DashUnit) * (PixelSize);
	}

	return TRUE;
}

/********************************************************************************************

>	INT32 DashRec::operator==(DashRec& Other)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Purpose:	Equality operator, checks that the two dash patterns are the same.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

INT32 DashRec::operator==(DashRec& Other)
{
	if (Elements == 0 && Other.Elements == 0)
		return TRUE;

	if (Elements != Other.Elements ||
		DashStart != Other.DashStart)
		return FALSE;

	if (LineWidth != Other.LineWidth)
		return FALSE;

	// They both have the same number of elements and the same dash start
	// But are all the elements the same ?
	for (INT32 element = 0; element < Elements; element++)
	{
		if (ElementData[element] != Other.ElementData[element])
			return FALSE;
	}

	return TRUE;
}

/********************************************************************************************

>	DashRec& DashRec::operator=(DashRec& Other)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Purpose:	Equals operator, makes one dash pattern the same as another.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DashRec& DashRec::operator=(DashRec& Other)
{
	DashID = Other.DashID;
	Elements = Other.Elements;

	if (Elements == 0)
		return *this;

	DashStart = Other.DashStart;
	LineWidth = Other.LineWidth;
	ScaleWithLineWidth = Other.ScaleWithLineWidth;

	if (ElementData != NULL)
		delete ElementData;

	ElementData = new INT32[Elements];

	if (ElementData == NULL)
		return *this;

	for (INT32 element = 0; element < Elements; element++)
	{
		ElementData[element] = Other.ElementData[element];
	}

	return *this;
}

/********************************************************************************************

>	INT32 DashRec::operator==(StockDash Other)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Purpose:	Equality operator, checks that the two dash patterns are the same.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

INT32 DashRec::operator==(StockDash Other)
{
	return DashID == Other;
}

/********************************************************************************************

>	DashRec& DashRec::operator=(StockDash Other)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Purpose:	Equals operator, makes one dash pattern the same as another.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DashRec& DashRec::operator=(StockDash Other)
{
	return *this;
}

/********************************************************************************************
>	virtual NodeAttribute* DashPatternAttribute::MakeNode()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/94
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to a NodeAttribute.
	Purpose:	Makes the appropriate NodeAttribute-derivative from an dash-pattern
				attribute.
	Errors:		-
	SeeAlso:	AttributeValue::MakeNode
********************************************************************************************/

NodeAttribute* DashPatternAttribute::MakeNode()
{
	AttrDashPattern* pAttr = new AttrDashPattern;
	pAttr->Value.SimpleCopy(this);
	return pAttr;
}

/********************************************************************************************

>	BOOL DashPatternAttribute::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers line width attribute, and provides a default attribute to give
				non-dotted lines.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL DashPatternAttribute::Init()
{
	// Default to non-dotted lines
	DashPatternAttribute *pAttr = new DashPatternAttribute;
	if (pAttr == NULL)
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	if (ID == ATTR_BAD_ID)
		return FALSE;
	ENSURE(ID == ATTR_DASHPATTERN, "Incorrect ID for dash pattern attribute!");
	return TRUE;
}

void DashPatternAttribute::SimpleCopy(AttributeValue *pValue)
{
	DashPattern = ((DashPatternAttribute *) pValue)->DashPattern;
}

BOOL DashPatternAttribute::IsDifferent(AttributeValue *pAttr)
{
	ENSURE(GetRuntimeClass() == pAttr->GetRuntimeClass(), 
		   "Different attribute types in AttributeValue::IsDifferent()");
	return !(((DashPatternAttribute *) pAttr)->DashPattern == DashPattern);
}


void EndArrowAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "EndArrowAttribute::Render - do nothing" )
//	TRACE( wxT("Warning - EndArrowAttribute::Render called\n") );
	pRegion->SetEndArrow(this, Temp);
}

void EndArrowAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "EndArrowAttribute::Restore - do nothing" );
//	TRACE( wxT("Warning - EndArrowAttribute::Restore called\n") );
	pRegion->RestoreEndArrow(this, Temp);
}

void EndArrowAttribute::SimpleCopy(AttributeValue *pValue)
{
	EndArrow = ((EndArrowAttribute *) pValue)->EndArrow;
	EndArrow.m_bExtendPath = ((EndArrowAttribute *) pValue)->EndArrow.m_bExtendPath;
}

BOOL EndArrowAttribute::IsDifferent(AttributeValue *pAttr)
{
	ENSURE(GetRuntimeClass() == pAttr->GetRuntimeClass(), 
		   "Different attribute types in AttributeValue::IsDifferent()");
	return !(((EndArrowAttribute *) pAttr)->EndArrow == EndArrow);
}

/********************************************************************************************
>	virtual NodeAttribute* EndArrowAttribute::MakeNode()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/94
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to a NodeAttribute.
	Purpose:	Makes the appropriate NodeAttribute-derivative from an end-arrow
				attribute.
	Errors:		-
	SeeAlso:	AttributeValue::MakeNode
********************************************************************************************/

NodeAttribute* EndArrowAttribute::MakeNode()
{
	AttrEndArrow* pAttr = new AttrEndArrow;
	pAttr->Value.SimpleCopy(this);
	return pAttr;
}

/********************************************************************************************

>	BOOL EndArrowAttribute::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers line width attribute, and provides a default attribute to give
				non-dotted lines.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL EndArrowAttribute::Init()
{
	// Default to non-dotted lines
	EndArrowAttribute *pAttr = new EndArrowAttribute;
	if (pAttr == NULL)
		return FALSE;

/*
	SA_STRAIGHTARROW,
	SA_ANGLEDARROW,
	SA_ROUNDEDARROW,
	SA_SPOT,
	SA_DIAMOND,
	SA_ARROWFEATHER,
	SA_ARROWFEATHER2,
	SA_HOLLOWDIAMOND,
*/

//	pAttr->EndArrow.CreateStockArrow(SA_ANGLEDARROW);

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	if (ID == ATTR_BAD_ID)
		return FALSE;
	ENSURE(ID == ATTR_ENDARROW, "Incorrect ID for End Arrow attribute!");
	return TRUE;
}



/********************************************************************************************

>	void DrawingModeAttribute::Render(RenderRegion *pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the drawing mode attribute for the given render region. i.e. all
				objects drawn will now be drawn using this drawing mode.
	SeeAlso:	DrawingModeAttribute; RenderStack; AttributeValue; NodeAttribute;
				DrawingModeAttribute::Restore; DrawingModeAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void DrawingModeAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "DrawingModeAttribute::Render - do nothing" )
//	TRACE( wxT("Warning - DrawingModeAttribute::Render called\n") );
	pRegion->SetDrawingMode(this, Temp);
}

/********************************************************************************************

>	void DrawingModeAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the drawing mode attribute for the given render region. i.e. all
				objects drawn will now be drawn using this drawing mode.
	SeeAlso:	DrawingModeAttribute; RenderStack; AttributeValue; NodeAttribute;
				DrawingModeAttribute::Render; DrawingModeAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void DrawingModeAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "DrawingModeAttribute::Restore - do nothing" )
//	TRACE( wxT("Warning - DrawingModeAttribute::Restore called\n") );
	pRegion->RestoreDrawingMode(this, Temp);
}

/********************************************************************************************

>	void DrawingModeAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	DrawingModeAttribute; RenderStack; AttributeValue; NodeAttribute;
				DrawingModeAttribute::Render; DrawingModeAttribute::Restore;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void DrawingModeAttribute::SimpleCopy(AttributeValue *pValue)
{
	DrawingMode = ((DrawingModeAttribute *) pValue)->DrawingMode;
}

BOOL DrawingModeAttribute::IsDifferent(AttributeValue *pAttr)
{
	ENSURE(GetRuntimeClass() == pAttr->GetRuntimeClass(), 
		   "Different attribute types in AttributeValue::IsDifferent()");
	return TRUE;
}

NodeAttribute* DrawingModeAttribute::MakeNode()
{
	return NULL;
}

UINT32 DrawingModeAttribute::ID = ATTR_BAD_ID;


MitreLimitAttribute::MitreLimitAttribute()
{
	MitreLimit = 4000;
}

void MitreLimitAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "MitreLimitAttribute::Restore - do nothing" )
//	TRACE( wxT("Warning - MitreLimitAttribute::Restore called\n") );
	pRegion->RestoreMitreLimit(this, Temp);
}

void MitreLimitAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "MitreLimitAttribute::Render - do nothing" )
//	TRACE( wxT("Warning - MitreLimitAttribute::Render called\n") );
	pRegion->SetMitreLimit(this, Temp);
}

void MitreLimitAttribute::SimpleCopy(AttributeValue *pValue)
{
	MitreLimit = ((MitreLimitAttribute *) pValue)->MitreLimit;
}

BOOL MitreLimitAttribute::IsDifferent(AttributeValue *pAttr)
{
	ENSURE(GetRuntimeClass() == pAttr->GetRuntimeClass(), 
		   "Different attribute types in AttributeValue::IsDifferent()");
	return ((MitreLimitAttribute *) pAttr)->MitreLimit != MitreLimit;
}

/********************************************************************************************

>	virtual NodeAttribute* MireLimitAttribute::MakeNode()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/94
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to a NodeAttribute.
	Purpose:	Makes the appropriate NodeAttribute-derivative from a mitre-limit
				attribute.
	Errors:		-
	SeeAlso:	AttributeValue::MakeNode
********************************************************************************************/

NodeAttribute* MitreLimitAttribute::MakeNode()
{
	AttrMitreLimit* pAttr = new AttrMitreLimit;
	pAttr->Value.SimpleCopy(this);
	return pAttr;
}

BOOL MitreLimitAttribute::Init()
{
	MitreLimitAttribute *pAttr = new MitreLimitAttribute;
	if (pAttr == NULL)
		return FALSE;

	pAttr->MitreLimit = 4000;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	if (ID == ATTR_BAD_ID)
		return FALSE;
	ENSURE(ID == ATTR_MITRELIMIT, "Incorrect ID for attribute!");
	return TRUE;
}


WindingRuleAttribute::WindingRuleAttribute()
{
	// Default to even-odd winding
	WindingRule = EvenOddWinding;
}

void WindingRuleAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "WindingRuleAttribute::Restore - do nothing" )
//	TRACE( wxT("Warning - WindingRuleAttribute::Restore called\n") );
	pRegion->RestoreWindingRule(this, Temp);
}

void WindingRuleAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "WindingRuleAttribute::Render - do nothing" )
//	TRACE( wxT("Warning - WindingRuleAttribute::Render called\n") );
	pRegion->SetWindingRule(this, Temp);
}

void WindingRuleAttribute::SimpleCopy(AttributeValue *pValue)
{
	WindingRule = ((WindingRuleAttribute *) pValue)->WindingRule;
}

NodeAttribute *WindingRuleAttribute::MakeNode()
{
	// Create new attribute node
	AttrWindingRule *pAttr = new AttrWindingRule;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	BOOL WindingRuleAttribute::IsDifferent(AttributeValue *pAttr)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL WindingRuleAttribute::IsDifferent(AttributeValue *pAttr)
{
	ENSURE(GetRuntimeClass() == pAttr->GetRuntimeClass(), 
		   "Different attribute types in AttributeValue::IsDifferent()");
	return ((WindingRuleAttribute *) pAttr)->WindingRule != WindingRule;
}

BOOL WindingRuleAttribute::Init()
{
	WindingRuleAttribute *pAttr = new WindingRuleAttribute;
	if (pAttr == NULL)
		return FALSE;

	// Default to even-odd winding
	pAttr->WindingRule = EvenOddWinding;
	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	if (ID == ATTR_BAD_ID)
		return FALSE;
	ENSURE(ID == ATTR_WINDINGRULE, "Incorrect ID for attribute!");
	return TRUE;
}


StartCapAttribute::StartCapAttribute()
{
	// Default to butt line caps (because ArtWorks does).
	StartCap = LineCapButt;
}

BOOL StartCapAttribute::IsDifferent(AttributeValue *pAttr)
{
	ENSURE(GetRuntimeClass() == pAttr->GetRuntimeClass(), 
		   "Different attribute types in AttributeValue::IsDifferent()");
	return ((StartCapAttribute *) pAttr)->StartCap != StartCap;
}

void StartCapAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "StartCapAttribute::Restore - do nothing" )
//	TRACE( wxT("Warning - StartCapAttribute::Restore called\n") );
	pRegion->RestoreStartCap(this, Temp);
}

void StartCapAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "StartCapAttribute::Render - do nothing" )
//	TRACE( wxT("Warning - StartCapAttribute::Render called\n") );
	pRegion->SetStartCap(this, Temp);
}

void StartCapAttribute::SimpleCopy(AttributeValue *pValue)
{
	StartCap = ((StartCapAttribute *) pValue)->StartCap;
}

NodeAttribute *StartCapAttribute::MakeNode()
{
	// Create new attribute node
	AttrStartCap *pAttr = new AttrStartCap;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}

BOOL StartCapAttribute::Init()
{
	StartCapAttribute *pAttr = new StartCapAttribute;
	if (pAttr == NULL)
		return FALSE;

	// Default to butt line caps (because ArtWorks does).
	pAttr->StartCap = LineCapButt;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	if (ID == ATTR_BAD_ID)
		return FALSE;
	ENSURE(ID == ATTR_STARTCAP, "Incorrect ID for attribute!");
	return TRUE;
}



JoinTypeAttribute::JoinTypeAttribute()
{
	// Default to Bevelled line joins.
	JoinType = BevelledJoin;
}

BOOL JoinTypeAttribute::IsDifferent(AttributeValue *pAttr)
{
	ENSURE(GetRuntimeClass() == pAttr->GetRuntimeClass(), 
		   "Different attribute types in AttributeValue::IsDifferent()");
	return ((JoinTypeAttribute *) pAttr)->JoinType != JoinType;
}

void JoinTypeAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "JoinTypeAttribute::Restore - do nothing" )
//	TRACE( wxT("Warning - JoinTypeAttribute::Restore called\n") );
	pRegion->RestoreJoinType(this, Temp);
}

void JoinTypeAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "JoinTypeAttribute::Render - do nothing" )
//	TRACE( wxT("Warning - JoinTypeAttribute::Render called\n") );
	pRegion->SetJoinType(this, Temp);
}

void JoinTypeAttribute::SimpleCopy(AttributeValue *pValue)
{
	JoinType = ((JoinTypeAttribute *) pValue)->JoinType;
}

NodeAttribute *JoinTypeAttribute::MakeNode()
{
	// Create new attribute node
	AttrJoinType *pAttr = new AttrJoinType;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}

BOOL JoinTypeAttribute::Init()
{
	JoinTypeAttribute *pAttr = new JoinTypeAttribute;
	if (pAttr == NULL)
		return FALSE;

	// Default to Bevelled line joins.
	pAttr->JoinType = 	BevelledJoin;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	if (ID == ATTR_BAD_ID)
		return FALSE;
	ENSURE(ID == ATTR_JOINTYPE, "Incorrect ID for attribute!");
	return TRUE;
}




void StartArrowAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "StartArrowAttribute::Render - do nothing" )
//	TRACE( wxT("Warning - StartArrowAttribute::Render called\n") );
	pRegion->SetStartArrow(this, Temp);
}

void StartArrowAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "StartArrowAttribute::Restore - do nothing" )
//	TRACE( wxT("Warning - StartArrowAttribute::Restore called\n") );
	pRegion->RestoreStartArrow(this, Temp);
}

void StartArrowAttribute::SimpleCopy(AttributeValue *pValue)
{
	StartArrow = ((StartArrowAttribute *) pValue)->StartArrow;
	StartArrow.m_bExtendPath = ((StartArrowAttribute *) pValue)->StartArrow.m_bExtendPath;
}

BOOL StartArrowAttribute::IsDifferent(AttributeValue *pAttr)
{
	ENSURE(GetRuntimeClass() == pAttr->GetRuntimeClass(), 
		   "Different attribute types in AttributeValue::IsDifferent()");
	return !(((StartArrowAttribute *) pAttr)->StartArrow == StartArrow);
}

/********************************************************************************************

>	virtual NodeAttribute* StartArrowAttribute::MakeNode()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/94
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to a NodeAttribute.
	Purpose:	Makes the appropriate NodeAttribute-derivative from a fill-type attribute.
	Errors:		-
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute* StartArrowAttribute::MakeNode()
{
	AttrStartArrow* pAttr = new AttrStartArrow;
	pAttr->Value.SimpleCopy(this);
	return pAttr;
}

/********************************************************************************************

>	BOOL StartArrowAttribute::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers line width attribute, and provides a default attribute to give
				non-dotted lines.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL StartArrowAttribute::Init()
{
	// Default to non-dotted lines
	StartArrowAttribute *pAttr = new StartArrowAttribute;
	if (pAttr == NULL)
		return FALSE;

/*
	SA_STRAIGHTARROW,
	SA_ANGLEDARROW,
	SA_ROUNDEDARROW,
	SA_SPOT,
	SA_DIAMOND,
	SA_ARROWFEATHER,
	SA_ARROWFEATHER2,
	SA_HOLLOWDIAMOND,
*/

//	pAttr->StartArrow.CreateStockArrow(SA_ARROWFEATHER2);

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	if (ID == ATTR_BAD_ID)
		return FALSE;
	ENSURE(ID == ATTR_STARTARROW, "Incorrect ID for End Arrow attribute!");
	return TRUE;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------


/********************************************************************************************
>	BlendAttrParam::BlendAttrParam()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BlendAttrParam::BlendAttrParam()
{
	pRenderRegion	= NULL;
	pOtherNodeAttr 	= NULL;
	pOtherAttrVal	= NULL;
	BlendRatio		= 0.0;
	pBlendedNodeAttr= NULL;
	pBlendedAttrVal = NULL;
	ColourBlend		= COLOURBLEND_FADE;
	objectRatio		= 0.0;
	objectProfileProcessing = FALSE;
}

/********************************************************************************************
>	BlendAttrParam::~BlendAttrParam()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructor
				This will delete pBlendedNodeAttr and pBlendedAttrVal if these values are not NULL
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BlendAttrParam::~BlendAttrParam()
{
	if (pBlendedNodeAttr != NULL) delete pBlendedNodeAttr;
	if (pBlendedAttrVal  != NULL) delete pBlendedAttrVal;
}


/********************************************************************************************
>	BOOL BlendAttrParam::Init(RenderRegion *pThisRenderRegion,
							NodeAttribute* pThisOtherAttr, double ThisBlendRatio,ColourBlendType ColBlend,
							CCAttrMap* pStartAttrMap, CCAttrMap* pEndAttrMap)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94
	Inputs:		pRenderRegion	= the render region we are rendering into (this is currently
									used for finding the rendering ColourContext for blending colour)
									This may be NULL, but if you're rendering you must pass it in,
									so we separate tints properly as often as possible.
				pThisOtherAttr 	= the node attr that you want to blend to
				ThisBlendRatio 	= the amount you want to blend by (0.0 < ThisBlendRatio < 1.0)
				ColBlend		= the way in which colours should be blended
	Outputs:	-
	Returns:	TRUE if initialised correctly, FALSE otherwise
	Purpose:	This inits the object with the given params.
				Fails if pThisOtherAttr is NULL or ThisBlendRatio is out of range.
				If ok the following is true -
					GetRenderRegion		will return pThisRenderRegion
					GetOtherAttr() 		will return pThisOtherAttr
					GetBlendRatio() 	will return ThisBlendRatio
					GetOtherAttrVal() 	will return pThisOtherAttr->GetAttributeValue() (can be NULL!!)
					GetBlendedAttr()	will return NULL
					GetBlendedAttrVal()	will return NULL
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BlendAttrParam::Init(RenderRegion *pThisRenderRegion,
						  NodeAttribute* pThisOtherAttr, double ThisBlendRatio,ColourBlendType ColBlend,
						  CCAttrMap* pStartAttrMap, CCAttrMap* pEndAttrMap, BOOL LastStep)
{
	// Check that this is the first time it's been called
	ERROR3IF(pOtherNodeAttr != NULL,"This has already been initialised");
	if (pOtherNodeAttr != NULL) return FALSE;

	// check valid entry params
	ERROR3IF(pThisOtherAttr == NULL || (ThisBlendRatio <= 0.0) || (ThisBlendRatio >= 1.0),"Dodgy entry params");
	if (pThisOtherAttr == NULL || (ThisBlendRatio <= 0.0) || (ThisBlendRatio >= 1.0)) return FALSE;

	// Remember the render region
	pRenderRegion = pThisRenderRegion;

	// Store the other NodeAttribute's ptr
	pOtherNodeAttr = pThisOtherAttr;

	// Store the way colours should be blended
	ColourBlend = ColBlend;

	// Get pOtherNodeAttr's attr value ptr
	// NOTE!! 	This can be NULL as not all NodeAttributes implement this function.
	// 		  	This is OK as the ones that don't support GetAttributeValue() won't have a Blend() 
	//			function either, and if they do have a Blend() func, it will error when it's called
	pOtherAttrVal = pThisOtherAttr->GetAttributeValue();

	// Set up the other member vars
	BlendRatio		= ThisBlendRatio;
	pBlendedNodeAttr= NULL;
	pBlendedAttrVal = NULL;

	pAttrMapStart 	= pStartAttrMap;	
	pAttrMapEnd 	= pEndAttrMap;

	m_bLastStep = LastStep;

	return TRUE;
}

/********************************************************************************************
>	BOOL BlendAttrParam::Init(RenderRegion *pThisRenderRegion, NodeAttribute* pThisOtherAttr,
						  double ThisBlendRatio, double thisObjectRatio, BOOL objProcess,
						  ColourBlendType ColBlend, CCAttrMap* pStartAttrMap,
						  CCAttrMap* pEndAttrMap, BOOL LastStep)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/2000
	Inputs:		pRenderRegion	= the render region we are rendering into (this is currently
									used for finding the rendering ColourContext for blending colour)
									This may be NULL, but if you're rendering you must pass it in,
									so we separate tints properly as often as possible.
				pThisOtherAttr 	= the node attr that you want to blend to
				ThisBlendRatio 	= the amount you want to blend by (0.0 < ThisBlendRatio < 1.0)
				thisObjectRatio = the amount you to to blend object (i.e.  position data) by
				objProcess		= whether we are blending with respect to position within attributes
				ColBlend		= the way in which colours should be blended
	Outputs:	-
	Returns:	TRUE if initialised correctly, FALSE otherwise
	Purpose:	This inits the object with the given params.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BlendAttrParam::Init(RenderRegion *pThisRenderRegion, NodeAttribute* pThisOtherAttr,
						  double ThisBlendRatio, double thisObjectRatio, BOOL objProcess,
						  ColourBlendType ColBlend, CCAttrMap* pStartAttrMap,
						  CCAttrMap* pEndAttrMap, BOOL LastStep)
{
	// Check that this is the first time it's been called
	ERROR3IF(pOtherNodeAttr != NULL,"This has already been initialised");
	if (pOtherNodeAttr != NULL) return FALSE;

	// check valid entry params
	ERROR3IF(pThisOtherAttr == NULL || (ThisBlendRatio <= 0.0) || (ThisBlendRatio >= 1.0),"Dodgy entry params");
	if (pThisOtherAttr == NULL || (ThisBlendRatio <= 0.0) || (ThisBlendRatio >= 1.0)) return FALSE;

	// Remember the render region
	pRenderRegion = pThisRenderRegion;

	// Store the other NodeAttribute's ptr
	pOtherNodeAttr = pThisOtherAttr;

	// Store the way colours should be blended
	ColourBlend = ColBlend;

	// Get pOtherNodeAttr's attr value ptr
	// NOTE!! 	This can be NULL as not all NodeAttributes implement this function.
	// 		  	This is OK as the ones that don't support GetAttributeValue() won't have a Blend() 
	//			function either, and if they do have a Blend() func, it will error when it's called
	pOtherAttrVal = pThisOtherAttr->GetAttributeValue();

	// Set up the other member vars
	BlendRatio		= ThisBlendRatio;
	pBlendedNodeAttr= NULL;
	pBlendedAttrVal = NULL;

	pAttrMapStart 	= pStartAttrMap;	
	pAttrMapEnd 	= pEndAttrMap;

	m_bLastStep = LastStep;

	objectRatio = thisObjectRatio;
	
	objectProfileProcessing = objProcess;

	return TRUE;
}

/********************************************************************************************
>	NodeAttribute* BlendAttrParam::GetBlendedAttr()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94
	Inputs:		-
	Outputs:	-
	Returns:	Ptr to NodeAttribute, or NULL if it doesn't have one
	Purpose:	When you call this, the pointer passed back becomes your responsibility (if its not NULL).
				This means that you will have to delete it if you don't want to cause memory leaks.
				Also, you will get NULL for all subsequent calls, until a new value is set by SetBlendedAttr()
				
				I.e. GetBlendedAttr() returns it's internal node attr ptr, then sets it to NULL

	Errors:		In debug builds, it'll error if it returns NULL
	SeeAlso:	-

********************************************************************************************/

NodeAttribute* BlendAttrParam::GetBlendedAttr()
{
	ERROR3IF(pBlendedNodeAttr == NULL,"pBlendedNodeAttr is NULL");

	NodeAttribute* p = pBlendedNodeAttr;
	pBlendedNodeAttr = NULL;
	return (p);
}

/********************************************************************************************
>	AttributeValue* BlendAttrParam::GetBlendedAttrVal()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94
	Inputs:		-
	Outputs:	-
	Returns:	Ptr to AttributeValue, or NULL if it doesn't have one
	Purpose:	When you call this, the pointer passed back becomes your responsibility (if its not NULL).
				This means that you will have to delete it if you don't want to cause memory leaks.
				Also, you will get NULL for all subsequent calls, until a new value is set by SetBlendedAttrVal()
				
				I.e. GetBlendedAttrVal() returns it's internal attr value ptr, then sets it to NULL

	Errors:		In debug builds, it'll error if it returns NULL
	SeeAlso:	-

********************************************************************************************/

AttributeValue* BlendAttrParam::GetBlendedAttrVal()
{
	ERROR3IF(pBlendedAttrVal == NULL,"pBlendedAttrVal is NULL");

	AttributeValue* p = pBlendedAttrVal;
	pBlendedAttrVal = NULL;
	return (p);
}

/********************************************************************************************
>	void BlendAttrParam::SetBlendedAttr(NodeAttribute* pAttr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94
	Inputs:		pAttr = ptr to a node attr
	Outputs:	-
	Returns:	-
	Purpose:	When you call this func, the object you pass it becomes the property of the BlendAttrParam object.
				If it's internal pointer is the same as the entry param, nothing happends
				If it's internal pointer is not NULL and different to the entry param, the old one is deleted first.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void BlendAttrParam::SetBlendedAttr(NodeAttribute* pAttr)
{
	if (pBlendedNodeAttr != pAttr)
	{
		if (pBlendedNodeAttr != NULL) delete pBlendedNodeAttr;
		pBlendedNodeAttr = pAttr;
	}
}

/********************************************************************************************
>	void BlendAttrParam::SetBlendedAttrVal(AttributeValue* pAttrVal)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94
	Inputs:		pAttrVal = ptr to a attr value
	Outputs:	-
	Returns:	-
	Purpose:	When you call this func, the object you pass it becomes the property of the BlendAttrParam object.
				If it's internal pointer is the same as the entry param, nothing happends
				If it's internal pointer is not NULL and different to the entry param, the old one is deleted first.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void BlendAttrParam::SetBlendedAttrVal(AttributeValue* pAttrVal)
{
	if (pBlendedAttrVal != pAttrVal)
	{
		if (pBlendedAttrVal != NULL) delete pBlendedAttrVal;
		pBlendedAttrVal = pAttrVal;
	}
}

