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

// Handles lists of Tools loaded from Modules

#include "camtypes.h"
#include "toollist.h"
//#include "ensure.h"
//#include "mainfrm.h"
#include "errors.h"
#include "tool.h"
#include "opdesc.h"
#include "dlgmgr.h"
#include "bars.h"
//#include "basebar.h"
#include "ink.h"
#include "keypress.h"
#include "app.h"
#include "blobs.h"
#include "toolmsg.h"

#define SELECTED TRUE
#define DESELECTED FALSE

CC_IMPLEMENT_DYNAMIC(ToolsCurrentAttrGroupChangedMsg, Msg);


/********************************************************************************************

>	class TempToolItem : public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/94
	Purpose:	Item that represent a temp tool selection
	Errors:		-
	SeeAlso:	Tool::ActivateTemporaryTool(), Tool::DeactivateTemporaryTool()


********************************************************************************************/

class TempToolItem : public ListItem
{
CC_DECLARE_MEMDUMP(TempToolItem)
public:
	TempToolItem() { pTempTool = pPrevTool = NULL; }

	Tool* 	GetTempTool() 					{ return pTempTool; }
	void	SetTempTool(Tool* pThisTool) 	{ pTempTool = pThisTool; }

	Tool* 	GetPrevTool() 					{ return pPrevTool; }
	void	SetPrevTool(Tool* pThisTool) 	{ pPrevTool = pThisTool; }

private:
	Tool* pTempTool;
	Tool* pPrevTool;
};


//-------------------------------------------------------------

/********************************************************************************************

>	class ToolSwitch : public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/94
	Purpose:	Holds a key press that will temporarily switch to another tool
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

class ToolSwitch : public ListItem
{
CC_DECLARE_MEMDUMP(ToolSwitch)

public:
	ToolSwitch(Tool* pThisTool,KeyPress* pKeyPress,BOOL FromFile);
	~ToolSwitch();

	Tool* 		pTool;				// The tool
	KeyPress* 	pKeyPress;			// The key press that will activate the tool

	BOOL		Temp		: 1;	// TRUE if this switch is temporary
	BOOL		DefFromFile : 1;	// TRUE if defined from a file

	static ToolSwitch* pCurrentToolSwitch;
};

// The ToolSwitch that is currently active
ToolSwitch* ToolSwitch::pCurrentToolSwitch = NULL;

// List of all the tool switching key combinations for all the tools
List ToolSwitchList;


/********************************************************************************************

>	ToolSwitch::ToolSwitch(Tool* pThisTool, KeyPress* pThisKeyPress, BOOL FromFile)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/94
	Inputs:		pThisTool 		= the tool this key press will activate
				pThisKeyPress 	= ptr to the key press combination that activates the tool
				FromFile		= TRUE if defined from a file, FALSE if a direct definition (usu. direct from a tool) 
	Outputs:	-
	Returns:	-
	Purpose:	The constructor.  Creates a ToolSwitch object that represents a key press combination
				that invokes the given tool
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ToolSwitch::ToolSwitch(Tool* pThisTool, KeyPress* pThisKeyPress, BOOL FromFile)
{
	pTool 		= pThisTool;
	pKeyPress 	= pThisKeyPress;
 	Temp		= FALSE;
	DefFromFile = FromFile;
}

/********************************************************************************************

>	ToolSwitch::~ToolSwitch()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Inputs:		0
	Outputs:	-
	Returns:	-
	Purpose:	Default destructor.
				Main job is to delete the key press object it constains
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ToolSwitch::~ToolSwitch()
{
	if (pKeyPress != NULL)
	{
		delete pKeyPress;
		pKeyPress = NULL;
	}
}

//-------------------------------------------------------------

// Runtime class support for debug builds
CC_IMPLEMENT_MEMDUMP(ToolInfo_v1, CCObject)
CC_IMPLEMENT_MEMDUMP(ToolInfo, ToolInfo_v1)
CC_IMPLEMENT_MEMDUMP(Tool_v1, CCObject)
CC_IMPLEMENT_MEMDUMP(Tool, Tool_v1)
CC_IMPLEMENT_MEMDUMP(ToolSwitch,ListItem)
CC_IMPLEMENT_MEMDUMP(TempToolItem,ListItem);


// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


DECLARE_SOURCE("$Revision: 662 $");

ToolList *Tool::Tools = NULL;

Tool* 	Tool::Current  = NULL;	  				// Pointer to the current tool
//Tool* 	Tool::PrevTool = NULL;	  				// Pointer to the temporarily deselected tool when a
								  				// tool is temporarily active.
BOOL  	Tool::ToolChanging = FALSE;				// TRUE while the current tool is changing
BOOL	Tool::PendingDeactivateTempTool = FALSE;// TRUE while waiting to deactivate the temporary tool after a drag

List	Tool::TempToolList;						// Holds a record per stacked temp tool

UINT32 	Tool::InitTool; 						// Used as a pref - maintains current tool id


/********************************************************************************************

>	ToolInfo_v1::ToolInfo_v1()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	For specifying default tool structure values 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ToolInfo_v1::ToolInfo_v1()
{		   
	// When specifying current attributes most of the time we will want to add the
	// attribute to the NodeRenderableInk current attribute group.
	CurrentAttributeGroup = CC_RUNTIME_CLASS(NodeRenderableInk); 
}

/********************************************************************************************

>	BOOL Tool::InitToolList()

	Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if list initialised ok, FALSE otherwise.
	Purpose:	Initialise the static ToolList object which holds all tools that the kernel
				knows about.
	Errors:		Assert if list is already initialised.

********************************************************************************************/

BOOL Tool::InitToolList()
{
	ENSURE(Tools == NULL, "Attempt to initialise the ToolList more than once");
	Tools = new ToolList;
	return (Tools != NULL);
}



/********************************************************************************************

>	BOOL Tool::InitTools()

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if OK, FALSE for catastrophic failure
	Purpose:	Called at the end of initialisation (as it may well need all other 
				initialisations to have been done) to add Tools and Modules to the Kernel's
				knowledgebase.
	Errors:		None
	Scope:		Public

********************************************************************************************/

BOOL Tool::InitTools()
{
	BOOL Finished;
	
	do
	{
		// All the tools have been 'new'ed now. We can now ask them to initialise themselves.
		// If they don't want to, they get 'delete'ed.

		ToolListItem *Item = (ToolListItem *) Tools->GetHead();
		
		Finished = TRUE;
		
		while (Item != NULL)
		{
			Tool *Tool = Item->m_pTool;
			
			// If this tool exists and has not been initialised yet, then try to
			// initialise it.  If it initialises ok, set the item's flag, and force
			// another scan of the remaining tools in case one of them depends on
			// this tool being present.
			if( NULL != Tool && 
				!Item->m_fInitialised && 
				Tool->Init() )
			{
				Item->m_fInitialised = TRUE;	// Tool initialised ok.
				Finished = FALSE;			// At least one more loop to try
			}

			// Try the next tool
			Item = (ToolListItem *) Tools->GetNext(Item);
		}
		
	} while (!Finished);

	// Delete any un-initialised tools
	ToolListItem *Item = (ToolListItem *) Tools->GetHead();

	while (Item != NULL)
	{
		ToolListItem *NextItem = (ToolListItem*) Tools->GetNext(Item);

		if ((Item->m_pTool != NULL) && (!Item->m_fInitialised))
		{
			// the Tool didn't want to play, so remove it
			
			// we've already got the next item, so its OK to kill the old one

			delete Tools->RemoveItem( Item );				// remove from list & free up
		}

		// Try the next tool
		Item = NextItem;
	}

	
	// default to Selector tool in case we cant find a pref
	InitTool = TOOLID_SELECTOR;
	
	// declare a pref
	if( Camelot.DeclareSection( _T("InitialTool"), 1 ) )
		Camelot.DeclarePref( _T("InitialTool"), _T("InitTool"), &InitTool );
				
	return TRUE;									// no error return possible at present
}



/********************************************************************************************
>	static void Tool::SelectFirstTool()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Selects the tool that is retrieved from the prefs file.
				If no pref is found will default to Selector Tool -
				If the appropriate tool can't be found the first will be used instead
	Errors:		-
	SeeAlso:	Tool::InitTools
********************************************************************************************/

void Tool::SelectFirstTool()
{	

// look for the initial tool
ToolListItem* pItem = (ToolListItem*) Find(InitTool);

// if we can't find it go for the first
if(pItem == NULL)
  	pItem = (ToolListItem*) Tools->GetHead();

if (pItem != NULL && pItem->m_pTool != NULL) pItem->m_pTool->SetCurrent();

}



/********************************************************************************************

>	BOOL Tool::Declare(Tool *NewTool, UINT32 ParentID)

	Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/93
	Inputs:		NewTool - Pointer to a new Tool and its ID
				ParentID - Module ID of the module that provides this tool.
	Outputs:	None
	Returns:	Pointer to new tool list item.
	Purpose:	Called by OIL code to declare new tool types.
	Errors:		Will return NULL if invalid ID or same tool ID used twice.

********************************************************************************************/

ToolListItem *Tool::Declare(Tool *NewTool, UINT32 ParentID)
{
	// Try to add the tool to the kernel's list.
	ToolListItem *NewItem = Tools->Add(NewTool, ParentID);
	
	if (NewItem == NULL)
	{
		// Tool has invalid ID - print error message and destroy tool.
		ToolInfo Info;
		NewTool->Describe(&Info);
		TRACE( _T("Tool %u is invalid\n"), Info.ID);
		delete NewTool;
	}
	
	return NewItem;
}



/********************************************************************************************

>	static UINT32 Tool::GetNumTools()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Returns:	The number of tools registered with the kernel.
	Purpose:	Find out how many tools have registered and initialised successfully.

********************************************************************************************/

UINT32 Tool::GetNumTools()
{
	if (Tools == NULL)
		return 0;
	else
		return (UINT32) Tools->GetCount();
}



/********************************************************************************************

>	Tool *Tool::Find(UINT32 id)

	Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/93
	Inputs:		Tool ID
	Outputs:	None
	Returns:	Pointer to required tool, or NULL if not found.
	Purpose:	Anyone can ask about a tool's presence by its ID.
	Errors:		None

********************************************************************************************/

ToolListItem *Tool::Find(UINT32 ToolID)
{
	// Simple search of list for ID
	
	ToolListItem *Item = (ToolListItem *) Tools->GetHead();
	
	while (Item != NULL)
	{
		if (Item->m_ToolInfo.ID == ToolID)
			// Found ID - return pointer to its tool.
			return Item;
		
		Item = (ToolListItem *) Tools->GetNext(Item);
	}
	
	// Not found
	return NULL;
}



/********************************************************************************************

>	Tool* Tool::FindTool(UINT32 id)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/2004
	Inputs:		Tool ID
	Outputs:	None
	Returns:	Pointer to required tool, or NULL if not found.
	Purpose:	Anyone can ask about a tool's presence by its ID.
				But Geez! The caller doesn't want to know about ToolListItems!!!
	Errors:		None

********************************************************************************************/

Tool* Tool::FindTool(UINT32 ToolID)
{
	ToolListItem* pItem = Find(ToolID);
	if (pItem)
		return pItem->m_pTool;

	return NULL;
}



/********************************************************************************************

>	static UINT32 Tool::GetModuleID(UINT32 ToolID)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/8/93
	Inputs:		Tool ID
	Outputs:	None
	Returns:	Module ID of parent module, or 0 if not found.
	Purpose:	Particulary useful for error handling, it allows the owner module ID to be
				determined given just a tool ID.
	Errors:		0 if cannot be found.

********************************************************************************************/

UINT32 Tool::GetModuleID(UINT32 ToolID)
{
	if ( ToolID == TOOLID_INVALID ) return 0;				// If unknown tool do nothing!

	const ToolListItem *Tool = Tool::Find(ToolID);

	if (Tool)
		return Tool->m_ToolInfo.ParentModuleID;				// return ID if found
	else
		return 0;											// 0=not found
}

/********************************************************************************************

>	void  Tool::SetButtons(BOOL State) 

	Author: 	Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/94
	Inputs:		State to set buttons to
	Outputs:	None
	Returns:	None
	Purpose:	Selects/Deselects the buttons associated with this buttons.
	Errors:		None

********************************************************************************************/

void  Tool::SetButtons(BOOL State) 
{
#if !defined(EXCLUDE_FROM_RALPH)
		String OpToken;
		tsprintf(OpToken, 16, _T("TOOL%u"), GetID());
		ToolOpDescriptor* ToolOp = (ToolOpDescriptor *) OpDescriptor::FindOpDescriptor(OpToken);

		// Keep Select state in the OpDescriptor
		ToolOp->SetCurrentTool(State);
		
		List * pGadgetList = new List;
		if (pGadgetList == 0)
		{
			InformError();
			return;
		}
		//Build list of all controls linked to this Op
		if (!ToolOp->BuildGadgetList(pGadgetList))
		{
			TRACEUSER( "Chris", _T("Couldn't build gadget list in ")
					  "Tool::SetCurrent\n");
			delete pGadgetList;
			return;
		}
		ListItem* pListItem = pGadgetList->GetHead();
		while (pListItem != NULL)
		{
			// Set the Select State  of each gadget 
			GadgetListItem* pGadgetItem = (GadgetListItem*) pListItem;
			pGadgetItem->pDialogOp->SetLongGadgetValue(pGadgetItem->gidGadgetID,
															State,
															FALSE,
															FALSE);
			// Do the next control in the list, if any.
			pListItem = pGadgetList->GetNext(pListItem);
		}

		// Don't forget to delete the list afterwards!
		pGadgetList->DeleteAll();
		delete pGadgetList;
#endif
}

/********************************************************************************************

>	void  Tool::SetCurrent() 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/93
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Sets the 'this' tool to be the current tool.
	Errors:		None

********************************************************************************************/

void  Tool::SetCurrent() 
{
   	if(Current == this)
	{ 
		// Ensure the buttons are set right
		Current->SetButtons(SELECTED);
  		return ;
	}

	SetToolChangingState(TRUE);
	// Deselect current tool
 
	
	if (Current)
	{
		Current->SetButtons(DESELECTED);
		Current->SelectChange(FALSE);
		OpDescriptor::RemoveAllAliases();
	}

	// Find the CurrentAttributeGroup associated with the Tool we are switching away from
	CCRuntimeClass* OldCurrentAttrGroup = NULL; 
	if (Current)
		OldCurrentAttrGroup = Current->Parent->m_ToolInfo.CurrentAttributeGroup;

	// Select new tool.
	Current = this;
	
	 SetButtons(SELECTED);	

	// Inform tool that it has been selected.
	SelectChange(TRUE);
   	
	// save ID in preference
   	InitTool = GetID();

	SetToolChangingState(FALSE);

	TempToolItem* pTempToolItem = (TempToolItem*)TempToolList.GetHead();
	if (pTempToolItem != NULL && pTempToolItem->GetTempTool() != Current)
		DeleteTempToolList();

	// Finally if the CurrentAttributeGroup has changed then tell people
	if (Parent->m_ToolInfo.CurrentAttributeGroup != OldCurrentAttrGroup)
	{
		BROADCAST_TO_ALL(ToolsCurrentAttrGroupChangedMsg); 
	}
}



/********************************************************************************************

>	void Tool::DeinitTools()

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Deletes all allocated tools. Safe to call more than once.
	
				Do NOT try to use any tools after calling this.

	Errors:		None

********************************************************************************************/

void Tool::DeinitTools()
{
	if (Tools != NULL)
	{
		// Deselect the current tool, if there is one.
		if (Current)
		{
			Current->SelectChange(FALSE);
			Current = 0;
		}

		// Delete the tool list.
		delete Tools;
		Tools = NULL;
	}

	ToolSwitchList.DeleteAll();
	DeleteTempToolList();
}



/********************************************************************************************

>	OILTool *Tool::GetOILTool(UINT32 ID)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		ID - The ID of the Tool
	Outputs:	-
	Returns:	Pointer to the OILTool object for this tool.
	Purpose:	Given a Tool ID, returns a pointer to the OILTool object associated
				with this tool.
	Errors:		Returns NULL if the associated OILTool object does not exist (maybe
				the tool ID is invalid?)
	SeeAlso:	ToolListItem

********************************************************************************************/

OILTool *Tool::GetOILTool(UINT32 ID)
{
	/*ToolListItem *Item =*/ Find(ID);
//	return Item ? Item->m_pOILTool : NULL;
	return NULL;
}


/********************************************************************************************

>	void Tool::ActivateTemporaryTool(Tool *pTempTool)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/05/94
	Inputs:		pTempTool - the tool to be made active.
	Purpose:	Temporarily invoke the specified tool.  This is called when the user
				holds down modifier keys to switch tools - this function remembers the
				currently active tool and will switch back to it when you call
				Tool::DeactivateTemporaryTool().
				It is ok to keep calling this without first deactivating the temporary
				tool - the system will correctly remember which tool was originally
				active and restore it when all DeactivateTemporaryTool() is called.
	SeeAlso:	Tool::DeactivateTemporaryTool

********************************************************************************************/

void Tool::ActivateTemporaryTool(Tool *pTempTool)
{
	// If the current tool is not temporary, then remember it so we can revert
	// to it when the user lets go of the hot-keys.

	// NB. We have to make this test so if the user invokes one tool, then another in such
	// a way that the original tool si not switched back to inbetween, then things will
	// go wrong and we won't return to the original tool.

	TempToolItem* pTempToolItem = new TempToolItem;
	if (pTempToolItem != NULL)
	{
		pTempToolItem->SetPrevTool(Current);
		pTempToolItem->SetTempTool(pTempTool);
		TempToolList.AddHead(pTempToolItem);

//		if (PrevTool == NULL)
//			PrevTool = Current;
		//Current->SelectChange(FALSE);
		// Find the instance of the ToolOpDescriptor
		String_64		OpToken;
		tsprintf( OpToken, 64, _T("TOOL%u"), pTempTool->GetID() );
		ToolOpDescriptor* ToolOp = (ToolOpDescriptor *) OpDescriptor::FindOpDescriptor(OpToken);

		// Use the OpDescriptor to invoke the tool (i.e. make it become the selected tool)
		ToolOp->Invoke();
	}
}

/********************************************************************************************

>	void Tool::DeactivateTemporaryTool()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/05/94
	Purpose:	Deactivate the temporary tool and restore the original one.
	SeeAlso:	void Tool::DeactivateTemporaryTool()

********************************************************************************************/

void Tool::DeactivateTemporaryTool()
{
/*	// Do nothing if we don't have a previous tool
	if (PrevTool == NULL) return;

	// If the previous tool is the current one, don't have to do nowt
	if (PrevTool == Current) return;
*/
	TempToolItem* pTempToolItem = (TempToolItem*)TempToolList.RemoveHead();

	// Only do something if we have a temp tool item
	if (pTempToolItem != NULL)
	{
		UINT32 PrevToolID = pTempToolItem->GetPrevTool()->GetID();

		// We don't need this temp tool item any more.
		delete pTempToolItem;
		pTempToolItem = NULL;

		// Find the instance of the ToolOpDescriptor
		String_64		OpToken;
		tsprintf( OpToken, 64, _T("TOOL%u"), PrevToolID );
		ToolOpDescriptor* ToolOp = (ToolOpDescriptor *) OpDescriptor::FindOpDescriptor(OpToken);

		// Use the OpDescriptor to invoke the tool (i.e. make it become the selected tool)
		ToolOp->Invoke();
	}


	// Not temporary any more
//	PrevTool = NULL;
}


/********************************************************************************************

>	void Tool::DeleteTempToolList()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/94
	Returns:	-
	Purpose:	This deletes all temp tool items in the list temp tools.
				After this call, the temp tool stacking info is lost.
				This should be called whenever a user selects a new tool directly (i.e. via clicking on
				the tool icon, a hot key, or menu item).
	SeeAlso:	Tool::ActivateTemporaryTool; Tool::DeactivateTemporaryTool

********************************************************************************************/

void Tool::DeleteTempToolList()
{
	TempToolList.DeleteAll();
}

/********************************************************************************************

>	BOOL Tool::IsCurrentToolTemporary()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/05/94
	Returns:	TRUE if the tool is temporary;
				FALSE if not.
	Purpose:	Check to see if the currently active tool is permanently or temporarily
				active. If, e.g. the user is holding down ALT to get the selector tool,
				then this returns TRUE.
	SeeAlso:	Tool::ActivateTemporaryTool; Tool::DeactivateTemporaryTool

********************************************************************************************/

/*
BOOL Tool::IsCurrentToolTemporary()
{
	// If PrevTool is not NULL, then we are waiting for the user to let go of the
	// modifier keys so we can change back to the original tool.
	return (PrevTool != NULL);
}
*/

/********************************************************************************************

>	void Tool::SetToolChangingState(BOOL state)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/05/94
	Inputs:		state - boolean used to set ToolChanging flag
	Outputs:	-
	Returns:	-
	Purpose:	Lets you set the ToolChanging flag
	SeeAlso:	-

********************************************************************************************/

void Tool::SetToolChangingState(BOOL state)
{
	ToolChanging = state;
}

/********************************************************************************************

>	BOOL Tool::IsToolChanging()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/05/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE  - tool is in the middle of changing
				FALSE - oh no it's not!
	Purpose:	Lets you see the state of the ToolChanging flag
	SeeAlso:	-

********************************************************************************************/

BOOL Tool::IsToolChanging()
{
	return (ToolChanging);
}

/********************************************************************************************

>	static BOOL Tool::SwitchTool(KeyPress* pKeyPress)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/94
	Inputs:		pKeyPress = ptr to the key press that's just occurred
	Outputs:	-
	Returns:	TRUE  - the key press caused a tool switch, do not pass on
				FALSE - no tool switching, see if anyone else wants it
	Purpose:	The place where tool switching takes place.
	SeeAlso:	-

********************************************************************************************/

BOOL Tool::SwitchTool(KeyPress* pKeyPress)
{
	BOOL Processed = FALSE;

	BOOL DuringADrag = (Operation::GetCurrentDragOp() != NULL);
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	DuringADrag = (DuringADrag || BaseBar::IsDragging());
#endif

	ToolSwitch* pToolSwitch = (ToolSwitch*)ToolSwitchList.GetHead();
	while (pToolSwitch != NULL && !Processed)
	{
		// Is it a tool switch key press?
		if (*pToolSwitch->pKeyPress == *pKeyPress)
		{
			// This key press is a tool switch combination for a tool

			// Are we waiting to deactivate the temp tool at the end of a drag?
			if (!PendingDeactivateTempTool)
			{
				// We are NOT wait to deactivate the temp tool at the end of a drag, so proceed

				// pTool is the ptr to the tool in question
				Tool* pTool = pToolSwitch->pTool;

				// Is it a "key down" key press?
				if (!pKeyPress->IsRelease())
				{
					// It's a "key down" event

					// Is it the first "key down", or has it auto-repeated?
					if (!pKeyPress->IsRepeat())
					{
						// It's the first press

						// is it already the current tool?
						if (pTool != Current)
						{
							// We now have the first "key down" of a tool switch for a tool that
							// is NOT the current tool
							
							// is a drag happening?
							if (!DuringADrag)
							{
								// If we are not doing this during a drag, make it the current tool.
								// Also, set the Temp flag to FALSE as it may be a permanent change
								ActivateTemporaryTool(pTool);
								pToolSwitch->Temp = FALSE;
							}
						}
						else
						{
							// We now have the first "key down" of a tool switch for a tool that
							// IS the current tool
							
							// If we are not dragging, deactivate the tool
							if (!DuringADrag)
								DeactivateTemporaryTool();
							else
								// Remember to deactivate the tool after the drag has ended
								PendingDeactivateTempTool = TRUE;
						}
					}
					else
					{
						// It's an auto-repeated key press

						// if the switch belongs to the current tool, and is auto-repeated
						// make the switch temporary 
						if (pTool == Current)
							pToolSwitch->Temp = TRUE;
					}
				}
				else
				{
					// It's a "key up" event

					// Does it belong to the current tool?
					if (pTool == Current)
					{
						// "Key up" on a tool switch for the current tool.
						// If the current tool was selected temporarily, select the previous current tool
						if (pToolSwitch->Temp)
						{
							// If we are not dragging, deactivate the tool
							if (!DuringADrag)
								DeactivateTemporaryTool();
							else
								// Remember to deactivate the tool after the drag has ended
								PendingDeactivateTempTool = TRUE;
						}
					}
				}
			}

			// The key press object was a tool switching one, so we have processed the key press
			Processed = TRUE;
		}

		// Get the next tool switch in the list
		pToolSwitch = (ToolSwitch*)ToolSwitchList.GetNext(pToolSwitch);
	}

	return Processed;
}

/********************************************************************************************

>	static void Tool::EndDrag()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This is called when a drag ends
	SeeAlso:	ServicePendingToolChange

********************************************************************************************/

void Tool::EndDrag()
{
	ServicePendingToolChange();
}

/********************************************************************************************

>	static void Tool::OnActivateApp(BOOL bActive)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/96
	Inputs:		bActive - this is ignored
	Outputs:	-
	Returns:	-
	Purpose:	This is called when the app's active state changes
	SeeAlso:	ServicePendingToolChange

********************************************************************************************/

void Tool::OnActivateApp(BOOL bActive)
{
	ServicePendingToolChange();
}

/********************************************************************************************

>	static void Tool::ServicePendingToolChange()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This is called when a drag ends, or the app's active state changes
				This will deactivate the temporary tool if a deactivate key event was encounted
				during a drag
	SeeAlso:	EndDrag(), OnActivateApp()

********************************************************************************************/

void Tool::ServicePendingToolChange()
{
	if (PendingDeactivateTempTool)
	{
		DeactivateTemporaryTool();
		PendingDeactivateTempTool = FALSE;
	}
}

/********************************************************************************************
> static void Tool::RemoveDirectRegToolSwitches(Tool* pThisTool)

    Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:	6/9/94
	Inputs:		pThisTool = the tool in question
    Outputs:    -
    Returns:   	-
    Purpose:    This goes through the list of registered tool switches and removes all
				switches that belong to the given tool and were defined directly, 
				i.e. ones that were NOT defined via a file
	Errors:		-
                   			                                     
********************************************************************************************/

void Tool::RemoveDirectRegToolSwitches(Tool* pThisTool)
{
	ToolSwitch* pToolSwitch;
	ToolSwitch* pNextToolSwitch = (ToolSwitch*)ToolSwitchList.GetHead();

	while (pNextToolSwitch != NULL)
	{
		pToolSwitch     = pNextToolSwitch;
		pNextToolSwitch = (ToolSwitch*)ToolSwitchList.GetNext(pNextToolSwitch);

		if (pToolSwitch->pTool == pThisTool && !pToolSwitch->DefFromFile)
		{
			// if this tool switch belongs to the given tool 
			// and it was a directly defined one, remove it from
			// the list and delete it

			ToolSwitchList.RemoveItem(pToolSwitch);
			delete pToolSwitch;
		}
	}
}


//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// Empty functions for the default tool.

/********************************************************************************************

>	BOOL Tool_v1::Init()

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE if tool doesn't want to initialise.
	Purpose:	Called after tool's constructor to ask it whether it wants to exist or
				not. Each tool is asked in two passes, so that its existence can depend
				on another tool. If a tool does not want to exist, it should return FALSE,
				when it will be deleted.
	Errors:		None

********************************************************************************************/

BOOL Tool_v1::Init()
{
	ENSURE(FALSE, "Init called for base tool class!");
	return FALSE; 					// Should never try to instantiate a tool base class
}



/********************************************************************************************

>	Tool_v1::~Tool_v1()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys the tool, performing any clean-up necessary.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Tool_v1::~Tool_v1()
{
	// Null destructor
}



/********************************************************************************************

>	BOOL Tool_v1::Describe(void *Infoptr)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		Pointer to area to recieve info
	Outputs:	Info area is updated
	Returns:	None
	Purpose:	Asks a tool for more information. The pointer points to a struct such as
				ToolInfo_v1 which the tool should fill in. All fields should be filled
				(NULL is OK for char* variables). The type of the pointer is not explicitly
				defined so the structure can evolve while keeping backward compatibility.
	Errors:		None

********************************************************************************************/

void Tool_v1::Describe(void *InfoPtr)
{
	ENSURE(FALSE, "Describe called for base tool class!");
	
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;
	
	Info->InfoVersion = 1;
	
	Info->InterfaceVersion = GetToolInterfaceVersion();  // You should always have this line.
	
	// These are all garbage as we should never try to create one of these...
	Info->Version = 1;
	Info->ID = TOOLID_INVALID;
	Info->TextID = 0;
	
	// so we never get instantiated
	Info->Family = Info->Name = Info->Purpose = Info->Author = NULL;
}



/********************************************************************************************

> 	void Tool_v1::OnClick(DocCoord PointerPos, ClickType Click,
							ClickModifiers Mods, Spread* pSpread)

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	2/7/93
	Inputs:		Position of click in spread coordinates
				Click type (single, double or drag)
				Modifiers (adjusted, constrained, etc...)
				Pointer to the spread in which the click occurred
    Outputs:    None
    Returns:   	None
    Purpose:    This function should be overridden by any tool that wants to receive clicks
			    on the document. When DocView receives a click event from the OIL layer it
			    finds the current tool and calls this function.
	Errors:		None.
                   			                                     
********************************************************************************************/

void Tool_v1::OnClick(DocCoord /*PointerPos*/, ClickType /*Click*/,
						ClickModifiers /*Mods*/, Spread* /*pSpread*/)
{
	ENSURE(FALSE, "OnClick called for base tool class!");
	
	// DO nothing!
	// Click not handled by base class!
}



/********************************************************************************************
> 	void Tool_v1::OnMouseMove(DocCoord mousepos, Spread* pSpread, ClickModifiers)

    Author: 	Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
    Created:	14 Oct 93
	Inputs:		Position of the mouse, in spread coordinates
				Pointer to the spread in which the mouse lies.
				Which button/keys are down.
    Outputs:    -
    Returns:   	-
    Purpose:    This function should be overridden by any tool that wants to receive mouse
    			move messages, when a drag is NOT in progress.  For example, tools can use
    			this function to change the mouse cursor as it moves over a "hot spot".  The
    			default implementation does nothing.
	Errors:		-
                   			                                     
********************************************************************************************/

void Tool_v1::OnMouseMove(DocCoord, Spread*, ClickModifiers)
{
	// Do nothing at all, if you like.  It's your tool!
}



/********************************************************************************************
> 	BOOL Tool_v1::OnKeyPress(KeyPress* pKeyPress)

    Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:	30/8/94
	Inputs:		pKeyPress = Ptr to a platform-indy key press object
    Outputs:    -
    Returns:   	TRUE  - the key press was processed, so do NOT pass on
				FALSE - not processed, so let others have a bash
    Purpose:    This function should be overridden by any tool that wants to process key presses.
    			The current tool is the first part of Camelot that gets the chance to process
				a key press, so tools can override all tool-switching and hot-key combinations
				if it needs to.
	Errors:		-
                   			                                     
********************************************************************************************/

BOOL Tool_v1::OnKeyPress(KeyPress* pKeyPress)
{
	// Return FALSE so the key press gets passed on to others
	return FALSE;
}



/********************************************************************************************
> 	BOOL Tool_v1::OnIdle()

    Author: 	Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
    Created:	14/09/94
	Inputs:		-
    Outputs:    -
    Returns:   	TRUE  - the tool wants more idle events.
				FALSE - no more idle events are required.
    Purpose:    This function should be overridden by any tool that wants to do processing
    			on idle events.  This processing should be quick (or split into small, quick
    			bits).  Once processing is finished FALSE should be returned (although this
				does not guarentee then function will not be called again).
	Errors:		-
                   			                                     
********************************************************************************************/

BOOL Tool_v1::OnIdle()
{
	// Return FALSE to turn off
	return FALSE;
}

/********************************************************************************************
> 	BOOL Tool_v1::GetStatusLineText(String_256* StatusLineText, Spread* pSpread, DocCoord DocPos, ClickModifiers Mods)

    Author: 	Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
    Created:	14/11/94
    Returns:   	TRUE if returning valid text
    Purpose:    Each tool should override this function to return valid status line text
********************************************************************************************/

BOOL Tool_v1::GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)
{
	return FALSE;	// defaults to no text
}

/********************************************************************************************
> 	BOOL Tool_v1::RegisterToolSwitch(KeyPress* pKeyPress,BOOL FromFile = FALSE)

    Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:	30/8/94
	Inputs:		pKeyPress = Ptr to a platform-indy key press object
				FromFile  = TRUE if the tool switch was defined in a file
							FALSE if it's being registered directly
    Outputs:    -
    Returns:   	TRUE  - the key press has been registered for temporary switching to this tool
				FALSE - failed. Either another tool has this combo, or the keypress is invalid

    Purpose:    This allows you to specify a key press that will be used to switch to the tool
				temporarily.  The pKeyPress should be a valid one created by one of its public constructors.

				If the FromFile flag is TRUE then all registered tool switches for this tool that have been 
				registered directly (i.e. by the tool itself calling this routine with FromFile==FALSE) 
				are deleted.

				The means that tool switches defined in file take priority over directly defined ones, so
				that in the future we may allow the user to choose which key presses he wants to temp tool
				switching. 
				Also means that a tool can have temp switching codes with defs in a file; useful for third
				party tools who can't get hold of the bound in kernel resource file.
	Errors:		-
	SeeAlso:	KeyPress::KeyPress
                   			                                     
********************************************************************************************/

BOOL Tool_v1::RegisterToolSwitch(KeyPress* pKeyPress,BOOL FromFile)
{
	ENSURE(pKeyPress->IsValid(),"Tool trying to register a tool key switch with an invalid key press object");
	if (!pKeyPress->IsValid()) return FALSE;

	Tool* pThisTool = (Tool*)this;

	if (FromFile) Tool::RemoveDirectRegToolSwitches(pThisTool);

	ToolSwitch* pToolSwitch = (ToolSwitch*)ToolSwitchList.GetHead();
	while (pToolSwitch != NULL)
	{
		if (pToolSwitch->pTool == pThisTool)
		{
			// if this tool switch has already been assigned for this tool, return TRUE
			if (*pToolSwitch->pKeyPress == *pKeyPress)
				return TRUE;
		}
		else
		{
			// If this tool switch belongs to another tool, return FALSE
			if (*pToolSwitch->pKeyPress == *pKeyPress)
				return FALSE;
		}

		pToolSwitch = (ToolSwitch*)ToolSwitchList.GetNext(pToolSwitch);
	}

	// At this point we know that it is safe to assign the tool switch key press for this tool

	// Make a new tool switch
	ToolSwitch* pNewToolSwitch = new ToolSwitch(pThisTool,pKeyPress,FromFile);

	if (pNewToolSwitch != NULL)
		ToolSwitchList.AddTail(pNewToolSwitch);

	return (pNewToolSwitch != NULL);
}

/********************************************************************************************

>	virtual BOOL Tool_v1::DragFinished(DragEndType HowEnded)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/94
	Inputs:		HowEnded - Describes how the drag ended (an if it was a drag at all really
	Returns:	TRUE if it is OK for the operation to finish doing the things that it was
				going to do, or FALSE if the tool wants the operation to stop right now.
	Purpose:	Tools that start drag operations can use this function to find out how well
				the drag went if the operation calls this function.

********************************************************************************************/

BOOL Tool_v1::DragFinished(DragEndType HowEnded)
{
	// Do nothing in the base class version of this function.
	// Just let the operation get on with what it was doing
	return TRUE;
}

/********************************************************************************************

>	BOOL Tool_v1::IsCurrent() const

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/94
	Returns:	TRUE if 'this' is the current tool, FALSE if not.
	Purpose:	Centralised way of doing this. Amazing this function didn't exist before.

********************************************************************************************/

BOOL Tool_v1::IsCurrent() const
{
	if (Tool::GetCurrent() == this)
		return TRUE;
	else
		return FALSE;
}

/********************************************************************************************

>	void Tool_v1::RenderToolBlobs()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/93
	Purpose:	This function is called in the main drawing loop to get the tool
				to render its blobs (ie the centre of rotation for the rotation
				tool). Since this is the base class version of this function and
				will be called if the tool does not provide its own RenderToolBlobs
				function, then it can be assumed that the tool does not need to 
				draw anything, so it does nothing.
	SeeAlso:	DocView::RenderOnTop

********************************************************************************************/

void Tool_v1::RenderToolBlobs(Spread *, DocRect*)
{
	// If the tool does not want to draw anything then it need not have a 
	// RenderToolBlobs function and this one will be called instead.
	// This draws nothing
}

/********************************************************************************************

>	void Tool_v1::SelectChange(BOOL IsSelected)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		IsSelected - Indicates whether tool is being selected (TRUE) or
							 deselected (FALSE).
	Purpose:	Used to inform a tool that it is being selected or deselected.  Default
				behaviour is to do nothing.  A real tool should update the infobar when the 
				tool is selected - this is usually all a tool will do.

********************************************************************************************/

void Tool_v1::SelectChange(BOOL /*IsSelected*/)
{
	// Override me!
}




/********************************************************************************************

>	virtual void Tool_v1::CurrentDocViewChange()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/12/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called for the currently-selected tool when the current DocView changes. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

//void Tool_v1::CurrentDocViewChange()
//{
//	// Override me!
//}




/********************************************************************************************

>	virtual void SelectedDocViewChange()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/12/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the selected DocView changes, ie. the user has moved the focus
				to another document view window.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

//void Tool_v1::SelectedDocViewChange()
//{
//	// Override me!
//}



/********************************************************************************************

>	void Tool_v1::UpdateInfoBar()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/93
	Purpose:	Allows the Tool to update the info bar according to its internal settings
				and values.  Usually called when the info bar is first created and when a
				new tool becomes selected.

********************************************************************************************/

/* Removed by MarkN 25/7/94
void Tool_v1::UpdateInfoBar()
{
}
*/


/********************************************************************************************

>	void Tool_v1::HandleInfoBarMessage(CDlgMessage Message, CGadgetID ID)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/11/93
	Inputs:		Message - the message (see kernel\dlgtypes.h).
				ID - the ID of the dialog gadget that generated the message.
	Purpose:	Allows the tool to handle UI events from the info bar.  It is similar to
				the DialogOp::HandleMessage function.
	SeeAlso:	DialogOp::HandleMessage

********************************************************************************************/

// Removed by MarkN 25/7/94
//void Tool_v1::HandleInfoBarMessage(CDlgMessage /* Message*/, CGadgetID /*ID*/)
//{
//}



/********************************************************************************************

>	static ToolListItem* Tool::GetFirstTool()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	19/8/93
	Returns:	A pointer to the first tool if list is not empty and NULL otherwise
	Purpose:	returns the first Tool in the list

********************************************************************************************/
ToolListItem* Tool::GetFirstTool()	
{ 
	if (Tool::Tools)
		return (ToolListItem*) Tool::Tools->GetHead(); 
	else
		return NULL;
}


	
/********************************************************************************************

>	static ToolListItem* Tool::GetNextTool( ToolListItem* CurrTool )

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/93
	Inputs:		pointer to last Tool return by GetFirst ot GetNextDescriptor
	Returns:	A pointer to the first tool if list is not empty and NULL otherwise
	Purpose:	return the next Tool in the list

********************************************************************************************/
ToolListItem* Tool::GetNextTool( ToolListItem* CurrTool )
{ 
	if (Tool::Tools)
		return (ToolListItem*) Tool::Tools->GetNext( CurrTool ); 
	else
		return NULL;
}


// these two static functions used to be in-lines. They were'nt commented in there either

/********************************************************************************************

>	Tool *Tool::GetCurrent()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/94
	Returns:	A pointer to the current tool, or NULL if there isn't one.
	Purpose:	Determine the current tool.
	Scope:		Public static

********************************************************************************************/

Tool *Tool::GetCurrent()
{
	return Current;
}

/********************************************************************************************

>	void Tool::SetNoCurrent()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/94
	Purpose:	Sets no tool as the current tool.
	Scope:		Public static

********************************************************************************************/

void Tool::SetNoCurrent()
{
	if (Current)
	{
		Current->SelectChange(FALSE);
	}
	Current = NULL; 
}

/********************************************************************************************

>	UINT32 Tool::GetCurrentID()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/94
	Returns:	The ID of the current tool, or 0 if none is selected.
	Purpose:	Determine the current tool.
	Scope:		Public static

********************************************************************************************/

UINT32 Tool::GetCurrentID()
{
	if (Current)
		return Current->GetID();
	else
		return 0;
}
