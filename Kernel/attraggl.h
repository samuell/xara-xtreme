// $Id: attraggl.h 662 2006-03-14 21:31:49Z alex $
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
//

#ifndef INC_ATTRAGGL
#define INC_ATTRAGGL



class CommonAttrMultiSet;
class NodeAttribute;
class MultiCommonAttrItem;

//#include <afxtempl.h>

inline BOOL CompareElements(const StringBase** pElement1, const StringBase** pElement2)
{
	const StringBase* const pString1 = *pElement1;
	const StringBase* const pString2 = *pElement2;

	return (*pString1 == *pString2);
}

// This function isn't needed on VC5 or later...
//#if _MSC_VER < 1100
/*inline UINT32 AFXAPI HashKey( const StringBase* pKey )
{
	return HashKey((const TCHAR*)(*pKey));
} */
//#endif

class CommonAttrMap : public CMap<const StringBase*, const StringBase*, MultiCommonAttrItem*, MultiCommonAttrItem*>
{
public:
	CommonAttrMap(INT32 nBlockSize = 10) : CMap<const StringBase*, const StringBase*, MultiCommonAttrItem*, MultiCommonAttrItem*>(nBlockSize) {}


};


class NodePointer : public ListItem
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(NodePointer);
public:
	NodePointer(NodeAttribute& AttributeNode, AttributeIdentifier AttrID) :
		m_AttrID(AttrID),
		m_AttributeNode(AttributeNode)
		{}

	AttributeIdentifier		GetAttrID() const		{	return m_AttrID;	}
	NodeAttribute&			GetNode() const			{	return m_AttributeNode;	}

private:
	AttributeIdentifier	m_AttrID;
	NodeAttribute&		m_AttributeNode;
};


class CCUserAttrMap : public CMap<const StringBase*, const StringBase*, NodeAttribute*, NodeAttribute*>
{
	typedef CMap<const StringBase*, const StringBase*, NodeAttribute*, NodeAttribute*> BaseMap;
public:
	CCUserAttrMap(INT32 nBlockSize = 10) : 
		BaseMap(nBlockSize)
		{}

	virtual ~CCUserAttrMap();
		
	BOOL SetAt(const StringBase* key, NodeAttribute* pNewAttributeNode);
	void RemoveAll();

	NodePointer*	FindFirst() const;
	NodePointer*	FindNext(const NodePointer* const pPrevious) const;

private:
	List	m_NodeAttributes;
};


extern String_8	NonConstNullString;
/********************************************************************************************

>	class RenderableNodeEnumerator : public CC_CLASS_MEMDUMP

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/06/97

	Purpose:	Iterates through a series of NodeRenderable nodes

********************************************************************************************/
class RenderableNodeEnumerator : public CC_CLASS_MEMDUMP
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(RenderableNodeEnumerator);

public:
	virtual ~RenderableNodeEnumerator() { }
	virtual NodeRenderable* FindFirst() const = 0;
	virtual NodeRenderable* FindNext(NodeRenderable* const pPrevious) const = 0;
};



/********************************************************************************************

>	class RangeEnumerator : public RenderableNodeEnumerator

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/06/97

	Purpose:	Iterates through a series of NodeRenderable nodes

********************************************************************************************/
class RangeEnumerator : public RenderableNodeEnumerator
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(RangeEnumerator);

public:
	RangeEnumerator(Range* const pRange) : m_pRange(pRange)	{}

	// Not sure how we get away with this...
	virtual NodeRenderable* FindFirst() const									{	return (NodeRenderable*)m_pRange->FindFirst();	}
	virtual NodeRenderable* FindNext(NodeRenderable* const pPrevious) const	{	return (NodeRenderable*)m_pRange->FindNext((Node*)pPrevious);	}

private:
	Range*	m_pRange;
};

class DocPseudoEnumerator : public RenderableNodeEnumerator
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(DocPseudoEnumerator);

public:
	DocPseudoEnumerator(Document* const pDocument) : m_pDocument(pDocument)	{}

	// Not sure how we get away with this...
	virtual NodeRenderable* FindFirst() const;
	virtual NodeRenderable* FindNext(NodeRenderable* const pPrevious)	 const	{	return NULL;	}

private:
	Document*		m_pDocument;
};


/********************************************************************************************

>	class AttributeAgglomerator : public CC_CLASS_MEMDUMP

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/06/97

	Purpose:	An AttributeAgglomerator finds the Common Attributes among the nodes
				provided by an RenderableNodeEnumerator.
				This allows us to derive classes to retrieve more specialized attributes,
				namely AttrUser's.
	Notes:		This code was copied from Range. It should either replace the code there
				or be thrown out when plug-in attributes are supported.

********************************************************************************************/
class AttributeAgglomerator : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(AttributeAgglomerator);

public:
	virtual ~AttributeAgglomerator() { }
	BOOL FindCommonAttributes(	RenderableNodeEnumerator& Enumerator, 
								CommonAttrMultiSet* const pCommonAttributeSet);

	static const UINT32 MaxAttributes;

protected:
	virtual AttributeIdentifier GetAttributeType(NodeAttribute* const pAttrib) const = 0;
	virtual BOOL FindAppliedAttributes(Node* const pNode, CCUserAttrMap* const pAttribMap) const;

};



/********************************************************************************************

>	class UserAttributeAgglomerator : public AttributeAgglomerator

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/06/97

	Purpose:	An AttributeAgglomerator finds the Common Attributes among the nodes
				provided by an RenderableNodeEnumerator.
				This allows us to derive classes to retrieve more specialized attributes,
				namely AttrUser's.

********************************************************************************************/
class UserAttributeAgglomerator : public AttributeAgglomerator
{
	CC_DECLARE_MEMDUMP(UserAttributeAgglomerator);

public:

protected:
	virtual AttributeIdentifier GetAttributeType(NodeAttribute* const pAttrib) const;
};



/********************************************************************************************

>	class AppliedAttribute : public VisibleListItem

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/07/97

	Purpose:	At the moment it allows applied attributes to appear in a list, but will 
				probably supersede the MultiCommonAttrItem class.

********************************************************************************************/
class AppliedAttribute : public VisibleListItem
{
	CC_DECLARE_MEMDUMP(AppliedAttribute);
public:
//	Don't forget to supply these
//	virtual BOOL Display(DialogOp& Dialog) = 0;
//	virtual void Hide() = 0;
//	virtual StringBase& GetText(StringBase& Description) const = 0;
	virtual BOOL operator > (const VisibleListItem& OtherItem) const;
	virtual AttributeIdentifier GetAttrID()	const = 0;
	virtual BOOL ApplyAttribute(Operation& OpToApplyWith) = 0;
	virtual VisibleAttribute* GetVisibleAttribute() = 0;
};


/********************************************************************************************

>	class SingletonAppliedAttribute : public AppliedAttribute

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/07/97

	Purpose:	Represents an applied attribute of which there is only one for the objects
				given by the iterator.

********************************************************************************************/
class SingletonAppliedAttribute : public AppliedAttribute
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(SingletonAppliedAttribute);

public:
	SingletonAppliedAttribute(NodeAttribute& TheAttribute);
	SingletonAppliedAttribute(VisibleAttribute& TheVisibleAttribute);
	SingletonAppliedAttribute(const SingletonAppliedAttribute& Other) : m_AttrID(NonConstNullString)
		{	TRACE( wxT("SingletonAppliedAttribute copy constructor not implemented\n") );	}

	virtual ~SingletonAppliedAttribute();

// Implemented pure functions
//	from UserInterface...
	virtual BOOL Display(DialogOp& Dialog);
//	virtual BOOL Interact(DialogOp& Dialog);	// well it should be in UserInterface
	virtual void Hide();

//	from VisibleListItem...
	virtual StringBase& GetText(StringBase& Description) const;

//	from AppliedAttribute...
	virtual AttributeIdentifier GetAttrID()	const				{	return m_AttrID;	}
	virtual BOOL ApplyAttribute(Operation& OpToApplyWith);
	virtual VisibleAttribute* GetVisibleAttribute()	{	return m_pVisibleAttribute;	}

private:
	// should be a reference...
	VisibleAttribute*	m_pVisibleAttribute;
	String_128			m_AttrID;
	DialogOp*			m_pDialog;
};


/********************************************************************************************

>	class CommonAppliedAttribute : public AppliedAttribute

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/07/97

	Purpose:	Represents an applied attribute of which there is only one for the objects
				given by the iterator.

********************************************************************************************/
class CommonAppliedAttribute : public AppliedAttribute
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(CommonAppliedAttribute);

public:
	CommonAppliedAttribute(NodeAttribute& AttrInstance);
	virtual ~CommonAppliedAttribute();
	CommonAppliedAttribute(const CommonAppliedAttribute& Other) : m_AttrID(NullString)
		{ TRACE( wxT("CommonAppliedAttribute copy constructor not implemented\n") ); }

// Implemented pure functions
//	from UserInterface...
	virtual BOOL Display(DialogOp& Dialog);
	virtual void Hide();

//	from VisibleListItem...
	virtual StringBase& GetText(StringBase& Description) const;

//	from AppliedAttribute...
	virtual AttributeIdentifier GetAttrID()	const				{	return m_AttrID;	}
	virtual BOOL ApplyAttribute(Operation& OpToApplyWith);
	virtual VisibleAttribute* GetVisibleAttribute()				{	return m_pVisibleAttribute;	}

private:
	// should be a reference...
	VisibleAttribute*	m_pVisibleAttribute;
	AttributeIdentifier	m_AttrID;
};


/********************************************************************************************

>	class MultiAppliedAttribute : public AppliedAttribute

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/07/97

	Purpose:	Represents an applied attribute of which there is only one for the objects
				given by the iterator.

********************************************************************************************/
class MultiAppliedAttribute : public AppliedAttribute
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(MultiAppliedAttribute);

public:
	MultiAppliedAttribute(NodeAttribute& AttrInstance);
// Implemented pure functions
//	from UserInterface...
	virtual BOOL Display(DialogOp& Dialog);
	virtual void Hide();

//	from VisibleListItem...
	virtual StringBase& GetText(StringBase& Description) const;

//	from AppliedAttribute...
	virtual AttributeIdentifier GetAttrID()	const			{	return m_AttrID;	}
	virtual BOOL ApplyAttribute(Operation& OpToApplyWith)	{	return TRUE;	}	// DON'T DO IT!
	virtual VisibleAttribute* GetVisibleAttribute()			{	return NULL;	}

protected:
	const StringBase& GetAttributeDescription() const;

private:
	String_64			m_AttributeDescription;
	AttributeIdentifier	m_AttrID;
};


/********************************************************************************************

>	class MultiCommonAttrItem: public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94

	Purpose:	This ListItem is used by Range::FindCommonAttributes.

********************************************************************************************/
class MultiCommonAttrItem : public ListItem
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(MultiCommonAttrItem);
 
public:
	MultiCommonAttrItem(AttributeIdentifier AttrType, NodeAttribute* const pAttr = NULL);
	~MultiCommonAttrItem();  

	void ClearResults();

	enum CAResult {		AA_INVALID = 0,		// indicates uninitialized data
						AA_NONE = 1, 
						AA_SINGLE,
						AA_COMMON,
						AA_MANY
					};

	AttributeIdentifier			GetAttrID() const		{	return m_AttrType;	}
	const CAResult&				GetStatus() const		{	return m_Status;	}
	NodeAttribute*				GetNode() const			{	return m_pAttr;	}

	void SetStatus(const CAResult& Status)				{	m_Status = Status;	}
	void SetNode(NodeAttribute* const pAttr)			{	m_pAttr = pAttr;	}

	AppliedAttribute* CreateVisibleAppliedAttribute() const;	// don't do it like this

private: 		
	CAResult					m_Status;
	AttributeIdentifier			m_AttrType;		// The RuntimeClass of the attribute
	NodeAttribute*				m_pAttr;		// A pointer to an instance of an attribute with AttrType
												// Runtime class - can be NULL
	BOOL						m_AttrIsCopy;	// Is pAttr a pointer to an attribute, or a copy. If it is a 
												// copy then it is the responsibility of the MultiCommonAttrItem's
												// destructor to destroy it.
};



/********************************************************************************************

>	class CommonAttrMultiSet : public CommonAttrMap

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Simon)
	Created:	13/9/95

	Purpose:	A CommonAttributeSet. See Range::FindCommonAttributes 

	SeeAlso:	MultiCommonAttrItem

********************************************************************************************/
class CommonAttrMultiSet : public CommonAttrMap
{
public:
	virtual ~CommonAttrMultiSet();

	void ClearResults();

	// Methods for setting up the CommonAttrMultiSet prior to a call to FindCommonAttributes
	virtual BOOL AddTypeToSet(AttributeIdentifier AttrType, NodeAttribute* const pAttr);

	// Methods for accessing the CommonAttrMultiSet 
	MultiCommonAttrItem*	FindAttrItem(AttributeIdentifier AttrType);
	MultiCommonAttrItem*	FindFirst() const;
	MultiCommonAttrItem*	FindNext(const MultiCommonAttrItem* const pPrevious) const;

protected:
private:
	List	m_CommonAttrList;
};


#endif	// INC_ATTRAGGL
