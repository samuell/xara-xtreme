// $Id: cxflists.h 662 2006-03-14 21:31:49Z alex $
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

// Defines the various lists and list items used by the new file format

#include "list.h"
#include "listitem.h"

class NodePath;
class TextStory;
class NodeGroup;
class Node;
class CXaraFileRecordHandler;
class BaseCamelotFilter;

/***********************************************************************************************

>	class CXaraFilePathRecordRefListItem : public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/96
	Purpose:	An item that appears in the CXaraFilePathRecordRefList

***********************************************************************************************/

class CXaraFilePathRecordRefListItem : public ListItem
{
CC_DECLARE_MEMDUMP(CXaraFilePathRecordRefListItem)

public:
	CXaraFilePathRecordRefListItem(NodePath* pPath,UINT32 RecNum) { pNodePath = pPath; RecordNumber = RecNum; }

	NodePath*	GetNodePath()		{ return pNodePath; }
	UINT32		GetRecordNumber()	{ return RecordNumber; }

private:
	NodePath*	pNodePath;
	UINT32		RecordNumber;
};

/***********************************************************************************************

>	class CXaraFilePathRecordRefList : public List

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/96
	Purpose:	Holds a list of node path objects, and the corresponding record numbers of the
				records that represent the paths in the file

***********************************************************************************************/

class CXaraFilePathRecordRefList : public List
{
CC_DECLARE_MEMDUMP(CXaraFilePathRecordRefList)

public:
	CXaraFilePathRecordRefList() {}

	void AddTail(CXaraFilePathRecordRefListItem* pItem)								{ List::AddTail(pItem); };
	CXaraFilePathRecordRefListItem* GetHead()										{ return (CXaraFilePathRecordRefListItem*)List::GetHead(); }
	CXaraFilePathRecordRefListItem* GetTail()										{ return (CXaraFilePathRecordRefListItem*)List::GetTail(); }
	CXaraFilePathRecordRefListItem* GetNext(CXaraFilePathRecordRefListItem* pItem)	{ return (CXaraFilePathRecordRefListItem*)List::GetNext(pItem); }
	CXaraFilePathRecordRefListItem* GetPrev(CXaraFilePathRecordRefListItem* pItem)	{ return (CXaraFilePathRecordRefListItem*)List::GetPrev(pItem); }
};

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

/***********************************************************************************************

>	class CXaraFileTextStoryGroupRefListItem : public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/96
	Purpose:	An item that appears in the CXaraFileTextStoryGroupRefList

				The group node that is in this object is assumed to be deletable and not in the tree.
				The destructor will CascadeDelete() the group, then delete the group itself

***********************************************************************************************/

// WEBSTER - markn 31/1/97
// Replaced with general system
/*
class CXaraFileTextStoryGroupRefListItem : public ListItem
{
CC_DECLARE_MEMDUMP(CXaraFileTextStoryGroupRefListItem)

public:
	CXaraFileTextStoryGroupRefListItem(TextStory* pStory,NodeGroup* pGroup) { pTextStory = pStory; pNodeGroup = pGroup; }

	// NOTE! The destructor deletes the group and all it's children
	~CXaraFileTextStoryGroupRefListItem();

	TextStory*	GetTextStory()	{ return pTextStory; }
	NodeGroup*	GetNodeGroup()	{ return pNodeGroup; }

private:
	TextStory* pTextStory;
	NodeGroup* pNodeGroup;
};
*/

/***********************************************************************************************

>	class CXaraFileTextStoryGroupRefList : public List

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/96
	Purpose:	Holds a list of text stories and corresponding group objects.

				When automatically converting text to outlines during export of the web format,
				an out-of-tree group of paths is generated for each text story converted to outlines.

				This list allows the group of paths to be generated once during the export process
				(instead of once for calc of file size, then again for actual output)
				It also allows the paths to exist for the entire export lifetime, so that the paths
				can safely be used during export for other processes (such as the smart path similarity
				code that detects paths that differ only by a transformation).

***********************************************************************************************/

// WEBSTER - markn 31/1/97
// Replaced with general system
/*
class CXaraFileTextStoryGroupRefList : public List
{
CC_DECLARE_MEMDUMP(CXaraFileTextStoryGroupRefList)

public:
	CXaraFileTextStoryGroupRefList() {}

	void AddTail(CXaraFileTextStoryGroupRefListItem* pItem)									{ List::AddTail(pItem); };
	CXaraFileTextStoryGroupRefListItem* GetHead()											{ return (CXaraFileTextStoryGroupRefListItem*)List::GetHead(); }
	CXaraFileTextStoryGroupRefListItem* GetNext(CXaraFileTextStoryGroupRefListItem* pItem)	{ return (CXaraFileTextStoryGroupRefListItem*)List::GetNext(pItem); }
};
*/

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

/***********************************************************************************************

>	class CXaraFileNodeGroupRefListItem : public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/97
	Purpose:	An item that appears in the CXaraFileNodeGroupRefList

				The group node that is in this object is assumed to be deletable and not in the tree.
				The destructor will CascadeDelete() the group, then delete the group itself

  				WEBSTER - markn 29/1/97
				Part of the general form of the system used to convert text to outlines in v1.5

***********************************************************************************************/

class CXaraFileNodeGroupRefListItem : public ListItem
{
CC_DECLARE_MEMDUMP(CXaraFileNodeGroupRefListItem)

public:
	CXaraFileNodeGroupRefListItem(Node* pThisNode,NodeGroup* pGroup) { pNode = pThisNode; pNodeGroup = pGroup; }

	// NOTE! The destructor deletes the group and all it's children
	~CXaraFileNodeGroupRefListItem();

	Node*		GetNode()	{ return pNode; }
	NodeGroup*	GetNodeGroup()	{ return pNodeGroup; }

private:
	Node*		pNode;
	NodeGroup*	pNodeGroup;
};

/***********************************************************************************************

>	class CXaraFileNodeGroupRefList : public List

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/97
	Purpose:	Holds a list of nodes and corresponding group objects.

				When automatically converting a node to outlines during export of the web format,
				an out-of-tree group of paths is generated for each node converted to outlines.

				This list allows the group of paths to be generated once during the export process
				(instead of once for calc of file size, then again for actual output)
				It also allows the paths to exist for the entire export lifetime, so that the paths
				can safely be used during export for other processes (such as the smart path similarity
				code that detects paths that differ only by a transformation).

  				WEBSTER - markn 29/1/97
				Part of the general form of the system used to convert text to outlines in v1.5

***********************************************************************************************/

class CXaraFileNodeGroupRefList : public List
{
CC_DECLARE_MEMDUMP(CXaraFileNodeGroupRefList)

public:
	CXaraFileNodeGroupRefList() {}

	void AddTail(CXaraFileNodeGroupRefListItem* pItem)								{ List::AddTail(pItem); };
	CXaraFileNodeGroupRefListItem* GetHead()										{ return (CXaraFileNodeGroupRefListItem*)List::GetHead(); }
	CXaraFileNodeGroupRefListItem* GetNext(CXaraFileNodeGroupRefListItem* pItem)	{ return (CXaraFileNodeGroupRefListItem*)List::GetNext(pItem); }
};

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

/********************************************************************************************

>	class InsertLevelStackItem : public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/96
	Purpose:	This item holds all the info for a given insert level of the tree.
				It is used when building the tree on import

				Use it on conjunction with the InsertLevelStack class

********************************************************************************************/

class InsertLevelStackItem : public ListItem
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(InsertLevelStackItem);

public:
	InsertLevelStackItem(BaseCamelotFilter* pFilter);

	UINT32 GetNumNodesInserted() { return NumNodesInserted; }

	BOOL  SetLastInsertedNode(Node* pNode);
	Node* GetLastInsertedNode()				{ return pLastInsertedNode; }

	void SetLastHandler(CXaraFileRecordHandler* pHandler, UINT32 Tag) { pLastHandler = pHandler; LastHandledTag = Tag; }
	CXaraFileRecordHandler* GetLastHandler()	{ return pLastHandler; }
	UINT32					GetLastHandledTag()	{ return LastHandledTag; }

	BOOL InformLastInsertedNodeChildrenComplete();

private:
	void IncNumNodesInserted() { NumNodesInserted++; }

	UINT32					NumNodesInserted;
	Node*					pLastInsertedNode;
	CXaraFileRecordHandler* pLastHandler;
	UINT32					LastHandledTag;
	BaseCamelotFilter*		pFilter;
	BOOL					LastNodeInformedChildrenComplete;
};

/***********************************************************************************************

>	class InsertLevelStack : public List

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/96
	Purpose:	Holds a list of level items
				The class gives the list a stack-like interface

				Each item holds info on a particular level during import.

				The list is actually treated as a stack, where the tail item is the item on the
				top of the stack, and items are only added and removed from the tail of the list

***********************************************************************************************/

class InsertLevelStack : public List
{
CC_DECLARE_MEMDUMP(InsertLevelStack)

public:
	InsertLevelStack();
	~InsertLevelStack();

	void Add(InsertLevelStackItem* pItem);
	InsertLevelStackItem* Get();
	InsertLevelStackItem* Pop();

private:
	UINT32 InsertLevel;
};

