// $Id: menuitem.cpp 662 2006-03-14 21:31:49Z alex $
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

/********************************************************************************************

	menuitem.cpp - This class models menu items. It has a similar purpose to the MFC CMenu
	class, but instead of modelling the whole menu it models individual menu items and 
	further more is platform independent.

********************************************************************************************/


#include "camtypes.h"
#include "menuitem.h"
#include "ensure.h"
//#include "mario.h"
#include "errors.h"
#include "oilmenus.h"
#include "hotkeys.h"

DECLARE_SOURCE("$Revision: 662 $");  


const OpState MenuItem::DefaultMenuState( FALSE, TRUE );		// not ticked, but greyed

CC_IMPLEMENT_MEMDUMP(MenuItem, CmdControl)
CC_IMPLEMENT_DYNAMIC(OpMenuParam, OpParam)


/********************************************************************************************

>	MenuItem::MenuItem(	UINT32 parentMenuID,
						OpDescriptor *Owner,
						BOOL separator
					  )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/93
	Inputs:		Parent Menu Identifier
				Pointer to owner OpDescriptor
				Separator flag (defaults to FALSE)
	Outputs:	None
	Returns:	None
	Purpose:	Constructor for the MenuItem Class.	Creates a new menu items and connects
				to an OpDescriptor. The ID of the menu is determined algorithmically.
	Errors:		None

********************************************************************************************/

MenuItem::MenuItem( UINT32 parentMenuID, OpDescriptor* pOwner, BOOL separator )
  : CmdControl(pOwner),
	ParentMenuId(parentMenuID),
	SubMenuItems(0),
	FollowedBySeparator(separator),
	m_nSelectedBitmapID(0),
	m_nUnselectedBitmapID(0)
{

	ENSURE( pOwner, "No owner for MenuItem" );

	pOwner->GetText(&Description, OP_MENU_TEXT);					
	ControlState		= DefaultMenuState;
	OwnerOperation		= pOwner;								// the connection between the
																// menu and an operation
	ControlId 			= GetNextAutomaticMenuID();				// wrong, but temp hack
	pMenuParam			= NULL;
}
	
/********************************************************************************************

>	MenuItem::MenuItem(	const String_256& description, UINT32 parentMenuID,  BOOL separator)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/93
	Inputs:		Parent Menu Identifier
				Constanst String Reference for description of Menu.
				Boolean - Followed By Separator Flag
	Outputs:	None
	Returns:	None
	Purpose:	Constructor for the MenuItem Class.	Used for creating new group menu items. 
	Errors:		None

********************************************************************************************/

MenuItem::MenuItem(const String_256& description, UINT32 parentMenuID, BOOL separator)
  :	ParentMenuId(parentMenuID),
	SubMenuItems(0),
	Description(description),
	FollowedBySeparator(separator),
	m_nSelectedBitmapID(0),
	m_nUnselectedBitmapID(0)
{
	ControlState		= DefaultMenuState;
	ControlId 			= GetNextAutomaticMenuID();
	pMenuParam			= NULL;
}
	
/********************************************************************************************

>	MenuItem::~MenuItem()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/93
	Purpose:	Default destructor.
	Errors:		None

********************************************************************************************/

MenuItem::~MenuItem()
{
	if (SubMenuItems != NULL)
		delete SubMenuItems;

	if (pMenuParam)
	{
		delete pMenuParam;
		pMenuParam = NULL;
	}
}

/********************************************************************************************

>	BOOL MenuItem::IsFollowedBySeparator()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/93
	Inputs:		None
	Outputs:	None
	Returns:	True if MenuItem is followed by a separator in the menu.
				False if MenuItem is not followed by a separator
	Purpose:	
		Identifies if the current MenuItem has a separator following it in the menu to which  
		it belongs.
	Errors:		None

********************************************************************************************/

BOOL MenuItem::IsFollowedBySeparator()
{
	 return FollowedBySeparator;
}

/********************************************************************************************

>	BOOL MenuItem::SetIsFollowedBySeparator(BOOL NewValue)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/97
	Inputs:		NewValue to be applied as the IsFollowedBySeparator flag
	Outputs:	None
	Returns:	the old state of the IsFollowedBySeparator flag.
	Purpose:	Sets a new state for the IsFollowedBySeparator flag. This Identifies if the
				current MenuItem has a separator following it in the menu to which it belongs.
	Errors:		None

********************************************************************************************/

BOOL MenuItem::SetIsFollowedBySeparator(BOOL NewValue)
{
	 BOOL OldValue = FollowedBySeparator;
	 FollowedBySeparator = NewValue;
	 return OldValue;
}

/********************************************************************************************

>	BOOL MenuItem::IsMenuItemGroup()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/93
	Inputs:		None
	Outputs:	None
	Returns:	True if there are any SubMenuItems in current MenuItem 
				False if MenuItem is singular
	Purpose:	Identifies if the current MenuItem is a group.
	Errors:		None

********************************************************************************************/

BOOL MenuItem::IsMenuItemGroup()
{
	return (SubMenuItems == NULL)? FALSE : TRUE;
}

/********************************************************************************************

>	BOOL MenuItem::IsCheckable()

	Author:		Luke_Hart (Xara Group Ltd) <lukeh@xara.com>
	Created:	10/03/06
	Inputs:		None
	Outputs:	None
	Returns:	True if the item should have a check box else it shouldn't
	Errors:		None

********************************************************************************************/

BOOL MenuItem::IsCheckable()
{
	return OwnerOperation->GetOpFlags().fCheckable;
}

/********************************************************************************************

>	virtual void MenuItem::PerformAction()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/93
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Performs the action assoicated with a CmdControl. 
	Errors:		None

********************************************************************************************/

void MenuItem::PerformAction()
{
	ENSURE(OwnerOperation, "Cannot Perform Menu Action with NULL Owner");

	if (pMenuParam==NULL)
	{
		OwnerOperation->Invoke();
	}
	else
	{
		// The OpDescriptor wants more info about which menu item
		// has been invoked...
		// So make a special param for DoWithParam
		PopulateMenuParam();
		OwnerOperation->Invoke(pMenuParam);
	}
}


/********************************************************************************************

>	BOOL MenuItem::UpdateControlState(BOOL AppendHotKeyName = TRUE)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/93
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if state is updated 
				FALSE otherwise
	Purpose:	Obtains the state of a MenuItem. 
	Errors:		None

********************************************************************************************/

BOOL MenuItem::UpdateControlState(BOOL AppendHotKeyName)
{                      
	ENSURE(OwnerOperation, "Cannot update the state of a NULL operation");

	// Reset the Menu text to the Default Menu Description
	OwnerOperation->GetText(&Description, OP_MENU_TEXT);

	// Pass Default Menu Description to GetState
	String_256 	MenuDesc = Description;
	
	// get new menu state
	PopulateMenuParam();
	OpState newState = OwnerOperation->GetOpsState(&MenuDesc, pMenuParam);

	// if a description has been returned
	if (!MenuDesc.IsEmpty())
	{	
		if (newState.Greyed)
		{
			// Check to see that Disabled Message is being returned!
			if (MenuDesc != Description)
			{
				// Set up reason why item is disabled
				String_256 disabledStub(_R(IDS_DISABLED_BECAUSE));
				String_256 menuOption;
				
				OwnerOperation->GetText(&menuOption, OP_MENU_NAME);
			
				WhyDisabled = disabledStub;
				WhyDisabled += MenuDesc;
			}
		}
		else                 
			// Set Description to be 
			Description = MenuDesc;
    }

	if (AppendHotKeyName)
	{
		// Is there a hot key assigned to this menuitem
		HotKey *pHotKey = HotKey::FindHotKey(OwnerOperation);

		// if there is then get menu text description of hot key and attach to
		// end of menu item description.
		if (pHotKey)
		{
			String_256 Str;
			pHotKey->GetTextDesc(&Str);
			Description += String( _T("\t") );
			Description += Str;

		//	Description += *(pHotKey->GetMenuTextDesc());
		}
	}

	// if the state has changed
	if ((newState.Greyed != ControlState.Greyed) || 			// if changed then
		(newState.Ticked != ControlState.Ticked) ||
		(newState.RemoveFromMenu != ControlState.RemoveFromMenu))
	{
		ControlState = newState;								// record new state
		return TRUE;  
	}
	return FALSE;
}

	
/********************************************************************************************

>	void MenuItem::PopulateMenuParam()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Fills in the OpMenuParam supplied by the menu builder with details of this
				specific menu item
	Errors:		None

********************************************************************************************/

void MenuItem::PopulateMenuParam()
{
	if (pMenuParam)
	{
		MenuItem* pItem = GetMenuItem(ParentMenuId);
		String_256 strFullPath = Description;
		while (pItem && !pItem->GetMenuText().IsEmpty())
		{
			String_256 strParentText = pItem->GetMenuText();
			strParentText += String( _T("/") );
			strParentText += strFullPath;
			strFullPath = strParentText;
//			strFullPath = pItem->GetMenuText() + String("/") + strFullPath;

			pItem = GetMenuItem(pItem->GetMenuItemParentId());
		}
		pMenuParam->strMenuText = Description;			// Give common ops access to individual menu identity
		pMenuParam->strMenuPath = strFullPath;
	}
}


/********************************************************************************************

>	BOOL MenuItem::UpdateMenuState(BOOL AppendHotKeyNames = TRUE)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/93
	Inputs:		None
	Outputs:	None
	Returns:	True if all Menu Enabled
				False Otherwize
	Purpose:	Obtains the state of all MenuItems in MenuItem. 
	Errors:		None

********************************************************************************************/

BOOL MenuItem::UpdateMenuState(BOOL AppendHotKeyNames)
{
	MenuItem *pMItem = NULL;
	BOOL updateMenu = FALSE;
	
	if (SubMenuItems != NULL)
		pMItem = (MenuItem *) SubMenuItems->GetHead();     	//Get first MenuItem in List
	                                                        
	//Loop while not end of list
	while (pMItem != NULL) // && (SubMenuItems != NULL))                            
	{                                     
		if (pMItem->IsMenuItemGroup()) 
		{
			if (pMItem->UpdateMenuState(AppendHotKeyNames))	//Set Update Menu Flag if 
				updateMenu = TRUE;                          //menu state has changed
		}
		else		
		{
			if (pMItem->UpdateControlState(AppendHotKeyNames))//Set Update Menu Flag if 
				updateMenu = TRUE;                          //menu state has changed
		}
				
		pMItem = (MenuItem *) SubMenuItems->GetNext(pMItem);	//Get next MenuItem in List	
	}
	
	return updateMenu;
}

/********************************************************************************************

>	UINT32 MenuItem::GetMenuId()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/93
	Inputs:		None
	Outputs:	None
	Returns:	Menu Id of current MenuItem
	Purpose:	Gets the menu Id of the current MenuItem.
	Errors:		None

********************************************************************************************/

UINT32 MenuItem::GetMenuId()
{
	return ControlId;
}

/********************************************************************************************

>	UINT32 MenuItem::GetHelpId()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/94
	Inputs:		None
	Outputs:	None
	Returns:	Help Id of current MenuItem
	Purpose:	Gets the Help Id of the current MenuItem.
	Errors:		None

********************************************************************************************/

UINT32 MenuItem::GetHelpId()
{
	return OwnerOperation->GetHelpId();
}

/********************************************************************************************

>	UINT32 MenuItem::GetMenuItemParentId()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/93
	Inputs:		None
	Outputs:	None
	Returns:	Menu Id of parent of current MenuItem
	Purpose:	Gets the menu Id of the parent of the current MenuItem.
	Errors:		None

********************************************************************************************/

UINT32 MenuItem::GetMenuItemParentId()
{
	return ParentMenuId;
}

/********************************************************************************************

>	String_256 MenuItem::GetMenuText()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/93
	Inputs:		None
	Outputs:	None
	Returns:	String representing a description of the MenuItem
	Purpose:	Gets MenuItem Text Description.
	Errors:		None

********************************************************************************************/

String_256 MenuItem::GetMenuText()
{
	return Description;
}



/********************************************************************************************
>	UINT32 MenuItem::GetMenuSelectedBitmapID()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/00
	Returns:	Resource identifier of the small bitmap associated with this menu item,
				or 0 if there isn't one available.
	Purpose:	Looks up and caches the bitmap resource for this MenuItem.
********************************************************************************************/

UINT32 MenuItem::GetMenuSelectedBitmapID()
{
	if (m_nSelectedBitmapID == 0)
	{
		// We have to look up the small button bitmap that may be associated with the
		// operation this menu item refers to.

	}

	return m_nSelectedBitmapID;
}



/********************************************************************************************
>	UINT32 MenuItem::GetMenuUnselectedBitmapID()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/00
	Returns:	Resource identifier of the small bitmap associated with this menu item,
				or 0 if there isn't one available.
	Purpose:	Looks up and caches the bitmap resource for this MenuItem.
********************************************************************************************/

UINT32 MenuItem::GetMenuUnselectedBitmapID()
{
	if (m_nUnselectedBitmapID == 0)
	{
		// We have to look up the small button bitmap that may be associated with the
		// operation this menu item refers to.

	}

	return m_nUnselectedBitmapID;
}



/********************************************************************************************

>	String_256 *MenuItem::GetMenuDesc()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/93
	Inputs:		None
	Outputs:	None
	Returns:	If Succesfull returns a string representing a description of the MenuItem
				otherwise NULL
	Purpose:	Gets MenuItem Text Description.
	Errors:		None

********************************************************************************************/

String_256 *MenuItem::GetMenuDesc()
{
	static String_256 MenuDesc = "";                 
	
	if (OwnerOperation->GetText(&MenuDesc, OP_DESC_TEXT))
    	return &MenuDesc;
    else
    	return NULL;
}

/********************************************************************************************

>	String_32 *MenuItem::GetOpToken()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/97
	Inputs:		None
	Outputs:	None
	Returns:	If Succesfull returns a string representing the OpToken of the MenuItem
				otherwise returns a NULL
	Purpose:	Gets the OpToken for the menu item.
	Errors:		None

********************************************************************************************/

String_32 * MenuItem::GetOpToken()
{
	static String_32 OpToken = _T("");                 

	if (OwnerOperation)
	{
		OpToken = OwnerOperation->Token;
		return &OpToken;
	}

	return NULL;
}


/********************************************************************************************

>	MenuItem *MenuItem::GetFirstMenuItem()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/93
	Inputs:		None
	Outputs:	None
	Returns:	First MenuItem in Group
	Purpose:	Obtains the first MenuItem in MenuItem. 
	Errors:		None

********************************************************************************************/

MenuItem *MenuItem::GetFirstMenuItem() 
{                                         
	if (SubMenuItems == NULL)
		return NULL;
	else
		return (MenuItem*)(SubMenuItems->GetHead());
}

/********************************************************************************************

>	MenuItem *MenuItem::GetNextMenuItem(MenuItem* CurrItem)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/93
	Inputs:		None
	Outputs:	None
	Returns:	First MenuItem in Group
	Purpose:	Obtains the next MenuItem in MenuItem. 
	Errors:		None

********************************************************************************************/

MenuItem *MenuItem::GetNextMenuItem(MenuItem* CurrItem) 
{
	if (SubMenuItems == NULL)
		return NULL;
	else
		return (MenuItem*)(SubMenuItems->GetNext(CurrItem));
}

/********************************************************************************************

>	BOOL MenuItem::AddMenuItem(MenuItem newItem)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/93
	Inputs:		New MenuItem
	Outputs:	None
	Returns:	True if MenuItem is added successfully
				False if MenuItem isn't added 
	Purpose:	Adds a MenuItem to a MenuItem. 
	Errors:		None

********************************************************************************************/

BOOL MenuItem::AddMenuItem(MenuItem *newItem)
{
	if (SubMenuItems == NULL)     
	{
		SubMenuItems = new List();
		ERRORIF(!SubMenuItems, _R(IDE_NOMORE_MEMORY), FALSE);
	}

	if (SubMenuItems->FindPosition(newItem) <= NOT_IN_LIST)
	{
		SubMenuItems->AddTail(newItem);
		return TRUE;
	}
	else
		return FALSE;
}

/********************************************************************************************

>	MenuItem *MenuItem::RemoveMenuItem(MenuItem* Item)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/93
	Inputs:		None
	Outputs:	None
	Returns:	MenuItem removed from SubMenuItems List
	Purpose:	Removes MenuItem from SubMenuItems List. 
	Errors:		None

********************************************************************************************/

MenuItem *MenuItem::RemoveMenuItem(MenuItem* Item)
{
	if (SubMenuItems == NULL)
		return NULL;
	else	
		return (MenuItem*)(SubMenuItems->RemoveItem(Item));
}


// this is used to automatically create unique menu IDs. It ranges from two (OIL specific)
// values, AUTO_MENU_ID_MIN and AUTO_MENU_ID_MAX inclusive (defined in oilmenus.h)

UINT32 MenuItem::AutomaticMenuID = AUTO_MENU_ID_MIN;


/********************************************************************************************

>	static UINT32 GetNextAutomaticMenuID()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/93
	Inputs:		None
	Outputs:	None
	Returns:	The next automatic menu ID value
	Purpose:	Creates a unique ID for each menu, based on a monotonic increasing value
				(whose range is determined by the OIL layer). When the range fills, a search
				is done to find individual unused IDs.
	Friend:		MenuItem
	Errors:		Will ENSURE when range is filled.
	Scope:		Private

********************************************************************************************/

UINT32 MenuItem::GetNextAutomaticMenuID()
{
	if (AutomaticMenuID > AUTO_MENU_ID_MAX)
	{
		// we have run out of IDs, so search for gaps in the allocation
		// UNFINISHED CODE!!
		ERROR2(0,"Run out of menu IDs" );
	}
	else
	{
		return AutomaticMenuID++;						// return old value, inc for next time
	}
	
}




/********************************************************************************************

>	static UINT32 MenuItem::GetAutomaticIDState()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/95
	Inputs:		None
	Outputs:	None
	Returns:	The next automatic menu ID value without incrementing the internal counter.
	Purpose:	Finds the current state of the menu ID generator so that it can be preserved.
	Scope:		Public

********************************************************************************************/

UINT32 MenuItem::GetAutomaticIDState()
{
	return AutomaticMenuID;							// return current value
}




/********************************************************************************************

>	static BOOL MenuItem::SetAutomaticIDState(UINT32 newvalue)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/95
	Inputs:		The new value for the internal menu ID generator.
	Outputs:	None
	Returns:	-
	Purpose:	Allows the state of the menu ID generator to be restored after it been 
				preserved.
	Scope:		Public

********************************************************************************************/

BOOL MenuItem::SetAutomaticIDState(UINT32 newvalue)
{
	ERROR2IF(newvalue<AUTO_MENU_ID_MIN,FALSE,"Attempt to set menu ID generator below valid range.");
	ERROR2IF(newvalue>AUTO_MENU_ID_MAX,FALSE,"Attempt to set menu ID generator above valid range.");

	AutomaticMenuID = newvalue;						// return current value
	return TRUE;
}




