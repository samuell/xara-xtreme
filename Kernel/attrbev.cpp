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

/*
*/
    
#include "camtypes.h"                          
#include "attrbev.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "mario.h"
//#include "simon.h"
//#include "tim.h"
//#include "will.h"
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "tranform.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "blobs.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fixed16.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "blendatt.h"
#include "nodepath.h"
//#include "ink.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "attrmap.h"
//#include "camfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxftags.h"
//#include "cxfdefs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "rechattr.h"	// Record handling classes for attributes
//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxfdash.h"	// Default dash pattern references
#include "cxfarrow.h"	// Default arrow head/tail references
#include "cxfcols.h"	// REF_DEFAULTCOLOUR_TRANSPARENT ...
//#include "bevres.h"
#include "ppbevel.h"
#include "cxftags.h"
//#include "cxfdefs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nbevcont.h"
//#include "view.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "beveler.h"
//#include "biasgain.h" - in camtypes.h [AUTOMATICALLY REMOVED]

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(AttrBevel,	NodeAttribute)
CC_IMPLEMENT_DYNCREATE(AttrBevelIndent,	AttrBevel)
CC_IMPLEMENT_DYNCREATE(BevelAttributeValueIndent,	AttributeValue)
CC_IMPLEMENT_DYNCREATE(AttrBevelLightAngle,	AttrBevel)
CC_IMPLEMENT_DYNCREATE(BevelAttributeValueLightAngle,	AttributeValue)
CC_IMPLEMENT_DYNCREATE(AttrBevelContrast,	AttrBevel)
CC_IMPLEMENT_DYNCREATE(BevelAttributeValueContrast,	AttributeValue)
CC_IMPLEMENT_DYNCREATE(AttrBevelType,	AttrBevel)
CC_IMPLEMENT_DYNCREATE(BevelAttributeValueType,	AttributeValue)
CC_IMPLEMENT_DYNCREATE(AttrBevelLightTilt,	AttrBevel)
CC_IMPLEMENT_DYNCREATE(BevelAttributeValueLightTilt,	AttributeValue)



// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	AttrBevel::AttrBevelIndent(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected, TRUE)  

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Bevel attribute constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
AttrBevel::AttrBevel(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{
	
}

/********************************************************************************************

>	AttrBevel::AttrBevel()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Bevel attribute constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
AttrBevel::AttrBevel()
{
}

/********************************************************************************************

>	AttrBevelIndent::AttrBevelIndent(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected, TRUE)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a bValue Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrBevelIndent::AttrBevelIndent(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrBevel(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrBevelIndent::AttrBevelIndent()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for Line Width Attribute class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrBevelIndent::AttrBevelIndent()
{
}

/********************************************************************************************

>	void AttrBevelIndent::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a Line Width attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrBevelIndent::Render( RenderRegion* pRender)
{
}

/********************************************************************************************

>	void AttrBevelIndent::Transform( TransformBase& Trans )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/94
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Scale the Line Width.
	SeeAlso:	NodeRenderable::Transform

********************************************************************************************/

void AttrBevelIndent::Transform( TransformBase& Trans )
{
	INT32 OldIndent = Value.m_Indent;
	
	// Find out the equivalent scale factor
	FIXED16 ScaleFactor = Trans.GetScalar();

	// no scaling then lets leave
	if (ScaleFactor.MakeDouble() == 1.0)
		return;

	// And scale the line width
	Value.m_Indent= labs( INT32(ScaleFactor.MakeDouble() * ((double)Value.m_Indent)) );

	// MRH - 3/7/00 - Added a quick check to see if the indent is greater than the slider limit.
	// If so then reset the value to the limit! ERROR3 For Debugging - can be taken out if anouying!
	if (Value.m_Indent > 250000)
	{
		ERROR3("Bevel Indent > Max Value! Setting to Max Value!");
		Value.m_Indent = 250000;
	}

	if (OldIndent < 0)
	{
		Value.m_Indent = -Value.m_Indent;
	}
}

/***********************************************************************************************
> Node* AttrBevelIndent::SimpleCopy() 

    Author:     Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:    21/6/93
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   	
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrBevelIndent::SimpleCopy()
{
	AttrBevelIndent* NodeCopy = new AttrBevelIndent();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	NodeCopy->Value.m_Indent = Value.m_Indent;
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 AttrBevelIndent::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94
	Inputs:		Atrib:	The attribute to compare, which must be an AttrBevelIndent 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrBevelIndent 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrBevelIndent::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrBevelIndent)), 
		"Trying to compare two objects with different types"); 
	AttrBevelIndent * Attr = (AttrBevelIndent *) &Attrib;
	return (Attr->Value.m_Indent == Value.m_Indent); 
} 


/********************************************************************************************

>	virtual UINT32 AttrBevelIndent::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrBevelIndent::GetAttrNameID(void)  
{
	return (_R(IDS_BEVEL_INDENT_ID)); 
}               
                 

/***********************************************************************************************
> ` void AttrBevel::CopyNodeContents( AttrBevelIndent* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrBevelIndent::CopyNodeContents( AttrBevelIndent* NodeCopy)
{
	NodeAttribute::CopyNodeContents( NodeCopy );

	NodeCopy->Value = Value;
} 

/***********************************************************************************************
>   void NodeRenderableBounded::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrBevelIndent::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrBevelIndent), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrBevelIndent))
		CopyNodeContents((AttrBevelIndent*)pNodeCopy);
}



/***********************************************************************************************

>	void AttrBevelIndent::ShowDebugTreeDetails() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Displays debugging info of the tree

***********************************************************************************************/

void AttrBevelIndent::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	// For now, we will just call the base class version
#ifdef _DEBUG
	NodeAttribute::ShowDebugTreeDetails();	
#endif
}


void AttrBevelIndent::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;
	TempStr._MakeMsg( TEXT("\r\nValue=#1%d\r\n"), Value.m_Indent );
	(*Str) += TempStr;
#else
	if (Str)
	{
		*Str = wxT("");
	}
#endif
}



/********************************************************************************************

>	virtual UINT32 AttrBevelIndent::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrBevelIndent::GetNodeSize() const 
{     
	return (sizeof(AttrBevelIndent)); 
}  

/********************************************************************************************

>	BOOL AttrBevelIndent::Blend(BlendAttrParam* pBlendParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/99
	Inputs:		The blend attribute param
	Purpose:	Blends this indent to another
	SeeAlso:	

********************************************************************************************/
BOOL AttrBevelIndent::Blend(BlendAttrParam* pBlendParam)
{
	AttrBevelIndent * pBlendedAttr = NULL;

	// assign the other attribute we're looking at
	AttrBevelIndent * pOtherAttr = (AttrBevelIndent *)pBlendParam->GetOtherAttr();

	// split at the half way mark
	if (pBlendParam->GetBlendRatio() <= 0.5)
	{
		pBlendedAttr = (AttrBevelIndent *)SimpleCopy();
	}
	else
	{
		pBlendedAttr = (AttrBevelIndent *)pOtherAttr->SimpleCopy();
	}

	// set the blended attribute
	pBlendParam->SetBlendedAttr(pBlendedAttr);

	if (!pBlendedAttr)
		return FALSE;

	double Indent1			= Value.m_Indent;
	double Indent2			= pOtherAttr->Value.m_Indent;
	double NewIndent		= ((Indent2 - Indent1) * pBlendParam->GetBlendRatio()) + Indent1;

	// set the blended attribute's values
	pBlendedAttr->Value.m_Indent = (MILLIPOINT)NewIndent;
	
	return TRUE;
}



/********************************************************************************************

  >	virtual BOOL AttrBevelIndent::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the line width record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrBevelIndent::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	return TRUE;
}

//--------------------------------------------------------------
// See AttrBevelIndent::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrBevelIndent::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}

////////////////////////////////////////////////////////////////////////
// the actual attribute

BevelAttributeValueIndent::BevelAttributeValueIndent()
{
//	m_Indent = -3000;
	m_Indent = 0;
}

/********************************************************************************************

>	void BevelAttributeValueIndent::Render(RenderRegion *pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the line width attribute for the given render region. i.e. all
				lines drawn will now be drawn with this line width.
	SeeAlso:	BevelAttributeValueIndent; RenderStack; AttributeValue; NodeAttribute;
				BevelAttributeValueIndent::Restore; BevelAttributeValueIndent::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BevelAttributeValueIndent::Render(RenderRegion *pRegion, BOOL Temp)
{
}

/********************************************************************************************

>	void BevelAttributeValueIndent::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the line width attribute for the given render region. i.e. all
				lines drawn will now be drawn with this line width.
	SeeAlso:	BevelAttributeValueIndent; RenderStack; AttributeValue; NodeAttribute;
				BevelAttributeValueIndent::Render; BevelAttributeValueIndent::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BevelAttributeValueIndent::Restore(RenderRegion *pRegion, BOOL Temp)
{
}

/********************************************************************************************

>	void BevelAttributeValueIndent::SimpleCopy(AttributeValue *pValue)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	BevelAttributeValueIndent; RenderStack; AttributeValue; NodeAttribute;
				BevelAttributeValueIndent::Render; BevelAttributeValueIndent::Restore;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BevelAttributeValueIndent::SimpleCopy(AttributeValue *pValue)
{
	m_Indent = ((BevelAttributeValueIndent *) pValue)->m_Indent;
}

/********************************************************************************************

>	BOOL BevelAttributeValueIndent::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers line width attribute, and provides a default attribute to give
				0.25pt lines.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL BevelAttributeValueIndent::Init()
{
	BevelAttributeValueIndent *pAttr = new BevelAttributeValueIndent;
	if (pAttr == NULL)
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	if (ID == ATTR_BAD_ID)
		return FALSE;
	ENSURE(ID == ATTR_BEVELINDENT, "Incorrect ID for attribute!");
	return TRUE;
}

/********************************************************************************************

>	NodeAttribute *BevelAttributeValueIndent::MakeNode()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrbValue node from this line width attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *BevelAttributeValueIndent::MakeNode()
{
	// Create new attribute node
	AttrBevelIndent *pAttr = new AttrBevelIndent();

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	BOOL BevelAttributeValueIndent::IsDifferent(AttributeValue *pAttr)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL BevelAttributeValueIndent::IsDifferent(AttributeValue *pAttr)
{
	ENSURE(GetRuntimeClass() == pAttr->GetRuntimeClass(), 
		   "Different attribute types in AttributeValue::IsDifferent()");
	return ((BevelAttributeValueIndent *) pAttr)->m_Indent != m_Indent;
}

/********************************************************************************************

>	AttrBevelContrast::AttrBevelContrast(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected, TRUE)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a bValue Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrBevelContrast::AttrBevelContrast(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrBevel(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrBevelContrast::AttrBevelContrast()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for Line Width Attribute class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrBevelContrast::AttrBevelContrast()
{
}


/********************************************************************************************

>	void AttrBevelContrast::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a Line Width attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrBevelContrast::Render( RenderRegion* pRender)
{
}

/********************************************************************************************

>	void AttrBevelContrast::Transform( TransformBase& Trans )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/94
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Scale the Line Width.
	SeeAlso:	NodeRenderable::Transform

********************************************************************************************/

void AttrBevelContrast::Transform( TransformBase& Trans )
{
	
}

/***********************************************************************************************
> Node* AttrBevelContrast::SimpleCopy() 

    Author:     Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:    21/6/93
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   	
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrBevelContrast::SimpleCopy()
{
	AttrBevelContrast* NodeCopy = new AttrBevelContrast();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	NodeCopy->Value.m_Contrast = Value.m_Contrast;
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 AttrBevelContrast::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94
	Inputs:		Atrib:	The attribute to compare, which must be an AttrBevelContrast 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrBevelContrast 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrBevelContrast::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrBevelContrast)), 
		"Trying to compare two objects with different types"); 
	AttrBevelContrast * Attr = (AttrBevelContrast *) &Attrib;
	return (Attr->Value.m_Contrast == Value.m_Contrast); 
} 


/********************************************************************************************

>	virtual UINT32 AttrBevelContrast::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrBevelContrast::GetAttrNameID(void)  
{
	return (_R(IDS_BEVEL_CONTRAST_ID)); 
}               
                 

/***********************************************************************************************
> ` void AttrBevel::CopyNodeContents( AttrBevelContrast* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrBevelContrast::CopyNodeContents( AttrBevelContrast* NodeCopy)
{
	NodeAttribute::CopyNodeContents( NodeCopy );

	NodeCopy->Value = Value;
} 

/***********************************************************************************************
>   void AttrBevelContrast::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrBevelContrast::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrBevelContrast), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrBevelContrast))
		CopyNodeContents((AttrBevelContrast*)pNodeCopy);
}



/********************************************************************************************

>	BOOL AttrBevelContrast::Blend(BlendAttrParam* pBlendParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/99
	Inputs:		The blend attribute param
	Purpose:	Blends this light angle to another
	SeeAlso:	

********************************************************************************************/
BOOL AttrBevelContrast::Blend(BlendAttrParam* pBlendParam)
{
	ERROR2IF(pBlendParam == NULL,FALSE,"pBlendParam == NULL");
	ERROR3IF(!(pBlendParam->GetOtherAttr()->IS_KIND_OF(AttrBevelContrast)),
		"Blend attribute isn't a bevel contrast attribute");

	AttrBevelContrast * pBlendedAttr = NULL;

	// assign the other attribute we're looking at
	AttrBevelContrast * pOtherAttr = (AttrBevelContrast *)pBlendParam->GetOtherAttr();

	// split at the half way mark
	if (pBlendParam->GetBlendRatio() <= 0.5)
	{
		pBlendedAttr = (AttrBevelContrast *)SimpleCopy();
	}
	else
	{
		pBlendedAttr = (AttrBevelContrast *)pOtherAttr->SimpleCopy();
	}

	// set the blended attribute
	pBlendParam->SetBlendedAttr(pBlendedAttr);
	
	if (!pBlendedAttr)
		return FALSE;

	double Contrast1		= Value.m_Contrast;
	double Contrast2		= pOtherAttr->Value.m_Contrast;
	double NewContrast		= ((Contrast2 - Contrast1) * pBlendParam->GetBlendRatio()) +
								Contrast1;

	// set the blended attribute's values
	pBlendedAttr->Value.m_Contrast = (INT32)NewContrast;

	return TRUE;
}


/***********************************************************************************************

>	void AttrBevelContrast::ShowDebugTreeDetails() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Displays debugging info of the tree

***********************************************************************************************/

void AttrBevelContrast::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	// For now, we will just call the base class version
#ifdef _DEBUG
	NodeAttribute::ShowDebugTreeDetails();	
#endif
}


void AttrBevelContrast::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;
	TempStr._MakeMsg( TEXT("\r\nValue=#1%d\r\n"), Value.m_Contrast );
	(*Str)+= TempStr;
#else
	if (Str)
	{
		*Str = wxT("");
	}
#endif
}


/********************************************************************************************

>	virtual UINT32 AttrBevelContrast::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrBevelContrast::GetNodeSize() const 
{     
	return (sizeof(AttrBevelContrast)); 
}  

/********************************************************************************************

  >	virtual BOOL AttrBevelContrast::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the line width record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrBevelContrast::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	return TRUE;
}

//--------------------------------------------------------------
// See AttrBevelContrast::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrBevelContrast::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}

////////////////////////////////////////////////////////////////////////
// the actual attribute

BevelAttributeValueContrast::BevelAttributeValueContrast()
{
	m_Contrast = 50;
}

/********************************************************************************************

>	void BevelAttributeValueContrast::Render(RenderRegion *pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the line width attribute for the given render region. i.e. all
				lines drawn will now be drawn with this line width.
	SeeAlso:	BevelAttributeValueContrast; RenderStack; AttributeValue; NodeAttribute;
				BevelAttributeValueContrast::Restore; BevelAttributeValueContrast::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BevelAttributeValueContrast::Render(RenderRegion *pRegion, BOOL Temp)
{
}

/********************************************************************************************

>	void BevelAttributeValueContrast::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the line width attribute for the given render region. i.e. all
				lines drawn will now be drawn with this line width.
	SeeAlso:	BevelAttributeValueContrast; RenderStack; AttributeValue; NodeAttribute;
				BevelAttributeValueContrast::Render; BevelAttributeValueContrast::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BevelAttributeValueContrast::Restore(RenderRegion *pRegion, BOOL Temp)
{
}

/********************************************************************************************

>	void BevelAttributeValueContrast::SimpleCopy(AttributeValue *pValue)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	BevelAttributeValueContrast; RenderStack; AttributeValue; NodeAttribute;
				BevelAttributeValueContrast::Render; BevelAttributeValueContrast::Restore;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BevelAttributeValueContrast::SimpleCopy(AttributeValue *pValue)
{
	m_Contrast = ((BevelAttributeValueContrast *) pValue)->m_Contrast;
}

/********************************************************************************************

>	BOOL BevelAttributeValueContrast::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers line width attribute, and provides a default attribute to give
				0.25pt lines.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL BevelAttributeValueContrast::Init()
{
	BevelAttributeValueContrast *pAttr = new BevelAttributeValueContrast;
	if (pAttr == NULL)
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	if (ID == ATTR_BAD_ID)
		return FALSE;
	ENSURE(ID == ATTR_BEVELCONTRAST, "Incorrect ID for attribute!");
	return TRUE;
}

/********************************************************************************************

>	NodeAttribute *BevelAttributeValueContrast::MakeNode()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrbValue node from this line width attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *BevelAttributeValueContrast::MakeNode()
{
	// Create new attribute node
	AttrBevelContrast *pAttr = new AttrBevelContrast();

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	BOOL BevelAttributeValueContrast::IsDifferent(AttributeValue *pAttr)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL BevelAttributeValueContrast::IsDifferent(AttributeValue *pAttr)
{
	ENSURE(GetRuntimeClass() == pAttr->GetRuntimeClass(), 
		   "Different attribute types in AttributeValue::IsDifferent()");
	return ((BevelAttributeValueContrast *) pAttr)->m_Contrast != m_Contrast;
}

/********************************************************************************************

>	AttrBevelType::AttrBevelType(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected, TRUE)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a bValue Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrBevelType::AttrBevelType(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrBevel(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrBevelType::AttrBevelType()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for Line Width Attribute class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrBevelType::AttrBevelType()
{
}

/********************************************************************************************

>	void AttrBevelType::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a Line Width attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrBevelType::Render( RenderRegion* pRender)
{
}


/********************************************************************************************

>	void AttrBevelType::Transform( TransformBase& Trans )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/94
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Scale the Line Width.
	SeeAlso:	NodeRenderable::Transform

********************************************************************************************/

void AttrBevelType::Transform( TransformBase& Trans )
{
	
}

/***********************************************************************************************
> Node* AttrBevelType::SimpleCopy() 

    Author:     Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:    21/6/93
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   	
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrBevelType::SimpleCopy()
{
	AttrBevelType* NodeCopy = new AttrBevelType();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	NodeCopy->Value.SimpleCopy(&Value);
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 AttrBevelType::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94
	Inputs:		Atrib:	The attribute to compare, which must be an AttrBevelType 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrBevelType 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrBevelType::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrBevelType)), 
		"Trying to compare two objects with different types"); 
	AttrBevelType * Attr = (AttrBevelType *) &Attrib;

	if (Attr->Value.IsDifferent(&Value))
	{
		return FALSE;
	}

	return TRUE; 
	
} 


/********************************************************************************************

>	virtual UINT32 AttrBevelType::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrBevelType::GetAttrNameID(void)  
{
	return (_R(IDS_BEVEL_TYPE_ID)); 
}               
                 

/***********************************************************************************************
> ` void AttrBevel::CopyNodeContents( AttrBevelType* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrBevelType::CopyNodeContents( AttrBevelType* NodeCopy)
{
	NodeAttribute::CopyNodeContents( NodeCopy );

	NodeCopy->Value.SimpleCopy(&Value);
} 

/***********************************************************************************************
>   void AttrBevelContrast::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrBevelType::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrBevelType), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrBevelType))
		CopyNodeContents((AttrBevelType*)pNodeCopy);
}



/***********************************************************************************************

>	void AttrBevelType::ShowDebugTreeDetails() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Displays debugging info of the tree

***********************************************************************************************/

void AttrBevelType::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	// For now, we will just call the base class version
#ifdef _DEBUG
	NodeAttribute::ShowDebugTreeDetails();	
#endif
}


void AttrBevelType::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	if (!Str)
		return ;

	NodeAttribute::GetDebugDetails( Str );
	String_256 TempStr;
	TempStr._MakeMsg( TEXT("\r\nValue=#1%d\r\n"), Value.m_Type );
	(*Str) += TempStr;
#else
	if (Str)
	{
		*Str = wxT("");
	}
#endif
}

/***********************************************************************************************

>	DocRect AttrBevelType::GetAttrBoundingRect(NodeRenderableInk* pParent, 
						CCAttrMap* pAttribMap)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/12/99
	Purpose:	Gets the bounding rect of this attribute
***********************************************************************************************/
DocRect AttrBevelType::GetAttrBoundingRect(NodeRenderableInk* pParent, CCAttrMap* pAttribMap)
{
	if (!pParent)
		return DocRect(0,0,0,0);
	
	if (!Value.m_bPassback)
		return pParent->GetBoundingRect(TRUE, FALSE);

	DocRect dr = pParent->GetBoundingRect(TRUE, FALSE);

	if (Value.GetIndent() < 0)
	{
		dr.Inflate(-Value.GetIndent());
	}

	return dr;
}


/********************************************************************************************

>	virtual UINT32 AttrBevelType::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrBevelType::GetNodeSize() const 
{     
	return (sizeof(AttrBevelType)); 
}  

/********************************************************************************************

>	BOOL AttrBevelType::Blend(BlendAttrParam* pBlendParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/99
	Inputs:		The blend attribute param
	Purpose:	Blends this light angle to another
	SeeAlso:	

********************************************************************************************/
BOOL AttrBevelType::Blend(BlendAttrParam* pBlendParam)
{
	ERROR2IF(pBlendParam == NULL,FALSE,"pBlendParam == NULL");
	ERROR3IF(!(pBlendParam->GetOtherAttr()->IS_KIND_OF(AttrBevelType)),
		"Blend attribute isn't a bevel type attribute");

	AttrBevelType * pBlendedAttr = NULL;

	// assign the other attribute we're looking at
	AttrBevelType * pOtherAttr = (AttrBevelType *)pBlendParam->GetOtherAttr();

	// split at the half way mark
	if (pBlendParam->GetBlendRatio() <= 0.5)
	{
		pBlendedAttr = (AttrBevelType *)SimpleCopy();
	}
	else
	{
		pBlendedAttr = (AttrBevelType *)pOtherAttr->SimpleCopy();
	}

	if (!pBlendedAttr)
		return FALSE;

	// set the blended attribute
	pBlendParam->SetBlendedAttr(pBlendedAttr);

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL AttrBevelType::CanBecomeA(BecomeA* pBecomeA)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	17/12/99
	Inputs:		pClass	-	the class to convert this to
				pNumObjects -  returns the number of objects this is converted into
	Returns:	TRUE if we can do the change, FALSE if otherwise
	Purpose:	Informs whether this node can be changed into another node or not
	SeeAlso:	-

********************************************************************************************/
BOOL AttrBevelType::CanBecomeA(BecomeA* pBecomeA)
{
	if (!Value.m_bPassback)
		return FALSE;

	if (pBecomeA->BAPath())
	{
		pBecomeA->AddCount(1);
		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

>	virtual BOOL AttrBevelType::DoBecomeA(BecomeA* pBecomeA)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	17/12/99
	Inputs:		pBecomeA	-  The become A structure to use
	Returns:	TRUE if we can do the change, FALSE if otherwise
	Purpose:	Changes this node into something else, maybe even altering the original
				node
	SeeAlso:	-

********************************************************************************************/
BOOL AttrBevelType::DoBecomeA(BecomeA* pBecomeA)
{
	return TRUE;
}

/********************************************************************************************

  >	virtual BOOL AttrBevelType::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the line width record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrBevelType::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	return TRUE;
}

//--------------------------------------------------------------
// See AttrBevelType::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrBevelType::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}

////////////////////////////////////////////////////////////////////////
// the actual attribute

BevelAttributeValueType::BevelAttributeValueType()
{
	m_Type = -1;
	m_bPassback = FALSE;
	m_pProc = NULL;

	m_Indent = 0;
	m_LightAngle = 0;
	m_Contrast = 0;
}

BevelAttributeValueType::BevelAttributeValueType(INT32 Val)
{
	m_Type = Val;
	m_bPassback = FALSE;
	m_pProc = NULL;

	m_Indent = 0;
	m_LightAngle = 0;
	m_Contrast = 0;
}

BevelAttributeValueType::~BevelAttributeValueType()
{
	if (m_pProc)
	{
		delete m_pProc;
		m_pProc = NULL;
	}
}

/********************************************************************************************

>	void BevelAttributeValueType::Render(RenderRegion *pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the line width attribute for the given render region. i.e. all
				lines drawn will now be drawn with this line width.
	SeeAlso:	BevelAttributeValueType; RenderStack; AttributeValue; NodeAttribute;
				BevelAttributeValueType::Restore; BevelAttributeValueType::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BevelAttributeValueType::Render(RenderRegion *pRegion, BOOL Temp)
{
}

/********************************************************************************************

>	void BevelAttributeValueType::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the line width attribute for the given render region. i.e. all
				lines drawn will now be drawn with this line width.
	SeeAlso:	BevelAttributeValueType; RenderStack; AttributeValue; NodeAttribute;
				BevelAttributeValueType::Render; BevelAttributeValueType::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BevelAttributeValueType::Restore(RenderRegion *pRegion, BOOL Temp)
{
}

/********************************************************************************************

>	void BevelAttributeValueType::GoingOutOfScope(RenderRegion * pRegion)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/99
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Enables this attribute to remove its path processor when it's restored
	SeeAlso:	BevelAttributeValueType; RenderStack; AttributeValue; NodeAttribute;
				BevelAttributeValueType::Restore; BevelAttributeValueType::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/
void BevelAttributeValueType::GoingOutOfScope(RenderRegion * pRegion)
{
}

/********************************************************************************************

>	KernelBitmap * BevelAttributeValueType::CreateBevelBitmap(RenderRegion * pRegion);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/99
	Inputs:		pRegion	-	The render region to use
	Returns:	A new KernelBitmap with the bevel bitmap in it
				NULL for failure
	Purpose:	Creates the bevel bitmap from the region
	SeeAlso:	

********************************************************************************************/
KernelBitmap * BevelAttributeValueType::CreateBevelBitmap(RenderRegion * pRegion)
{
	// get all the necessary variables out of the render region
	// set up the outer flag
	BOOL bOuter = FALSE;
				
	if (m_Indent < 0)
		bOuter = TRUE;
				
	// calculate the width & height of the bitmap necessary
	DocRect dr = m_pProc->GetSummedPath()->GetBoundingRect();

	if (bOuter)
	{
		dr.lo.x -= abs(m_Indent);
		dr.lo.y -= abs(m_Indent);
		dr.hi.x += abs(m_Indent);
		dr.hi.y += abs(m_Indent);
	}

	double lWidth = ((double)dr.Width()) / 750.0;
	double lHeight = ((double)dr.Height()) / 750.0;

	INT32 Width = (INT32)lWidth;
	INT32 Height = (INT32)lHeight;
				
	// calculate the light angle vector
	double angle = m_LightAngle;
				
	angle = (3.142 * angle) / 180;
			
	NormCoord LightAngle;
				
	LightAngle.x = cos(angle);
	LightAngle.y = sin(angle);	
				
	// calculate the light vector from the light angle attribute
				
	// ok, build the bitmap for the bevel
	CBeveler Beveler(m_pProc->GetSummedPath());
	Beveler.SetBevelAngle(m_LightAngle);
	Beveler.SetBevelType(m_Type);
	BOOL Ok = Beveler.SetUpBeveler(abs(m_Indent), RoundJoin, &LightAngle, bOuter, 100, NULL, 200);
	Ok = Beveler.SetBevelerSize(Width,Height);

	return NULL;
}

/********************************************************************************************

>	BOOL BevelAttributeValueType::RenderBitmap(RenderRegion * pRegion, 
						KernelBitmap * pBitmap, Path * pSrcPath, Path * pRenderPath)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/99
	Inputs:		pRegion	-	The render region to use
				pBitmap -	The bitmap to render
				pSrcPath	  - The path to be bevelled
				pRenderPath   -	The path to use to render the bitmap
	Returns:	TRUE for success
	Purpose:	Renders the bevel bitmap using the given path
	SeeAlso:	

********************************************************************************************/
BOOL BevelAttributeValueType::RenderBitmap(RenderRegion * pRegion, 
						KernelBitmap * pBitmap, Path * pSrcPath, Path * pRenderPath)
{
	return TRUE;
}

/********************************************************************************************

>	void BevelAttributeValueType::SimpleCopy(AttributeValue *pValue)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	BevelAttributeValueType; RenderStack; AttributeValue; NodeAttribute;
				BevelAttributeValueType::Render; BevelAttributeValueType::Restore;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BevelAttributeValueType::SimpleCopy(AttributeValue *pValue)
{
	m_Type			= ((BevelAttributeValueType *) pValue)->m_Type;
	m_bPassback		= ((BevelAttributeValueType *) pValue)->m_bPassback;
	m_Indent		= ((BevelAttributeValueType *) pValue)->m_Indent;
	m_LightAngle	= ((BevelAttributeValueType *) pValue)->m_LightAngle;
	m_Contrast		= ((BevelAttributeValueType *) pValue)->m_Contrast;
}

/********************************************************************************************

>	INT32 BevelAttributeValueType::operator=(AttributeValue& Attrib)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	Equality operator
	SeeAlso:	

********************************************************************************************/

INT32 BevelAttributeValueType::operator=(AttributeValue& Attrib)
{
	SimpleCopy(&Attrib);

	return TRUE;
}

/********************************************************************************************

>	BOOL BevelAttributeValueType::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers line width attribute, and provides a default attribute to give
				0.25pt lines.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL BevelAttributeValueType::Init()
{
	BevelAttributeValueType *pAttr = new BevelAttributeValueType;
	if (pAttr == NULL)
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	if (ID == ATTR_BAD_ID)
		return FALSE;
	ENSURE(ID == ATTR_BEVELTYPE, "Incorrect ID for attribute!");
	return TRUE;
}

/********************************************************************************************

>	NodeAttribute *BevelAttributeValueType::MakeNode()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrbValue node from this line width attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *BevelAttributeValueType::MakeNode()
{
	// Create new attribute node
	AttrBevelType *pAttr = new AttrBevelType();

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	INT32 BevelAttributeValueType::operator==(AttributeValue& Attrib)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Equation operator
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/
INT32 BevelAttributeValueType::operator==(AttributeValue& Attrib)
{
	if (!IsDifferent(&Attrib))
	{
		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

>	NodeAttribute *BevelAttributeValueType::MakeNode()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrbValue node from this line width attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *BevelAttributeValueType::MakeNode(Node* pContextNode, 
												 AttachNodeDirection Direction)
{
	NodeAttribute * pAttr = MakeNode();

	if (!pAttr)
		return NULL;

	pAttr->AttachNode(pContextNode, Direction);

	return pAttr;
}


/********************************************************************************************

>	BOOL BevelAttributeValueType::IsDifferent(AttributeValue *pAttr)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL BevelAttributeValueType::IsDifferent(AttributeValue *pAttr)
{
	ENSURE(GetRuntimeClass() == pAttr->GetRuntimeClass(), 
		   "Different attribute types in AttributeValue::IsDifferent()");

	if (((BevelAttributeValueType *) pAttr)->m_bPassback !=
		m_bPassback)
		return TRUE;

	if (m_bPassback)
	{
		if (((BevelAttributeValueType *) pAttr)->m_Type != m_Type ||
			((BevelAttributeValueType *) pAttr)->m_LightAngle != m_LightAngle ||
			((BevelAttributeValueType *) pAttr)->m_Contrast != m_Contrast ||
			((BevelAttributeValueType *) pAttr)->m_Indent != m_Indent)
			return TRUE;
		else
			return FALSE;
	}

	return ((BevelAttributeValueType *) pAttr)->m_Type != m_Type;
}

/********************************************************************************************

>	AttrBevelLightAngle::AttrBevelLightAngle(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected, TRUE)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a bValue Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrBevelLightAngle::AttrBevelLightAngle(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrBevel(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrBevelLightAngle::AttrBevelLightAngle()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for Line Width Attribute class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrBevelLightAngle::AttrBevelLightAngle()
{
}


/********************************************************************************************

>	void AttrBevelLightAngle::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a Line Width attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrBevelLightAngle::Render( RenderRegion* pRender)
{
}

/********************************************************************************************

>	void AttrBevelLightAngle::Transform( TransformBase& Trans )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/94
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Scale the Line Width.
	SeeAlso:	NodeRenderable::Transform

********************************************************************************************/

void AttrBevelLightAngle::Transform( TransformBase& Trans )
{
	
}

/***********************************************************************************************
> Node* AttrBevelLightAngle::SimpleCopy() 

    Author:     Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:    21/6/93
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   	
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrBevelLightAngle::SimpleCopy()
{
	AttrBevelLightAngle* NodeCopy = new AttrBevelLightAngle();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	NodeCopy->Value.m_LightAngle = Value.m_LightAngle;
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 AttrBevelLightAngle::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94
	Inputs:		Atrib:	The attribute to compare, which must be an AttrBevelLightAngle 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrBevelLightAngle 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrBevelLightAngle::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrBevelLightAngle)), 
		"Trying to compare two objects with different types"); 
	AttrBevelLightAngle * Attr = (AttrBevelLightAngle *) &Attrib;
	return (Attr->Value.m_LightAngle == Value.m_LightAngle); 
} 


/********************************************************************************************

>	virtual UINT32 AttrBevelLightAngle::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrBevelLightAngle::GetAttrNameID(void)  
{
	return (_R(IDS_BEVEL_LIGHTANGLE_ID)); 
}               
                 

/***********************************************************************************************
> ` void AttrBevel::CopyNodeContents( AttrBevelLightAngle* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrBevelLightAngle::CopyNodeContents( AttrBevelLightAngle* NodeCopy)
{
	NodeAttribute::CopyNodeContents( NodeCopy );

	NodeCopy->Value = Value;
} 

/***********************************************************************************************
>   void AttrBevelContrast::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrBevelLightAngle::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrBevelLightAngle), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrBevelLightAngle))
		CopyNodeContents((AttrBevelLightAngle*)pNodeCopy);
}



/***********************************************************************************************

>	void AttrBevelLightAngle::ShowDebugTreeDetails() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Displays debugging info of the tree

***********************************************************************************************/

void AttrBevelLightAngle::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	// For now, we will just call the base class version
#ifdef _DEBUG
	NodeAttribute::ShowDebugTreeDetails();	
#endif
}

/********************************************************************************************

>	BOOL AttrBevelLightAngle::Blend(BlendAttrParam* pBlendParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/99
	Inputs:		The blend attribute param
	Purpose:	Blends this light angle to another
	SeeAlso:	

********************************************************************************************/
BOOL AttrBevelLightAngle::Blend(BlendAttrParam* pBlendParam)
{
	ERROR2IF(pBlendParam == NULL,FALSE,"pBlendParam == NULL");
	ERROR3IF(!(pBlendParam->GetOtherAttr()->IS_KIND_OF(AttrBevelLightAngle)),
		"Blend attribute isn't a bevel light angle attribute");

	AttrBevelLightAngle * pBlendedAttr = NULL;

	// assign the other attribute we're looking at
	AttrBevelLightAngle * pOtherAttr = (AttrBevelLightAngle *)pBlendParam->GetOtherAttr();

	// split at the half way mark
	if (pBlendParam->GetBlendRatio() <= 0.5)
	{
		pBlendedAttr = (AttrBevelLightAngle *)SimpleCopy();
	}
	else
	{
		pBlendedAttr = (AttrBevelLightAngle *)pOtherAttr->SimpleCopy();
	}

	// set the blended attribute
	pBlendParam->SetBlendedAttr(pBlendedAttr);

	if (!pBlendedAttr)
		return FALSE;

	double LightAngle1		= Value.m_LightAngle;
	double LightAngle2		= pOtherAttr->Value.m_LightAngle;

	// make both light angles between 0 and 360
	while (LightAngle1 >= 360)
		LightAngle1 -= 360;

	while (LightAngle1 < 0)
		LightAngle1 += 360;

	while (LightAngle2 >= 360)
		LightAngle2 -= 360;

	while (LightAngle2 < 0)
		LightAngle2 += 360;

	// check the difference between the two
	if (fabs(LightAngle2 - LightAngle1) > 180.0)
	{
		// choose the smallest difference
		if (LightAngle2 > LightAngle1)
		{
			LightAngle1 += 360.0;
		}
		else
		{
			LightAngle2 += 360.0;
		}
	}

	double NewLightAngle	= ((LightAngle2 - LightAngle1) * pBlendParam->GetBlendRatio()) +
									LightAngle1;

	pBlendedAttr->Value.m_LightAngle = (INT32)NewLightAngle;

	return TRUE;
}



void AttrBevelLightAngle::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	if (Str)
	{
		NodeAttribute::GetDebugDetails( Str );
		String_256 TempStr;
		TempStr._MakeMsg( TEXT("\r\nValue=#1%d\r\n"), Value.m_LightAngle );
		(*Str) += TempStr;
	}
#else
	if (Str)
	{
		*Str = wxT("");
	}
#endif
}


/********************************************************************************************

>	virtual UINT32 AttrBevelLightAngle::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrBevelLightAngle::GetNodeSize() const 
{     
	return (sizeof(AttrBevelLightAngle)); 
}  

/********************************************************************************************

  >	virtual BOOL AttrBevelLightAngle::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the line width record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrBevelLightAngle::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	return TRUE;
}

//--------------------------------------------------------------
// See AttrBevelLightAngle::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrBevelLightAngle::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}

////////////////////////////////////////////////////////////////////////
// the actual attribute

BevelAttributeValueLightAngle::BevelAttributeValueLightAngle()
{
	m_LightAngle = 300;
}

/********************************************************************************************

>	void BevelAttributeValueLightAngle::Render(RenderRegion *pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the line width attribute for the given render region. i.e. all
				lines drawn will now be drawn with this line width.
	SeeAlso:	BevelAttributeValueLightAngle; RenderStack; AttributeValue; NodeAttribute;
				BevelAttributeValueLightAngle::Restore; BevelAttributeValueLightAngle::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BevelAttributeValueLightAngle::Render(RenderRegion *pRegion, BOOL Temp)
{
}

/********************************************************************************************

>	void BevelAttributeValueLightAngle::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the line width attribute for the given render region. i.e. all
				lines drawn will now be drawn with this line width.
	SeeAlso:	BevelAttributeValueLightAngle; RenderStack; AttributeValue; NodeAttribute;
				BevelAttributeValueLightAngle::Render; BevelAttributeValueLightAngle::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BevelAttributeValueLightAngle::Restore(RenderRegion *pRegion, BOOL Temp)
{
}

/********************************************************************************************

>	void BevelAttributeValueLightAngle::SimpleCopy(AttributeValue *pValue)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	BevelAttributeValueLightAngle; RenderStack; AttributeValue; NodeAttribute;
				BevelAttributeValueLightAngle::Render; BevelAttributeValueLightAngle::Restore;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BevelAttributeValueLightAngle::SimpleCopy(AttributeValue *pValue)
{
	m_LightAngle = ((BevelAttributeValueLightAngle *) pValue)->m_LightAngle;
}

/********************************************************************************************

>	BOOL BevelAttributeValueLightAngle::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers line width attribute, and provides a default attribute to give
				0.25pt lines.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL BevelAttributeValueLightAngle::Init()
{
	BevelAttributeValueLightAngle *pAttr = new BevelAttributeValueLightAngle;
	if (pAttr == NULL)
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	if (ID == ATTR_BAD_ID)
		return FALSE;
	ENSURE(ID == ATTR_BEVELLIGHTANGLE, "Incorrect ID for attribute!");
	return TRUE;
}

/********************************************************************************************

>	NodeAttribute *BevelAttributeValueLightAngle::MakeNode()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrbValue node from this line width attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *BevelAttributeValueLightAngle::MakeNode()
{
	// Create new attribute node
	AttrBevelLightAngle *pAttr = new AttrBevelLightAngle();

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	BOOL BevelAttributeValueLightAngle::IsDifferent(AttributeValue *pAttr)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL BevelAttributeValueLightAngle::IsDifferent(AttributeValue *pAttr)
{
	ENSURE(GetRuntimeClass() == pAttr->GetRuntimeClass(), 
		   "Different attribute types in AttributeValue::IsDifferent()");
	return ((BevelAttributeValueLightAngle *) pAttr)->m_LightAngle != m_LightAngle;
}

/********************************************************************************************

>	AttrBevelLightTilt::AttrBevelLightTilt(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected, TRUE)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a bValue Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrBevelLightTilt::AttrBevelLightTilt(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrBevel(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrBevelLightTilt::AttrBevelLightTilt()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for Line Width Attribute class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrBevelLightTilt::AttrBevelLightTilt()
{
}


/********************************************************************************************

>	void AttrBevelLightTilt::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a Line Width attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrBevelLightTilt::Render( RenderRegion* pRender)
{
}

/********************************************************************************************

>	void AttrBevelLightTilt::Transform( TransformBase& Trans )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/94
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Scale the Line Width.
	SeeAlso:	NodeRenderable::Transform

********************************************************************************************/

void AttrBevelLightTilt::Transform( TransformBase& Trans )
{
	
}

/***********************************************************************************************
> Node* AttrBevelLightTilt::SimpleCopy() 

    Author:     Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:    21/6/93
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   	
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrBevelLightTilt::SimpleCopy()
{
	AttrBevelLightTilt* NodeCopy = new AttrBevelLightTilt();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	NodeCopy->Value.m_LightTilt = Value.m_LightTilt;
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 AttrBevelLightTilt::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94
	Inputs:		Atrib:	The attribute to compare, which must be an AttrBevelLightTilt 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrBevelLightTilt 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrBevelLightTilt::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrBevelLightTilt)), 
		"Trying to compare two objects with different types"); 
	AttrBevelLightTilt * Attr = (AttrBevelLightTilt *) &Attrib;
	return (Attr->Value.m_LightTilt == Value.m_LightTilt); 
} 

/********************************************************************************************

>	BOOL AttrBevelLightTilt::Blend(BlendAttrParam* pBlendParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/99
	Inputs:		The blend attribute param
	Purpose:	Blends this light angle to another
	SeeAlso:	

********************************************************************************************/
BOOL AttrBevelLightTilt::Blend(BlendAttrParam* pBlendParam)
{
	ERROR2IF(pBlendParam == NULL,FALSE,"pBlendParam == NULL");
	ERROR3IF(!(pBlendParam->GetOtherAttr()->IS_KIND_OF(AttrBevelLightTilt)),
		"Blend attribute isn't a bevel light tilt attribute");

	AttrBevelLightTilt * pBlendedAttr = NULL;

	// assign the other attribute we're looking at
	AttrBevelLightTilt * pOtherAttr = (AttrBevelLightTilt *)pBlendParam->GetOtherAttr();

	// split at the half way mark
	if (pBlendParam->GetBlendRatio() <= 0.5)
	{
		pBlendedAttr = (AttrBevelLightTilt *)SimpleCopy();
	}
	else
	{
		pBlendedAttr = (AttrBevelLightTilt *)pOtherAttr->SimpleCopy();
	}

	// set the blended attribute
	pBlendParam->SetBlendedAttr(pBlendedAttr);

	if (!pBlendedAttr)
		return FALSE;

	double LightTilt1			= Value.m_LightTilt;
	double LightTilt2			= pOtherAttr->Value.m_LightTilt;
	double NewLightTilt			= ((LightTilt2 - LightTilt1) * pBlendParam->GetBlendRatio()) + LightTilt1;

	// set the blended attribute's values
	pBlendedAttr->Value.m_LightTilt = (INT32) NewLightTilt;
	
	return TRUE;
}


/********************************************************************************************

>	virtual UINT32 AttrBevelLightTilt::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrBevelLightTilt::GetAttrNameID(void)  
{
	return (_R(IDS_BEVEL_LIGHTTILT_ID)); 
}               
                 

/***********************************************************************************************
> ` void AttrBevel::CopyNodeContents( AttrBevelLightTilt* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrBevelLightTilt::CopyNodeContents( AttrBevelLightTilt* NodeCopy)
{
	NodeAttribute::CopyNodeContents( NodeCopy );

	NodeCopy->Value = Value;
} 

/***********************************************************************************************
>   void AttrBevelLightTilt::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrBevelLightTilt::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrBevelLightTilt), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrBevelLightTilt))
		CopyNodeContents((AttrBevelLightTilt*)pNodeCopy);
}



/***********************************************************************************************

>	void AttrBevelLightTilt::ShowDebugTreeDetails() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Displays debugging info of the tree

***********************************************************************************************/

void AttrBevelLightTilt::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	// For now, we will just call the base class version
#ifdef _DEBUG
	NodeAttribute::ShowDebugTreeDetails();	
#endif
}


void AttrBevelLightTilt::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	if (Str)
	{
		NodeAttribute::GetDebugDetails( Str );
		String_256 TempStr;
		TempStr._MakeMsg( TEXT("\r\nValue=#1%d\r\n"), Value.m_LightTilt );
		(*Str) += TempStr;
	}
#else
	if (Str)
	{
		*Str = wxT("");
	}
#endif
}


/********************************************************************************************

>	virtual UINT32 AttrBevelLightTilt::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrBevelLightTilt::GetNodeSize() const 
{     
	return (sizeof(AttrBevelLightTilt)); 
}  

/********************************************************************************************

  >	virtual BOOL AttrBevelLightTilt::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the line width record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrBevelLightTilt::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
/*
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	CamelotFileRecord Rec(pFilter,TAG_BEVATTR_LightTilt,sizeof(Value.m_LightTilt));

	BOOL ok = Rec.Init();

	if (ok) ok = Rec.WriteINT32(Value.m_LightTilt);
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
	*/
	return TRUE;
}

//--------------------------------------------------------------
// See AttrBevelLightTilt::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrBevelLightTilt::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}

////////////////////////////////////////////////////////////////////////
// the actual attribute

BevelAttributeValueLightTilt::BevelAttributeValueLightTilt()
{
	m_LightTilt = 32;
}

/********************************************************************************************

>	void BevelAttributeValueLightTilt::Render(RenderRegion *pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the line width attribute for the given render region. i.e. all
				lines drawn will now be drawn with this line width.
	SeeAlso:	BevelAttributeValueLightTilt; RenderStack; AttributeValue; NodeAttribute;
				BevelAttributeValueLightTilt::Restore; BevelAttributeValueLightTilt::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BevelAttributeValueLightTilt::Render(RenderRegion *pRegion, BOOL Temp)
{
}

/********************************************************************************************

>	void BevelAttributeValueLightTilt::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the line width attribute for the given render region. i.e. all
				lines drawn will now be drawn with this line width.
	SeeAlso:	BevelAttributeValueLightTilt; RenderStack; AttributeValue; NodeAttribute;
				BevelAttributeValueLightTilt::Render; BevelAttributeValueLightTilt::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BevelAttributeValueLightTilt::Restore(RenderRegion *pRegion, BOOL Temp)
{
}

/********************************************************************************************

>	void BevelAttributeValueLightTilt::SimpleCopy(AttributeValue *pValue)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	BevelAttributeValueLightTilt; RenderStack; AttributeValue; NodeAttribute;
				BevelAttributeValueLightTilt::Render; BevelAttributeValueLightTilt::Restore;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void BevelAttributeValueLightTilt::SimpleCopy(AttributeValue *pValue)
{
	m_LightTilt = ((BevelAttributeValueLightTilt *) pValue)->m_LightTilt;
}

/********************************************************************************************

>	BOOL BevelAttributeValueLightTilt::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers line width attribute, and provides a default attribute to give
				0.25pt lines.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL BevelAttributeValueLightTilt::Init()
{
	BevelAttributeValueLightTilt *pAttr = new BevelAttributeValueLightTilt;
	if (pAttr == NULL)
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	if (ID == ATTR_BAD_ID)
		return FALSE;
	ENSURE(ID == ATTR_BEVELLIGHTTILT, "Incorrect ID for attribute!");
	return TRUE;
}

/********************************************************************************************

>	NodeAttribute *BevelAttributeValueLightTilt::MakeNode()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrbValue node from this line width attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *BevelAttributeValueLightTilt::MakeNode()
{
	// Create new attribute node
	AttrBevelLightTilt *pAttr = new AttrBevelLightTilt();

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	BOOL BevelAttributeValueLightTilt::IsDifferent(AttributeValue *pAttr)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL BevelAttributeValueLightTilt::IsDifferent(AttributeValue *pAttr)
{
	ENSURE(GetRuntimeClass() == pAttr->GetRuntimeClass(), 
		   "Different attribute types in AttributeValue::IsDifferent()");
	return ((BevelAttributeValueLightTilt *) pAttr)->m_LightTilt != m_LightTilt;
}
