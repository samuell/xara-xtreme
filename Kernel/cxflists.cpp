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

// Implements bits of the stuff defined in cxflists.h

#include "camtypes.h"
//#include "camfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxflists.h"
//#include "group.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxftags.h"

/***********************************************************************************************

>	CXaraFileTextStoryGroupRefListItem::~CXaraFileTextStoryGroupRefListItem();

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/96
	Inputs:		-
	Returns:	-
	Purpose:	Destroys every last fragmented morsal of this pathetic excuse for an object.

				If it has a group, its children are deleted via CascadeDelete(), then the group
				itself is deleted

***********************************************************************************************/

// WEBSTER - markn 31/1/97
// Replaced with general system
/*
CXaraFileTextStoryGroupRefListItem::~CXaraFileTextStoryGroupRefListItem()
{
	if (pNodeGroup != NULL)
	{
		pNodeGroup->CascadeDelete();
		delete pNodeGroup;
		pNodeGroup = NULL;
	}
}
*/

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

/***********************************************************************************************

>	CXaraFileNodeGroupRefListItem::~CXaraFileNodeGroupRefListItem();

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/97
	Inputs:		-
	Returns:	-
	Purpose:	Destroys every last fragmented morsal of this pathetic excuse for an object.

				If it has a group, its children are deleted via CascadeDelete(), then the group
				itself is deleted

  				WEBSTER - markn 29/1/97
				Part of the general form of the system used to convert text to outlines in v1.5

***********************************************************************************************/

CXaraFileNodeGroupRefListItem::~CXaraFileNodeGroupRefListItem()
{
	if (pNodeGroup != NULL)
	{
		pNodeGroup->CascadeDelete();
		delete pNodeGroup;
		pNodeGroup = NULL;
	}
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

/***********************************************************************************************

>	InsertLevelStackItem::InsertLevelStackItem(BaseCamelotFilter* pThisFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/9/96
	Inputs:		pThisFilter = ptr to the filter this item is used by
	Returns:	-
	Purpose:	The default constructor

***********************************************************************************************/

InsertLevelStackItem::InsertLevelStackItem(BaseCamelotFilter* pThisFilter)
{ 
	NumNodesInserted	= 0; 
	pLastInsertedNode	= NULL; 
	pLastHandler		= NULL; 
	LastHandledTag		= TAG_UNDEFINED;
	pFilter				= pThisFilter;
	LastNodeInformedChildrenComplete = FALSE;

	ERROR3IF(pFilter == NULL,"pFilter is NULL");
}

/***********************************************************************************************

>	BOOL InsertLevelStackItem::SetLastInsertedNode(Node* pNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/96
	Inputs:		pNode = ptr to node just inserted into the tree
	Returns:	TRUE if ok, FALSE if a failure has ocurred
	Purpose:	Sets the last inserted node of this tree

***********************************************************************************************/

BOOL InsertLevelStackItem::SetLastInsertedNode(Node* pNode)
{
	ERROR3IF(pNode == NULL,"pNode is NULL");

	if (pNode != NULL)
	{
		if (pNode->FindPrevious() == pLastInsertedNode)
		{
			if (!InformLastInsertedNodeChildrenComplete())
				return FALSE;
		}

		pLastInsertedNode = pNode;
		LastNodeInformedChildrenComplete = FALSE;

		IncNumNodesInserted();			// Inc the number of nodes on this level
	}

	return TRUE;
}

/***********************************************************************************************

>	BOOL InsertLevelStackItem::InformLastInsertedNodeChildrenComplete()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/96
	Inputs:		-
	Returns:	TRUE if ok, FALSE if a failure has ocurred
	Purpose:	Tells the last inserted node that its children are complete

***********************************************************************************************/

BOOL InsertLevelStackItem::InformLastInsertedNodeChildrenComplete()
{
	ERROR3IF(pFilter == NULL,"pFilter is NULL");

	if (!LastNodeInformedChildrenComplete && pLastInsertedNode != NULL && pFilter != NULL)
	{
		if (!pFilter->ReadPostChildren(pLastInsertedNode))
			return FALSE;

		LastNodeInformedChildrenComplete = TRUE;
	}

	return TRUE;
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

/***********************************************************************************************

>	InsertLevelStack::InsertLevelStack();

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/96
	Inputs:		-
	Returns:	-
	Purpose:	The default constructor

***********************************************************************************************/

InsertLevelStack::InsertLevelStack()
{
	InsertLevel = 0;
}

/***********************************************************************************************

>	InsertLevelStack::~InsertLevelStack();

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/96
	Inputs:		-
	Returns:	-
	Purpose:	The default destructor - deletes all the list's items

***********************************************************************************************/

InsertLevelStack::~InsertLevelStack()
{
	DeleteAll();
}

/***********************************************************************************************

>	void InsertLevelStack::Add(InsertLevelStackItem* pItem)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/96
	Inputs:		pItem
	Returns:	-
	Purpose:	Adds the item to the top of the stack

***********************************************************************************************/

void InsertLevelStack::Add(InsertLevelStackItem* pItem)
{
	InsertLevel++;
	AddTail(pItem);
}

/***********************************************************************************************

>	InsertLevelStackItem* InsertLevelStack::Get()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/96
	Inputs:		-
	Returns:	ptr to a InsertLevelStackItem 
	Purpose:	Returns a ptr to the item on the top of the stack, without removing it

***********************************************************************************************/

InsertLevelStackItem* InsertLevelStack::Get()
{
	return (InsertLevelStackItem*)List::GetTail();
}

/***********************************************************************************************

>	InsertLevelStackItem* InsertLevelStack::Pop()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/96
	Inputs:		-
	Returns:	ptr to a InsertLevelStackItem 
	Purpose:	Removes the item from the top of the stack, and returns a ptr to it

***********************************************************************************************/

InsertLevelStackItem* InsertLevelStack::Pop()
{
	if (InsertLevel == 0)
	{
		ERROR3("InsertLevel == 0");
		return NULL;
	}

	InsertLevel--;

	return (InsertLevelStackItem*)List::RemoveTail();
}

