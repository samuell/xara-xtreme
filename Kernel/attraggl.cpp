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
// 


#include "camtypes.h"
#include "list.h"
#include "range.h"
#include "dialogop.h"
#include "gadget.h"
#include "uielem.h"
#include "nodeattr.h"		// for AttributeIdentifier
#include "attraggl.h"

#include "attrappl.h"
#include "ink.h"
#include "nodeattr.h"

#include "visiattr.h"
#include "tmpltdlg.h"

//#include "tmpltres.h"
//#include "richard2.h"		// for _R(IDS_ERRORS_UNKNOWN)

#include "document.h"
#include "spread.h"
#include "chapter.h"
#include "nodedoc.h"


// Place any IMPLEMENT type statements here
CC_IMPLEMENT_MEMDUMP(AttributeAgglomerator, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(UserAttributeAgglomerator, AttributeAgglomerator)
CC_IMPLEMENT_MEMDUMP(RenderableNodeEnumerator, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(RangeEnumerator, RenderableNodeEnumerator);
CC_IMPLEMENT_MEMDUMP(DocPseudoEnumerator, RenderableNodeEnumerator);
CC_IMPLEMENT_MEMDUMP(AppliedAttribute, VisibleListItem);
CC_IMPLEMENT_MEMDUMP(SingletonAppliedAttribute, AppliedAttribute)
CC_IMPLEMENT_MEMDUMP(CommonAppliedAttribute, AppliedAttribute)
CC_IMPLEMENT_MEMDUMP(MultiAppliedAttribute, AppliedAttribute)
CC_IMPLEMENT_MEMDUMP(MultiCommonAttrItem, ListItem);
CC_IMPLEMENT_MEMDUMP(NodePointer, ListItem);


// We want better memory tracking
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


// Functions follow
#undef ENSURE_NOT_NULL
#define ENSURE_NOT_NULL(Pointer)		ERROR2IF(Pointer == NULL, 0, "NULL Args")
#define ENSURE_KIND(pNode, Kind)		{if (pNode != NULL) {ERROR3IF(!pNode->IS_KIND_OF(Kind), "pNode is not kind of " #Kind);}}

String_8				NonConstNullString = TEXT("");


// Should be in OIL
CCUserAttrMap::~CCUserAttrMap()
{
	m_NodeAttributes.DeleteAll();
}
		
BOOL CCUserAttrMap::SetAt(const StringBase* pKey, NodeAttribute* pNewAttributeNode)
{
	ENSURE_NOT_NULL(pKey);
	ENSURE_NOT_NULL(pNewAttributeNode);

	BOOL Ok = TRUE;

	try
	{
		BaseMap::SetAt(pKey, pNewAttributeNode);
	}
	catch (...)
	{
		Ok = FALSE;
	}		

	if (Ok)
	{
		NodePointer* const pNewItem = new NodePointer(*pNewAttributeNode, *pKey);
		if (pNewItem != NULL)
		{
			m_NodeAttributes.AddTail(pNewItem);
		}
		else
		{
			Ok = FALSE;
		}
	}

	return Ok;
}

NodePointer* CCUserAttrMap::FindFirst() const
{
	NodePointer* const pNodePointer = (NodePointer*)m_NodeAttributes.GetHead();
	ENSURE_KIND(pNodePointer, NodePointer);

	return pNodePointer;
}

NodePointer* CCUserAttrMap::FindNext(const NodePointer* const pPrevious) const
{
	NodePointer* const pNodePointer = (NodePointer*)m_NodeAttributes.GetNext(pPrevious);
	ENSURE_KIND(pNodePointer, NodePointer);

	return pNodePointer;
}


void CCUserAttrMap::RemoveAll()
{
	BaseMap::RemoveAll();

	m_NodeAttributes.DeleteAll();
}




NodeRenderable* DocPseudoEnumerator::FindFirst() const
{
	ASSERT(m_pDocument != NULL);

	Spread* const pSpread = m_pDocument->FindFirstSpread();
	ASSERT(pSpread != NULL);
	
	const Chapter* const pChapter = pSpread->FindParentChapter();
	ASSERT(pChapter != NULL);

	const NodeDocument* const pDocNode = (NodeDocument*)pChapter->FindParent();
	ASSERT(pDocNode != NULL);
	ENSURE_KIND(pDocNode, NodeDocument);

	return (NodeRenderable*)pDocNode;
}

const UINT32 AttributeAgglomerator::MaxAttributes = 5000;

/********************************************************************************************

>	BOOL AttributeAgglomerator::FindCommonAttributes(RenderableNodeEnumerator& Enumerator, 
											CommonAttrMultiSet* const pCommonAttributeSet)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Simon)
	Created:	30/06/97

	Purpose:	At present, allows the Wizard Properties dialog to reflect whether attributes
				are applied to the selection or a node when no selection (the spread).

	Inputs:		Enumerator:	An iterator of all the objects for which common attributes are
							to be found.
				
	Outputs:	pCommonAttributeSet:	The set contains a MultiCommonAttrItem for every 
										attribute type applied to objects enumerated by the
										iterator.

				Each item will have a status value returned by GetStatus():

				AA_NONE:	This case should not occur for the base class
							pAttr will be NULL

				AA_SINGLE:	There is a single instance of the attribute in the iterator's set
							pAttr will point to it 

				AA_COMMON:	There is a common attribute
							pAttr will point to an instance of it 
					 
				AA_MANY:	The attributes in the iterator's set have multiple values for the
							attribute class
							pAttr will be NULL

	Returns:	TRUE if successful, 
				FALSE otherwise

********************************************************************************************/
BOOL AttributeAgglomerator::FindCommonAttributes(RenderableNodeEnumerator& Enumerator, 
											CommonAttrMultiSet* const pCommonAttributeSet)
{
	ERROR2IF(pCommonAttributeSet == NULL, FALSE, "NULL Args"); 

	BOOL Success = TRUE;

	// For each object:
	//	Build list of applied attributes
	//	Lookup each item in the current set of common attributes
	//		If not in set, add to set as single
	//		If attribute in set
	//			If set entry marked multiple ignore attribute
	//			Otherwise compare values
	//				If values differ, mark multiple
	//				If values same, mark common

	
	// Initialise all CommonAttributeItems
	pCommonAttributeSet->ClearResults();
	
	// Create a map of Attribute types to applied attrs. This allows us to scan up the tree 
	// looking for attributes applied to each object once only.
	CCUserAttrMap* const pAttrMap = new CCUserAttrMap;
	if (pAttrMap == NULL)
	{
		return FALSE; // out of memory
	}

	// Scan all objects in the range,
	NodeRenderable* CurrentObject = Enumerator.FindFirst();
	// Stop when we have found all attribute values (all ATTR_MANY), or there are no more objects
	// to process.
	while (CurrentObject != NULL && Success)
	{
		// Remove any items that may have been in the map
		pAttrMap->RemoveAll();

		// and build list of applied attributes
		FindAppliedAttributes(CurrentObject, pAttrMap);

		//	Lookup each item in the current set of common attributes
		NodePointer* pNodePointer = pAttrMap->FindFirst();
		while (pNodePointer != NULL)
		{
			MultiCommonAttrItem* pCommonAttrItem = NULL;
			pCommonAttributeSet->Lookup(&(pNodePointer->GetAttrID()), pCommonAttrItem);

			AttributeIdentifier AttrID(pNodePointer->GetAttrID());
			NodeAttribute* const pCurrentObjectAttr = &(pNodePointer->GetNode());
			// Was the attribute in the common attribute set?
			if (pCommonAttrItem == NULL)
			{
				//	No, it's the first occurence of the attribute so mark it as single
				ERROR3IF(GetAttributeType(pCurrentObjectAttr) != AttrID, "GetAttributeType(pCurrentObjectAttr) != AttrID");

				pCommonAttributeSet->AddTypeToSet(AttrID, pCurrentObjectAttr);
			}
			else	// entry in set exists
			{
				// If set entry marked multiple ignore attribute
				if (pCommonAttrItem->GetStatus() != MultiCommonAttrItem::AA_MANY)
				{
					// Otherwise compare values
					NodeAttribute* const pCommonAttribute = pCommonAttrItem->GetNode();
					if (pCommonAttribute != NULL)
					{

						if ((*pCurrentObjectAttr) == (*pCommonAttribute))
						{
							// The attribute becomes a common attribute
							pCommonAttrItem->SetStatus(MultiCommonAttrItem::AA_COMMON);  // up to this point anyway
						}
						else	// If values differ, mark multiple
						{
							pCommonAttrItem->SetStatus(MultiCommonAttrItem::AA_MANY);
							// Hang on to an instance of the attribute so we can get a
							// description of it.
						}
					}
					else
					{
						ERROR3("CommonAttrItem->GetNode() == NULL");
					}
				}	// else ignore attribute
			}

			pNodePointer = pAttrMap->FindNext(pNodePointer);

		} // Attribute scan

		// Get the next object
		CurrentObject = Enumerator.FindNext(CurrentObject);
	} // range scan


	delete pAttrMap;

	return Success;
}


/********************************************************************************************

>	BOOL AttributeAgglomerator::FindAppliedAttributes(	const Node* const pNode, 
														CCUserAttrMap* const pAttribMap) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Phil)
	Created:	13/06/97

	Purpose:	Searches attributes with non-NULL AttributeIdentifier's which are local to 
				the given Node.

	Inputs:		pNode:		The node whose attributes you want
	Outputs:	pAttribMap	This CCUserAttrMap is filled with tuples 
							(AttributeIdentifier, NodeAttribute*) for the locally applied
							attributes (those whose parent is *pNode)
	
	Returns:	TRUE if any attributes were found
				FALSE otherwise (if no attributes were found)


********************************************************************************************/
BOOL AttributeAgglomerator::FindAppliedAttributes(Node* const pNode, CCUserAttrMap* const pAttribMap) const
{
	// Precondition checks
	ERROR2IF(pNode == NULL || pAttribMap == NULL, FALSE, "NULL Args");

	BOOL AttributesFound = FALSE;
	// Find the closest attribute applied to "this" node...
	NodeAttribute* pAttrib = NodeAttribute::FindFirstAppliedAttr(pNode);

	// Repeat the following loop for all attributes we encounter in the tree.
	while (pAttrib != NULL && pAttrib->FindParent() == pNode)
	{
		// Get the run-time type of the attribute, which serves as a hash "key".
		AttributeIdentifier AttrID = GetAttributeType(pAttrib);

		// if the AttrID is NULL we're not interested
		if (!AttrID.IsEmpty())
		{
			// Check if this type is already in the hash table.  If it isn't then insert
			// its address with its run-time type as the key, and check if we need to look
			// for any more.
			NodeAttribute* pDummy = NULL;
			if (!pAttribMap->Lookup(&AttrID, pDummy))
			{
				// This is a new attribute for the map
				pAttribMap->SetAt(&AttrID, pAttrib);
				AttributesFound = TRUE;
			}
			else
			{
				ERROR3("AttributeAgglomerator::FindAppliedAttributes - Duplicate attribute found");
			}
		}

		// Go on to find the next attribute.
		pAttrib = NodeAttribute::FindPrevAppliedAttr(pAttrib);
	}

	return AttributesFound;
}



/********************************************************************************************

>	MultiCommonAttrItem::MultiCommonAttrItem(	AttributeIdentifier AttrType, 
												const NodeAttribute* const pAttr)
	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Simon)
	Created:	22/9/95

	Purpose:	MultiCommonAttrItem constructor

********************************************************************************************/
MultiCommonAttrItem::MultiCommonAttrItem(	AttributeIdentifier AttrType, 
											NodeAttribute* const pAttr) :
	m_AttrType(AttrType)
{
	m_pAttr			= pAttr;
	m_Status		= (pAttr == NULL) ? AA_NONE : AA_SINGLE;
	m_AttrIsCopy	= FALSE;
}


/********************************************************************************************

>	MultiCommonAttrItem::~MultiCommonAttrItem()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Purpose:	MultiCommonAttrItem destructor

********************************************************************************************/
MultiCommonAttrItem::~MultiCommonAttrItem()  
{
	if (m_AttrIsCopy && m_pAttr != NULL)
	{
		delete m_pAttr;
		m_pAttr = NULL;
	}
}
											   

/********************************************************************************************

>	void MultiCommonAttrItem::ClearResults()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95

	Purpose:	???

********************************************************************************************/
void MultiCommonAttrItem::ClearResults()
{
	if (m_AttrIsCopy && m_pAttr != NULL)
	{
	 	delete m_pAttr;
		m_pAttr = NULL;
	}
	m_pAttr = NULL; 
	m_Status = AA_NONE;
}


/********************************************************************************************

>	AppliedAttribute* MultiCommonAttrItem::CreateVisibleAppliedAttribute() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/07/97

	Purpose:	This will be superseded (replace this class with the AppliedAttribute class)

********************************************************************************************/
AppliedAttribute* MultiCommonAttrItem::CreateVisibleAppliedAttribute() const
{
	AppliedAttribute* pAppliedAttribute = NULL;

	switch (GetStatus())
	{
		case MultiCommonAttrItem::AA_NONE:
			TRACE( wxT("MultiCommonAttrItem::CreateVisibleAppliedAttribute() - MultiCommonAttrItem::AA_NONE\n") );
			break;

		case MultiCommonAttrItem::AA_SINGLE:
		{
			NodeAttribute* const pAttr = GetNode();
			ENSURE_NOT_NULL(pAttr);
			pAppliedAttribute = new SingletonAppliedAttribute(*pAttr);
			break;
		}
		case MultiCommonAttrItem::AA_COMMON:
		{
			NodeAttribute* const pAttr = GetNode();
			ENSURE_NOT_NULL(pAttr);
			pAppliedAttribute = new CommonAppliedAttribute(*pAttr);
			break;
		}
		case MultiCommonAttrItem::AA_MANY:
		{
			// There's more than one, they're all different, but we have one for the 
			// description
			NodeAttribute* const pAttr = GetNode();
			ENSURE_NOT_NULL(pAttr);
			pAppliedAttribute = new MultiAppliedAttribute(*pAttr);
			break;
		}
		case MultiCommonAttrItem::AA_INVALID:
			ERROR3("AA_INVALID received - why? Alex put this in to suppress compiler warning");
			break;
	}

	return pAppliedAttribute;
}


/********************************************************************************************

>	virtual CommonAttrMultiSet::~CommonAttrMultiSet()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/97

	Purpose:	The MultiCommonAttrItems in the set need to be deleted when the set is 
				destroyed. This destructor ensures they are.

********************************************************************************************/
CommonAttrMultiSet::~CommonAttrMultiSet()
{
	m_CommonAttrList.DeleteAll();
}


/********************************************************************************************

>	BOOL CommonAttrMultiSet::AddTypeToSet(	AttributeIdentifier AttrType, 
											NodeAttribute* const pAttr)
	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/97

	Purpose:	Call this function to build up a subset of attribute types to find common
				attribute values for. It creates an MultiCommonAttrItem and adds it to the set.

	Inputs:		AttrType: The type to add to the set

	Returns:	FALSE if we run out of memory

********************************************************************************************/
BOOL CommonAttrMultiSet::AddTypeToSet(	AttributeIdentifier AttrType, 
										NodeAttribute* const pAttr)
{
	ENSURE_NOT_NULL(pAttr);
	ERROR3IF(FindAttrItem(AttrType), "Trying to add a duplicate item to a CommonAttrMultiSet"); 

	BOOL Ok = TRUE;

	MultiCommonAttrItem* pCommonAttrItem = new MultiCommonAttrItem(AttrType, pAttr);
	if (pCommonAttrItem == NULL)
	{
		// Oh no we've run out of memory
		Ok = FALSE; 
	}
	if (Ok)
	{
		try
		{
			SetAt(&AttrType, pCommonAttrItem);
		}
		catch(...)
		{
			Ok = FALSE;
		}
	}

	if (Ok)
	{
		m_CommonAttrList.AddTail(pCommonAttrItem);
	}

	if (!Ok)
	{
		delete pCommonAttrItem;
		pCommonAttrItem = NULL;
	}

	return Ok;
}



MultiCommonAttrItem* CommonAttrMultiSet::FindFirst() const
{
	MultiCommonAttrItem* pFoundItem = (MultiCommonAttrItem*)m_CommonAttrList.GetHead();
	ENSURE_KIND(pFoundItem, MultiCommonAttrItem);
	
	return pFoundItem;
}


MultiCommonAttrItem* CommonAttrMultiSet::FindNext(const MultiCommonAttrItem* const pPrevious) const
{
	MultiCommonAttrItem* pFoundItem = (MultiCommonAttrItem*)m_CommonAttrList.GetNext(pPrevious);
	ENSURE_KIND(pFoundItem, MultiCommonAttrItem);
	
	return pFoundItem;
}

/********************************************************************************************

>	MultiCommonAttrItem* CommonAttrMultiSet::FindAttrItem(AttributeIdentifier AttrType)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/95

	Purpose:	To find an item in the CommonAttrMultiSet

	Inputs:		AttrType: The attribute type to search for
	Returns:	A pointer to the MultiCommonAttrItem with type AttrType.
				NULL if the item does not exist.

********************************************************************************************/
MultiCommonAttrItem* CommonAttrMultiSet::FindAttrItem(AttributeIdentifier AttrType)
{
	MultiCommonAttrItem* pAttrItem = NULL;

	if (!Lookup(&AttrType, pAttrItem))
	{
		pAttrItem = NULL;
	}

	return pAttrItem;
}




/********************************************************************************************

>	void CommonAttrMultiSet::ClearResults();  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95

	Purpose:	This function initialises the CommonAttrMultiSet ready for finding common attributes
				Each MultiCommonAttrItem in the set has its Status set to ATTR_NONE and its 
				pAttr set to NULL. You will probably never need to call this function as
				Range::FindCommonAttributes makes a call to it prior to looking for
				common attributes. 

	Errors:		The CommonAttrMultiSet must not be empty
				The AttrType field in each MultiCommonAttrItem must be a valid attribute type
	SeeAlso:	Range::FindCommonAttributes

********************************************************************************************/
void CommonAttrMultiSet::ClearResults()
{
	MultiCommonAttrItem* pAttrItem = FindFirst();
	while (pAttrItem)
	{
		ERROR3IF(!pAttrItem->GetAttrID(), "CommonAttrMultiSet contains invalid attr type"); 
		pAttrItem->ClearResults(); 

		pAttrItem = FindNext(pAttrItem);
	}
}











/********************************************************************************************

>	virtual AttributeIdentifier UserAttributeAgglomerator::GetAttributeType(
														NodeAttribute* const pAttrib) const
	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/07/97

	Purpose:	Provides a key for the given NodeAttribute that distinguishes the attribute
				from other other than by the runtime class.

	Returns:	An AttributeIdentifier from GetAttributeClassID()

********************************************************************************************/
AttributeIdentifier UserAttributeAgglomerator::GetAttributeType(NodeAttribute* const pAttrib) const
{
	ERROR2IF(pAttrib == NULL, NullString, "NULL Args");
	
	AttributeIdentifier AttrID = pAttrib->GetAttributeClassID();			// return this

	return AttrID;
}





/*
	Some thoughts:

	I	Register new nodes a la
			
			MyNodeID = AttrMgr.RegisterNode(StringDescribingNodeClass);




	II	Replace MultiCommonAttrItem with AppliedAttribute (see below)

			// If set entry marked multiple ignore attribute
			if (pCommonAttrItem->NumberOfInstances() > 1)
			{
				// Otherwise compare values
				NodeAttribute* const pCommonAttribute = pCommonAttrItem->GetNode();
				if (pCommonAttribute != NULL)
				{

					delete pCommonAttrItem;
					if ((*pCurrentObjectAttr) == (*pCommonAttribute))
					{
						SetAt( , new CommonAppliedAttribute(pAttrInstance);
					}
					else	// If values differ, mark multiple
					{
						SetAt( , new MultiAppliedAttribute(pAttrInstance);
					}
				}
				else
				{
					ERROR3("CommonAttrItem->GetNode() == NULL");
				}
			}	// else ignore attribute
*/








// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//						A  P  P  L  I  E  D  A  T  T  R  I  B  U  T  E
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //





/********************************************************************************************

>	virtual BOOL AppliedAttribute::operator > (const VisibleListItem& OtherItem) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Makes Applied Attributes appear in the order in which they're applied

********************************************************************************************/
BOOL AppliedAttribute::operator > (const VisibleListItem& OtherItem) const
{
	return FALSE;
}








// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//			S  I  N  G  L  E  T  O  N  A  P  P  L  I  E  D  A  T  T  R  I  B  U  T  E
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //





/********************************************************************************************

>	SingletonAppliedAttribute::SingletonAppliedAttribute(NodeAttribute& TheAttribute)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	This constructor tries to create a UserInterface in the form of a 
				VisibleUserAttribute so the user can alter the attributes' contents.
	
	Notes:		If this constructor fails there will be no user interface for the 
				NodeAttribute.

********************************************************************************************/
SingletonAppliedAttribute::SingletonAppliedAttribute(NodeAttribute& TheAttribute) :
	m_AttrID(TheAttribute.GetAttributeClassID())
{
	
	m_pVisibleAttribute	= TheAttribute.CreateVisibleAttribute();
	if (m_pVisibleAttribute != NULL)
	{
		m_pVisibleAttribute->MarkAsUsed();
	}
	m_pDialog = NULL;
}


/********************************************************************************************

>	SingletonAppliedAttribute::SingletonAppliedAttribute(VisibleAttribute& TheVisibleAttribute)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	This constructor is used for new VisibleAttributes (i.e., ones that haven't
				yet been applied)

	Inputs:		The VisibleAttribute that is the one and only attribute with the associated
				AttributeIdentifier (from GetAttributeClassID()) applied to the node (or will
				be)

	Notes:		If this constructor fails there will be no user interface for the 
				VisibleAttribute.

********************************************************************************************/
SingletonAppliedAttribute::SingletonAppliedAttribute(VisibleAttribute& TheVisibleAttribute) :
	m_AttrID(NullString)
{
	m_pVisibleAttribute = &TheVisibleAttribute;

	// Create us a quick NodeAttribute so we can get the ClassID thing
	NodeAttribute* pAttrib = m_pVisibleAttribute->CreateNewAttribute();

	if (pAttrib != NULL)
	{
		m_AttrID = pAttrib->GetAttributeClassID();
		delete pAttrib;
	}
	m_pDialog = NULL;
}


/********************************************************************************************

>	SingletonAppliedAttribute::~SingletonAppliedAttribute()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Destructor removes any internal heap muck

********************************************************************************************/
SingletonAppliedAttribute::~SingletonAppliedAttribute()
{
	delete m_pVisibleAttribute;
	m_pVisibleAttribute = NULL;
}


/********************************************************************************************

>	BOOL SingletonAppliedAttribute::Display(DialogOp& Dialog)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	This SingletonAppliedAttribute appears in the Wizard Properties dialog as
				a "Used Property". When selected it displays its innards.
	Inputs:		Dialog:	The dialog on which to display this SingletonAppliedAttribute
	Returns:	TRUE if it succeeded in doing what it wanted, FALSE if it didn't

	Notes:		The given DialogOp must be derived from TemplateDialog.
				SingletonAppliedAttribute delegates the work to the VisibleAttribute that
				is the singleton in the selection.

********************************************************************************************/
BOOL SingletonAppliedAttribute::Display(DialogOp& Dialog)
{
PORTNOTETRACE("dialog","SingletonAppliedAttribute::Display - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	BOOL Ok = TRUE;

	if (m_pVisibleAttribute != NULL)
	{
		m_pVisibleAttribute->Display(Dialog);

		// god, it's another bodge...it will get sorted, honest!
		// if the user changes the text then we'll enable the apply button 
		// but we need to make sure we don't interpret our own internal changes
		// We disable the message processing (in TemplateDialog) in Hide()
		m_pDialog = &Dialog;

		ENSURE_KIND((&Dialog), TemplateDialog);

		TemplateDialog& BetterBeThisDialog = (TemplateDialog&)Dialog;
		BetterBeThisDialog.SetUserCanModifyQuestion(TRUE);

		Ok = m_pVisibleAttribute->Interact(Dialog);
	}
	else
	{
		TRACE(_T("SingletonAppliedAttribute::Display() : NULL Members"));
		Ok = FALSE;
	}

	return Ok;
#endif
	return FALSE;
}


/********************************************************************************************

>	virtual void SingletonAppliedAttribute::Hide()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	This SingletonAppliedAttribute appears in the Wizard Properties dialog as
				a "Used Property". When de-selected it hides what it was displaying and makes
				any updates to the actual VisibleAttribute.

	Notes:		SingletonAppliedAttribute delegates the work to the VisibleAttribute that
				is the singleton in the selection.

********************************************************************************************/
void SingletonAppliedAttribute::Hide()
{
PORTNOTETRACE("dialog","SingletonAppliedAttribute::Hide - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (m_pDialog != NULL && m_pVisibleAttribute != NULL)
	{
		ENSURE_KIND(m_pDialog, TemplateDialog);

		// Disable the processing of TEXT_CHANGED message (in TemplateDialog)
		// otherwise we end up processing our own changes
		TemplateDialog* pBetterBeThisDialog = (TemplateDialog*)m_pDialog;
		pBetterBeThisDialog->SetUserCanModifyQuestion(FALSE);

		m_pVisibleAttribute->Hide();

		// we'll say we're not on a dialog any more so we can ignore further requests
		m_pDialog = NULL;
	}
	else
	{
		TRACE(_T("SingletonAppliedAttribute::Hide() : NULL Members"));
	}
#endif
}


/********************************************************************************************

>	StringBase& SingletonAppliedAttribute::GetText(StringBase& Description) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Provides the text that goes in any UI.

	Notes:		The Wizard Properties dialog uses this text in the "Used Property" list.
				SingletonAppliedAttribute gets the text from the actual VisibleAttribute that
				is the Singleton in the selection.

********************************************************************************************/
StringBase& SingletonAppliedAttribute::GetText(StringBase& Description) const
{
	if (m_pVisibleAttribute != NULL)
	{
		return m_pVisibleAttribute->GetText(Description);
	}
	else
	{
		return Description = String_256(_R(IDS_ERRORS_UNKNOWN));
	}
}


/********************************************************************************************

>	virtual BOOL SingletonAppliedAttribute::ApplyAttribute(Operation& OpToApplyWith)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	This is called when the AppliedAttribute should apply itself to the
				selection.

	Inputs:		OpToApplyWith:	This is quite useless 'cos we need to apply an op to an
								object. Unfortunately there's no hierarchy in the OpParam
								class.

	Notes:		Always uses OpApplyAttribToSelected

********************************************************************************************/
BOOL SingletonAppliedAttribute::ApplyAttribute(Operation& OpToApplyWith)
{
	BOOL Ok = TRUE;

	if (GetVisibleAttribute() != NULL)
	{
		NodeAttribute* pAttrib = GetVisibleAttribute()->CreateNewAttribute();
		Ok = (pAttrib != NULL);

		if (Ok)
		{
			// Obtain a pointer to the op descriptor for the attribute operation 
			OpDescriptor* const pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpApplyAttribToSelected));
			ENSURE_NOT_NULL(pOpDesc);

			// Invoke the operation, passing Attrib as a parameter
			OpParam		Param( pAttrib, TRUE );
			pOpDesc->Invoke( &Param );	// TRUE for undo
		}

		delete pAttrib;
		pAttrib = NULL;
	}
	else
	{
		TRACE( _T("SingletonAppliedAttribute::ApplyAttribute() : NULL Members") );
	}

	return Ok;
}





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//			C  O  M  M  O  N  A  P  P  L  I  E  D  A  T  T  R  I  B  U  T  E
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //




/********************************************************************************************

>	CommonAppliedAttribute::CommonAppliedAttribute(NodeAttribute& AttrInstance)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	This constructor tries to create a UserInterface in the form of a 
				VisibleUserAttribute so the user can alter the attributes' contents.
	
	Notes:		If this constructor fails there will be no user interface for the 
				UserAttribute.

********************************************************************************************/
CommonAppliedAttribute::CommonAppliedAttribute(NodeAttribute& AttrInstance) :
	m_AttrID(AttrInstance.GetAttributeClassID())
{
	m_pVisibleAttribute = AttrInstance.CreateVisibleAttribute();
}

CommonAppliedAttribute::~CommonAppliedAttribute()
{
	delete m_pVisibleAttribute;
	m_pVisibleAttribute = NULL;
}


/********************************************************************************************

>	StringBase& CommonAppliedAttribute::GetText(StringBase& Description) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	This text is gonna be stuck in the Used Properties list of the Wizard 
				Properties Dialog (aka TemplateDialog).
	
	Returns:	Some user visible text describing this CommonAppliedAttribute, something like
				"Duplicate WizOp's"

********************************************************************************************/
StringBase& CommonAppliedAttribute::GetText(StringBase& Description) const
{
	if (m_pVisibleAttribute != NULL)
	{
		String_64 AttrDescription;
		m_pVisibleAttribute->GetText(AttrDescription);
		Description.MakeMsg(_R(IDS_DUPLICATE), &AttrDescription);
	}
	else
	{
		String_64 AttrDescription(_R(IDS_ERRORS_UNKNOWN));
		Description.MakeMsg(_R(IDS_DUPLICATE), &AttrDescription);
	}

	return Description;
}


/********************************************************************************************

>	BOOL CommonAppliedAttribute::Display(DialogOp& Dialog)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Displays some information about the applied attribute.
	
	Notes:		This function takes a DialopOp as its argument to determine where to
				put this object. Since this requires an internal knowledge of the DialogOp
				at the moment it is cast to a TemplateDialog which has the fields all ready.
				This is likely to change in the future.

********************************************************************************************/
BOOL CommonAppliedAttribute::Display(DialogOp& Dialog)
{
	BOOL Ok = TRUE;

	if (m_pVisibleAttribute != NULL)
	{
		m_pVisibleAttribute->Display(Dialog);
	}
	else
	{
		TRACE(_T("CommonAppliedAttribute::Display() : NULL Members"));
		Ok = FALSE;
	}

	return Ok;
}


/********************************************************************************************

>	void CommonAppliedAttribute::Hide()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Hides the information provided by Display().
	
	Notes:		This function takes a DialopOp as its argument to determine where to
				put this object. Since this requires an internal knowledge of the DialogOp
				at the moment it is cast to a TemplateDialog which has the fields all ready.
				This is likely to change in the future.

********************************************************************************************/
void CommonAppliedAttribute::Hide()
{
	if (m_pVisibleAttribute != NULL)
	{
		m_pVisibleAttribute->Hide();
	}
	else
	{
		TRACE(_T("CommonAppliedAttribute::Hide() : NULL Members"));
	}
}



/********************************************************************************************

>	virtual BOOL CommonAppliedAttribute::ApplyAttribute(Operation& OpToApplyWith)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Applies the attribute using the given op (slight bodge here)

	Inputs:		OpToApplyWith: the op to use to apply the attribute
	Returns:	TRUE always cos it can't fail
	
	Notes:		Applying a CommonAttribute does nothing

********************************************************************************************/
BOOL CommonAppliedAttribute::ApplyAttribute(Operation& OpToApplyWith)
{
	return TRUE;
}





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//			M  U  L  T  I  A  P  P  L  I  E  D  A  T  T  R  I  B  U  T  E
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //


/********************************************************************************************

>	MultiAppliedAttribute::MultiAppliedAttribute(NodeAttribute& AttrInstance)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	This constructor tries to create a string describing what we have multiple
				instances of.

	Inputs:		
	
	Notes:		If this constructor fails there will be no user interface for the 
				UserAttribute.

********************************************************************************************/
MultiAppliedAttribute::MultiAppliedAttribute(NodeAttribute& AttrInstance) :
	m_AttrID(AttrInstance.GetAttributeClassID())
{
	// Create a VisibleAttribute from which we can get a description
	VisibleAttribute* const pVisibleAttribute = AttrInstance.CreateVisibleAttribute();

	if (pVisibleAttribute != NULL)
	{
		pVisibleAttribute->GetText(m_AttributeDescription);
	}

	delete pVisibleAttribute;
}



/********************************************************************************************

>	const StringBase& MultiAppliedAttribute::GetAttributeDescription() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	This text is gonna be stuck in the Used Properties list of the Wizard 
				Properties Dialog (aka TemplateDialog).
	
	Returns:	Some user visible text describing this MultiAppliedAttribute, like "Various
				WizOp's"

********************************************************************************************/
const StringBase& MultiAppliedAttribute::GetAttributeDescription() const
{
	return m_AttributeDescription;
}

/********************************************************************************************

>	const StringBase& MultiAppliedAttribute::GetText() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	This text is gonna be stuck in the Used Properties list of the Wizard 
				Properties Dialog (aka TemplateDialog).
	
	Returns:	Some user visible text describing this MultiAppliedAttribute, like "Various
				WizOp's"

********************************************************************************************/
StringBase& MultiAppliedAttribute::GetText(StringBase& Description) const
{
	Description.MakeMsg(_R(IDS_VARIOUS), &GetAttributeDescription());

	return Description;
}


/********************************************************************************************

>	BOOL MultiAppliedAttribute::Display(DialogOp& Dialog)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Would normally display some information about the applied attribute but since
				this is too complicated we won't bother.
	
	Notes:		This function takes a DialopOp as its argument to determine where to
				put this object. Since this requires an internal knowledge of the DialogOp
				at the moment it is cast to a TemplateDialog which has the fields all ready.
				This is likely to change in the future.

********************************************************************************************/
BOOL MultiAppliedAttribute::Display(DialogOp& Dialog)
{
	return TRUE;
}


/********************************************************************************************

>	void MultiAppliedAttribute::Hide()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Would normally hide the information provided by display, but since we didn't
				do anything then we won't do anything here either.
	
	Notes:		This function takes a DialopOp as its argument to determine where to
				put this object. Since this requires an internal knowledge of the DialogOp
				at the moment it is cast to a TemplateDialog which has the fields all ready.
				This is likely to change in the future.

********************************************************************************************/
void MultiAppliedAttribute::Hide()
{
}
