// $Id: userattr.cpp 662 2006-03-14 21:31:49Z alex $
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
#include "attrval.h"
#include "nodeattr.h"
#include "userattr.h"

#include "errors.h"
#include "ink.h"
#include "blendatt.h"

#include "camfiltr.h"	// Record handling classes for attributes
#include "rechattr.h"
#include "cxfrec.h"
#include "cxftags.h"
#include "cxfdefs.h"

//#include "mario.h"		// for _R(IDE_NOMORE_MEMORY)
//#include "hotlink.h"	// for _R(IDS_USERATTRIBUTE_ID)
//#include "tmpltres.h"	// for _R(IDS_CUSTOM)

#include "gadget.h"		// for the UI side
#include "uielem.h"
#include "visiattr.h"

//#include "dialogop.h"	// for slight bodge
//#include "tmpltdlg.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNCREATE(AttrUser, NodeAttribute)
CC_IMPLEMENT_MEMDUMP(VisibleAttribute, UserInterface)
CC_IMPLEMENT_MEMDUMP(VisibleUserAttribute, VisibleAttribute)

// Give this file in memory dumps
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

#undef ENSURE_NOT_NULL
#define ENSURE_NOT_NULL(Pointer)		ERROR2IF(Pointer == NULL, 0, "NULL Args")
#define ENSURE_KIND(pInstance, Class)	ERROR3IF(!pInstance->IS_KIND_OF(Class), #pInstance" is not kind of "#Class);

/******************************************************************************************

  FUNCTIONS FOR THE UserATTRIBUTE CLASS (Graham 18/8/96)

**********************************************************************************************/
UserAttribute::UserAttribute()
  :	m_pStyle(0)
{
	// To ensure default constructed UserAttributes can be copied, alloc the strings.
	if (!Key.Alloc(0) || !LongKey.Alloc(0) || !Value.Alloc(0)) return;
}



/********************************************************************************************
>	UserAttribute& UserAttribute::operator=(const UserAttribute& other)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/99
	Purpose:	Assignment operator for UserAttributes.
	Errors:		_R(IDE_NOMORE_MEMORY)
	Notes:		*DOESN'T* COPY THE m_pStyle MEMBER!  What the hell is that doing in this
				class, anyway??
	SeeAlso:	UserAttribute::SimpleCopy; AttrUser::CopyNodeContents
********************************************************************************************/

UserAttribute& UserAttribute::operator=(const UserAttribute& other)
{
	if (Key.Alloc(other.Key.Length()) &&
		LongKey.Alloc(other.LongKey.Length()) &&
		Value.Alloc(other.Value.Length()))
	{		
		Key = other.Key;
		LongKey = other.LongKey;
		Value = other.Value;
	}

	return *this;
}



/********************************************************************************************

>	static BOOL UserAttribute::Init()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	There's no need for this function at the moment but I'll leave it in
				in case someone needs it in the future.

				This function usually registers Default Attributes (see commented out
				code below). There's never a need to register a Default User Attribute.

	Errors:		None
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL UserAttribute::Init()
{
	UserAttribute *pAttr = new UserAttribute;
	if (pAttr == NULL)
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
														 pAttr);
	if (ID == ATTR_BAD_ID)
		return FALSE;
	ENSURE(ID == ATTR_USERATTRIBUTE, "Incorrect ID for attribute!");
	return TRUE;
}


/********************************************************************************************

>	void UserAttribute::Render(RenderRegion *pRegion)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Rendering a User Attribute does nothing. That's what
				this function does.
	SeeAlso:	UserAttribute; RenderStack; AttributeValue; NodeAttribute;
				ValueAttribute::Restore; ValueAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/
void UserAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
	//Do nothing
}


/********************************************************************************************

>	void UserAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Again, restoring a User Attribute does nothing
	SeeAlso:	UserAttribute; RenderStack; AttributeValue; NodeAttribute;
				ValueAttribute::Render; ValueAttribute::SimpleCopy;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void UserAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
	// Do nothing
}

/********************************************************************************************

>	void UserAttribute::SimpleCopy(AttributeValue* pValue)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		pValue - pointer to the AttributeValue to copy.
	Purpose:	Copies both bits of the data from pValue
	SeeAlso:	ValueAttribute; RenderStack; AttributeValue; NodeAttribute;
				ValueAttribute::Render; ValueAttribute::Restore;
				AttributeValue::Render; AttributeValue::Restore; AttributeValue::SimpleCopy

********************************************************************************************/

void UserAttribute::SimpleCopy(AttributeValue* pValue)
{
	*this = *((UserAttribute*) pValue);
}


/********************************************************************************************

>	NodeAttribute *UserAttribute::MakeNode()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrUser node from this line width attribute.
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *UserAttribute::MakeNode()
{
	// Create new attribute node
	AttrUser *pAttr = new AttrUser();

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}

/********************************************************************************************

>	BOOL UserAttribute::IsDifferent(AttributeUser *pAttr)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> from	Tim
	Created:	12/04/94
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL UserAttribute::IsDifferent(AttributeValue *pAttr)
{
	ENSURE_NOT_NULL(pAttr);

	ENSURE(GetRuntimeClass() == pAttr->GetRuntimeClass(), 
		   "Different attribute types in AttributeValue::IsDifferent()");

	ENSURE_KIND(pAttr, UserAttribute);
	UserAttribute* const pUserAttrVal = (UserAttribute*)pAttr;

	// LongKey should always start with Key
	return pUserAttrVal->LongKey != LongKey || pUserAttrVal->Value != Value;
}

/********************************************************************************************

>	BOOL UserAttribute::Blend(BlendAttrParam *pBlendParam)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> from Markn
	Created:	15/8/96
	Inputs:		pBlendParam = ptr to param holding all data needed for the attr val to blend
	Outputs:	if TRUE  returned, pBlendParam->GetBlendedAttrVal() will get ptr to the blended attr val
				if FALSE returned, pBlendParam->GetBlendedAttrVal() will return NULL
	Returns:	TRUE if successful, FALSE otherwaie
	Purpose:	Blends this attr val with the attr val held in pBlendParam
	SeeAlso:	-

********************************************************************************************/

BOOL UserAttribute::Blend(BlendAttrParam *pBlendParam)
{
	// Check entry param
	ERROR3IF(pBlendParam == NULL,"NULL entry param");
	if (pBlendParam == NULL) return FALSE;

	// Check that the pointer to the other User attr val is not NULL, and is a UserAttribute
	UserAttribute* pOtherUserAttr = (UserAttribute*) pBlendParam->GetOtherAttrVal();
	ERROR3IF(pOtherUserAttr == NULL,"NULL other attr val");
	ERROR3IF(!IS_A(pOtherUserAttr, UserAttribute),"other attr val not a user attr");
	if (pOtherUserAttr == NULL || !IS_A(pOtherUserAttr, UserAttribute)) return FALSE;

	//Graham: Here's the change. If the Keys of the attributes are not the same, we must
	//not blend these attributes.
	if (Key != pOtherUserAttr->Key && LongKey != pOtherUserAttr->LongKey)
	{
		return FALSE;
	}

	// Get a new UserAttribute to hold the blended version, (return FALSE if this fails)
	UserAttribute* pBlendedUserAttr = new UserAttribute;
	if (pBlendedUserAttr == NULL) return FALSE;

	//Graham: For blending two User Attributes with the same Key, we simply
	//use the first User Attribute for the first half of the blend, and the
	//second User Attribute for the second half of the blend.
	StringBase* pStr;	
	if (pBlendParam->GetBlendRatio() < 0.5)
		pStr = &Value;
	else
		pStr = &(pOtherUserAttr->Value);
	
	ERRORIF(!pBlendedUserAttr->Value.Alloc(pStr->Length()), _R(IDE_NOMORE_MEMORY), FALSE);
	pBlendedUserAttr->Value = *pStr;
	
	// Store the ptr to the new blended User width attr val
	pBlendParam->SetBlendedAttrVal(pBlendedUserAttr);
	return TRUE;
}







// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//								A  T  T  R  U  S  E  R
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //






const TCHAR AttrUser::s_KeyDelimiter = TEXT('\\');
const TCHAR s_KeyDelimiter = TEXT('\\');

/********************************************************************************************

>	AttrUser::AttrUser (Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected, TRUE)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> (from Will)
	Created:	14/8/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a User Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrUser::AttrUser(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected)
  : NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{
	// Empty.
} 

/********************************************************************************************

>	AttrUser::AttrUser()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/8/96

	Purpose:	Default constructor for User Attribute class

********************************************************************************************/

AttrUser::AttrUser()
{
}

/********************************************************************************************

>	AttrUser::AttrUser(	const StringBase& NewKey, const StringBase& NewSubKey, 
						const StringBase& NewValue)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/8/96

	Inputs:		
	Purpose:	Constructs a User Attribute node given a Key, SubKey and Value

********************************************************************************************/
AttrUser::AttrUser(	const StringBase& NewKey, const StringBase& NewSubKey, 
					const StringBase& NewValue)
{
	if (!Value.Key.Alloc(NewKey.Length())) return;
	Value.Key = NewKey;

	INT32 n = NewKey.Length();
	if (!NewSubKey.IsEmpty())  n += 1 + NewSubKey.Length();
	
	if (!Value.LongKey.Alloc(n)) return;
	Value.LongKey = NewKey;
	if (!NewSubKey.IsEmpty())
	{
		Value.LongKey += s_KeyDelimiter;
		Value.LongKey += NewSubKey;
	}

	if (!Value.Value.Alloc(NewValue.Length())) return;
	Value.Value = NewValue;
}


/********************************************************************************************

>	AttrUser::AttrUser(const StringBase& NewLongKey)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/97

	Purpose:	Constructs an AttrUser (User attribute node) from a long key

********************************************************************************************/
AttrUser::AttrUser(const StringBase& NewLongKey, const StringBase& NewValue)
{
	// Find the first bit of the long key
	INT32 DelimiterPosition = NewLongKey.FindNextChar(s_KeyDelimiter);
	if (DelimiterPosition < 0)
	{
		// couldn't find the end so the delimiter is beyond the end of the string
		DelimiterPosition = NewLongKey.Length();
	}

	INT32 ShortKeyLength = DelimiterPosition;
	if (ShortKeyLength > 0)
	{
		// Added by Craig Hamilton 2/8/00.
		// This section of code deals with the situation where the length of Value.Key.Length is
		// less than ShortKeyLength. This was causing problems with the call to StringBase::Left
		// which would fail in this eventuality. To prevent this we just allocate Value.Key more
		// space. We did try another method involving the use of a String_256 being passed to 
		// StringBase::Left and then asigning Value.Key to the String_256, but this caused a
		// Damage: after normal block error in the VC++ debug library.
		if(Value.Key.Length() < ShortKeyLength)
		{
			Value.Key.Alloc(ShortKeyLength);
		}
		// End Added.

		NewLongKey.Left(&Value.Key, ShortKeyLength);
	}
	else
	{
		TRACE( _T("AttrUser::AttrUser - no key\n"));
	}

	if (!Value.LongKey.Alloc(NewLongKey.Length())) return;
	Value.LongKey	= NewLongKey;

	if (!Value.Value.Alloc(NewValue.Length())) return;
	Value.Value		= NewValue;
}


/********************************************************************************************

>	void AttrUser::Render( RenderRegion* pRender)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/96

	Purpose:	We don't need to render a user attribute. So simply do nothing.

********************************************************************************************/

void AttrUser::Render(RenderRegion* pRender)
{
	//Don't need to do anything to render a User Attribute.
}

/********************************************************************************************

>	void AttrUser::Transform( TransformBase& Trans )

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	We don't need to transform a User Attribute - so this function does nothing
	SeeAlso:	NodeRenderable::Transform

********************************************************************************************/

void AttrUser::Transform( TransformBase& Trans )
{
	//Transforming a User Attribute does nothing.
}

/***********************************************************************************************
> Node* AttrUser::SimpleCopy() 

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
     
Node* AttrUser::SimpleCopy()
{
	AttrUser* pNodeCopy = new AttrUser;
	ERRORIF(pNodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(pNodeCopy);
	return pNodeCopy;
} 

/********************************************************************************************

>	INT32 AttrUser::operator==(const NodeAttribute& Attrib)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>, from Simon
	Created:	9/2/94
	Inputs:		Atrib:	The attribute to compare, which must be an AttrUser 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 

			  Graham says: with this class you actually need to compare two values (the Value and
				and the Key) not just one.
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrUser 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrUser::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrUser)), 
		"Trying to compare two objects with different types"); 

	AttrUser& OtherUserNode = (AttrUser&) Attrib;
	return !(Value.IsDifferent(&OtherUserNode.Value));
} 


/********************************************************************************************

>	virtual UINT32 AttrUser::GetAttrNameID(void)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> from Simon
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrUser::GetAttrNameID(void)  
{
	return (_R(IDS_USERATTRIBUTE_ID)); 
}               
                 

/***********************************************************************************************
> ` void AttrUser::CopyNodeContents( AttrUser* NodeCopy)

    Author:     Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> from Simon
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrUser::CopyNodeContents( AttrUser* NodeCopy)
{
	NodeAttribute::CopyNodeContents(NodeCopy);
	NodeCopy->Value = Value;
} 



/***********************************************************************************************
>   void AttrUser::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrUser::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrUser), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrUser))
		CopyNodeContents((AttrUser*)pNodeCopy);
}



#ifdef _DEBUG
/***********************************************************************************************

>	void AttrUser::ShowDebugTreeDetails() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Displays debugging info of the tree

***********************************************************************************************/

void AttrUser::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	// For now, we will just call the base class version
	NodeAttribute::ShowDebugTreeDetails();	
}
#endif


void AttrUser::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	*Str += TEXT("\r\nKey = ");
	*Str += Value.Key;
	*Str += TEXT("\r\nLongKey = ");
	*Str += Value.LongKey;
	*Str += TEXT("\r\nData= ");
	*Str += Value.Value;
#endif
}


/********************************************************************************************

>	virtual UINT32 AttrUser::GetNodeSize() const

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> from Simon
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrUser::GetNodeSize() const 
{     
	return sizeof(*this); 
}  


/********************************************************************************************

>	virtual BOOL AttrUser::ShouldBeOptimized()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/06/97

	Purpose:	When applying multiple template args / WizOp user attributes we want them to
				remain associated with the object they were applied to rather than migrating
				to groups, otherwise where there was once two questions for two objects in
				a group there would instead be one.
				The default AttrUser should always be optimized out.
				
	Returns:	TRUE if the attribute should be optimized, normalized, etc.
				FALSE otherwise

	See Also:	NodeRenderableInk::NormaliseAttributes()
				NodeRenderableInk::FindCommonAttributesToFactorOut()
				NodeRenderableInk::MakeAttributeComplete()

********************************************************************************************/
BOOL AttrUser::ShouldBeOptimized()
{
	return Value.Key.IsEmpty() && Value.LongKey.IsEmpty() && Value.Value.IsEmpty();
}  


/********************************************************************************************

>	virtual BOOL AttrUser::Blend(BlendAttrParam* pBlendParam)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> from MarkN
	Created:	14/8/96
	Inputs:		pBlendParam = ptr to blend attribute param object holding all info needed
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Blends one User attribute with another.
				
	SeeAlso:	-

********************************************************************************************/

BOOL AttrUser::Blend(BlendAttrParam* pBlendParam)
{
	// Get the User member to blend to the User member of the other NodeAttribute.
	// If it succeeds, ask the blended Value to make a NodeAttribute out of itself.
	ERROR3IF(pBlendParam == NULL,"NULL entry param");
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
			}
		}
	}
	
	return FALSE;
}



/********************************************************************************************

  >	virtual BOOL AttrUser::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the line width record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrUser::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL ok = TRUE;

	CamelotFileRecord Rec(pFilter,TAG_USERVALUE,TAG_USERVALUEATTRSIZE);

	ok = Rec.Init();

	if (ok) ok = Rec.WriteUnicode((TCHAR*)Value.LongKey);
	if (ok) ok = Rec.WriteUnicode((TCHAR*)Value.Value);

	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrUser::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrUser::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL AttrUser::ShouldBecomeCurrent()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/96
	Returns:	TRUE if this attribute should become current.

				FALSE if the user must deliberately apply this attribute to an object
				every time he wants to use it.

	Purpose:	The attribute manager calls this function before making an attribute current.

				There are some attributes - like this URL Hot Link attribute I'm about
				to put in - that the user probably won't want to become current.
				For example, if a user puts a HotLink on an object and happens to have
				the Give Other Objects Most Recent Attributes option on, all his new objects are
				going to have HotLinks. That's not good.

				So those attributes will override this function and return FALSE.

********************************************************************************************/
BOOL AttrUser::ShouldBecomeCurrent()
{ 
	//No, don't pass this attribute on
	return FALSE; 
}


/********************************************************************************************

>	StringBase& AttrUser::GetWebAddress()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/96

	Returns:	The URL of the Hot Link

	Purpose:	This is only used for Web Address User Attributes.
			
********************************************************************************************/
StringBase& AttrUser::GetWebAddress()
{ 
	return Value.Value;
}


/********************************************************************************************

>	virtual AttributeIdentifier	AttrUser::GetAttributeClassID() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Generates an AttributeIdentifier for this Attribute.
				Since the RuntimeClass is inadequate for dynamically generated Atrributes
				(read Nodes), we need to provide an ID that can be created dynamically too.
				This is what an AttributeIdentifier is supposed to be.
				In the case of an AttrUser it returns a pointer to a template
				handler.

********************************************************************************************/
AttributeIdentifier	AttrUser::GetAttributeClassID() const
{
	return Value.LongKey;
}


/********************************************************************************************

>	VisibleAttribute* AttrUser::CreateVisibleAttribute() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Creates a VisibleAttribute representing this AttrUser that the user can edit.
				(this is all horribly wrong).

	Returns:	A new VIsibleAttribute

********************************************************************************************/
VisibleAttribute* AttrUser::CreateVisibleAttribute() const
{
	VisibleAttribute* pVisibleAttribute = NULL;	// return this

	PORTNOTETRACE("other","AttrUser::CreateVisibleAttribute - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	pVisibleAttribute = new VisibleUserAttribute(Value.LongKey, Value.Value);
#endif
	return pVisibleAttribute;
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//				V  I  S  I  B  L  E  U  S  E  R  A  T  T  R  I  B  U  T  E
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //






/********************************************************************************************

>	VisibleUserAttribute::VisibleUserAttribute(const StringBase& Key, const StringBase& Value)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Represents an AttrUser that the user can edit
				(this is all horribly wrong).

********************************************************************************************/

VisibleUserAttribute::VisibleUserAttribute(const StringBase& Key, const StringBase& Value) :
	m_LongKey(Key),
	m_Value(Value)
{
}



void VisibleUserAttribute::SetLongKey(const StringBase& NewKey)
{
	m_LongKey = NewKey;
}


const StringBase& VisibleUserAttribute::GetKey()
{
	const StringBase& SourceString = (const StringBase&)m_LongKey;

	// Find the first bit of the long key
	INT32 EndDelimiterPosition = SourceString.FindNextChar(s_KeyDelimiter);

	if (EndDelimiterPosition < 0)
	{
		// couldn't find the end so the delimiter is beyond the end of the string
		EndDelimiterPosition = SourceString.Length();
	}

	const UINT32 LengthToCopy = EndDelimiterPosition;
	if (LengthToCopy > 0)
	{
		SourceString.Left(&m_Key, LengthToCopy);
	}
	else
	{
		TRACE( _T("VisibleUserAttribute::GetKey - no key\n"));
	}

	return m_Key;
}

const StringBase& VisibleUserAttribute::GetLongKey()
{
	return m_LongKey;
}

const StringBase& VisibleUserAttribute::GetValue()
{
	return m_Value;
}

/********************************************************************************************

>	virtual NodeAttribute* VisibleUserAttribute::CreateNewAttribute()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Creates a new NodeAttribute from what the user entered.
				(this is all horribly wrong).

********************************************************************************************/
NodeAttribute* VisibleUserAttribute::CreateNewAttribute()
{
	return new AttrUser(GetLongKey(), GetValue());
}


/********************************************************************************************

>	virtual StringBase& GetText(StringBase& Description) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	
	
	Returns:	Some user visible text describing this VisibleUserAttribute

********************************************************************************************/
StringBase& VisibleUserAttribute::GetText(StringBase& Description) const
{
	PORTNOTETRACE("other","VisibleUserAttribute::GetText - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	String_32 TruncatedKey((const StringBase&)m_LongKey);
	Description.MakeMsg(_R(IDS_APPLIED_CUSTOM), TruncatedKey);
#endif
	return Description; 
}


/********************************************************************************************

>	BOOL VisibleUserAttribute::Display(DialogOp& Dialog)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	This allows the user to change this VisibleUserAttribute
	
	Notes:		This function takes a DialopOp as its argument to determine where to
				put this object. Since this requires an internal knowledge of the DialogOp
				at the moment it is cast to a TemplateDialog which has the fields all ready.
				This is likely to change in the future.

********************************************************************************************/
BOOL VisibleUserAttribute::Display(DialogOp& Dialog)
{
	PORTNOTETRACE("other","VisibleUserAttribute::Display - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR3IF(!((&Dialog)->IS_KIND_OF(TemplateDialog)), "Dialog isn't kind of TemplateDialog");

	TemplateDialog& BetterBeThisDialog = (TemplateDialog&)Dialog;
	BOOL Success = TRUE;

	Success = m_LongKey.Display(BetterBeThisDialog.GetKeyControl());
	if (Success)
	{
		Success = m_Value.Display(BetterBeThisDialog.GetValueControl());
	}

	return Success;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	BOOL VisibleUserAttribute::Interact(DialogOp& Dialog)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	This allows the user to change this VisibleUserAttribute
	
	Notes:		This function takes a DialopOp as its argument to determine where to
				put this object. Since this requires an internal knowledge of the DialogOp
				at the moment it is cast to a TemplateDialog which has the fields all ready.
				This is likely to change in the future.

********************************************************************************************/
BOOL VisibleUserAttribute::Interact(DialogOp& Dialog)
{
	PORTNOTETRACE("other","VisibleUserAttribute::Interact - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (!((&Dialog)->IS_KIND_OF(TemplateDialog)))
	{
		ERROR3("Dialog isn't kind of TemplateDialog");
		return FALSE;
	}

	TemplateDialog& BetterBeThisDialog = (TemplateDialog&)Dialog;
	
	if (IsNew())
	{
		BetterBeThisDialog.GetKeyControl().Enable();
	}
	BetterBeThisDialog.GetValueControl().Enable();

	return TRUE;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	void VisibleUserAttribute::Hide()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Hides this VisibleUserAttribute from the user.
	
********************************************************************************************/
void VisibleUserAttribute::Hide()
{
	m_LongKey.Hide();
	m_Value.Hide();
}
