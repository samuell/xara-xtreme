// $Id: modlist.cpp 662 2006-03-14 21:31:49Z alex $
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

#include "module.h"
#include "modlist.h"
#include "oilmods.h"	// for pOILModule.

CC_IMPLEMENT_MEMDUMP(ModuleListItem, ListItem)
CC_IMPLEMENT_MEMDUMP(ModuleList, List)

/********************************************************************************************

>	ModuleListItem(Module *pNewModule, OILModule *pNewOILModule)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		NewModule - The module to fill in the list item with.
				pNewOILModule - pointer to the OILModule object to associate with this
				module.
	Outputs:	-
	Returns:	-
	Purpose:	Create a new ModuleListItem with the module pointer and info fields
				filled in.  The info fields are filled in by interrogating the module.
	Errors:		-

********************************************************************************************/

ModuleListItem::ModuleListItem(Module *pNewModule, OILModule *pNewOILModule)
{
	m_Initialised = FALSE;
	pNewModule->Describe( &m_ModInfo );
	m_pModule = pNewModule;
	m_pOILModule = pNewOILModule;
}

/********************************************************************************************

>	ModuleListItem::~ModuleListItem()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys a ModuleListItem.  Deletes the module contained within it.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ModuleListItem::~ModuleListItem()
{
	delete m_pModule;
	m_pModule = NULL;
	
	if( NULL != m_pOILModule )
	{
		PORTNOTETRACE("other","ModuleListItem::~ModuleListItem - m_pOILModule NOT deleted");
#ifndef EXCLUDE_FROM_XARALX
		delete m_pOILModule;
#endif
		m_pOILModule = NULL;
	}
}

/********************************************************************************************

>	ModuleList::~ModuleList

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys a module list, destroying all modules in the list too.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ModuleList::~ModuleList()
{
	// Call destructors on all objects in the list
	
	ModuleListItem *Item = (ModuleListItem *) RemoveHead();
	
	while (Item != NULL)
	{
		// Delete the list item
		delete Item;
		
		// Try the next item
		Item = (ModuleListItem *) RemoveHead();
	}

}

/********************************************************************************************

>	ModuleListItem *ModuleList::Add(Module *pNewModule, OILModule *pNewOILModule)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		pNewModule - pointer to the module to add into the list.
				pNewOILModule - pointer to the OILModule object to associate with this
				module.
	Outputs:	-
	Returns:	Pointer to the new item in the list if the module was successfully added,
				NULL otherwise.
	Purpose:	Safe addition of modules to the kernel's list of known modules.  A module
				will only be added if its ID has not already been used.
	Errors:		-
	SeeAlso:	ModuleListItem

********************************************************************************************/

ModuleListItem *ModuleList::Add(Module *pNewModule, OILModule *pNewOILModule)
{
	// Construct a tentative list item.
	ModuleListItem *NewItem = new ModuleListItem(pNewModule, pNewOILModule);
	if (NewItem == NULL)
		return NULL;
		
	// Fill in the OILModule pointer with the pointer provided by the caller.
	NewItem->m_pOILModule = pNewOILModule;
	
	// Interrogate module about itself.
	if( NewItem->m_ModInfo.ID == MODULEID_INVALID )
	{
		// Invalid ID - discard new list item and abort.
		NewItem->m_pModule = NULL;							// dont delete the module
		delete NewItem;
		return NULL;
	}
	
	
	// Search list to make sure this ID is not already in use.
	ModuleListItem *Item = (ModuleListItem *) GetHead();
	
	while (Item != NULL)
	{
		if( Item->m_ModInfo.ID == NewItem->m_ModInfo.ID )
		{
			// Found ID already in use - discard new list item and abort.
			NewItem->m_pModule = NULL;						// dont delete the module
			delete NewItem;
			return NULL;
		}
		
		// Try the next node in the list.
		Item = (ModuleListItem *) GetNext(Item);
	}
	
	// The ID is not already in use so add the module to the list.
	AddTail(NewItem);
	return NewItem;
}


