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

#include "toollist.h"
//#include "oiltool.h"
#include "ensure.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_MEMDUMP(ToolListItem, ListItem);
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	ToolListItem(Tool *NewTool, UINT32 ParentID)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		NewTool - Pointer to the tool to fill in the list item with.
				ParentID - Module ID of the module that provides this tool.
	Purpose:	Create a new ToolListItem with the tool pointer and info fields
				filled in.  The info fields are filled in by interrogating the tool.
				A new OILTool object is created for this tool, and linked in to the
				list item.
				The tool is linked to the ToolListItem via the Parent member variable, as
				this means it is easy to get hold of the tool's info (ToolInfo object)
				given a pointer to a tool (otherwise you have to do GetID() and call
				Tool::Find() to get the Tool info, which is a bit daft).

********************************************************************************************/

ToolListItem::ToolListItem(Tool *NewTool, UINT32 ParentID)
{
	m_fInitialised = FALSE;
	NewTool->Describe( &m_ToolInfo );
	m_ToolInfo.ParentModuleID = ParentID;
	m_pTool = NewTool;
	NewTool->Parent = this;
}

/********************************************************************************************

>	ToolListItem::~ToolListItem()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Purpose:	Destructor.  Deletes the associated Tool and OILTool objects.
	Errors:		Asserts if the Tool object is not valid (i.e. NULL)

********************************************************************************************/

ToolListItem::~ToolListItem()
{
	// m_pTool may be NULL if the tool didn't initialise properly, because the Add() function 
	// will 'de-install' the tool from this list item to stop it being deleted
	// prematurely.
	delete m_pTool;

	// m_pOILTool may be NULL if the 'new' failed so don't assert (and our 
	// SimpleCCObject::delete can handle 'delete NULL;').
//	delete m_pOILTool;
}


CC_IMPLEMENT_MEMDUMP(ToolList, List);

/********************************************************************************************

>	ToolList::~ToolList()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys a ToolList object.  Deletes the Tool and OILTool objects contained
				in the list items, and deletes the list items themselves.
	Errors:		-
	SeeAlso:	~ToolListItem

********************************************************************************************/

ToolList::~ToolList()
{
	// Call destructors on all objects in the list
	
	ToolListItem *Item = (ToolListItem *) RemoveHead();
	
	while (Item != NULL)
	{
		// Delete the list item
		delete Item;
		
		// Try the next item
		Item = (ToolListItem *) RemoveHead();
	}
}


/********************************************************************************************

>	ToolListItem *ToolList::Add(Tool *NewTool, UINT32 ParentID)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		NewTool - pointer to the tool to add into the list.
				ParentID - Module ID of the module that provides this tool.
	Outputs:	-
	Returns:	Pointer to the new item in the list if the tool was successfully added,
				NULL otherwise.
	Purpose:	Safe addition of tools to the kernel's list of known tools.  A tool
				will only be added if its ID has not already been used.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ToolListItem *ToolList::Add(Tool *NewTool, UINT32 ParentID)
{
//#pragma message( __LOCMSG__ "ToolList::Add - do nothing" )
//	TRACE( _T("Warning - ToolList::Add called\n") );
//	return NULL;
	// Construct a tentative list item
	ToolListItem *NewItem = new ToolListItem(NewTool, ParentID);
    if (NewItem == NULL)
    	goto AbnormalExit;
	
	// Create a new OIL tool for this node
//	NewItem->m_pOILTool = new OILTool(ParentID);
//	if (NewItem->m_pOILTool == NULL)
//  	goto AbnormalExit;
	
	// Interrogate tool about itself.
/*	if (NewItem->ToolInfo.ID == TOOLID_INVALID)
    	goto AbnormalExit;
	
	
	// Search list to make sure this ID is not already in use.
	// (If I use one line initialisation the compiler complains that the gotos avoid it.)
	ToolListItem *Item;
	Item = (ToolListItem *) GetHead();
	
	while (Item != NULL)
	{
		if (Item->ToolInfo.ID == NewItem->ToolInfo.ID)
			// Found ID already in use - discard new list item and abort.
			goto AbnormalExit;
		
		// Try the next node in the list.
		Item = (ToolListItem *) GetNext(Item);
	}
*/	
	// The ID is not already in use so add the tool to the list.
	AddTail(NewItem);
	return NewItem;
	
	// Abnormal exit - clean up and return a NULL pointer to indicate it didn't work.
AbnormalExit:
	// Set Tool pointer to NULL to avoid it being deleted by the ToolListItem's destructor, because
	// the caller is not expecting this tool to be deleted from under its feet.
	NewItem->m_pTool = NULL;

	// Delete the ToolListItem
	delete NewItem;

	return NULL;
}


