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
	$Header: /wxCamelot/Kernel/ngiter.cpp 7     20/07/05 15:39 Luke $
	Classes to iterate through the objects in the document and over Attribute gallery
	groups and items.
*/

#include "camtypes.h"

//#include "ngcore.h"
#include "ngitem.h"
#include "ngiter.h"
#include "ngsentry.h"

#include "app.h"
#include "node.h"

#if 0
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
#endif

DECLARE_SOURCE("$Revision$");

// This definition must appear after the above CC_IMPLEMENT_ definitions.
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


// These global objects implement the various Sources for NodeScans.
SelectDocSource		theSelectedDocument;
SelectObjSource		theSelectedObjects;
NotSelectSource		theUnselectedObjects;
SetSentinelSource	theSetSentinel;


/********************************************************************************************
>	BOOL NodeScan::Scan()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Returns:	TRUE if scan terminated successfully, FALSE if the NodeScan
				cancelled the scan.
	Purpose:	Scans through the 'selected' document's tree extracting Nodes and calling
				the polymorphic Do function with them.
********************************************************************************************/

BOOL NodeScan::Scan()
{
	// Call Pre() before the scan.
	if (!Pre()) return FALSE;

	// Perform the appropriate type of scan.
	for (Node* pNode = m_pSource->GetFirst();
		 pNode != 0;
		 pNode = m_pSource->GetNext(pNode))
	{
		// NB. Post() not called if scan fails.
		if (!Do(pNode)) return FALSE;
	}

	// Call Post() after a successful scan.
	return Post();
}



/********************************************************************************************
>	virtual BOOL NodeScan::Pre()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Returns:	TRUE to successfully start scan, FALSE on error.
	Purpose:	Default override called by NodeScan::Scan prior to scanning the document.
********************************************************************************************/

BOOL NodeScan::Pre()
{
	return TRUE;
}



/********************************************************************************************
>	virtual BOOL NodeScan::Post()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Returns:	TRUE to successfully finish scan, FALSE on error.
	Purpose:	Default override called by NodeScan::Scan prior to scanning the document.
********************************************************************************************/

BOOL NodeScan::Post()
{
	return TRUE;
}



/********************************************************************************************
>	NodeScan::NodeScan(NodeScan::Source* ps)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Inputs:		The source of objects over which to scan, eg. theSelectedDocument or
				theSelectedObjects.
	Purpose:	Constructs an NodeScan object.
	SeeAlso:	NodeScan::Source; DocTreeSource; RangeSource
********************************************************************************************/

NodeScan::NodeScan(NodeScan::Source* ps)
  :	m_pSource(ps)
{
	// Empty.
}



/********************************************************************************************
>	virtual NodeScan::~NodeScan()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Purpose:	Destroys an NodeScan object.
********************************************************************************************/

NodeScan::~NodeScan()
{
	// Empty.
}



/********************************************************************************************
>	UndoableNodeScan::UndoableNodeScan(UndoableOperation* pOp, NodeScan::Source* ps)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Inputs:		pOp		---		the UndoableOperation that defines the context for the
								scans implemented by derived classes.
	Purpose:	Constructs an UndoableNodeScan object.
	SeeAlso:	NodeScan
********************************************************************************************/

UndoableNodeScan::UndoableNodeScan(UndoableOperation* pOp, NodeScan::Source* ps)
  :	NodeScan(ps),
	m_pOp(pOp)
{
	// Empty.
}



/********************************************************************************************
>	DocTreeSource::DocTreeSource(BaseDocument* pDoc = 0)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Inputs:		pDoc		---		the document whose tree is to be scanned, if any
	Purpose:	Constructs a DocTreeSource object.
	SeeAlso:	NodeScan::Source; NodeScan
********************************************************************************************/

DocTreeSource::DocTreeSource(BaseDocument* pDoc)
  :	m_pDoc(pDoc)
{
	// Empty.
}



/********************************************************************************************
>	virtual Node* DocTreeSource::GetFirst()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Returns:	The first non-hidden node in a depth-first traversal of the tree, or null.
	Purpose:	Identical to Node::DocFindFirstDepthFirst but skips over hidden nodes.
				NB. the first node in the document tree is guaranteed to never be
				hidden.
	SeeAlso:	NodeScan::Source; DocTreeSource::GetNext
********************************************************************************************/

Node* DocTreeSource::GetFirst()
{
	ERROR3IF(m_pDoc == 0, "DocTreeSource::GetFirst: no document");

	Node* pNode = m_pDoc->GetFirstNode();
	if (pNode != 0 && !pNode->IsSetCandidate()) pNode = GetNext(pNode);
	return pNode;
}



/********************************************************************************************
>	virtual Node* DocTreeSource::GetNext(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Inputs:		pNode	---		the previous node returned by _GetNext
	Returns:	The next non-hidden node in a depth-first traversal of the tree, or null.
	Purpose:	Identical to Node::DocFindNextDepthFirst but skips over hidden nodes.
				To be precise, it doesn't follow child links for NodeHiddens.
	SeeAlso:	NodeScan::Source; DocTreeSource::GetFirst

	Notes:		Karim 14/12/2000
				Modified so that we also skip NodeMoulds in the tree,
				as the user must not be allowed to select moulded objects.
				This is achieved by effectively hiding them from the NameGallery's scans.
				ps, TODO:	Rather than test specifically for NodeMould,
							use a similar mechanism to IsSetCandidate().
********************************************************************************************/

Node* DocTreeSource::GetNext(Node* pNode)
{
	do {
		if (pNode->FindNext() == 0)
			pNode = pNode->FindParent();
		else
		{
			pNode = pNode->FindNext();
			while (	!pNode->IsNodeHidden() &&
					!pNode->IsANodeMould() && pNode->FindFirstChild() != 0 )
				pNode = pNode->FindFirstChild();
		}
	} while (pNode != 0 && !pNode->IsSetCandidate());

	return pNode;
}



/********************************************************************************************
>	virtual Node* SelectDocSource::GetFirst()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Purpose:	Customises DocTreeSource::_GetFirst to use the 'selected' documemt's tree.
	SeeAlso:	NodeScan::Source; DocTreeSource; DocTreeSource::GetFirst
********************************************************************************************/

Node* SelectDocSource::GetFirst()
{
	m_pDoc = Document::GetSelected();
	return m_pDoc != 0 ? DocTreeSource::GetFirst() : 0;
}



/********************************************************************************************
>	RangeSource::RangeSource(Range* pRange = 0)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Inputs:		pRange	---	  the range to be scanned, if any.
	Purpose:	Constructs a RangeSource object.
	SeeAlso:	NodeScan::Source; NodeScan
********************************************************************************************/

RangeSource::RangeSource(Range* pRange)
  : m_pRange(pRange)
{
	// Empty.
}



/********************************************************************************************
>	virtual Node* RangeSource::GetFirst()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Returns:	The first object in the selection to scan over, or null if there isn't one.
	SeeAlso:	RangeSource::GetNext; NodeScan::Source; NodeScan
********************************************************************************************/

Node* RangeSource::GetFirst()
{
	ERROR3IF(m_pRange == 0, "RangeSource::GetFirst: no range");
	
	Node* pNode = m_pRange->FindFirst();
	if (pNode != 0 && !pNode->IsSetCandidate()) pNode = GetNext(pNode);
	return pNode;
}



/********************************************************************************************
>	virtual Node* RangeSource::GetNext(Node* pLastNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Returns:	The next object in the selection to scan over, or null if there isn't one.
	SeeAlso:	RangeSource::GetFirst; NodeScan::Source; NodeScan
********************************************************************************************/

Node* RangeSource::GetNext(Node* pNode)
{
	for (pNode = m_pRange->FindNext(pNode);
		 pNode != 0 && !pNode->IsSetCandidate();
		 pNode = m_pRange->FindNext(pNode))
			/* empty */ ;

	return pNode;
}



/********************************************************************************************
>	virtual Node* SelectObjSource::GetFirst()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Returns:	The first object in the selection to scan over, or null if there isn't one.
	SeeAlso:	RangeSource::_GetFirst; RangeSource; NodeScan::Source; NodeScan
********************************************************************************************/

Node* SelectObjSource::GetFirst()
{
	m_pRange = GetApplication()->FindSelection();
	return RangeSource::GetFirst();
}



/********************************************************************************************
>	virtual Node* NotSelectSource::GetFirst()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Returns:	The first unselected node in a depth-first traversal of the tree, or null.
	Purpose:	Identical to Node::DocFindFirstDepthFirst but skips over hidden nodes.
				NB. the first node in the document tree is guaranteed to never be
				hidden.
	SeeAlso:	NodeScan::Source; NotSelectSource::GetNext
********************************************************************************************/

Node* NotSelectSource::GetFirst()
{
	Node* pNode = SelectDocSource::GetFirst();
	if (pNode != 0 && pNode->IsSelected()) pNode = GetNext(pNode);
	return pNode;
}



/********************************************************************************************
>	virtual Node* NotSelectSource::GetNext(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Inputs:		pNode	---		the previous node returned by _GetNext
	Returns:	The next non-hidden node in a depth-first traversal of the tree, or null.
	Purpose:	Identical to Node::DocFindNextDepthFirst but skips over hidden nodes.
				To be precise, it doesn't follow child links for NodeHiddens.
	SeeAlso:	NodeScan::Source; NotSelectSource::GetFirst
********************************************************************************************/

Node* NotSelectSource::GetNext(Node* pNode)
{
	do {
		if (pNode->FindNext() == 0)
			pNode = pNode->FindParent();
		else
		{
			pNode = pNode->FindNext();
			while (!pNode->IsNodeHidden() && pNode->FindFirstChild() != 0)
				pNode = pNode->FindFirstChild();
		}
	} while (pNode != 0 && (!pNode->IsSetCandidate() || pNode->IsSelected()));

	return pNode;
}



/********************************************************************************************
>	SingleNodeSource::SingleNodeSource(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Inputs:		pNode	---		the node to "scan" over
	Returns:	Scan over a single Node.
	SeeAlso:	NodeScan::Source; NodeScan
********************************************************************************************/

SingleNodeSource::SingleNodeSource(Node* pNode)
  :	m_pNode(pNode)
{
	// Empty.
}



/********************************************************************************************
>	virtual Node* SingleNodeSource::GetFirst()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Returns:	A single object.
	SeeAlso:	NodeScan::Source; NodeScan; SingleNodeSource::GetNext
********************************************************************************************/

Node* SingleNodeSource::GetFirst()
{
	return m_pNode;
}



/********************************************************************************************
>	virtual Node* SingleNodeSource::GetNext(Node* pLastNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Returns:	Null - the sole object has already been returned by _GetFirst.
	SeeAlso:	NodeScan::Source; NodeScan; SingleNodeSource::GetFirst
********************************************************************************************/

Node* SingleNodeSource::GetNext(Node*)
{
	return 0;
}



/********************************************************************************************
>	virtual Node* SetSentinelSource::GetFirst()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Returns:	The 'selected' document's NodeSetSentinel.
	SeeAlso:	SingleNodeSource; NodeScan::Source; NodeScan
********************************************************************************************/

Node* SetSentinelSource::GetFirst()
{
	m_pNode = Document::GetSelected()->GetSetSentinel();
	return SingleNodeSource::GetFirst();
}



/********************************************************************************************
>	virtual SGNameGroup* NameGroupIter::ForEach()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Returns:	Null if all Do() calls succeed, a pointer to the failing group if a Do()
				call fails.
	Purpose:	Calls NameGroupIter::Do for every group in the name gallery.
	SeeAlso:	NodeScan
********************************************************************************************/

SGNameGroup* NameGroupIter::ForEach()
{
	PORTNOTETRACE("other","NameGroupIter::ForEach - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	SGNameGroup* pGroup = NameGallery::Instance()->GetFirstGroup();
	while (pGroup != 0)
	{
		SGNameGroup* pNextGroup = (SGNameGroup*) pGroup->GetNext();
		if (!Do(pGroup)) return pGroup;
		pGroup = pNextGroup;
	}
#endif
	return NULL;
}



/********************************************************************************************
>	NameItemIter::NameItemIter(NameItemIter::Mask mask = NameItemIter::ALL_ITEMS)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Inputs:		mask	---		what to iterate over (ALL_ITEMS, ALL_NAMES, 
								HIGHLIGHTED_ITEMS, HIGHLIGHTED_NAMES)
	Purpose:	Constructs a NameItemIter.
	SeeAlso:	NameGroupIter; NameIterOp
********************************************************************************************/

NameItemIter::NameItemIter(NameItemIter::Mask mask)
  :	m_eMask(mask)
{
	// Empty.
}


/********************************************************************************************
>	virtual SGNameItem* NameItemIter::ForEach()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Returns:	Null if all Do() calls succeed, a pointer to the failing item if a Do()
				call fails.
	Purpose:	Calls NameItemIter::Do for every item in the name gallery.
	SeeAlso:	NodeScan
********************************************************************************************/

SGNameItem* NameItemIter::ForEach()
{
	PORTNOTETRACE("other","NameItemIter::ForEach - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Just the items in 'Used Names'?
	if (INT32(m_eMask) & 1)
		return ForEachItem(NameGallery::Instance()->GetUsedNames());

	// Items in all the groups.
	SGNameGroup* pGroup = NameGallery::Instance()->GetFirstGroup();
	while (pGroup != 0)
	{
		SGNameGroup* pNextGroup = (SGNameGroup*) pGroup->GetNext();
		SGNameItem* pStopItem = ForEachItem(pGroup);
		if (pStopItem != 0) return pStopItem;
		pGroup = pNextGroup;
	}
#endif
	// No item stopped the iteration.
	return NULL;
}



/********************************************************************************************
>	SGNameItem* NameItemIter::ForEachItem(SGNameGroup* pGroup)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Returns:	Null if all Do() calls succeed, a pointer to the failing item if a Do()
				call fails.
	Purpose:	Calls NameItemIter::Do for every item in the given group.
	SeeAlso:	NameIterItem::ForEach
********************************************************************************************/

SGNameItem* NameItemIter::ForEachItem(SGNameGroup* pGroup)
{
	if (pGroup == 0) return 0;

	SGNameItem* pItem = (SGNameItem*) pGroup->GetChild();
	while (pItem != 0)
	{
		// NB. careful to fetch next item in iteration before calling Do() in case
		// Do() deletes the item and we lose its next link.
		SGNameItem* pNextItem = (SGNameItem*) pItem->GetNext();
		if (((INT32(m_eMask) & 2) == 0 || pItem->IsSelected()) && !Do(pItem))
			return pItem;
		pItem = pNextItem;
	}

	return 0;
}



/********************************************************************************************
>	NameIterOp::NameIterOp(Node* pNode, UndoableOperation* pOp = 0,
						   NameItemIter::Mask mask = NameItemIter::HIGHLIGHTED_NAMES)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Inputs:		pNode	---		the node to iterate gallery items over.
				pOp		---		if supplied by a derived class, the operation context in
								which this iteration is working on objects.
	Purpose:	Specialisation of NameItemIter base class to iterate over highlighted Name
				gallery 'Used Names' items and perform undoable operations on the nodes
				that are members of their sets.
	SeeAlso:	NameItemIter
********************************************************************************************/

NameIterOp::NameIterOp(Node* pNode, UndoableOperation* pOp, NameItemIter::Mask mask)
  : NameItemIter(mask),
	m_pNode(pNode),
	m_pOp(pOp)
{
	// Empty.
}
