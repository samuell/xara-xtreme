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
	$Header: /wxCamelot/Kernel/ngsentry.cpp 23    20/07/05 15:39 Luke $
	Special sentinel node that has all existing set names applied to it.
*/

#include "camtypes.h"

//#include "ngcore.h"
//#include "ngprop.h"
#include "ngsentry.h"
//#include "ngsetop.h"

#include "opbarcreation.h"

#include "selop.h"
#include "cxfdefs.h"
#include "cxftags.h"
#include "cxfrec.h"
#include "cxfrech.h"
#include "attrval.h"
#include "userattr.h"
#include "tmpltatr.h"

//#include "mario.h"
//#include "tim.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(NodeSetSentinel, Node);
CC_IMPLEMENT_DYNAMIC(NodeSetProperty, Node);
CC_IMPLEMENT_DYNAMIC(NodeBarProperty, Node);

// This line mustn't go before any CC_IMPLEMENT_... macros
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW



/********************************************************************************************
>	NodeSetProperty::NodeSetProperty()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Inputs:		strName		---		name of the set to hold properties for
	Purpose:	Default constructor for a NodeSetProperty, a container node for a group of
				properties for a named set.
	SeeAlso:	SGNameProp; Node; NodeSetSentinel; NameGallery
********************************************************************************************/

NodeSetProperty::NodeSetProperty()
{
	memset(m_pProp, 0, sizeof(m_pProp));
	m_Imported = FALSE;
}



/********************************************************************************************
>	NodeSetProperty::NodeSetProperty(const StringBase& strName)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Inputs:		(See Node::Node)
				strName		---		the name of the set to hold properties for
	Purpose:	Constructor for a NodeSetProperty, a container of properties associated
				with particular SGNameItems.
	SeeAlso:	SGNameProp; Node; NodeSetSentinel; NameGallery
********************************************************************************************/

NodeSetProperty::NodeSetProperty(const StringBase& strName)
{
	memset(m_pProp, 0, sizeof(m_pProp));
	m_strName.Alloc(strName.Length() + 1);
	m_strName = strName;
	m_Imported = FALSE;
}



/********************************************************************************************
>	virtual NodeSetProperty::~NodeSetProperty()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Purpose:	Destroys a NodeSetProperty.
	SeeAlso:	SGNameProp; Node; NodeSetSentinel; NameGallery
********************************************************************************************/

NodeSetProperty::~NodeSetProperty()
{
	// If we still have valid pointers to properties then take responsibility for
	// deallocating them.
	for (SGNameProp** ppProp = &m_pProp[SGNameProp::nPropertyCount];
		 --ppProp >= &m_pProp[0]; /* empty */ )
			if (*ppProp != 0)
			{
				delete *ppProp;
				*ppProp = 0;
			}
}



/********************************************************************************************
>	virtual SGNameProp* NodeSetProperty::SetProperty(SGNameProp* pNewProp)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Inputs:		pNewProp	---		the new property this node should hold.
	Returns:	The old property.
	Purpose:	See Node::GetNodeSize
	SeeAlso:	SGNameProp; Node; NodeSetSentinel; NameGallery
********************************************************************************************/

SGNameProp* NodeSetProperty::SetProperty(SGNameProp* pNewProp)
{
	ERROR3IF(pNewProp == 0, "NodeSetProperty::SetProperty: null input");

	INT32 i = pNewProp->GetIndex();
	ERROR3IF(i < 0 || i >= SGNameProp::nPropertyCount,
				"NodeSetProperty::SetProperty: out of range");

	SGNameProp* pOld = m_pProp[i];
	m_pProp[i] = pNewProp;

	m_Imported = FALSE;
	return pOld;
}



/********************************************************************************************
>	BOOL NodeSetProperty::CreateDefaults()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Returns:	FALSE if out of memory.
	Purpose:	Create default properties of every type for this set.
	SeeAlso:	SGNameProp; Node; NodeSetSentinel; NameGallery
********************************************************************************************/

BOOL NodeSetProperty::CreateDefaults()
{
	// Create a new default property of every type and delete any old ones.
	for (INT32 i = 0; i < SGNameProp::nPropertyCount; i++)
	{
		if (m_pProp[i] != 0) delete m_pProp[i];
		m_pProp[i] = SGNameProp::CreateDefault(m_strName, i);
		if (m_pProp[i] == 0) return FALSE;
	}

	return TRUE;
}



/********************************************************************************************
>	BOOL NodeSetProperty::CopyProperties(NodeSetProperty* pOther)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Inputs:		pOther		---		where to copy properties from
	Returns:	FALSE if out of memory.
	Purpose:	Copy the given NodeSetProperty's SGNameProps into this.
	SeeAlso:	SGNameProp; Node; NodeSetSentinel; NameGallery
********************************************************************************************/

BOOL NodeSetProperty::CopyProperties(NodeSetProperty* pOther)
{
	// Replace properties in this with clones of the other's.
	for (INT32 i = 0; i < SGNameProp::nPropertyCount; i++)
	{
		SGNameProp* pNewProp;
		if (pOther->m_pProp[i] == 0)
			pNewProp = 0;
		else
		{
			pNewProp = pOther->m_pProp[i]->Clone();
			ERRORIF(pNewProp == 0, _R(IDE_NOMORE_MEMORY), FALSE);
		}

		delete m_pProp[i];
		m_pProp[i] = pNewProp;
	}

	return TRUE;
}



/********************************************************************************************
>	virtual UINT32 NodeSetProperty::GetNodeSize() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Purpose:	See Node::GetNodeSize
	SeeAlso:	SGNameProp; Node; NodeSetSentinel; NameGallery
********************************************************************************************/

UINT32 NodeSetProperty::GetNodeSize() const
{
	return sizeof(*this);
}



/********************************************************************************************
>	virtual Node* NodeSetProperty::SimpleCopy()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Purpose:	See Node::SimpleCopy.
	SeeAlso:	SGNameProp; Node; NodeSetSentinel; NameGallery
	Notes:		If you add any data members to class NodeSetProperty, then you should
				define a (non-virtual) CopyNodeContents function and call that to
				do the copy.
********************************************************************************************/

Node* NodeSetProperty::SimpleCopy()
{
	NodeSetProperty* pCopy = new NodeSetProperty;
	ERRORIF(pCopy == 0, _R(IDE_NOMORE_MEMORY), 0);
	CopyNodeContents(pCopy);
	return pCopy;
}



/********************************************************************************************
>	void NodeSetProperty::CopyNodeContents(NodeSetProperty* pCopy)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Purpose:	See Node::SimpleCopy.
	SeeAlso:	SGNameProp; Node; NodeSetSentinel; NameGallery
********************************************************************************************/

void NodeSetProperty::CopyNodeContents(NodeSetProperty* pCopy)
{
	ERROR3IF(pCopy == 0, "NodeSetProperty::CopyNodeContents: null input");
	Node::CopyNodeContents(pCopy);

	// Copy the name of the set.
	pCopy->m_strName.Alloc(m_strName.Length() + 1);
	pCopy->m_strName = m_strName;

	// Create copies in the heap of this node's contained properties.
	for (INT32 i = 0; i < SGNameProp::nPropertyCount; i++)
	{
		if (m_pProp[i] == 0)
			pCopy->m_pProp[i] = 0;
		else
		{
			// TODO: verify that this is the out of memory convention for this func.
			pCopy->m_pProp[i] = m_pProp[i]->Clone();
			if (pCopy->m_pProp[i] == 0)
			{
				ERROR1RAW(_R(IDE_NOMORE_MEMORY));
				return;
			}
		}
	}
}


/***********************************************************************************************
>   void NodeSetProperty::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeSetProperty::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeSetProperty), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeSetProperty))
		CopyNodeContents((NodeSetProperty*)pNodeCopy);
}



/********************************************************************************************
>	virtual BOOL NodeSetProperty::WritePreChildrenNative(BaseCamelotFilter* pFilter)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pFilter		---		filter to write to
	Returns:	TRUE if the node has written out a record to the filter, FALSE otherwise.
	Purpose:	Writes out a SetProperty record.
********************************************************************************************/

BOOL NodeSetProperty::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	// Write out the SetProperty record header.
	CXaraFileRecord rec(TAG_SETPROPERTY, TAG_SETPROPERTY_SIZE);
	if (!rec.Init() ||
		!rec.WriteUnicode(m_strName) ||
		!rec.WriteINT16((INT16) SGNameProp::nPropertyCount))
	{
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));
		return FALSE;
	}

	// Write out each property in ascending index order.
	for (INT32 i = 0; i < SGNameProp::nPropertyCount; i++)
	{
		ERROR3IF(m_pProp[i] == NULL, "NodeSetProperty::WritePreChildrenNative: no property");
		if (!rec.WriteINT16((INT16) i) || !m_pProp[i]->Write(&rec))
		{
			pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));
			return FALSE;			
		}
	}
	
	// We've built up a record, now write it out.
	UINT32 r = pFilter->Write(&rec);
	if (r == 0) pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));
	return (BOOL) r;
#else
	return FALSE;
#endif
}



/********************************************************************************************
>	virtual BOOL NodeSetProperty::CanWriteChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pFilter		---		filter to write to
	Returns:	FALSE.
	Purpose:	Prevents children of the sentinel being written out in the web format.
********************************************************************************************/

BOOL NodeSetProperty::CanWriteChildrenWeb(BaseCamelotFilter*)
{
	return FALSE;
}



/********************************************************************************************
>	virtual BOOL NodeSetProperty::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pFilter		---		filter to write to
	Returns:	FALSE.
	Purpose:	Web files don't write out SetSentinel records.  This code assumes the
				document will only contain one SetSentinel.
********************************************************************************************/

BOOL NodeSetProperty::WritePreChildrenWeb(BaseCamelotFilter*)
{
	return FALSE;
}



/********************************************************************************************
> 	BOOL NodeSetProperty::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/99
	Inputs:		pFilter		---		filter to write to
	Returns:	TRUE.
	Purpose:	Begins the child record sequence for SetSentinel in the web format.
				Web export doesn't write out SetSentinel records or children of the
				sentinel, so this overrides the default behaviour in Node by ensuring
				the DOWN record does not get written.
********************************************************************************************/

BOOL NodeSetProperty::WriteBeginChildRecordsWeb(BaseCamelotFilter*)
{
	return TRUE;
}



/********************************************************************************************
> 	BOOL NodeSetProperty::WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/99
	Inputs:		pFilter		---		filter to write to
	Returns:	TRUE.
	Purpose:	Ends the child record sequence for SetSentinel in the web format.
				Web export doesn't write out SetSentinel records or children of the
				sentinel, so this overrides the default behaviour in Node by ensuring
				the UP record does not get written.
********************************************************************************************/

BOOL NodeSetProperty::WriteEndChildRecordsWeb(BaseCamelotFilter*)
{
	return TRUE;
}



/********************************************************************************************
>	virtual void NodeSetProperty::GetDebugDetails(StringBase* pStr)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Purpose:	See Node::GetDebugDetails
	SeeAlso:	Node; NameGallery; OpApplyNames; OpDeleteNames; OpRenameNames etc
********************************************************************************************/

#if DEBUG_TREE

void NodeSetProperty::GetDebugDetails(StringBase* pStr)
{
	Node::GetDebugDetails(pStr);

	*pStr += TEXT("\r\nProperties for set \"");
	*pStr += m_strName;
	*pStr += TEXT("\"\r\n");

	for (INT32 i = 0; i < SGNameProp::nPropertyCount; i++)
		if (m_pProp[i] != 0) m_pProp[i]->GetDebugInfo(pStr);
}

#endif


/********************************************************************************************
>	virtual void NodeSetProperty::ShowDebugTreeDetails() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Purpose:	See Node::ShowDebugTreeDetails
	SeeAlso:	Node; NameGallery; OpApplyNames; OpDeleteNames; OpRenameNames etc
********************************************************************************************/

#if DEBUG_TREE

void NodeSetProperty::ShowDebugTreeDetails() const
{
	TRACE( _T("NodeSetProperty "));
	Node::ShowDebugTreeDetails();
}

#endif





/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////





/********************************************************************************************
>	NodeBarProperty::NodeBarProperty()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Purpose:	Default constructor for a NodeBarProperty, which retains bar attributes
				in an undoable storage.
	SeeAlso:	Node; NameGallery; OpApplyNames; OpDeleteNames; OpRenameNames etc
********************************************************************************************/

NodeBarProperty::NodeBarProperty()
  : m_nCount(0)
{
	m_vecBars.reserve(MAX_BAR_PROPERTIES);

	m_NoBar.IsLive = TRUE;
}



/********************************************************************************************
>	NodeBarProperty::NodeBarProperty(Node* pContext, AttachNodeDirection eDir)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Inputs:		See Node::Node
	Purpose:	Constructor for a NodeBarProperty, which retains bar attributes
				in an undoable storage.
	SeeAlso:	Node; NameGallery; OpApplyNames; OpDeleteNames; OpRenameNames etc
********************************************************************************************/

NodeBarProperty::NodeBarProperty(Node* pContext, AttachNodeDirection eDir)
  :	Node(pContext, eDir),
	m_nCount(0)
{
	m_vecBars.reserve(MAX_BAR_PROPERTIES);
}

/********************************************************************************************
>	BarDataType& NodeBarProperty::Bar(INT32 n)			

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/00
	Inputs:		Entry to retrieve
	Purpose:	Read the bars property
********************************************************************************************/
BarDataType& NodeBarProperty::Bar(INT32 n)				
{
	if (n >= 0)
		return m_vecBars[n];

	return m_NoBar;
}

const BarDataType& NodeBarProperty::Bar(INT32 n) const
{
	if (n >= 0)
		return m_vecBars[n];

	return m_NoBar;
}



/********************************************************************************************
>	size_t NodeBarProperty::Add(const BarDataType& bdt)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Inputs:		bdt		---		the BarDataType to add to the end of the array
	Returns:	The index of the entry that was appended, or UINT_MAX for an error.
	Purpose:	Appends the given property to the end of the array of bar properties.
	SeeAlso:	Node; NameGallery; OpApplyNames; OpDeleteNames; OpRenameNames etc
********************************************************************************************/

size_t NodeBarProperty::Add(const BarDataType& bdt)
{
	// Test for overflow.
	if (HowMany() >= MAX_BAR_PROPERTIES)
	{
		ERROR3("NodeBarProperty::Add: array overflow");
		return UINT_MAX;
	}

	// Append to the end.
	m_vecBars[m_nCount] = bdt;
	return m_nCount++;
}



/********************************************************************************************
>	BOOL NodeBarProperty::MakeChange(INT32 n, const BarDataType& bdt)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Inputs:		n		---		the index of the bar to change
				bdt		---		the new data for the given bar
	Returns:	TRUE if successful.
	Purpose:	Creates a duplicate of this node and then hides this node, writing the
				given changes into the duplicate.
	SeeAlso:	Node; NameGallery; OpApplyNames; OpDeleteNames; OpRenameNames etc
********************************************************************************************/

BOOL NodeBarProperty::MakeChange(INT32 n, const BarDataType& bdt)
{
	PORTNOTETRACE("dialog","NodeBarProperty::MakeChange - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Package up the parameters into an OpParam.
	OpChangeBarPropParam prm;
	prm.m_nIndex = n;
	prm.m_pbdtInfo = &bdt;
	prm.m_Prop = this;

	// Call the op to duplicate and hide this node.
	OpDescriptor* pDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGE_BAR_PROPERTY);
	ERROR3IF(pDesc == 0, "NodeBarProperty::MakeChange: can't find descriptor");
	pDesc->Invoke(&prm);
	return TRUE;
#else
	return FALSE;
#endif
}



/********************************************************************************************
>	virtual UINT32 NodeBarProperty::GetNodeSize() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Purpose:	See Node::GetNodeSize
	SeeAlso:	SGNameProp; Node; NodeSetSentinel; NameGallery
********************************************************************************************/

UINT32 NodeBarProperty::GetNodeSize() const
{
	return sizeof(*this);
}



/********************************************************************************************
>	virtual Node* NodeBarProperty::SimpleCopy()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Purpose:	See Node::SimpleCopy.
	SeeAlso:	SGNameProp; Node; NodeSetSentinel; NameGallery
	Notes:		If you add any data members to class NodeBarProperty, then you should
				define a (non-virtual) CopyNodeContents function and call that to
				do the copy.
********************************************************************************************/

Node* NodeBarProperty::SimpleCopy()
{
	NodeBarProperty* pCopy = new NodeBarProperty;
	ERRORIF(pCopy == 0, _R(IDE_NOMORE_MEMORY), 0);
	CopyNodeContents(pCopy);
	return pCopy;
}



/********************************************************************************************
>	void NodeBarProperty::CopyNodeContents(NodeBarProperty* pCopy)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Purpose:	See Node::SimpleCopy.
	SeeAlso:	SGNameProp; Node; NodeSetSentinel; NameGallery
********************************************************************************************/

void NodeBarProperty::CopyNodeContents(NodeBarProperty* pCopy)
{
	ERROR3IF(pCopy == 0, "NodeBarProperty::CopyNodeContents: null input");
	Node::CopyNodeContents(pCopy);

	// copy over the data of the first count bars
	for( size_t i = 0; i < m_nCount; i++ )
		pCopy->m_vecBars[i] = m_vecBars[i];

	pCopy->m_nCount = m_nCount;
}



/***********************************************************************************************
>   void NodeBarProperty::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeBarProperty::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeBarProperty), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeBarProperty))
		CopyNodeContents((NodeBarProperty*)pNodeCopy);
}



/********************************************************************************************
>	virtual BOOL NodeBarProperty::WritePreChildrenNative(BaseCamelotFilter* pFilter)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pFilter		---		filter to write to
	Returns:	TRUE if the node has written out a record to the filter, FALSE otherwise.
	Purpose:	Writes out a bar property record.
********************************************************************************************/

BOOL NodeBarProperty::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	// Write out the SetProperty record header.
	CXaraFileRecord rec(TAG_BARPROPERTY, TAG_BARPROPERTY_SIZE);
	if (!rec.Init() || !rec.WriteINT32(HowMany()))
	{
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));
		return FALSE;
	}

	// Write out each property in ascending index order.
	for (UINT32 i = 0; i < HowMany(); i++)
	{
		const BarDataType& bdt = Bar(i);
		BYTE nCode = bdt.IsLive |
					 (bdt.IsHorizontal << 1) |
					 (bdt.RequiresShuffle << 2) |
					 (bdt.ButtonsExtend << 3) |
					 (bdt.ButtonsScale << 4) |
					 (bdt.GroupsStretch << 5);

		if (!rec.WriteINT32(bdt.Spacing) || !rec.WriteBYTE(nCode) || !rec.WriteBYTE(bdt.SameSize))
		{
			pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));
			return FALSE;			
		}
	}
	
	// We've built up a record, now write it out.
	UINT32 r = pFilter->Write(&rec);
	if (r == 0) pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));
	return (BOOL) r;
#else
	return FALSE;
#endif
}



/********************************************************************************************
>	virtual BOOL NodeBarProperty::CanWriteChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pFilter		---		filter to write to
	Returns:	FALSE.
	Purpose:	Prevents children of the sentinel being written out in the web format.
********************************************************************************************/

BOOL NodeBarProperty::CanWriteChildrenWeb(BaseCamelotFilter*)
{
	return FALSE;
}



/********************************************************************************************
>	virtual BOOL NodeBarProperty::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pFilter		---		filter to write to
	Returns:	FALSE.
	Purpose:	Web files don't write out SetSentinel records.  This code assumes the
				document will only contain one SetSentinel.
********************************************************************************************/

BOOL NodeBarProperty::WritePreChildrenWeb(BaseCamelotFilter*)
{
	return FALSE;
}



/********************************************************************************************
> 	BOOL NodeBarProperty::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/99
	Inputs:		pFilter		---		filter to write to
	Returns:	TRUE.
	Purpose:	Begins the child record sequence for SetSentinel in the web format.
				Web export doesn't write out SetSentinel records or children of the
				sentinel, so this overrides the default behaviour in Node by ensuring
				the DOWN record does not get written.
********************************************************************************************/

BOOL NodeBarProperty::WriteBeginChildRecordsWeb(BaseCamelotFilter*)
{
	return TRUE;
}



/********************************************************************************************
> 	BOOL NodeBarProperty::WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/99
	Inputs:		pFilter		---		filter to write to
	Returns:	TRUE.
	Purpose:	Ends the child record sequence for SetSentinel in the web format.
				Web export doesn't write out SetSentinel records or children of the
				sentinel, so this overrides the default behaviour in Node by ensuring
				the UP record does not get written.
********************************************************************************************/

BOOL NodeBarProperty::WriteEndChildRecordsWeb(BaseCamelotFilter*)
{
	return TRUE;
}



/********************************************************************************************
>	virtual void NodeBarProperty::GetDebugDetails(StringBase* pStr)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Purpose:	See Node::GetDebugDetails
	SeeAlso:	Node; NameGallery; OpApplyNames; OpDeleteNames; OpRenameNames etc
********************************************************************************************/

#if DEBUG_TREE

void NodeBarProperty::GetDebugDetails(StringBase* pStr)
{
	Node::GetDebugDetails(pStr);

	*pStr += TEXT("\r\nBar data\r\n");
}

#endif


/********************************************************************************************
>	virtual void NodeBarProperty::ShowDebugTreeDetails() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Purpose:	See Node::ShowDebugTreeDetails
	SeeAlso:	Node; NameGallery; OpApplyNames; OpDeleteNames; OpRenameNames etc
********************************************************************************************/

#if DEBUG_TREE

void NodeBarProperty::ShowDebugTreeDetails() const
{
	TRACE( _T("NodeBarProperty "));
	Node::ShowDebugTreeDetails();
}

#endif




/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////





/********************************************************************************************
>	NodeSetSentinel::NodeSetSentinel()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Purpose:	Default constructor for a NodeSetSentinel, a parent node of all the
				cloned Wix object name attributes.
	SeeAlso:	Node; NameGallery; OpApplyNames; OpDeleteNames; OpRenameNames etc
********************************************************************************************/

NodeSetSentinel::NodeSetSentinel()
{
	// Empty.
}



/********************************************************************************************
>	NodeSetSentinel::NodeSetSentinel(Node* pContext, AttachNodeDirection eDir)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Inputs:		See Node::Node
	Purpose:	Constructor for a NodeSetSentinel, a parent node of all the cloned
				Wix object name attributes.
	SeeAlso:	Node; NameGallery; OpApplyNames; OpDeleteNames; OpRenameNames etc
********************************************************************************************/

NodeSetSentinel::NodeSetSentinel(Node* pContext, AttachNodeDirection eDir)
  :	Node(pContext, eDir)
{
	// Empty.
}



/********************************************************************************************
>	virtual BOOL NodeSetSentinel::IsSetCandidate() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Returns:	TRUE.
	Purpose:	See Node::IsSetCandidate
	SeeAlso:	NodeRenderableInk::IsSetCandidate
********************************************************************************************/

BOOL NodeSetSentinel::IsSetCandidate() const
{
	return TRUE;
}



/********************************************************************************************
>	TemplateAttribute* NodeSetSentinel::GetNameAttr(const StringBase& strName) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Inputs:		strName		---		the name to search for
	Returns:	Returns the address of the child Wix ObjectName attribute of the given
				value, or null if there isn't one.  
	SeeAlso:	TemplateAttribute
********************************************************************************************/

TemplateAttribute* NodeSetSentinel::GetNameAttr(const StringBase& strName) const
{
	Node			   *pn;
	for ( pn = FindFirstChild(); pn != 0; pn = pn->FindNext())
		if (pn->IsAnObjectName() && ((TemplateAttribute*) pn)->GetParam() == strName)
			break;

	return (TemplateAttribute*) pn;
}



/********************************************************************************************
>	NodeSetProperty* NodeSetSentinel::FindPropertyNode(const StringBase& strName) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Inputs:		strName				---		the name of the set to retrieve properties for
	Returns:	Pointer to the node holding the given property, or null if there isn't one.
	SeeAlso:	NodeSetSentinel::CreatePropertyNode
********************************************************************************************/

NodeSetProperty* NodeSetSentinel::FindPropertyNode(const StringBase& strName) const
{
	// Search children for the given property.
	for (Node* pNode = FindLastChild();
		 pNode != 0;
		 pNode = pNode->FindPrevious())
			if (IS_A(pNode, NodeSetProperty) && 
				((NodeSetProperty*) pNode)->GetName() == strName)
				return (NodeSetProperty*) pNode;

	// Not found.
	return NULL;
}

/********************************************************************************************
>	NodeBarProperty* NodeSetSentinel::FindBarProperty()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Returns:	Pointer to the node holding the given property, or null if there isn't one.
	SeeAlso:	NodeSetSentinel::CreatePropertyNode
********************************************************************************************/

NodeBarProperty* NodeSetSentinel::FindBarProperty()
{
	// Search children for the given property.
	Node			   *pNode;
	for ( pNode = FindLastChild();
		 pNode != 0;
		 pNode = pNode->FindPrevious())
			if (IS_A(pNode, NodeBarProperty))
					break;

	return (NodeBarProperty*) pNode;
}



/********************************************************************************************
>	NodeSetProperty* NodeSetSentinel::CreatePropertyNode(const StringBase& strName)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Inputs:		strName				---		the name of the set to create properties for
	Returns:	Pointer to the new node, or null if out of memory.
	SeeAlso:	NodeSetSentinel::FindPropertyNode
********************************************************************************************/

NodeSetProperty* NodeSetSentinel::CreatePropertyNode(const StringBase& strName)
{
	// I commented this out since I call this function after I have added an action
	// that deletes the older copy of the properties, but this error3 finds them and
	// wont let me create the new version!
//	ERROR3IF(FindPropertyNode(strName) != 0,
//				"NodeSetSentinel::CreatePropertyNode: NodeSetProperty already exists");

	NodeSetProperty* pPropNode = new NodeSetProperty(strName);
	if (pPropNode == 0) return 0;
	pPropNode->AttachNode(FindLastChild(), PREV);
	return pPropNode;
}




/********************************************************************************************
>	BOOL NodeSetSentinel::TargetsExist() const

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/00
	Returns:	TRUE if sets within the document are targets for stretches.
	Purpose		Replaces the above TriggerExist since it is easier to work out if a target exists or not.
				
	SeeAlso:	NodeSetProperty; NamedStretchProp
********************************************************************************************/
BOOL NodeSetSentinel::TargetsExist() const
{
	// Search children for a trigger stretching property.
	for (Node* pNode = FindLastChild();
		 pNode != 0;
		 pNode = pNode->FindPrevious())
			if (IS_A(pNode, NodeSetProperty))
			{
				NamedStretchProp* pProp = (NamedStretchProp*)
					((NodeSetProperty*) pNode)->GetProperty(NamedStretchProp::nIndex);

				// if any are ticked and any have a tigger defined then a target exists
				if (pProp->GetState() && !pProp->GetTriggers().empty())
					return TRUE;
			}

	// None found.
	return FALSE;
}



/********************************************************************************************
>	BOOL NodeSetSentinel::OnLoadName(BaseCamelotFilter* pFilter,
									 TemplateAttribute* pImportedName)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Inputs:		pFilter				---		the filter which is loading/importing
				pImportedName		---		the newly imported Wix ObjectName attribute
	Returns:	TRUE if the name is successfully registered, FALSE if out of memory.
	Purpose:	Attaches duplicates of newly inserted Wix ObjectName attributes as
				children of the sentinel when the web importer is in operation, as
				the web exporter doesn't export children of the sentinel (ie. it
				discards editing information for the sake of compactness).
	SeeAlso:	TemplateAttrRecordHandler::HandleRecord; HideNodeAction::Init
********************************************************************************************/

BOOL NodeSetSentinel::OnLoadName(BaseCamelotFilter* pFilter,
								 TemplateAttribute* pImportedName)
{
	// If it's not the web filter, or we already have this name, there's nothing to do.
	if (!pFilter->IsWebFilter() || GetNameAttr(pImportedName->GetParam()) != 0)
		return TRUE;

	// Try to clone the attribute and attach the clone as a child, preserving the order.
	TemplateAttribute* pCopy = (TemplateAttribute*) pImportedName->SimpleCopy();
	ERRORIF(pCopy == 0, _R(IDE_NOMORE_MEMORY), FALSE);
	pCopy->AttachNode(this, FIRSTCHILD);

	// Create default properties for the new set name.
	NodeSetProperty* pProp = new NodeSetProperty(pImportedName->GetParam());
	ERRORIF(pProp == 0 || !pProp->CreateDefaults(), _R(IDE_NOMORE_MEMORY), FALSE);
	pProp->AttachNode(FindLastChild(), PREV);
	
	// If importing into a document, rather than loading, then try to create undo
	// actions for the new attachments.
	if (pFilter->IsImporting() && pFilter->GetImportSelOp() != 0)
	{
		// Attach and create an action to hide the new attribute when we Undo.
		HideNodeAction* pHideAct;
		if (AC_FAIL == HideNodeAction::Init(pFilter->GetImportSelOp(),
											pFilter->GetImportSelOp()->GetUndoActions(),
											pCopy, TRUE, (Action**) &pHideAct))
		{
			// Tidy up on fail.
			pCopy->UnlinkNodeFromTree();
			delete pCopy;
			return FALSE;
		}

		// Ditto for the set property.
		if (AC_FAIL == HideNodeAction::Init(pFilter->GetImportSelOp(),
											pFilter->GetImportSelOp()->GetUndoActions(),
											pProp, TRUE, (Action**) &pHideAct))
		{
			// Tidy up on fail.
			pProp->UnlinkNodeFromTree();
			delete pProp;
			return FALSE;
		}
	}

	// Success.
	return TRUE;
}



/********************************************************************************************
>	virtual BOOL NodeSetSentinel::WritePreChildrenNative(BaseCamelotFilter* pFilter)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pFilter		---		filter to write to
	Returns:	TRUE if the node has written out a record to the filter, FALSE otherwise.
	Purpose:	Writes out a SetSentinel record.
********************************************************************************************/

BOOL NodeSetSentinel::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	CXaraFileRecord rec(TAG_SETSENTINEL, TAG_SETSENTINEL_SIZE);
	if (!rec.Init() || !pFilter->Write(&rec))
	{
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));
		return FALSE;
	}

	return TRUE;
#else
	return FALSE;
#endif
}



/********************************************************************************************
>	virtual BOOL NodeSetSentinel::CanWriteChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pFilter		---		filter to write to
	Returns:	FALSE.
	Purpose:	Prevents children of the sentinel being written out in the web format.
********************************************************************************************/

BOOL NodeSetSentinel::CanWriteChildrenWeb(BaseCamelotFilter*)
{
	return FALSE;
}



/********************************************************************************************
>	virtual BOOL NodeSetSentinel::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pFilter		---		filter to write to
	Returns:	FALSE.
	Purpose:	Web files don't write out SetSentinel records.  This code assumes the
				document will only contain one SetSentinel.
********************************************************************************************/

BOOL NodeSetSentinel::WritePreChildrenWeb(BaseCamelotFilter*)
{
	return FALSE;
}



/********************************************************************************************
> 	BOOL NodeSetSentinel::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/99
	Inputs:		pFilter		---		filter to write to
	Returns:	TRUE.
	Purpose:	Begins the child record sequence for SetSentinel in the web format.
				Web export doesn't write out SetSentinel records or children of the
				sentinel, so this overrides the default behaviour in Node by ensuring
				the DOWN record does not get written.
********************************************************************************************/

BOOL NodeSetSentinel::WriteBeginChildRecordsWeb(BaseCamelotFilter*)
{
	return TRUE;
}



/********************************************************************************************
> 	BOOL NodeSetSentinel::WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/99
	Inputs:		pFilter		---		filter to write to
	Returns:	TRUE.
	Purpose:	Ends the child record sequence for SetSentinel in the web format.
				Web export doesn't write out SetSentinel records or children of the
				sentinel, so this overrides the default behaviour in Node by ensuring
				the UP record does not get written.
********************************************************************************************/

BOOL NodeSetSentinel::WriteEndChildRecordsWeb(BaseCamelotFilter*)
{
	return TRUE;
}



/********************************************************************************************
>	virtual UINT32 NodeSetSentinel::GetNodeSize() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Purpose:	See Node::GetNodeSize
	SeeAlso:	Node; NameGallery; OpApplyNames; OpDeleteNames; OpRenameNames etc
********************************************************************************************/

UINT32 NodeSetSentinel::GetNodeSize() const
{
	return sizeof(*this);
}



/********************************************************************************************
>	virtual Node* NodeSetSentinel::SimpleCopy()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Purpose:	See Node::SimpleCopy.
	SeeAlso:	Node; NameGallery; OpApplyNames; OpDeleteNames; OpRenameNames etc
	Notes:		If you add any data members to class NodeSetSentinel, then you should
				define a (non-virtual) CopyNodeContents function and call that to
				do the copy.
********************************************************************************************/

Node* NodeSetSentinel::SimpleCopy()
{
	NodeSetSentinel* pCopy = new NodeSetSentinel;
	ERRORIF(pCopy == 0, _R(IDE_NOMORE_MEMORY), 0);
	CopyNodeContents(pCopy);
	return pCopy;
}



/********************************************************************************************
> 	virtual BOOL NodeSetSentinel::AllowOp(ObjChangeParam* pParam,
										  BOOL SetOpPermissionState = TRUE,
										  BOOL DoPreTriggerEdit = TRUE)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/2000
	Inputs:		pParam					)
				SetOpPermissionState	) unused by this function
				DoPreTriggerEdit		)
	Outputs:	
	Returns:	TRUE.
	Purpose:	Virtual override of Node::AllowOp(), to prevent an AllowOp call to this node
				from chaining up the tree. AllowOp chains usually originate somewhere beneath
				a Layer node in the tree, propagate up to the Layer node and stop there.

				However, NodeSetSentinel lives above Layer in the tree and may have AllowOp
				called on it, hence the need for this override.

	SeeAlso:	Layer::AllowOp(); Node::AllowOp()
********************************************************************************************/

BOOL NodeSetSentinel::AllowOp(ObjChangeParam*, BOOL, BOOL)
{
	return TRUE;
}



/********************************************************************************************
>	virtual void NodeSetSentinel::GetDebugDetails(StringBase* pStr)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Purpose:	See Node::GetDebugDetails
	SeeAlso:	Node; NameGallery; OpApplyNames; OpDeleteNames; OpRenameNames etc
********************************************************************************************/

#if DEBUG_TREE

void NodeSetSentinel::GetDebugDetails(StringBase* pStr)
{
	Node::GetDebugDetails(pStr);
}

#endif


/********************************************************************************************
>	virtual void NodeSetSentinel::ShowDebugTreeDetails() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Purpose:	See Node::ShowDebugTreeDetails
	SeeAlso:	Node; NameGallery; OpApplyNames; OpDeleteNames; OpRenameNames etc
********************************************************************************************/

#if DEBUG_TREE

void NodeSetSentinel::ShowDebugTreeDetails() const
{
	TRACE( _T("NodeSetSentinel "));
	Node::ShowDebugTreeDetails();
}

#endif
