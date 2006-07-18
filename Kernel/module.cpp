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

#include "oilmods.h"
#include "module.h"
#include "modlist.h"
//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "viewrc.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "zoomtool.h"
//#include "pushtool.h"


DECLARE_SOURCE("$Revision$");

ModuleList *Module::Modules = NULL;


CC_IMPLEMENT_MEMDUMP(ModInfo_v1, CCObject)
CC_IMPLEMENT_MEMDUMP(ModInfo, ModInfo_v1)
CC_IMPLEMENT_MEMDUMP(Module_v1, CCObject)
CC_IMPLEMENT_MEMDUMP(Module, Module_v1)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	void Module::InitModules()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE for catastrophic failure
	Purpose:	Called at the end of initialisation (as it may well need all other 
				initialisations to have been done) to add Tools and Modules to the Kernel's
				knowledgebase.
	Errors:		None
	Scope:		Public

********************************************************************************************/

BOOL Module::InitModules()
{
	// Initialise the list of Modules.
	Modules = new ModuleList;
	if (!Modules)
		return FALSE;
		
	if (!OILModule::Init())						// get OIL's list of modules. This works
		return FALSE;							// by calling back to DescribeModule.
												// NB. OILModule::Init does it own error
	BOOL Finished;								// reporting
	
	do
	{
		// All the modules have been 'new'ed now. We can now ask them to initialise themselves.
		// If they don't want to, they get 'delete'ed.

		ModuleListItem *Item = (ModuleListItem *) Modules->GetHead();
		
		Finished = TRUE;
		
		while (Item != NULL)
		{
			Module *Module = Item->m_pModule;
			
			// If this module exists and has not been initialised yet, then try to
			// initialise it.  If it initialises ok, set the item's flag, and force
			// another scan of the remaining modules in case one of them depends on
			// this module being present.
			if ((Module != NULL) && (!Item->m_Initialised) && (Module->Init()))
			{
				Item->m_Initialised = TRUE;	// Tool initialised ok.
				Finished = FALSE;			// At least one more loop to try
			}

			// Try the next module
			Item = (ModuleListItem *) Modules->GetNext(Item);
		}
		
	} while (!Finished);

	// Delete any un-initialised modules
	ModuleListItem *Item = (ModuleListItem *) Modules->GetHead();

	while (Item != NULL)
	{
		if ((Item->m_pModule != NULL) && (!Item->m_Initialised))
		{
			delete Item->m_pModule;
			Item->m_pModule = NULL;
		}

		// Try the next module
		Item = (ModuleListItem *) Modules->GetNext(Item);
	}

	// Scan all the modules and create (but do not initialise) any tools they provide.
	if (!Tool::InitToolList() || !CreateTools())
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	ModuleListItem *Module::Declare(Module *NewModule)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		NewModule - pointer to a new Tool and its ID
				pOILModule - pointer to the OILModule object to be associated with this
				module.
	Outputs:	None
	Returns:	Pointer to new module list item if module.
	Purpose:	Called by OIL code to declare new modules.
	Errors:		Will return NULL if invalid ID or same module ID used twice, and NewModule
				will have been deleted.

********************************************************************************************/

ModuleListItem *Module::Declare(Module *NewModule, OILModule *pOILModule)
{
	// Try to add the module to the kernel's list.
	ModuleListItem *NewItem = Modules->Add(NewModule, pOILModule);
	
	if (NewItem == NULL)
	{
		// Either out of memory or Module has invalid ID - print error message and 
		// destroy module.
		#ifdef _DEBUG
		ModInfo Info;
		NewModule->Describe(&Info);
		TRACE( _T("Module %u is invalid or has no memory to initialise with.\n"), Info.ID );
		#endif
		
		delete NewModule;
	}
	
	return NewItem;
}


/********************************************************************************************

>	ModuleListItem *Module::Find(UINT32 ModuleID)

	Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		Module ID
	Outputs:	None
	Returns:	Pointer to list item of the required module, or NULL if not found.
	Purpose:	Anyone can ask about a module's presence by its ID.
	Errors:		None.

********************************************************************************************/

ModuleListItem *Module::Find(UINT32 ModuleID)
{
	// Simple search of list for ID
	
	ModuleListItem *Item = (ModuleListItem *) Modules->GetHead();
	
	while (Item != NULL)
	{
		if (Item->m_ModInfo.ID == ModuleID)
			// Found ID - return pointer to this list item.
			return Item;
		
		Item = (ModuleListItem *) Modules->GetNext(Item);
	}
	
	// Not found
	return NULL;
}

/********************************************************************************************

>	void Module::DeinitModules()

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Deletes all allocated modules (and hence tools), then asks OIL to tidy up. 
				Safe to call more than once.
				
				Do NOT try to use any tools or modules after calling this.
				
	Errors:		None

********************************************************************************************/

void Module::DeinitModules()
{
	if (Modules != NULL)
	{
		// Delete the list itself.
		delete Modules;
		Modules = NULL;

		// Perform any clean-up required by the OIL layer.
PORTNOTE("other","Removed OILModule usage")
#ifndef EXCLUDE_FROM_XARALX
		OILModule::Deinit();
#endif
	}
}

/********************************************************************************************

>	BOOL Module::CreateTools()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE => Tools created OK.
	Purpose:	Scan all modules known to the kernel, and try to create an instance of each
				tool they contain, and add this tool to the kernel's list.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Module::CreateTools()
{
	// Scan the list of modules
	ModuleListItem *Item = (ModuleListItem *) Modules->GetHead();
	
	while (Item != NULL)
	{
		// Interrogate module
		ModInfo Info;
		Item->m_pModule->Describe(&Info);

		// Try to create an instance of each tool in this module
		for (unsigned tool = 1; tool <= Info.NumTools; tool++)
		{
			Tool *NewTool = (Tool *) Item->m_pModule->CreateTool(tool);
			
			if (NewTool != NULL)
			{
				// Tool was created - add it to the list
				Tool::Declare(NewTool, Info.ID);
				
 			}
		}

		// Try the next module in the list.		
		Item = (ModuleListItem *) Modules->GetNext(Item);
	}
//	Tool* NewTool ;
//	NewTool = (Tool*)new ZoomTool; if ( NewTool ) Tool::Declare(NewTool,1);
//	NewTool = (Tool*)new PushTool; if ( NewTool ) Tool::Declare(NewTool,Info.ID);

	// No error codes yet
	return TRUE;
}



// Empty functions for the default module.

/********************************************************************************************

>	BOOL Module_v1::Init()

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE if module doesn't want to initialise.
	Purpose:	Called after module's constructor to ask it whether it wants to exist or
				not. Each module is asked in two passes, so that its existence can depend
				on another module. If a module does not want to exist, it should return FALSE,
				whereupon it will be deleted.
	Errors:		None

********************************************************************************************/


BOOL Module_v1::Init()
{
	return FALSE; 					// Should never try to instantiate a module base class
}


/********************************************************************************************

>	Module_v1::~Module_v1()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys the module, performing any cleanup necessary.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Module_v1::~Module_v1()
{
	// Null destructor
}

/********************************************************************************************

>	BOOL Module_v1::Describe(void *Infoptr)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		Pointer to area to recieve info
	Outputs:	Info area is updated
	Returns:	None
	Purpose:	Asks a module for more information. The pointer points to a struct such as
				ModInfo_v1 which the module should fill in. All fields should be filled
				(NULL is OK for char* variables). The type of the pointer is not explicitly
				defined so the structure can evolve while keeping backward compatibility.
	Errors:		None

********************************************************************************************/

void Module_v1::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ModInfo_v1 *Info = (ModInfo_v1 *) InfoPtr;
	
	Info->InterfaceVersion = GetInterfaceVersion();  	// You should always have this line.
	
	// These are all garbage as we should never try to create one of these...
	Info->Version = 1;
	Info->ID = MODULEID_INVALID;						// so we never get instantiated
	Info->NumTools = 0;
	Info->Name = Info->Purpose = Info->Author = NULL;
}

/********************************************************************************************

>	void *Module_v1::CreateTool(UINT32 ToolNumber)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		ToolNumber - The number of the tool to create (1 to NumTools)
	Outputs:	-
	Returns:	Pointer to the new instantiation of the desired tool.
	Purpose:	Provide a clean interface from module to kernel code for the creation
				of tools.
	Errors:		-

********************************************************************************************/

void *Module_v1::CreateTool(UINT32)
{
	return NULL; // Should never call this for the base class
}




