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
#include "lineattr.h"
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

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(AttrStrokeColour,	AttrFillGeometry)
CC_IMPLEMENT_DYNCREATE(AttrStrokeTransp,	AttrFillGeometry)
CC_IMPLEMENT_DYNCREATE(AttrLineWidth,		NodeAttribute)
CC_IMPLEMENT_DYNCREATE(AttrStartArrow,		NodeAttribute)
CC_IMPLEMENT_DYNCREATE(AttrEndArrow,		NodeAttribute)
CC_IMPLEMENT_DYNCREATE(AttrStartCap,		NodeAttribute)
CC_IMPLEMENT_DYNCREATE(AttrJoinType, 		NodeAttribute)
CC_IMPLEMENT_DYNCREATE(AttrMitreLimit, 		NodeAttribute)
CC_IMPLEMENT_DYNCREATE(AttrWindingRule, 	NodeAttribute)
CC_IMPLEMENT_DYNCREATE(AttrDashPattern,		NodeAttribute)

//#ifdef NEW_NATIVE_FILTER
CC_IMPLEMENT_DYNAMIC(LineAttrRecordHandler, CamelotRecordHandler)
//#endif //NEW_NATIVE_FILTER

// Declare smart memory handling in Debug builds
// Give this file in memory dumps
#define new CAM_DEBUG_NEW
	
/********************************************************************************************

>	AttrLineWidth::AttrLineWidth(Node* ContextNode,  
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
	Purpose:	Creates a LineWidth Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrLineWidth::AttrLineWidth(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrLineWidth::AttrLineWidth()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for Line Width Attribute class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrLineWidth::AttrLineWidth()
{
}

/********************************************************************************************

>	void AttrLineWidth::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a Line Width attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrLineWidth::Render( RenderRegion* pRender)
{
	pRender->SetLineWidth(&Value, FALSE);
}

/********************************************************************************************

>	void AttrLineWidth::Transform( TransformBase& Trans )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/94
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Scale the Line Width.
	SeeAlso:	NodeRenderable::Transform

********************************************************************************************/

void AttrLineWidth::Transform( TransformBase& Trans )
{
	if ( Trans.TransLines )
	{
		// Find out the equivalent scale factor
		FIXED16 ScaleFactor = Trans.GetScalar();

		// And scale the line width
		Value.LineWidth = labs( INT32(ScaleFactor.MakeDouble() * Value.LineWidth) );
		//TRACEUSER( "Diccon", _T("Transforming line width %d\n"), Value.LineWidth);
	}
}

/***********************************************************************************************
> Node* AttrLineWidth::SimpleCopy() 

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
     
Node* AttrLineWidth::SimpleCopy()
{
	AttrLineWidth* NodeCopy = new AttrLineWidth();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 AttrLineWidth::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94
	Inputs:		Atrib:	The attribute to compare, which must be an AttrLineWidth 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrLineWidth 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrLineWidth::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrLineWidth)), 
		"Trying to compare two objects with different types"); 
	AttrLineWidth* Attr = (AttrLineWidth*) &Attrib;
	return (Attr->Value.LineWidth == Value.LineWidth); 
} 


/********************************************************************************************

>	virtual UINT32 AttrLineWidth::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrLineWidth::GetAttrNameID(void)  
{
	return (_R(IDS_LINE_WIDTH)); 
}               
                 

/***********************************************************************************************
> ` void AttrLineWidth::CopyNodeContents( AttrLineWidth* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrLineWidth::CopyNodeContents( AttrLineWidth* NodeCopy)
{
	NodeAttribute::CopyNodeContents( NodeCopy );
	
	//Copy contents specific to derived class here
	NodeCopy->Value.LineWidth = Value.LineWidth;
} 



/***********************************************************************************************
>   void AttrLineWidth::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrLineWidth::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrLineWidth), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrLineWidth))
		CopyNodeContents((AttrLineWidth*)pNodeCopy);
}



#ifdef _DEBUG
/***********************************************************************************************

>	void AttrLineWidth::ShowDebugTreeDetails() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Displays debugging info of the tree

***********************************************************************************************/

void AttrLineWidth::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	// For now, we will just call the base class version
	NodeAttribute::ShowDebugTreeDetails();	
}
#endif


void AttrLineWidth::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;
	TempStr._MakeMsg( TEXT("\r\nLine width=#1%ld\r\n"), Value.LineWidth );
	(*Str) += TempStr;
#endif
}


/********************************************************************************************

>	virtual UINT32 AttrLineWidth::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrLineWidth::GetNodeSize() const 
{     
	return (sizeof(AttrLineWidth)); 
}  

/********************************************************************************************

>	virtual BOOL AttrLineWidth::Blend(BlendAttrParam* pBlendParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Inputs:		pBlendParam = ptr to blend attribute param object holding all info needed
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Blends a line width attribute with another.
				If successful, pBlendParam->GetBlendedAttr() will get a valid blended line attribute.
				
	SeeAlso:	-

********************************************************************************************/

BOOL AttrLineWidth::Blend(BlendAttrParam* pBlendParam)
{
	// Check entry param isn't NULL
	ERROR3IF(pBlendParam == NULL,"NULL entry param");
	if (pBlendParam == NULL) return FALSE;

	// Get the Value member to blend to the Value member of the other NodeAttribute.
	// If it succeeds, ask the blended Value to make a NodeAttribute out of itself.

	if (Value.Blend(pBlendParam))
	{
		// Get the blended attr val. After this call, the ptr is our reponsibility
		// so we have to delete it if it's no longer needed
		AttributeValue* pBlendedAttrVal = pBlendParam->GetBlendedAttrVal();

		if (pBlendedAttrVal != NULL)
		{
			// We have a blended attr val, so ask it to make a NodeAttribute out of itself
			// and set the pBlendParam's blended NodeAttribute ptr to it
			NodeAttribute* pBlendedAttr = pBlendedAttrVal->MakeNode();
			pBlendParam->SetBlendedAttr(pBlendedAttr);

			if (pBlendedAttr != NULL)
			{
				// We were able to make a blended NodeAttribute
				// so delete the blended attr val, and return TRUE
				delete pBlendedAttrVal;
				return TRUE;
			}
			else
			{
				// Couldn't make a blended NodeAttribute, so give the blended attr val back
				// and return FALSE
				pBlendParam->SetBlendedAttrVal(pBlendedAttrVal);
				return FALSE;
			}
		}
	}

	return FALSE;
}



/*********************************************************************************************

>    virtual BOOL AttrLineWidth::NeedsToRenderAtEachBrushStroke()

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   29/11/99
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if this attribute should be rendered at every step of a brush stroke
     Purpose:   So that don't have to keep re-rendering attributes whilst drawing a brush, this
				identifies whether or not the attribute need to be rendered at each step, 
				e.g. radial fills.
     Errors:    
	 See Also;  Brush code (ndbrshmk.cpp)

	 Notes:     You wouldn't think that you would need to render line width at each stroke but
				we want to make linewidth scale along with the brush.
**********************************************************************************************/
       
BOOL AttrLineWidth::NeedsToRenderAtEachBrushStroke() const
{
	return TRUE;
}

/*********************************************************************************************

>    BOOL NodeAttribute::IsALineWidthAttr() const
     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/2/95
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is a line width attribute, will return TRUE
     Purpose:   Virtual function for determining if the node is a line width attribute
     Errors:    

*************************************************************************************************/

BOOL AttrLineWidth::IsALineWidthAttr() const
{
	return TRUE;
}


/********************************************************************************************

  >	virtual BOOL AttrLineWidth::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the line width record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrLineWidth::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	CamelotFileRecord Rec(pFilter,TAG_LINEWIDTH,TAG_LINEWIDTH_SIZE);

	BOOL ok = Rec.Init();

	if (ok) ok = Rec.WriteINT32(Value.LineWidth);
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrLineWidth::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrLineWidth::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}



/********************************************************************************************

>	NodeAttribute* AttrLineWidth::GetOtherAttrToApply(BOOL* IsMutate)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/06/2000
	Inputs:		IsMutate	**don't know(!)**

	Returns:	The secondary attribute to apply, or NULL if none to apply.
	Purpose:	Some attributes require a secondary attribute to be changed when they are
				changed. This routine obtains a pointer to the secondary attribute to apply.

				In the case of AttrLineWidth, the other attribute is AttrStrokeColour.

	See also:	OpApplyAttribToSelected::ApplyToSelection().

********************************************************************************************/
NodeAttribute* AttrLineWidth::GetOtherAttrToApply(BOOL* IsMutate)
{
	ERROR3IF(IsMutate == NULL, "NULL flag pointer passed to GetOtherAttrToApply");

	// LineWidth attributes sometimes require a stroke colour attribute as well.
	AttrStrokeColour* pStrokeColour = NULL;
	pStrokeColour = new AttrStrokeColour;
	if (pStrokeColour == NULL)
		return NULL;

	*IsMutate = FALSE;

	return pStrokeColour;
}



//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------



/********************************************************************************************

>	AttrStrokeColour::AttrStrokeColour(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrFlatFill(ContextNode, Direction, Locked, Mangled, Marked, Selected)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a LineColour Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrStrokeColour::AttrStrokeColour(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrFillGeometry(ContextNode, Direction, Locked, Mangled, Marked, Selected)   
{                         
} 

/********************************************************************************************

>	AttrStrokeColour::AttrStrokeColour()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for Line Colour Attribute class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrStrokeColour::AttrStrokeColour()
{
}

/********************************************************************************************

>	void AttrStrokeColour::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a Line Colour attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrStrokeColour::Render( RenderRegion* pRender)
{
	pRender->SetLineColour((StrokeColourAttribute *) &Value, FALSE);
}

/***********************************************************************************************
> Node* AttrStrokeColour::SimpleCopy() 

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

Node* AttrStrokeColour::SimpleCopy()
{
	AttrStrokeColour* NodeCopy = new AttrStrokeColour();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	INT32 AttrStrokeColour::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94
	Inputs:		Atrib:	The attribute to compare, which must be an AttrStrokeColour 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrStrokeColour 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrStrokeColour::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrStrokeColour)), 
			"Trying to compare two objects with different types"); 
	AttrStrokeColour* Attr = (AttrStrokeColour*) &Attrib;
	return (Attr->Value.Colour == Value.Colour); 
}

/********************************************************************************************

>	virtual UINT32 AttrStrokeColour::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrStrokeColour::GetAttrNameID(void)  
{
	return (_R(IDS_LINE_COLOUR)); 
}                                  

/***********************************************************************************************
>   void AttrStrokeColour::CopyNodeContents( AttrStrokeColour* NodeCopy)


    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrStrokeColour::CopyNodeContents( AttrStrokeColour* NodeCopy)
{
	NodeAttribute::CopyNodeContents( NodeCopy );
	
	//Copy contents specific to derived class here 
	NodeCopy->Value.Colour = Value.Colour;
} 



/***********************************************************************************************
>   void AttrStrokeColour::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrStrokeColour::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrStrokeColour), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrStrokeColour))
		CopyNodeContents((AttrStrokeColour*)pNodeCopy);
}



#ifdef _DEBUG
/***********************************************************************************************

>	void AttrStrokeColour::ShowDebugTreeDetails() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Displays debugging info of the tree

***********************************************************************************************/

void AttrStrokeColour::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	// For now, we will just call the base class version
	NodeAttribute::ShowDebugTreeDetails();	
}
#endif


void AttrStrokeColour::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

#if FALSE
	if (Value.LineColour.IsTransparent())
		TempStr._MakeMsg( TEXT("\r\nLine colour=transparent\r\n"));
	else
	{
		INT32 Red, Green, Blue;
		Value.LineColour.GetRGBValue(&Red, &Green, &Blue);

		TempStr._MakeMsg( TEXT("\r\nLine colour=(#1%ld, #2%ld, #3%ld)\r\n"), 
						 Red, Green, Blue);
	}
#else
//	TempStr._MakeMsg( TEXT("\r\nLine"));
//	Value.Colour.GetDebugDetails(&TempStr);
#endif

	(*Str) += TempStr;
#endif
}


/********************************************************************************************

>	virtual UINT32 AttrStrokeColour::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrStrokeColour::GetNodeSize() const 
{     
	return (sizeof(AttrStrokeColour)); 
}  

/********************************************************************************************

>	virtual AttrFillGeometry* AttrStrokeColour::ChangeAttrValue(AttrValueChange* Mutator)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Inputs:		The value change attribute
	Returns:	A new stroke colour attr, with the appropriate changes made
	Purpose:	Changes the stroke colour in some way 

********************************************************************************************/

AttrFillGeometry* AttrStrokeColour::ChangeAttrValue(AttrValueChange* Mutator)
{
	AttrFillGeometry* NewFill = NULL;

	// Make a copy of this Fill and change the copys' control point colours
	NewFill	= (AttrFillGeometry*)this->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	if (Mutator->MutateFill(NewFill))
		return NewFill;

	return NewFill;
}


/********************************************************************************************

  >	virtual BOOL AttrStrokeColour::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the line colour record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrStrokeColour::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	INT32 ColourRef = pFilter->WriteRecord(&Value.Colour);

	BOOL ok = (ColourRef != 0);

	if (ok)
	{
		BOOL ColourRequired = TRUE;
		INT32 Tag = TAG_LINECOLOUR;
		INT32 Size = TAG_LINECOLOUR_SIZE;
		switch (ColourRef)
		{
			case REF_DEFAULTCOLOUR_TRANSPARENT:
				Tag = TAG_LINECOLOUR_NONE;
				Size = TAG_LINECOLOUR_NONE_SIZE;
				ColourRequired = FALSE;
				break;
			case REF_DEFAULTCOLOUR_BLACK:
				Tag = TAG_LINECOLOUR_BLACK;
				Size = TAG_LINECOLOUR_BLACK_SIZE;
				ColourRequired = FALSE;
				break;
			case REF_DEFAULTCOLOUR_WHITE:
				Tag = TAG_LINECOLOUR_WHITE;
				Size = TAG_LINECOLOUR_WHITE_SIZE;
				ColourRequired = FALSE;
				break;
		}

		CamelotFileRecord Rec(pFilter,Tag,Size);

		ok = Rec.Init();

		if (ok && ColourRequired) ok = Rec.WriteReference(ColourRef);
		if (ok) ok = pFilter->Write(&Rec);
	}

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;

/*	INT32 ColourRef = pFilter->WriteRecord(&Value.Colour);

	BOOL ok = (ColourRef >= 1);

	if (ok) ok = (pFilter->StartRecord(TAG_LINECOLOUR,TAG_LINECOLOUR_SIZE) != 0);
	if (ok) ok = pFilter->Write(ColourRef);
	if (ok) ok = pFilter->EndRecord();

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
*/
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrStrokeColour::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrStrokeColour::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


/********************************************************************************************

  >	virtual BOOL AttrStrokeColour::WriteColourDefinitions (BaseCamelotFilter* pFilter)

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/2000
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes out colour definitions for this fill.
	SeeAlso:	BaseCamelotFilter::WriteRemainingAtomicTagDefinitions ()
				Layer::WriteAtomicNodesColourRefs ()

********************************************************************************************/

BOOL AttrStrokeColour::WriteColourDefinitions (BaseCamelotFilter* pFilter)
{
	INT32 ColRef = pFilter->WriteRecord(&Value.Colour);

	// Is the colour reference ok?
	
	return (ColRef != 0);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/********************************************************************************************

>	AttrFillGeometry* AttrStrokeColourChange::MutateFill(AttrFillGeometry* FillToMutate) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Inputs:		The value change attribute
	Returns:	A new stroke colour attr, with the appropriate changes made
	Purpose:	Changes the stroke colour in some way 

********************************************************************************************/

AttrFillGeometry* AttrStrokeColourChange::MutateFill(AttrFillGeometry* FillToMutate) 
{ 
	// Make a copy of this Fill and change the copys' control point colours
	AttrFillGeometry* NewFill = (AttrFillGeometry*)FillToMutate->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	NewFill->SetStartColour(GetStartColour());

	return NewFill; 
}

/********************************************************************************************

>	virtual NodeAttribute* AttrStrokeColourChange::GetOtherAttrToApply(BOOL* IsMutate)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/95
	Returns:	The secondary attribute to apply, or NULL if none to apply
	Purpose:	Some attributes require a secondary atribute to be changed when they are
				changed.  This routine obtains a pointer to the secondary attribute to
				apply.

********************************************************************************************/

NodeAttribute* AttrStrokeColourChange::GetOtherAttrToApply(BOOL* IsMutate)
{
	return NULL;

/*	ERROR3IF(IsMutate == NULL, "NULL flag pointer passed to GetOtherAttrToApply");

	// A stroke colour change also needs to set the Stroke Transparency

	NodeAttribute* OtherAttr = new AttrStrokeTranspChange;
	if (OtherAttr == NULL)
		return NULL;

	UINT32 Transp;

	DocColour Col = *GetStartColour();

	if (Col.IsTransparent())
		Transp = 255;
	else
		Transp = 0;

	((AttrStrokeTranspChange *)OtherAttr)->SetStartTransp(&Transp);

	*IsMutate = TRUE;

	return OtherAttr; */
}

/********************************************************************************************

>	virtual UINT32 AttrStrokeColourChange::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/95
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrStrokeColourChange::GetAttrNameID(void)  
{
	// if it's called in the line width combo-box then we return "line width"
	// rather than "line colour"
	if (m_bLineWidth)
		return (_R(IDS_LINE_WIDTH));

	return (_R(IDS_STROKECOLOUR_CHANGE));
}                                  


/*********************************************************************************************

>    BOOL AttrStrokeColour::EffectsParentBounds()

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   12/5/95
     Inputs:    -
     Outputs:   -
     Returns:   TRUE
     Purpose:   Indicates that the stroke colour can effect the bounds of the object it is
	 			applied to (as it determines whether the path is stroked or not).

**********************************************************************************************/
       
BOOL AttrStrokeColour::EffectsParentBounds()
{
	return TRUE;
}

/*********************************************************************************************

>    BOOL AttrStrokeColourChange::EffectsParentBounds()

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   12/5/95
     Inputs:    -
     Outputs:   -
     Returns:   TRUE
     Purpose:   Indicates that the stroke colour can effect the bounds of the object it is
	 			applied to (as it determines whether the path is stroked or not).

**********************************************************************************************/
       
BOOL AttrStrokeColourChange::EffectsParentBounds()
{
	return TRUE;
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

/********************************************************************************************

>	AttrStrokeTransp::AttrStrokeTransp(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrFlatFill(ContextNode, Direction, Locked, Mangled, Marked, Selected)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a LineColour Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrStrokeTransp::AttrStrokeTransp(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrFillGeometry(ContextNode, Direction, Locked, Mangled, Marked, Selected)   
{                         
} 

/********************************************************************************************

>	AttrStrokeTransp::AttrStrokeTransp()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for Line Colour Attribute class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrStrokeTransp::AttrStrokeTransp()
{
}

/********************************************************************************************

>	void AttrStrokeTransp::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a Line Colour attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrStrokeTransp::Render( RenderRegion* pRender)
{
	pRender->SetLineTransp((StrokeTranspAttribute *) &Value, FALSE);
}

/***********************************************************************************************
> Node* AttrStrokeTransp::SimpleCopy() 

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

Node* AttrStrokeTransp::SimpleCopy()
{
	AttrStrokeTransp* NodeCopy = new AttrStrokeTransp();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	INT32 AttrStrokeTransp::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94
	Inputs:		Atrib:	The attribute to compare, which must be an AttrStrokeTransp 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrStrokeTransp 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrStrokeTransp::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrStrokeTransp)), 
			"Trying to compare two objects with different types"); 
	AttrStrokeTransp* Attr = (AttrStrokeTransp*) &Attrib;
	return (Attr->Value.Transp == Value.Transp &&
			Attr->Value.TranspType == Value.TranspType ); 
}

/********************************************************************************************

>	virtual UINT32 AttrStrokeTransp::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrStrokeTransp::GetAttrNameID(void)  
{
	return (_R(IDS_LINE_TRANSP)); 
}                                  

/***********************************************************************************************
>   void AttrStrokeTransp::CopyNodeContents( AttrStrokeTransp* NodeCopy)


    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrStrokeTransp::CopyNodeContents( AttrStrokeTransp* NodeCopy)
{
	NodeAttribute::CopyNodeContents( NodeCopy );
	
	//Copy contents specific to derived class here 
	NodeCopy->Value.Transp = Value.Transp;
	NodeCopy->Value.TranspType = Value.TranspType;
} 



/***********************************************************************************************
>   void AttrStrokeTransp::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrStrokeTransp::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrStrokeTransp), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrStrokeTransp))
		CopyNodeContents((AttrStrokeTransp*)pNodeCopy);
}



#ifdef _DEBUG
/***********************************************************************************************

>	void AttrStrokeTransp::ShowDebugTreeDetails() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Displays debugging info of the tree

***********************************************************************************************/

void AttrStrokeTransp::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	// For now, we will just call the base class version
	NodeAttribute::ShowDebugTreeDetails();	
}
#endif


void AttrStrokeTransp::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg(TEXT("\r\nLine Transparency = #1%ld "), 
					 *GetStartTransp());

	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\nTransparency Type = #1%ld "), 
					 GetTranspType());

	(*Str) += TempStr;
#endif
}


/********************************************************************************************

>	virtual UINT32 AttrStrokeTransp::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrStrokeTransp::GetNodeSize() const 
{     
	return (sizeof(AttrStrokeTransp)); 
}  

/********************************************************************************************

>	virtual AttrFillGeometry* AttrStrokeTransp::ChangeAttrValue(AttrValueChange* Mutator)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Inputs:		The value change attribute
	Returns:	A new stroke colour attr, with the appropriate changes made
	Purpose:	Changes the stroke colour in some way 

********************************************************************************************/

AttrFillGeometry* AttrStrokeTransp::ChangeAttrValue(AttrValueChange* Mutator)
{
	AttrFillGeometry* NewFill = NULL;

	// Make a copy of this Fill and change the copys' control point colours
	NewFill	= (AttrFillGeometry*)this->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	if (Mutator->MutateFill(NewFill))
		return NewFill;

	return NewFill;
}


/********************************************************************************************

  >	virtual BOOL AttrStrokeTransp::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the stroke transparancy record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrStrokeTransp::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");
	ERROR3IF(Value.Transp     > 255,"Transparency level is too high to be stored as a byte");
	ERROR3IF(Value.TranspType > 255,"Transparency type is too high to be stored as a byte");

	CamelotFileRecord Rec(pFilter,TAG_LINETRANSPARENCY,TAG_LINETRANSPARENCY_SIZE);

	BOOL ok = Rec.Init();
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.Transp));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.TranspType));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrStrokeTransp::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrStrokeTransp::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}

/********************************************************************************************

  >	virtual BOOL AttrStrokeTransp::NeedsTransparency() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/97
	Returns:	TRUE if this node requires transparency mode to render properly.
	Purpose:	Called to determine whether transparency is needed to render properly.

********************************************************************************************/

BOOL AttrStrokeTransp::NeedsTransparency() const
{
	AttrStrokeTransp* pNonConst = (AttrStrokeTransp*) this;
	return pNonConst->GetTranspType() != TT_Mix || *(pNonConst->GetStartTransp()) != 0;
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

/********************************************************************************************

>	AttrFillGeometry* AttrStrokeTranspChange::MutateFill(AttrFillGeometry* FillToMutate) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Inputs:		The value change attribute
	Returns:	A new stroke colour attr, with the appropriate changes made
	Purpose:	Changes the stroke colour in some way 

********************************************************************************************/

AttrFillGeometry* AttrStrokeTranspChange::MutateFill(AttrFillGeometry* FillToMutate) 
{ 
	// Make a copy of this Fill and change the copys' control point colours
	AttrFillGeometry* NewFill = (AttrFillGeometry*)FillToMutate->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	NewFill->SetStartTransp(GetStartTransp());

	return NewFill; 
}

/********************************************************************************************

>	virtual UINT32 AttrStrokeTranspChange::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/95
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrStrokeTranspChange::GetAttrNameID(void)  
{
	return (_R(IDS_STROKETRANSP_CHANGE));
}                                  

/********************************************************************************************

>	AttrFillGeometry* AttrStrokeTranspTypeChange::MutateFill(AttrFillGeometry* FillToMutate) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Inputs:		The value change attribute
	Returns:	A new stroke colour attr, with the appropriate changes made
	Purpose:	Changes the stroke colour in some way 

********************************************************************************************/

AttrFillGeometry* AttrStrokeTranspTypeChange::MutateFill(AttrFillGeometry* FillToMutate) 
{ 
	// Make a copy of this Fill and change the copys' control point colours
	AttrFillGeometry* NewFill = (AttrFillGeometry*)FillToMutate->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	NewFill->SetTranspType(GetTranspType());

	return NewFill; 
}

/********************************************************************************************

>	virtual UINT32 AttrStrokeTranspTypeChange::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/95
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrStrokeTranspTypeChange::GetAttrNameID(void)  
{
	return (_R(IDS_STROKETRANSPTYPE_CHANGE));
}                                  

/********************************************************************************************

>	AttrStartCap::AttrStartCap(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected, TRUE)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a StartCap Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrStartCap::AttrStartCap(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrStartCap::AttrStartCap()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for Start Cap Attribute class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrStartCap::AttrStartCap() : NodeAttribute()
{
}


/********************************************************************************************

>	void AttrStartCap::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a StartCap attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrStartCap::Render( RenderRegion* pRender)
{
	pRender->SetStartCap(&Value, FALSE);
}

/***********************************************************************************************
> Node* AttrStartCap::SimpleCopy() 

    Author:     Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:    9/7/93
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	       
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrStartCap::SimpleCopy()
{
	AttrStartCap* NodeCopy = new AttrStartCap();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
}                   

/********************************************************************************************

>	INT32 AttrStartCap::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94
	Inputs:		Atrib:	The attribute to compare, which must be an AttrStartCap 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrStartCap 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrStartCap::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrStartCap)),
		"Trying to compare two objects with different types"); 
	AttrStartCap* Attr = (AttrStartCap*) &Attrib;
	return (Attr->Value.StartCap == Value.StartCap); 
}

/********************************************************************************************

>	virtual UINT32 AttrStartCap::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrStartCap::GetAttrNameID(void)  
{
	return (_R(IDS_START_CAP)); 
}                 

/***********************************************************************************************
>   void AttrStartCap::CopyNodeContents( AttrStartCap* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrStartCap::CopyNodeContents( AttrStartCap* NodeCopy)
{
	NodeAttribute::CopyNodeContents( NodeCopy );
	NodeCopy->Value.StartCap = Value.StartCap; 
} 



/***********************************************************************************************
>   void AttrStartCap::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrStartCap::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrStartCap), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrStartCap))
		CopyNodeContents((AttrStartCap*)pNodeCopy);
}



#ifdef _DEBUG
/***********************************************************************************************

>	void AttrStartCap::ShowDebugTreeDetails() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	Displays debugging info of the tree

***********************************************************************************************/

void AttrStartCap::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	// For now, we will just call the base class version
	NodeAttribute::ShowDebugTreeDetails();	
}
#endif

void AttrStartCap::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;
	char *p;
	switch (Value.StartCap)
	{
		case LineCapButt: p = "Butt"; break;
		case LineCapRound: p = "Round"; break;
		case LineCapSquare: p = "Square"; break;
		default: p = "??"; break;
	}
	TempStr._MakeMsg( TEXT("\r\nStartCap=#1%s\r\n"), p );
	(*Str) += TempStr;
#endif
}


/********************************************************************************************

>	virtual UINT32 AttrStartCap::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrStartCap::GetNodeSize() const 
{     
	return (sizeof(AttrStartCap)); 
}  



/********************************************************************************************

  >	virtual BOOL AttrStartCap::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the start cap record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrStartCap::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	CamelotFileRecord Rec(pFilter,TAG_STARTCAP,TAG_STARTCAP_SIZE);

	BOOL ok = Rec.Init();
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.StartCap));
	if (ok) ok = pFilter->Write(&Rec);

	// Camelot uses the start cap attr to define both the start and end caps for the effected shapes.
	// However, the file format supports separate records for the two states.
	// Both the start & end cap records are saved with the same value, just in case another
	// program wishes to load the file.

	CamelotFileRecord Rec2(pFilter,TAG_ENDCAP,TAG_ENDCAP_SIZE);

	if (ok) ok = Rec2.Init();
	if (ok) ok = Rec2.WriteBYTE(BYTE(Value.StartCap));
	if (ok) ok = pFilter->Write(&Rec2);

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrStartCap::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrStartCap::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/********************************************************************************************

>	AttrStartArrow::AttrStartArrow(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected, TRUE)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a StartArrow Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrStartArrow::AttrStartArrow(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrStartArrow::AttrStartArrow()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for Start Arrow Attribute class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrStartArrow::AttrStartArrow() : NodeAttribute()
{
}

/********************************************************************************************

>	void AttrStartArrow::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a StartArrow attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrStartArrow::Render( RenderRegion* pRender)
{
	pRender->SetStartArrow(&Value, FALSE);
}

BOOL AttrStartArrow::EffectsParentBounds()
{
	return !Value.StartArrow.IsNullArrow();
}

/***********************************************************************************************

> 	DocRect AttrStartArrow::GetAttrBoundingRect(NodeRenderableInk* pParent)

    Author:     Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:    4/4/95
    Returns:    The bounding rect of the arrowhead
    Purpose:    Finds the bounding rect of the arrowhead.  

***********************************************************************************************/

DocRect AttrStartArrow::GetAttrBoundingRect(NodeRenderableInk* pParent, CCAttrMap* pAttribMap)
{
	if (!pParent->IsNodePath())
	{
//		ERROR3("Arrow is not applied to a Path !!");	
		return DocRect(0, 0, 0, 0);
	}

	INT32 LineWidth = 0;

	// Now extract the info we need from the applied attributes
	void* pLineWidth = NULL;
	pAttribMap->Lookup( CC_RUNTIME_CLASS(AttrLineWidth), pLineWidth );
	ENSURE(pLineWidth,"AttrStartArrow::GetAttrBoundingRect can't find LineWidth");

	if (pLineWidth) 
	{
		LineWidth = ((AttrLineWidth*)pLineWidth)->Value.LineWidth;
	}

	return Value.StartArrow.GetArrowBoundingRect(&((NodePath*)pParent)->InkPath, LineWidth, TRUE);
}

/***********************************************************************************************

> 	Node* AttrStartArrow::SimpleCopy() 

    Author:     Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:    9/7/93
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   
	Scope:      protected       

***********************************************************************************************/
     
Node* AttrStartArrow::SimpleCopy()
{
	AttrStartArrow* NodeCopy = new AttrStartArrow();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 AttrStartArrow::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94
	Inputs:		Atrib:	The attribute to compare, which must be an AttrStartArrow
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrStartArrow 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrStartArrow::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrStartArrow)),
		"Trying to compare two objects with different types"); 

	AttrStartArrow* Attr = (AttrStartArrow*) &Attrib;
	return (Attr->Value.StartArrow == Value.StartArrow); 
} 

/********************************************************************************************

>	virtual UINT32 AttrStartArrow::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrStartArrow::GetAttrNameID(void)  
{
	return (_R(IDS_START_ARROW)); 
}                                  

/***********************************************************************************************
>	void AttrStartArrow::CopyNodeContents( AttrStartArrow* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrStartArrow::CopyNodeContents( AttrStartArrow* NodeCopy)
{
	NodeAttribute::CopyNodeContents( NodeCopy );
	
	//Copy contents specific to derived class here
	NodeCopy->Value.StartArrow = Value.StartArrow;
} 



/***********************************************************************************************
>   void AttrStartArrow::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrStartArrow::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrStartArrow), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrStartArrow))
		CopyNodeContents((AttrStartArrow*)pNodeCopy);
}



#ifdef _DEBUG
/***********************************************************************************************

>	void AttrStartArrow::ShowDebugTreeDetails() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	Displays debugging info of the tree

***********************************************************************************************/

void AttrStartArrow::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	// For now, we will just call the base class version
	NodeAttribute::ShowDebugTreeDetails();	
}
#endif

void AttrStartArrow::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;
	//TempStr._MakeMsg( TEXT(!!!"\r\nLine width=#1%ld\r\n"), Value.LineWidth );
	(*Str) += TempStr;
#endif
}

/********************************************************************************************

>	virtual UINT32 AttrStartArrow::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrStartArrow::GetNodeSize() const 
{     
	return (sizeof(AttrStartArrow)); 
}  



/********************************************************************************************

  >	virtual BOOL AttrStartArrow::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the start arrow record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrStartArrow::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	CamelotFileRecord Rec(pFilter,TAG_ARROWHEAD,TAG_ARROWHEAD_SIZE);

	INT32		ArrowId		= 0;
	FIXED16		ScaleWidth	= Value.StartArrow.GetArrowWidth();
	FIXED16		ScaleHeight	= Value.StartArrow.GetArrowHeight();
	ArrowRec 	Arrow;

	INT32 ArrowType = Value.StartArrow.GetArrowID();

	// Convert the internal style into an arrow reference
	switch ((StockArrow)ArrowType)
	{
		case SA_NULLARROW		: ArrowId = REF_ARROW_NULL;				break;
		case SA_STRAIGHTARROW	: ArrowId = REF_ARROW_STRAIGHT;			break;
		case SA_ANGLEDARROW		: ArrowId = REF_ARROW_ANGLED;			break;
		case SA_ROUNDEDARROW	: ArrowId = REF_ARROW_ROUNDED;			break;
		case SA_SPOT			: ArrowId = REF_ARROW_SPOT;				break;
		case SA_DIAMOND			: ArrowId = REF_ARROW_DIAMOND;			break;
		case SA_ARROWFEATHER	: ArrowId = REF_ARROW_FEATHER;			break;
		case SA_ARROWFEATHER2	: ArrowId = REF_ARROW_FEATHER2;			break;
		case SA_HOLLOWDIAMOND	: ArrowId = REF_ARROW_HOLLOWDIAMOND;	break;
		default:
			ERROR3("Unkown arrow head type in AttrStartArrow::WritePreChildrenWeb");
			break;
	}

	BOOL ok = Rec.Init();
	if (ok) ok = Rec.WriteINT32(ArrowId);
	if (ok) ok = Rec.WriteFIXED16(ScaleWidth);
	if (ok) ok = Rec.WriteFIXED16(ScaleHeight);
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif // DO_EXPORT
}

//--------------------------------------------------------------
// See AttrStartArrow::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrStartArrow::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/********************************************************************************************

>	AttrEndArrow::AttrEndArrow(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected, TRUE)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates an EndArrow Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrEndArrow::AttrEndArrow(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrEndArrow::AttrEndArrow()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for End Arrow Attribute class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrEndArrow::AttrEndArrow() : NodeAttribute()
{
}

/********************************************************************************************

>	void AttrEndArrow::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a EndArrow attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrEndArrow::Render( RenderRegion* pRender)
{
	pRender->SetEndArrow(&Value, FALSE);
}

BOOL AttrEndArrow::EffectsParentBounds()
{
	return !Value.EndArrow.IsNullArrow();
}

/***********************************************************************************************

> 	DocRect AttrEndArrow::GetAttrBoundingRect(NodeRenderableInk* pParent)

    Author:     Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:    4/4/95
    Returns:    The bounding rect of the arrowhead
    Purpose:    Finds the bounding rect of the arrowhead.  

***********************************************************************************************/
     
DocRect AttrEndArrow::GetAttrBoundingRect(NodeRenderableInk* pParent, CCAttrMap* pAttribMap)
{
	if (!pParent->IsNodePath())
	{
//		ERROR3("Arrow is not applied to a Path !!");	
		return DocRect(0, 0, 0, 0);
	}


	INT32 LineWidth = 0;

	// Now extract the info we need from the applied attributes
	void* pLineWidth = NULL;
	pAttribMap->Lookup( CC_RUNTIME_CLASS(AttrLineWidth), pLineWidth );
	ENSURE(pLineWidth,"AttrEndArrow::GetAttrBoundingRect can't find LineWidth");

	if (pLineWidth) 
	{
		LineWidth = ((AttrLineWidth*)pLineWidth)->Value.LineWidth;
	}

	return Value.EndArrow.GetArrowBoundingRect(&((NodePath*)pParent)->InkPath, LineWidth, FALSE);
}

/***********************************************************************************************
> Node* AttrEndArrow::SimpleCopy() 

    Author:     Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:    9/7/93
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   	
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrEndArrow::SimpleCopy()
{
	AttrEndArrow* NodeCopy = new AttrEndArrow();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	INT32 AttrEndArrow::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94
	Inputs:		Atrib:	The attribute to compare, which must be an AttrEndArrow
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrEndArrow
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrEndArrow::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrEndArrow)),
		"Trying to compare two objects with different types"); 

	AttrEndArrow* Attr = (AttrEndArrow*) &Attrib;
	return (Attr->Value.EndArrow == Value.EndArrow); 
}

/********************************************************************************************

>	virtual UINT32 AttrEndArrow::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrEndArrow::GetAttrNameID(void)  
{
	return (_R(IDS_END_ARROW)); 
}                                  

/***********************************************************************************************
>  	void AttrEndArrow::CopyNodeContents( AttrEndArrow* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrEndArrow::CopyNodeContents( AttrEndArrow* NodeCopy)
{
	NodeAttribute::CopyNodeContents( NodeCopy );
	
	//Copy contents specific to derived class here
	NodeCopy->Value.EndArrow = Value.EndArrow;
} 



/***********************************************************************************************
>   void AttrEndArrow::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrEndArrow::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrEndArrow), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrEndArrow))
		CopyNodeContents((AttrEndArrow*)pNodeCopy);
}



#ifdef _DEBUG
/***********************************************************************************************

>	void AttrEndArrow::ShowDebugTreeDetails() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	Displays debugging info of the tree

***********************************************************************************************/

void AttrEndArrow::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	// For now, we will just call the base class version
	NodeAttribute::ShowDebugTreeDetails();	
}
#endif

void AttrEndArrow::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;
	//TempStr._MakeMsg( TEXT(!!!"\r\nLine width=#1%ld\r\n"), Value.LineWidth );
	(*Str) += TempStr;
#endif
}

/********************************************************************************************

>	virtual UINT32 AttrEndArrow::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrEndArrow::GetNodeSize() const 
{     
	return (sizeof(AttrEndArrow)); 
}  



/********************************************************************************************

  >	virtual BOOL AttrEndArrow::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the end arrow record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrEndArrow::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	CamelotFileRecord Rec(pFilter,TAG_ARROWTAIL,TAG_ARROWTAIL_SIZE);

	INT32		ArrowId		= 0;
	FIXED16		ScaleWidth	= Value.EndArrow.GetArrowWidth();
	FIXED16		ScaleHeight	= Value.EndArrow.GetArrowHeight();
	ArrowRec 	Arrow;

	INT32 ArrowType = Value.EndArrow.GetArrowID();

	// Convert the internal style into an arrow reference
	switch ((StockArrow)ArrowType)
	{
		case SA_NULLARROW		: ArrowId = REF_ARROW_NULL;				break;
		case SA_STRAIGHTARROW	: ArrowId = REF_ARROW_STRAIGHT;			break;
		case SA_ANGLEDARROW		: ArrowId = REF_ARROW_ANGLED;			break;
		case SA_ROUNDEDARROW	: ArrowId = REF_ARROW_ROUNDED;			break;
		case SA_SPOT			: ArrowId = REF_ARROW_SPOT;				break;
		case SA_DIAMOND			: ArrowId = REF_ARROW_DIAMOND;			break;
		case SA_ARROWFEATHER	: ArrowId = REF_ARROW_FEATHER;			break;
		case SA_ARROWFEATHER2	: ArrowId = REF_ARROW_FEATHER2;			break;
		case SA_HOLLOWDIAMOND	: ArrowId = REF_ARROW_HOLLOWDIAMOND;	break;
		default:
			ERROR3("Unkown arrow head type in AttrEndArrow::WritePreChildrenWeb");
			break;
	}

	BOOL ok = Rec.Init();
	if (ok) ok = Rec.WriteINT32(ArrowId);
	if (ok) ok = Rec.WriteFIXED16(ScaleWidth);
	if (ok) ok = Rec.WriteFIXED16(ScaleHeight);
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif // DO_EXPORT
}

//--------------------------------------------------------------
// See AttrEndArrow::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrEndArrow::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/********************************************************************************************

>	AttrJoinType::AttrJoinType(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected, TRUE)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a JoinType Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrJoinType::AttrJoinType(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrJoinType::AttrJoinType()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for Join Type Attribute class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrJoinType::AttrJoinType() : NodeAttribute()
{
}

/********************************************************************************************

>	void AttrJoinType::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a JoinType attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrJoinType::Render( RenderRegion* pRender)
{
	pRender->SetJoinType(&Value, FALSE);
}

/***********************************************************************************************
> Node* AttrJoinType::SimpleCopy() 

    Author:     Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:    9/7/93
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	       
   	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 

	Scope:      protected       
***********************************************************************************************/
     
Node* AttrJoinType::SimpleCopy()
{
	AttrJoinType* NodeCopy = new AttrJoinType();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	INT32 AttrJoinType::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94
	Inputs:		Atrib:	The attribute to compare, which must be an AttrJoinType
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrJoinType
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrJoinType::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrJoinType)),
		"Trying to compare two objects with different types"); 
	AttrJoinType* Attr = (AttrJoinType*) &Attrib;
	return (Attr->Value.JoinType == Value.JoinType); 
}

/********************************************************************************************

>	virtual UINT32 AttrJoinType::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrJoinType::GetAttrNameID(void)  
{
	return (_R(IDS_JOIN_TYPE)); 
}                                  

/***********************************************************************************************
>  	void AttrJoinType::CopyNodeContents( AttrJoinType* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrJoinType::CopyNodeContents( AttrJoinType* NodeCopy)
{
	NodeAttribute::CopyNodeContents( NodeCopy ); 
	NodeCopy->Value.JoinType = Value.JoinType; 
	
} 



/***********************************************************************************************
>   void AttrJoinType::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrJoinType::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrJoinType), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrJoinType))
		CopyNodeContents((AttrJoinType*)pNodeCopy);
}



#ifdef _DEBUG
/***********************************************************************************************

>	void AttrJoinType::ShowDebugTreeDetails() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	Displays debugging info of the tree

***********************************************************************************************/

void AttrJoinType::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	// For now, we will just call the base class version
	NodeAttribute::ShowDebugTreeDetails();	
}
#endif

void AttrJoinType::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;
	char *p;
	switch (Value.JoinType)
	{
		case MitreJoin: p = "Mitre"; break;
		case RoundJoin: p = "Round"; break;
		case BevelledJoin: p = "Bevelled"; break;
		default: p = "??"; break;
	}
	TempStr._MakeMsg( TEXT("\r\nJoin type=#1%s\r\n"), p );
	(*Str) += TempStr;
#endif
}


/********************************************************************************************

>	virtual UINT32 AttrJoinType::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrJoinType::GetNodeSize() const 
{     
	return (sizeof(AttrJoinType)); 
}  




/********************************************************************************************

  >	virtual BOOL AttrJoinType::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the join type record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrJoinType::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	CamelotFileRecord Rec(pFilter,TAG_JOINSTYLE,TAG_JOINSTYLE_SIZE);

	BOOL ok = Rec.Init();
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.JoinType));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrJoinType::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrJoinType::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------


/********************************************************************************************

>	AttrMitreLimit::AttrMitreLimit(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected, TRUE)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a MitreLimit Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrMitreLimit::AttrMitreLimit(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrMitreLimit::AttrMitreLimit()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for Join Type Attribute class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrMitreLimit::AttrMitreLimit() : NodeAttribute()
{
}

/********************************************************************************************

>	void AttrMitreLimit::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a MitreLimit attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrMitreLimit::Render( RenderRegion* pRender)
{
	pRender->SetMitreLimit(&Value, FALSE);
}

/***********************************************************************************************
> Node* AttrMitreLimit::SimpleCopy() 

    Author:     Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:    9/7/93
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	       
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrMitreLimit::SimpleCopy()
{
	AttrMitreLimit* NodeCopy = new AttrMitreLimit();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
}    

/********************************************************************************************

>	INT32 AttrMitreLimit::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94
	Inputs:		Atrib:	The attribute to compare, which must be an AttrMitreLimit
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrMitreLimit
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrMitreLimit::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrMitreLimit)),
		"Trying to compare two objects with different types"); 
	AttrMitreLimit* Attr = (AttrMitreLimit*) &Attrib;
	return (Attr->Value.MitreLimit == Value.MitreLimit); 
} 

/********************************************************************************************

>	virtual UINT32 AttrMitreLimit::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrMitreLimit::GetAttrNameID(void)  
{
	return (_R(IDS_MITRE_LIMIT)); 
}                               

/***********************************************************************************************
>   void AttrMitreLimit::CopyNodeContents( AttrMitreLimit* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrMitreLimit::CopyNodeContents( AttrMitreLimit* NodeCopy)
{
	NodeAttribute::CopyNodeContents( NodeCopy );
	NodeCopy->Value.MitreLimit = Value.MitreLimit; 
} 



/***********************************************************************************************
>   void AttrMitreLimit::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrMitreLimit::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrMitreLimit), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrMitreLimit))
		CopyNodeContents((AttrMitreLimit*)pNodeCopy);
}



#ifdef _DEBUG
/***********************************************************************************************

>	void AttrMitreLimit::ShowDebugTreeDetails() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	Displays debugging info of the tree

***********************************************************************************************/

void AttrMitreLimit::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	// For now, we will just call the base class version
	NodeAttribute::ShowDebugTreeDetails();	
}
#endif

void AttrMitreLimit::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;
	TempStr._MakeMsg( TEXT("\r\nMitre limit=#1%ld\r\n"), Value.MitreLimit );
	(*Str) += TempStr;
#endif
}

/********************************************************************************************

>	virtual UINT32 AttrMitreLimit::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrMitreLimit::GetNodeSize() const 
{     
	return (sizeof(AttrMitreLimit)); 
}  




/********************************************************************************************

  >	virtual BOOL AttrMitreLimit::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the mitre limit record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrMitreLimit::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	CamelotFileRecord Rec(pFilter,TAG_MITRELIMIT,TAG_MITRELIMIT_SIZE);

	BOOL ok = Rec.Init();
	if (ok) ok = Rec.WriteINT32(Value.MitreLimit);
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrMitreLimit::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrMitreLimit::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------


/********************************************************************************************

>	AttrWindingRule::AttrWindingRule(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected, TRUE)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a WindingRule Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrWindingRule::AttrWindingRule(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrWindingRule::AttrWindingRule()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for Join Type Attribute class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrWindingRule::AttrWindingRule() : NodeAttribute()
{
	Value.WindingRule = EvenOddWinding;
}

/********************************************************************************************

>	void AttrWindingRule::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a WindingRule attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrWindingRule::Render( RenderRegion* pRender )
{
	pRender->SetWindingRule(&Value, FALSE);
}

/***********************************************************************************************
> Node* AttrWindingRule::SimpleCopy() 

    Author:     Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:    9/7/93
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrWindingRule::SimpleCopy()
{
	AttrWindingRule* NodeCopy = new AttrWindingRule();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
}  

/********************************************************************************************

>	INT32 AttrWindingRule::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94
	Inputs:		Atrib:	The attribute to compare, which must be an AttrWindingRule
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrWindingRule
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrWindingRule::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrWindingRule)),
		"Trying to compare two objects with different types"); 
	AttrWindingRule* Attr = (AttrWindingRule*) &Attrib;
	return (Attr->Value.WindingRule == Value.WindingRule); 
} 

/********************************************************************************************

>	virtual UINT32 AttrWindingRule::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrWindingRule::GetAttrNameID(void)  
{
	return (_R(IDS_WINDING_RULE)); 
}                                 


/***********************************************************************************************
>	void AttrWindingRule::CopyNodeContents( AttrWindingRule* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrWindingRule::CopyNodeContents( AttrWindingRule* NodeCopy)
{
	NodeAttribute::CopyNodeContents( NodeCopy );
	NodeCopy->Value.WindingRule = Value.WindingRule; 
} 



/***********************************************************************************************
>   void AttrWindingRule::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrWindingRule::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrWindingRule), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrWindingRule))
		CopyNodeContents((AttrWindingRule*)pNodeCopy);
}



#ifdef _DEBUG
/***********************************************************************************************

>	void AttrWindingRule::ShowDebugTreeDetails() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	Displays debugging info of the tree

***********************************************************************************************/

void AttrWindingRule::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	// For now, we will just call the base class version
	NodeAttribute::ShowDebugTreeDetails();	
}
#endif

void AttrWindingRule::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;
	char *p;
	switch (Value.WindingRule)
	{
		case NonZeroWinding: p = "Non-zero"; break;
		case NegativeWinding: p = "Negative"; break;
		case EvenOddWinding: p = "Even-odd"; break;
		case PositiveWinding: p = "Positive"; break;
		default: p = "??"; break;
	}
	TempStr._MakeMsg( TEXT("\r\nWinding=#1%s\r\n"), p );
	(*Str) += TempStr;
#endif
}


/********************************************************************************************

>	virtual UINT32 AttrWindingRule::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrWindingRule::GetNodeSize() const 
{     
	return (sizeof(AttrWindingRule)); 
}  




/********************************************************************************************

  >	virtual BOOL AttrWindingRule::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the winding rule record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrWindingRule::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	CamelotFileRecord Rec(pFilter,TAG_WINDINGRULE,TAG_WINDINGRULE_SIZE);

	BOOL ok = Rec.Init();
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.WindingRule));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrWindingRule::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrWindingRule::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------


/********************************************************************************************

>	AttrDashPattern::AttrDashPattern(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected, TRUE)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a DashPattern Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrDashPattern::AttrDashPattern(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrDashPattern::AttrDashPattern()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for Join Type Attribute class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrDashPattern::AttrDashPattern() : NodeAttribute()
{
}

/********************************************************************************************

>	void AttrDashPattern::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a DashPattern attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrDashPattern::Render( RenderRegion* pRender)
{
	pRender->SetDashPattern(&Value, FALSE);
}

/***********************************************************************************************
> Node* AttrDashPattern::SimpleCopy() 

    Author:     Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:    9/7/93
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	       
   	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 

	Scope:      protected       
***********************************************************************************************/
     
Node* AttrDashPattern::SimpleCopy()
{
	AttrDashPattern* NodeCopy = new AttrDashPattern();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
}    

/********************************************************************************************

>	INT32 AttrDashPattern::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94
	Inputs:		Atrib:	The attribute to compare, which must be an AttrDashPattern
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrDashPattern
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrDashPattern::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrDashPattern)),
		"Trying to compare two objects with different types"); 

	AttrDashPattern* Attr = (AttrDashPattern*) &Attrib;
	return (Attr->Value.DashPattern == Value.DashPattern); 
}

/********************************************************************************************

>	virtual UINT32 AttrDashPattern::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrDashPattern::GetAttrNameID(void)  
{
	return (_R(IDS_DASH_PATTERN)); 
}                                


/***********************************************************************************************
>   void AttrDashPattern::CopyNodeContents( AttrDashPattern* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrDashPattern::CopyNodeContents( AttrDashPattern* NodeCopy)
{
	NodeAttribute::CopyNodeContents( NodeCopy );
	NodeCopy->Value.DashPattern = Value.DashPattern; 
} 



/***********************************************************************************************
>   void AttrDashPattern::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrDashPattern::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrDashPattern), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrDashPattern))
		CopyNodeContents((AttrDashPattern*)pNodeCopy);
}



#ifdef _DEBUG
/***********************************************************************************************

>	void AttrDashPattern::ShowDebugTreeDetails() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	Displays debugging info of the tree

***********************************************************************************************/

void AttrDashPattern::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	// For now, we will just call the base class version
	NodeAttribute::ShowDebugTreeDetails();	
}
#endif

void AttrDashPattern::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nDash ID=#1%ld\r\n"), Value.DashPattern.GetDashID() );
	(*Str) += TempStr;
	
	TempStr._MakeMsg( TEXT("\r\nLine width=#1%ld\r\n"), Value.DashPattern.LineWidth );
	(*Str) += TempStr;
	if (Value.DashPattern.ShouldScaleWithLineWidth())
		(*Str) += TEXT("Scale with line width\r\n");
	else
		(*Str) += TEXT("Don't scale with line width\r\n");

	TempStr._MakeMsg( TEXT("Dash start=#1%ld\r\n"), Value.DashPattern.DashStart );
	(*Str) += TempStr;

	INT32 Elements	= Value.DashPattern.Elements;
	TempStr._MakeMsg( TEXT("Elements=#1%ld\r\n"), Elements );
	(*Str) += TempStr;

	INT32* DashArray = Value.DashPattern.ElementData;
	for (INT32 el = 0; el < Elements; el++)
	{
		TempStr._MakeMsg( TEXT("\t=#1%ld\r\n"), DashArray[el]);
		(*Str) += TempStr;
	}			
	
#endif
}

/********************************************************************************************

>	virtual UINT32 AttrDashPattern::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrDashPattern::GetNodeSize() const 
{     
	return (sizeof(AttrDashPattern)); 
}  



/********************************************************************************************

  >	virtual BOOL AttrDashPattern::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the dash pattern record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrDashPattern::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	INT32 DashType = 0;
	INT32 Pattern = Value.DashPattern.GetDashID();
	
	BOOL ok = TRUE;

	if (Pattern != 0)
	{
		// We can save it simply using the ID
		switch ((StockDash)Pattern)
		{
			case SD_SOLID  : DashType = REF_DASH_SOLID; break;
			case SD_DASH1  : DashType = REF_DASH_1; break;
			case SD_DASH2  : DashType = REF_DASH_2; break;
			case SD_DASH3  : DashType = REF_DASH_3; break;
			case SD_DASH4  : DashType = REF_DASH_4; break;
			case SD_DASH5  : DashType = REF_DASH_5; break;
			case SD_DASH6  : DashType = REF_DASH_6; break;
			case SD_DASH7  : DashType = REF_DASH_7; break;
			case SD_DASH8  : DashType = REF_DASH_8; break;
			case SD_DASH9  : DashType = REF_DASH_9; break;
			case SD_DASH10 : DashType = REF_DASH_10; break;
			case SD_DASH11 : DashType = REF_DASH_11; break;
			case SD_DASH12 : DashType = REF_DASH_12; break;
			case SD_DASH13 : DashType = REF_DASH_13; break;
			case SD_DASH14 : DashType = REF_DASH_14; break;
			case SD_DASH15 : DashType = REF_DASH_15; break;
			case SD_DASH16 : DashType = REF_DASH_16; break;
			case SD_DASH17 : DashType = REF_DASH_17; break;
			case SD_DASH18 : DashType = REF_DASH_18; break;
			case SD_DASH19 : DashType = REF_DASH_19; break;
			case SD_DASH20 : DashType = REF_DASH_20; break;
			case SD_DASH_GUIDELAYER : DashType = REF_DASH_GUIDELAYER; break;
			default:
				ERROR3("Unkown dash type in AttrDashPattern::WritePreChildrenWeb");
		}

		// Save out the simple dash id details
		CamelotFileRecord Rec(pFilter,TAG_DASHSTYLE,TAG_DASHSTYLE_SIZE);
		ok = Rec.Init();
		if (ok) ok = Rec.WriteINT32(DashType);
		if (ok) ok = pFilter->Write(&Rec);
	}
	else
	{
		// Output the definition
		// Is it a scaled sort?
		BOOL DoScale	= Value.DashPattern.ShouldScaleWithLineWidth();
		INT32 Tag		= TAG_DEFINEDASH;
		if (DoScale)
			Tag			= TAG_DEFINEDASH_SCALED;
		
		INT32 DashStart	= Value.DashPattern.DashStart;
		INT32 LineWidth	= Value.DashPattern.LineWidth;
		INT32 Elements	= Value.DashPattern.Elements;
		INT32* DashArray = Value.DashPattern.ElementData;

		// Don't bother with zero width lines
		//if (LineWidth == 0)
		//	Elements = 0;
		
		//FIXED16 Scale = 1;
		//if (DoScale)
		//{
		//	Scale = (double(LineWidth) / double(RR_DASHPATTERN().LineWidth));
		//}

		CamelotFileRecord Rec(pFilter,Tag, CXF_UNKNOWN_SIZE);
		ok = Rec.Init();
		if (ok) ok = Rec.WriteINT32(DashStart);
		if (ok) ok = Rec.WriteINT32(LineWidth);
		if (ok) ok = Rec.WriteINT32(Elements);
		if (ok && Elements > 0)
		{
			for (INT32 el = 0; el < Elements; el++)
			{
				if (ok) ok = Rec.WriteINT32(DashArray[el]);
			}			
		}

		if (ok) ok = pFilter->Write(&Rec);
	}

	return ok;
#else
	return FALSE;
#endif // DO_EXPORT
}

//--------------------------------------------------------------
// See AttrDashPattern::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrDashPattern::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/********************************************************************************************

>	virtual UINT32* LineAttrRecordHandler::GetTagList()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		-
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler
	SeeAlso:	-

********************************************************************************************/

UINT32* LineAttrRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {	TAG_LINECOLOUR,
								TAG_LINECOLOUR_NONE,
								TAG_LINECOLOUR_BLACK,
								TAG_LINECOLOUR_WHITE,
								TAG_LINEWIDTH,
								TAG_LINETRANSPARENCY,
								TAG_STARTCAP,
								TAG_ENDCAP,
								TAG_JOINSTYLE,
								TAG_MITRELIMIT,
								TAG_WINDINGRULE,
								TAG_DASHSTYLE,
								TAG_DEFINEDASH, TAG_DEFINEDASH_SCALED,
								TAG_ARROWHEAD, TAG_ARROWTAIL,
								CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}

/********************************************************************************************

>	virtual BOOL LineAttrRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
	SeeAlso:	-

********************************************************************************************/

BOOL LineAttrRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	BOOL ok = TRUE;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_LINECOLOUR:
		case TAG_LINECOLOUR_NONE:
		case TAG_LINECOLOUR_BLACK:
		case TAG_LINECOLOUR_WHITE:
			ok = HandleLineColourRecord(pCXaraFileRecord);
			break;
		case TAG_LINEWIDTH :		ok = HandleLineWidthRecord(pCXaraFileRecord);		break;
		case TAG_LINETRANSPARENCY:	ok = HandleLineTransparencyRecord(pCXaraFileRecord);break;
		case TAG_STARTCAP:
		case TAG_ENDCAP:			ok = HandleCapRecord(pCXaraFileRecord);				break;
		case TAG_JOINSTYLE:			ok = HandleJoinStyleRecord(pCXaraFileRecord);		break;
		case TAG_MITRELIMIT:		ok = HandleMitreLimitRecord(pCXaraFileRecord);		break;
		case TAG_WINDINGRULE:		ok = HandleWindingRuleRecord(pCXaraFileRecord);		break;
		case TAG_DASHSTYLE:			ok = HandleDashStyleRecord(pCXaraFileRecord);		break;
		case TAG_DEFINEDASH:		ok = HandleDefineDashRecord(pCXaraFileRecord);		break;
		case TAG_DEFINEDASH_SCALED:	ok = HandleDefineDashRecord(pCXaraFileRecord);		break;
		case TAG_ARROWHEAD:			ok = HandleArrowHeadRecord(pCXaraFileRecord);		break;
		case TAG_ARROWTAIL:			ok = HandleArrowTailRecord(pCXaraFileRecord);		break;

		default:
			ok = FALSE;
			ERROR3_PF(("I don't handle records with the tag (%d)\n",pCXaraFileRecord->GetTag()));
			break;
	}

	return ok;
}


/********************************************************************************************

BOOL LineAttrRecordHandler::HandleLineColourRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a Line colour record
	SeeAlso:	-

********************************************************************************************/

BOOL LineAttrRecordHandler::HandleLineColourRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	//ERROR2IF(pCXaraFileRecord->GetTag() != TAG_LINECOLOUR,FALSE,"I don't handle this tag type");

	INT32 ColourRef;
	INT32 Tag = pCXaraFileRecord->GetTag();
	BOOL ok = TRUE;
	// If it is the plain line colour record then read the colour
	// If it is one of the line colour default colour records then this is not required
	switch (Tag)
	{
		case TAG_LINECOLOUR:
			ok = pCXaraFileRecord->ReadINT32(&ColourRef);
			break;
		case TAG_LINECOLOUR_NONE:
			ColourRef = REF_DEFAULTCOLOUR_TRANSPARENT;
			break;
		case TAG_LINECOLOUR_BLACK:
			ColourRef = REF_DEFAULTCOLOUR_BLACK;
			break;
		case TAG_LINECOLOUR_WHITE:
			ColourRef = REF_DEFAULTCOLOUR_WHITE;
			break;
		default:
			ERROR2(FALSE,"I don't handle this tag type");
	}
	
	if (ok)
	{
		AttrStrokeColour* pAttr = new AttrStrokeColour;
		if (pAttr != NULL)
		{
			if (ok) ok = GetDocColour(ColourRef,&(pAttr->Value.Colour));
			if (ok) ok = InsertNode(pAttr);
		}
		else
			ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

BOOL LineAttrRecordHandler::HandleLineWidthRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a Line width record
	SeeAlso:	-

********************************************************************************************/

BOOL LineAttrRecordHandler::HandleLineWidthRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_LINEWIDTH,FALSE,"I don't handle this tag type");

	INT32 Width = 0;
	AttrLineWidth* pAttr = NULL;

	BOOL	ok = pCXaraFileRecord->ReadINT32(&Width);// Read the width from the record
	if (ok)		 pAttr = new AttrLineWidth;			// Create a new line width attr
	if (ok) ok = (pAttr != NULL);					// Check it's not NULL
	if (ok)		 pAttr->Value.LineWidth = Width;	// Set the width to the read-in value
	if (ok) ok = InsertNode(pAttr);					// Insert the node into the tree

	return ok;
}

/********************************************************************************************

BOOL LineAttrRecordHandler::HandleLineTransparencyRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a Line transparency record
	SeeAlso:	-

********************************************************************************************/

BOOL LineAttrRecordHandler::HandleLineTransparencyRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_LINETRANSPARENCY,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	BYTE Transp, TranspType;
	AttrStrokeTransp* pAttr = NULL;

	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Transp);		// Read transparency level
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&TranspType);	// Read transparency type
	if (ok)		 pAttr = new AttrStrokeTransp;				// Create a new attr
	if (ok) ok = (pAttr != NULL);							// Check it's not NULL
	if (ok)		 pAttr->Value.Transp	 = UINT32(Transp);	// Set the transparency level
	if (ok)		 pAttr->Value.TranspType = UINT32(TranspType);// Set the transparency type
	if (ok) ok = InsertNode(pAttr);							// Insert the node into the tree

	return ok;
}

/********************************************************************************************

BOOL LineAttrRecordHandler::HandleCapRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a start or end cap record
	SeeAlso:	-

********************************************************************************************/

BOOL LineAttrRecordHandler::HandleCapRecord(CXaraFileRecord* pCXaraFileRecord)
{
	UINT32 Tag = pCXaraFileRecord->GetTag();

	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(Tag != TAG_STARTCAP && Tag != TAG_ENDCAP,FALSE,"I don't handle this tag type");

	// We don't have a separate end cap attribute, so just ignore it.
	// The cap def will come in with the start cap record
	if (Tag == TAG_ENDCAP)
		return TRUE;

	BOOL ok = TRUE;

	BYTE StartCap;
	AttrStartCap* pAttr = NULL;

	if (ok) ok = pCXaraFileRecord->ReadBYTE(&StartCap);			// Read cap type
	if (ok)		 pAttr = new AttrStartCap;						// Create a new attr
	if (ok) ok = (pAttr != NULL);								// Check it's not NULL
	if (ok)		 pAttr->Value.StartCap = LineCapType(StartCap);	// Set the cap type
	if (ok) ok = InsertNode(pAttr);								// Insert the node into the tree

	return ok;
}

/********************************************************************************************

BOOL LineAttrRecordHandler::HandleJoinStyleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a join style record
	SeeAlso:	-

********************************************************************************************/

BOOL LineAttrRecordHandler::HandleJoinStyleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_JOINSTYLE,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	BYTE JoinType;
	AttrJoinType* pAttr = NULL;

	if (ok) ok = pCXaraFileRecord->ReadBYTE(&JoinType);			// Read join type
	if (ok)		 pAttr = new AttrJoinType;						// Create a new attr
	if (ok) ok = (pAttr != NULL);								// Check it's not NULL
	if (ok)		 pAttr->Value.JoinType = JointType(JoinType);	// Set the join type
	if (ok) ok = InsertNode(pAttr);								// Insert the node into the tree

	return ok;
}

/********************************************************************************************

BOOL LineAttrRecordHandler::HandleMitreLimitRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a mitre limit record
	SeeAlso:	-

********************************************************************************************/

BOOL LineAttrRecordHandler::HandleMitreLimitRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_MITRELIMIT,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	INT32 MitreLimit;
	AttrMitreLimit* pAttr = NULL;

	if (ok) ok = pCXaraFileRecord->ReadINT32(&MitreLimit);	// Read mitre limit
	if (ok)		 pAttr = new AttrMitreLimit;				// Create a new attr
	if (ok) ok = (pAttr != NULL);							// Check it's not NULL
	if (ok)		 pAttr->Value.MitreLimit = MitreLimit;		// Set the mitre limit
	if (ok) ok = InsertNode(pAttr);							// Insert the node into the tree

	return ok;
}

/********************************************************************************************

BOOL LineAttrRecordHandler::HandleWindingRuleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a winding rule record
	SeeAlso:	-

********************************************************************************************/

BOOL LineAttrRecordHandler::HandleWindingRuleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_WINDINGRULE,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	BYTE WindingRule;
	AttrWindingRule* pAttr = NULL;

	if (ok) ok = pCXaraFileRecord->ReadBYTE(&WindingRule);				// Read mitre limit
	if (ok)		 pAttr = new AttrWindingRule;							// Create a new attr
	if (ok) ok = (pAttr != NULL);										// Check it's not NULL
	if (ok)		 pAttr->Value.WindingRule = WindingType(WindingRule);	// Set the mitre limit
	if (ok) ok = InsertNode(pAttr);										// Insert the node into the tree

	return ok;
}

/********************************************************************************************

BOOL LineAttrRecordHandler::HandleDashStyleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a dash style record
	SeeAlso:	-

********************************************************************************************/

BOOL LineAttrRecordHandler::HandleDashStyleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"HandleDashStyleRecord pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_DASHSTYLE,FALSE,"HandleDashStyleRecord I don't handle this tag type");

	BOOL ok = TRUE;

	INT32 DashType;
	AttrDashPattern* pAttr = NULL;

	// Read dash pattern type
	ok = pCXaraFileRecord->ReadINT32(&DashType);					

	if (ok)
	{
		// Convert this into the correct dash style
		StockDash Pattern = SD_SOLID;
		switch (DashType)
		{
			case REF_DASH_SOLID : Pattern = SD_SOLID; break;
			case REF_DASH_1 : Pattern = SD_DASH1; break;
			case REF_DASH_2 : Pattern = SD_DASH2; break;
			case REF_DASH_3 : Pattern = SD_DASH3; break;
			case REF_DASH_4 : Pattern = SD_DASH4; break;
			case REF_DASH_5 : Pattern = SD_DASH5; break;
			case REF_DASH_6 : Pattern = SD_DASH6; break;
			case REF_DASH_7 : Pattern = SD_DASH7; break;
			case REF_DASH_8 : Pattern = SD_DASH8; break;
			case REF_DASH_9 : Pattern = SD_DASH9; break;
			case REF_DASH_10 : Pattern = SD_DASH10; break;
			case REF_DASH_11 : Pattern = SD_DASH11; break;
			case REF_DASH_12 : Pattern = SD_DASH12; break;
			case REF_DASH_13 : Pattern = SD_DASH13; break;
			case REF_DASH_14 : Pattern = SD_DASH14; break;
			case REF_DASH_15 : Pattern = SD_DASH15; break;
			case REF_DASH_16 : Pattern = SD_DASH16; break;
			case REF_DASH_17 : Pattern = SD_DASH17; break;
			case REF_DASH_18 : Pattern = SD_DASH18; break;
			case REF_DASH_19 : Pattern = SD_DASH19; break;
			case REF_DASH_20 : Pattern = SD_DASH20; break;
			case REF_DASH_GUIDELAYER : Pattern = SD_DASH_GUIDELAYER; break;
			default:
				ERROR3("Unkown dash type in LineAttrRecordHandler::HandleDashStyleRecord");
		}

		pAttr = new AttrDashPattern;							// Create a new attr
		ok = (pAttr != NULL);									// Check it's not NULL
		if (ok)		 pAttr->Value.SetStockDashPattern(Pattern);	// Set the dash pattern type
		if (ok) ok = InsertNode(pAttr);							// Insert the node into the tree
	}

	return ok;
}

/********************************************************************************************

BOOL LineAttrRecordHandler::HandleDefineDashRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a define dash style record
	SeeAlso:	-

********************************************************************************************/

BOOL LineAttrRecordHandler::HandleDefineDashRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"HandleDefineDashRecord pCXaraFileRecord is NULL");
	
	INT32 Tag = pCXaraFileRecord->GetTag();
	
	ERROR2IF(Tag != TAG_DEFINEDASH && Tag != TAG_DEFINEDASH_SCALED,FALSE,"HandleDefineDashRecord I don't handle this tag type");

	BOOL ok = TRUE;

	AttrDashPattern* pAttr = NULL;
	BOOL Scale		= FALSE;
	if (Tag == TAG_DEFINEDASH_SCALED)
		Scale = TRUE;
//	INT32 DashType	= 0;
	INT32 DashStart	= 0;
	INT32 LineWidth	= 0;
	INT32 Elements	= 0;
	INT32* DashArray = NULL;	 

	// Read the information from the record
	ok = pCXaraFileRecord->ReadINT32(&DashStart);					
	ok = pCXaraFileRecord->ReadINT32(&LineWidth);					
	ok = pCXaraFileRecord->ReadINT32(&Elements);					
	
	// Don't bother with zero width lines
	//if (LineWidth == 0)
	//	Elements = 0;

	if (ok && Elements > 0)
	{
		// Sanity check on the number of elements
		ERROR3IF(Elements > 30,"Number of elements > 30, Are you sure this is correct?");

		// Create ourselves an array of the correct number of elements
		DashArray = new INT32[Elements];
		if (DashArray)
		{
			for (INT32 el = 0; el < Elements; el++)
			{
				if (ok) ok = pCXaraFileRecord->ReadINT32(&DashArray[el]);
			}
		}
		else
		{
			Elements = 0;
			ERROR3("Couldn't create the elements array");
		}
	}

	if (ok)
	{
		DashRec	Dash;
		Dash.DashStart			= DashStart; 
		Dash.LineWidth			= LineWidth; 
		Dash.Elements			= Elements;
		Dash.ElementData		= DashArray; 
		Dash.ScaleWithLineWidth = Scale;

		pAttr	   = new AttrDashPattern;						// Create a new attr
				ok = (pAttr != NULL);							// Check it's not NULL
		if (ok)		 pAttr->Value.SetDashPattern(Dash);			// Set the dash pattern
		if (ok) ok = InsertNode(pAttr);							// Insert the node into the tree
	}

	return ok;
}

/********************************************************************************************

BOOL LineAttrRecordHandler::HandleArrowHeadRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be an arrow head record
	SeeAlso:	-

********************************************************************************************/

BOOL LineAttrRecordHandler::HandleArrowHeadRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"HandleArrowHeadRecord pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_ARROWHEAD,FALSE,"HandleArrowHeadRecord I don't handle this tag type");

	BOOL ok = TRUE;

	AttrStartArrow* pAttr	= NULL;
	INT32		ArrowId		= 0;
	FIXED16		ScaleWidth;
	FIXED16		ScaleHeight;
	ArrowRec 	Arrow;
	StockArrow	ArrowType = SA_NULLARROW;

	if (ok) ok = pCXaraFileRecord->ReadINT32(&ArrowId);					// Read the type of arrow head
	if (ok) ok = pCXaraFileRecord->ReadFIXED16(&ScaleWidth);			// Read the width scaling
	if (ok) ok = pCXaraFileRecord->ReadFIXED16(&ScaleHeight);			// Read the height scaling

	// Convert the arrow id into the internal style
	if (ok)
	{
		switch (ArrowId)
		{
			case REF_ARROW_NULL:			ArrowType = SA_NULLARROW;		break;
			case REF_ARROW_STRAIGHT:		ArrowType = SA_STRAIGHTARROW;	break;
			case REF_ARROW_ANGLED:			ArrowType = SA_ANGLEDARROW;		break;
			case REF_ARROW_ROUNDED:			ArrowType = SA_ROUNDEDARROW;	break;
			case REF_ARROW_SPOT:			ArrowType = SA_SPOT;			break;
			case REF_ARROW_DIAMOND:			ArrowType = SA_DIAMOND;			break;
			case REF_ARROW_FEATHER:			ArrowType = SA_ARROWFEATHER;	break;
			case REF_ARROW_FEATHER2:		ArrowType = SA_ARROWFEATHER2;	break;
			case REF_ARROW_HOLLOWDIAMOND:	ArrowType = SA_HOLLOWDIAMOND;	break;
			default:
				ERROR3("Unkown arrow head type in LineAttrRecordHandler::HandleArrowTailRecord");
				break;
		}
	}

	if (ok) ok = Arrow.CreateStockArrow(ArrowType);
	if (ok)		 Arrow.SetArrowSize(ScaleWidth, ScaleHeight);

	if (ok)		 pAttr = new AttrStartArrow;							// Create a new attr
	if (ok) ok = (pAttr != NULL);										// Check it's not NULL
	if (ok)		 pAttr->Value.StartArrow = Arrow;						// Set the arrow
	if (ok) ok = InsertNode(pAttr);										// Insert the node into the tree

	return ok;
}

/********************************************************************************************

BOOL LineAttrRecordHandler::HandleArrowTailRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be an arrow tail record
	SeeAlso:	-

********************************************************************************************/

BOOL LineAttrRecordHandler::HandleArrowTailRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"HandleArrowTailRecord pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_ARROWTAIL,FALSE,"HandleArrowTailRecord I don't handle this tag type");

	BOOL ok = TRUE;

	AttrEndArrow* pAttr	= NULL;
	INT32		ArrowId		= 0;
	FIXED16		ScaleWidth;
	FIXED16		ScaleHeight;
	ArrowRec 	Arrow;
	StockArrow	ArrowType = SA_NULLARROW;

	if (ok) ok = pCXaraFileRecord->ReadINT32(&ArrowId);					// Read the type of arrow head
	if (ok) ok = pCXaraFileRecord->ReadFIXED16(&ScaleWidth);			// Read the width scaling
	if (ok) ok = pCXaraFileRecord->ReadFIXED16(&ScaleHeight);			// Read the height scaling

	// Convert the arrow id into the internal style
	if (ok)
	{
		switch (ArrowId)
		{
			case REF_ARROW_NULL:			ArrowType = SA_NULLARROW;		break;
			case REF_ARROW_STRAIGHT:		ArrowType = SA_STRAIGHTARROW;	break;
			case REF_ARROW_ANGLED:			ArrowType = SA_ANGLEDARROW;		break;
			case REF_ARROW_ROUNDED:			ArrowType = SA_ROUNDEDARROW;	break;
			case REF_ARROW_SPOT:			ArrowType = SA_SPOT;			break;
			case REF_ARROW_DIAMOND:			ArrowType = SA_DIAMOND;			break;
			case REF_ARROW_FEATHER:			ArrowType = SA_ARROWFEATHER;	break;
			case REF_ARROW_FEATHER2:		ArrowType = SA_ARROWFEATHER2;	break;
			case REF_ARROW_HOLLOWDIAMOND:	ArrowType = SA_HOLLOWDIAMOND;	break;
			default:
				ERROR3("Unkown arrow head type in LineAttrRecordHandler::HandleArrowTailRecord");
				break;
		}
	}

	if (ok) ok = Arrow.CreateStockArrow(ArrowType);
	if (ok)		 Arrow.SetArrowSize(ScaleWidth, ScaleHeight);

	if (ok)		 pAttr = new AttrEndArrow;								// Create a new attr
	if (ok) ok = (pAttr != NULL);										// Check it's not NULL
	if (ok)		 pAttr->Value.EndArrow = Arrow;							// Set the arrow
	if (ok) ok = InsertNode(pAttr);										// Insert the node into the tree

	return ok;
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

/********************************************************************************************

>	virtual void LineAttrRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/96
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This provides descriptions for the fill attribute record.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#ifdef XAR_TREE_DIALOG
void LineAttrRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	// Call base class first
	// This outputs the tag and size
	CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

	TCHAR s[256];

	UINT32 Tag = pRecord->GetTag();
	switch (Tag)
	{
		case TAG_LINECOLOUR:
		{
			// Read the colour reference number
			INT32 ColourRef;
			pRecord->ReadINT32(&ColourRef);
			camSprintf(s,_T("Colour reference = %d\r\n"),ColourRef);		(*pStr) += s;
		}
		break;

		case TAG_LINEWIDTH:
		{
			INT32 Width;
			pRecord->ReadINT32(&Width);
			camSprintf(s,_T("Width = %d\r\n"),Width);		(*pStr) += s;
		}
		break;

		case TAG_LINETRANSPARENCY:
		{
			BYTE Transp, TranspType;
			pRecord->ReadBYTE(&Transp);
			pRecord->ReadBYTE(&TranspType);
			camSprintf(s,_T("Transparency\t= %d\r\n"),INT32(Transp));			(*pStr) += s;
			camSprintf(s,_T("Transparency Type\t= %d\r\n"),INT32(TranspType));	(*pStr) += s;
		}
		break;

		case TAG_STARTCAP:
		case TAG_ENDCAP:
		{
			BYTE Cap;
			pRecord->ReadBYTE(&Cap);
			camSprintf(s,_T("Cap style = %d\r\n"),INT32(Cap));	(*pStr) += s;
		}
		break;

		case TAG_JOINSTYLE:
		{
			BYTE Join;
			pRecord->ReadBYTE(&Join);
			camSprintf(s,_T("Join style = %d\r\n"),INT32(Join)); (*pStr) += s;
		}
		break;

		case TAG_MITRELIMIT:
		{
			INT32 MitreLimit;
			pRecord->ReadINT32(&MitreLimit);
			camSprintf(s,_T("Mitre Limit = %d\r\n"),MitreLimit); (*pStr) += s;
		}
		break;

		case TAG_WINDINGRULE:
		{
			BYTE WindingRule;
			pRecord->ReadBYTE(&WindingRule);
			camSprintf(s,_T("Winding Rule = %d\r\n"),INT32(WindingRule)); (*pStr) += s;
		}
		break;

		case TAG_DASHSTYLE:
		{
			INT32 DashVal;
			pRecord->ReadINT32(&DashVal);
			camSprintf(s,_T("Dash Style = %d\r\n"),DashVal); (*pStr) += s;
		}
		break;

		case TAG_DEFINEDASH:
		case TAG_DEFINEDASH_SCALED:
		{
			if (Tag == TAG_DEFINEDASH_SCALED)
			{
				camSprintf(s,_T("Define scaled dash pattern\r\n\r\n"));
				(*pStr) += s;
			}
			else
			{				
				camSprintf(s,_T("Define dash pattern\r\n\r\n"));
				(*pStr) += s;
			}

			// Read the information from the record
			INT32 DashStart	= 0;
			pRecord->ReadINT32(&DashStart);
			camSprintf(s,_T("Dash Start = %d\r\n"),DashStart); (*pStr) += s;

			INT32 LineWidth	= 0;
			pRecord->ReadINT32(&LineWidth);
			camSprintf(s,_T("Line width = %d\r\n"),LineWidth); (*pStr) += s;

			INT32 Elements	= 0;
			pRecord->ReadINT32(&Elements);
			camSprintf(s,_T("Elements = %d\r\n"),Elements); (*pStr) += s;

			// Don't bother with zero width lines
			//if (LineWidth == 0)
			//	Elements = 0;

			if (Elements > 0)
			{
				// Show the elements in all their glory
				INT32 Data = 0L;
				for (INT32 el = 0; el < Elements; el++)
				{
					if (!pRecord->ReadINT32(&Data))
						break;
					camSprintf(s,_T("\t%d\r\n"),Data); (*pStr) += s;
				}
			}
		}
		break;

		case TAG_ARROWHEAD:
		{
			INT32 ArrowId;
			pRecord->ReadINT32(&ArrowId);
			camSprintf(s,_T("Arrow head Id = %d\r\n"),ArrowId); (*pStr) += s;
			FIXED16 ScaleWidth;
			pRecord->ReadFIXED16(&ScaleWidth);
			camSprintf(s,_T("Scale width = %f\r\n"),ScaleWidth.MakeDouble()); (*pStr) += s;
			FIXED16 ScaleHeight;
			pRecord->ReadFIXED16(&ScaleHeight);
			camSprintf(s,_T("Scale height = %f\r\n"),ScaleHeight.MakeDouble()); (*pStr) += s;
		}
		break;

		case TAG_ARROWTAIL:
		{
			INT32 ArrowId;
			pRecord->ReadINT32(&ArrowId);
			camSprintf(s,_T("Arrow tail Id = %d\r\n"),ArrowId); (*pStr) += s;
			FIXED16 ScaleWidth;
			pRecord->ReadFIXED16(&ScaleWidth);
			camSprintf(s,_T("Scale width = %f\r\n"),ScaleWidth.MakeDouble()); (*pStr) += s;
			FIXED16 ScaleHeight;
			pRecord->ReadFIXED16(&ScaleHeight);
			camSprintf(s,_T("Scale height = %f\r\n"),ScaleHeight.MakeDouble()); (*pStr) += s;
		}
		break;
	}
}
#endif
