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

// Quality class used for controlling rendering quality (known as WYSIWYG in ArtWorks)

/*
*/

#include "camtypes.h"
#include "qualattr.h"
#include "ensure.h"
#include "rndrgn.h"
#include "ink.h"
//#include "tim.h"
#include "attrmgr.h"
#include "layer.h"

#include "cxftags.h"
#include "cxfdefs.h"
#include "cxfrec.h"
#include "camfiltr.h"

// For now all GetxxxQuality fns are based on ArtWorks values. Once we are happy
// with them they should be made inline fns.

CC_IMPLEMENT_DYNCREATE(QualityAttribute, AttributeValue)
CC_IMPLEMENT_DYNCREATE(AttrQuality, NodeAttribute)

DocColour* AttrQuality::pCurrentColour = NULL;

// Give this file in memory dumps
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/********************************************************************************************

	Preference:	DefaultQuality
	Section:	Rendering
	Range:		10 to 110.
	Purpose:	The quality level to give to new views.

********************************************************************************************/ 

Quality Quality::DefaultQuality;



/********************************************************************************************

>	enum Quality::Line Quality::GetLineQuality() const

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Determines how lines should be rendered based on the quality. Returns
				Quality::NoLine, ThinLine or FullLine.

********************************************************************************************/

enum Quality::Line Quality::GetLineQuality() const
{
	if (QualityValue > 50 || QualityValue == QualityGuideLayer)
		return ((enum Quality::Line) FullLine);
	else if (QualityValue > 30)
		return (enum Quality::Line) ThinLine;
	else
		return (enum Quality::Line) BlackLine;
}

/********************************************************************************************

>	enum Quality::Fill Quality::GetFillQuality() const

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Determines how fills should be rendered based on the quality. Returns
				Quality::NoFill, Solid or Graduated.

********************************************************************************************/

enum Quality::Fill Quality::GetFillQuality() const
{
	if (QualityValue >= 60)
		return (enum Quality::Fill) Graduated;
	else if (QualityValue > 30)
		return (enum Quality::Fill) Solid;
	else if (QualityValue > 10)
		return (enum Quality::Fill) Bitmaps;
	else
		return (enum Quality::Fill) NoFill;
}

/********************************************************************************************

>	enum Quality::Blend Quality::GetBlendQuality() const

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Determines how Blends should be rendered based on the quality. Returns
				Quality::NoBlend, StartAndEnd or FullBlend.

********************************************************************************************/

enum Quality::Blend Quality::GetBlendQuality() const
{
	if (QualityValue <= 20)
		return (enum Quality::Blend) StartAndEnd;
	else
		return (enum Quality::Blend) FullBlend;
}

/********************************************************************************************

>	enum Quality::Transparency Quality::GetTransparencyQuality() const

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Determines how Transparencys should be rendered based on the quality. Returns
				Quality::NoTransparency, or FullTransparency. NOTE: ALWAYS NOTRANS currently.

********************************************************************************************/

enum Quality::Transparency Quality::GetTransparencyQuality() const
{
	// don't know how this is going to work yet
	return (enum Quality::Transparency) NoTransparency;
}

/********************************************************************************************

>	enum Quality::Antialias Quality::GetAntialiasQuality() const

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Determines how Antialiass should be rendered based on the quality. Returns
				Quality::NoAntialias, or FullAntialias

********************************************************************************************/

enum Quality::Antialias Quality::GetAntialiasQuality() const
{
	if (QualityValue <= 100)
		return (enum Quality::Antialias) NoAntialias;
	else
		return (enum Quality::Antialias) FullAntialias;
}

/********************************************************************************************

>	void Quality::SetDefault()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Resets value to the default value, whatever that might be.
	Errors:		ENSUREs if out of range.
	Scope:		Public

********************************************************************************************/

void Quality::SetDefault()
{
	QualityValue = QUALITY_DEFAULT;
}

/********************************************************************************************

>	BOOL Quality::SetQuality(INT32 NewQuality)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/94
	Inputs:		NewQuality should be from 0 to MAX_QUALITY.
	Returns:	TRUE if changed, FALSE if out of range OR STAYED SAME.
	Purpose:	Sets numeric value for quality. You should be using this, except in very
				special situations.
	Errors:		ENSUREs if out of range.
	SeeAlso:	Quality::GetQuality

********************************************************************************************/

BOOL Quality::SetQuality(INT32 NewQuality)
{
	if ( (NewQuality < 0 || NewQuality > QUALITY_MAX) && NewQuality != QUALITY_GUIDELAYER)
	{
		ENSURE(FALSE, "Bad quality in SetQuality");
		return FALSE;
	}
	else
	{
		if (QualityValue != NewQuality)
		{
			QualityValue = NewQuality;
			return TRUE;
		}
		else
			return FALSE;								// not an error, just 'no-change'
	}
}

/********************************************************************************************

>	INT32 Quality::GetQuality() const

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/94
	Inputs:		-
	Outputs:	-
	Returns:	Numeric value for quality.
	Purpose:	You should use the GetxxxQuality fns. Only in very special situations are
				you allowed to call this function. It was Protected to stop you from using
				it, but this has had to change for technical reasons.
				So, what does this function actually do. It tells you the numical value of
				the quality.
	SeeAlso:	Quality::SetQuality

********************************************************************************************/

INT32 Quality::GetQuality() const
{
	return QualityValue;
}


/********************************************************************************************

>	void QualityAttribute::Render(RenderRegion *pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/94
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the quality attribute for the given render region.
	SeeAlso:	QualityAttribute; RenderStack; AttributeValue; NodeAttribute;
				QualityAttribute::Restore; QualityAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void QualityAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "QualityAttribute::Render - do nothing" )
//	TRACE( _T("Warning - QualityAttribute::Render called\n") );
	pRegion->SetQuality(this, Temp);
}

/********************************************************************************************

>	void QualityAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the quality attribute for the given render region.
	SeeAlso:	QualityAttribute; RenderStack; AttributeValue; NodeAttribute;
				QualityAttribute::Render; QualityAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void QualityAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "QualityAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - QualityAttribute::Restore called\n") );
	pRegion->RestoreQuality(this, Temp);
}

/********************************************************************************************

>	void QualityAttribute::SimpleCopy(AttributeValue *pValue)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/94
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy
	SeeAlso:	QualityAttribute; RenderStack; AttributeValue; NodeAttribute;
				QualityAttribute::Render; QualityAttribute::Restore;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void QualityAttribute::SimpleCopy(AttributeValue *pValue)
{
	QualityValue = ((QualityAttribute *) pValue)->QualityValue;
}

/********************************************************************************************

>	BOOL QualityAttribute::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/94
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers quality attribute, and provides a default attribute.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL QualityAttribute::Init()
{
	// Defaults to QUALITY_DEFAULT (not surprisingly)
	QualityAttribute *pAttr = new QualityAttribute;
	if (pAttr == NULL)
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	if (ID == ATTR_BAD_ID)
		return FALSE;
	ENSURE(ID == ATTR_QUALITY, "Incorrect ID for attribute!");
	return TRUE;
}

/********************************************************************************************

>	NodeAttribute *QualityAttribute::MakeNode()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrQuality node from this line width attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *QualityAttribute::MakeNode()
{
	// Create new attribute node
	AttrQuality *pAttr = new AttrQuality();

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	BOOL QualityAttribute::IsDifferent(AttributeValue *pAttr)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/94
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL QualityAttribute::IsDifferent(AttributeValue *pAttr)
{
	ENSURE(GetRuntimeClass() == pAttr->GetRuntimeClass(), 
		   "Different attribute types in AttributeValue::IsDifferent()");
	return ((QualityAttribute *) pAttr)->QualityValue != QualityValue;
}


/********************************************************************************************

>	AttrQuality::AttrQuality(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected, TRUE)  

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/94
	Inputs:		See base class
	Purpose:	Creates a Quality Attribute.
	SeeAlso:	NodeAttribute

********************************************************************************************/

AttrQuality::AttrQuality(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrQuality::AttrQuality()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/94
	Purpose:	Default constructor for Quality Attribute class

********************************************************************************************/

AttrQuality::AttrQuality()
{
}

/********************************************************************************************

>	void AttrQuality::Render( RenderRegion* pRender)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/94
	Inputs:		pRender - the render region to render the attribute into.
	Purpose:	Renders a quality attribute.

********************************************************************************************/

void AttrQuality::Render( RenderRegion* pRender)
{
//#pragma message( __LOCMSG__ "AttrQuality::Render - do nothing" )
//	TRACE( _T("Warning - AttrQuality::Render called\n") );
	pRender->SetQuality(&Value, FALSE);

	Layer* pLayer = (Layer*)FindParent(CC_RUNTIME_CLASS(Layer));
	if (pLayer != NULL)
	{
		// Copy layer's colour
		Colour = *(pLayer->GetGuideColour());

		// Make static reference point to this colour
		pCurrentColour = &Colour;
	}
}

/********************************************************************************************

> 	Node* AttrQuality::SimpleCopy() 

    Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	23/04/94
	Returns:	A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
				memory error and the function returns NULL. 
	Scope:      protected

********************************************************************************************/
     
Node* AttrQuality::SimpleCopy()
{
	AttrQuality* NodeCopy = new AttrQuality();
	if (NodeCopy == NULL)
		return NULL; 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 AttrQuality::operator==(const NodeAttribute& Attrib)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/94
	Inputs:		Atrib:	The attribute to compare, which must be an AttrQuality 
	Returns:	Non-zero if objects are equal, zero if not.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrQuality 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrQuality::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrQuality)), 
		"Trying to compare two objects with different types"); 
	AttrQuality* Attr = (AttrQuality*) &Attrib;
	return (Attr->Value.QualityValue == Value.QualityValue);
} 


/********************************************************************************************

>	virtual UINT32 AttrQuality::GetAttrNameID(void)  

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/94
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrQuality::GetAttrNameID(void)  
{
	return (_R(IDS_QUALITY)); 
}               
                 

/********************************************************************************************

>  	void AttrQuality::CopyNodeContents( AttrQuality* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/04/94
    Outputs:    NodeCopy - A copy of this node.
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
********************************************************************************************/

void AttrQuality::CopyNodeContents( AttrQuality* NodeCopy)
{
	NodeAttribute::CopyNodeContents( NodeCopy );
	
	// Copy contents specific to derived class here
	NodeCopy->Value.QualityValue = Value.QualityValue;
} 



/***********************************************************************************************
>   void AttrQuality::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrQuality::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrQuality), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrQuality))
		CopyNodeContents((AttrQuality*)pNodeCopy);
}



#ifdef _DEBUG

/********************************************************************************************

>	void AttrQuality::ShowDebugTreeDetails() const

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/94
	Purpose:	Displays debugging info of the tree

********************************************************************************************/

void AttrQuality::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	// For now, we will just call the base class version
	NodeAttribute::ShowDebugTreeDetails();	
}
#endif


void AttrQuality::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;
	TempStr._MakeMsg( TEXT("\r\nQuality=#1%ld\r\n"), Value.QualityValue.GetQuality() );
	(*Str) += TempStr;
#endif
}


/********************************************************************************************

>	virtual UINT32 AttrQuality::GetNodeSize() const

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrQuality::GetNodeSize() const 
{     
	return (sizeof(AttrQuality)); 
}  

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

/********************************************************************************************

  >	virtual BOOL AttrQuality::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the quality record to the filter.
				*** DOESN'T WRITE ANYTHING OUT IN THE WEB FORMAT ***
	SeeAlso:	-

********************************************************************************************/

BOOL AttrQuality::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	return FALSE;
}

/********************************************************************************************

  >	virtual BOOL AttrQuality::WritePreChildrenNative(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the quality attribute record to the filter.
	SeeAlso:	-

********************************************************************************************/

BOOL AttrQuality::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL ok = TRUE;

	CXaraFileRecord Rec(TAG_QUALITY,TAG_QUALITY_SIZE);

	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteINT32(Value.QualityValue.GetQuality());
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

