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

/********************************************************************************************

	oilmenus.cpp - Responsibilities include menu initialisation and deinitialisation as well 
	as updating menu states and performing menu commands.

********************************************************************************************/


         
#include "camtypes.h"
#include "oilmenus.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "menupref.h"
//#include "opdesc.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "mario.h" 
#include "contmenu.h"
#include "menuops.h"

#include "plugmngr.h"	// PlugInManager
//#include "app.h"		// GetApplication() - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camframe.h"	// GetMainFrame()
#include "camelot.h"	// wxGetApp

DECLARE_SOURCE("$Revision$");  

// This wasn't here before 7/3/97
// We want better memory tracking
#define new CAM_DEBUG_NEW

MenuItem* CamelotDefMenu;				//Platform-Independent Menus
MenuItem* CamelotMDIMenu;
MenuItem* CamelotContextMenu;

wxCamMenuBar*	WinDefMenu;						//Windows Menus
wxCamMenuBar*	WinMDIMenu;
wxMenu*			WinContextMenu;

/********************************************************************************************

>	MenuItem *SetupDefaultMenu()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		None.
	Outputs:	None.
	Returns:	MenuItem - A platform independent mainframe menu.
	Purpose:	Creates the default mainframe menu for an MDI type interface.
	Errors:		None
	SeeAlso:	OilMenus
	SeeAlso:	MenuItem        
	SeeAlso:	SetupSharedMenu()
	SeeAlso:	GetMenuPreferences()

********************************************************************************************/

MenuItem *SetupDefaultMenu()
{
	MenuItem* pMenu = GetMenuPreferences(_R(IDM_DEFAULT_MENU));
	return pMenu;
}	                            

/********************************************************************************************

>	MenuItem *SetupSharedMenu()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		None.
	Outputs:	None.
	Returns:	MenuItem - A platform independent MDI menu.
	Purpose:	Creates a MDI type shared menu.
	Errors:		None
	SeeAlso:	OilMenus
	SeeAlso:	MenuItem
	SeeAlso:	SetupDefaultMenu()
	SeeAlso:	GetMenuPreferences()

********************************************************************************************/

MenuItem *SetupSharedMenu()
{
	MenuItem* pMenu = GetMenuPreferences(_R(IDM_SHARED_MENU));
	return pMenu;
}

/********************************************************************************************

>	wxMenuBar *ConvertToWinMenu(MenuItem *GroupSubMenu)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		MenuItem - A platform-independent menu.
	Outputs:	None.
	Returns:	wxMenu - Windows MFC-type menu object.
	Purpose:	Converts from a platform-independent menu system into a Windows specific one.
	Errors:		None
	SeeAlso:	OilMenus
	SeeAlso:	MenuItem
	SeeAlso:	CreatePopup()

********************************************************************************************/
/* 
Technical Notes:

ConvertToWinMenu iterates through the MenuItem tree structure creating new wxMenu objects for
each node. The recursive function CreatePopup() is called for each group MenuItem (popup) 
found in the tree. The function assumes that all menuItems on the MenuBar are group MenuItems
(popups)

********************************************************************************************/

wxCamMenuBar *ConvertToWinMenu(MenuItem *GroupSubMenu)
{                  
	wxCamMenuBar*	WinMenu = new wxCamMenuBar;			// Top Level Menu - Menu Bar
	MenuItem*		SubMenu;							// Sub Menu Iterator
	                            
	SubMenu = GroupSubMenu->GetFirstMenuItem();                           
	
	while (SubMenu != NULL)                     	// Assumes that all MenuItems on the 
	{                                           	// MenuBar are Group MenuItems
		wxMenu*		pSubMenu = CreatePopup( SubMenu );
		if (pSubMenu)
		{
			wxString sName((TCHAR*)SubMenu->GetMenuText());
			if (!WinMenu->Append( pSubMenu, sName ))
			{
				TRACEUSER("luke", _T("Append failed\n"));
			}
		}
		SubMenu = GroupSubMenu->GetNextMenuItem(SubMenu);
	} 

	return WinMenu;
}

/********************************************************************************************

>	wxMenu *CreatePopup(MenuItem *CamSubMenu)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		MenuItem - A platform-independent menu (popup).
	Outputs:	None.
	Returns:	wxMenu - Windows MFC-type menu object.
	Purpose:	Converts from a platform-independent menu into a Windows specific one.
	Errors:		None
	Scope: 		Private - can only be called from within the OilMenus module
	SeeAlso:	OilMenus
	SeeAlso:	MenuItem

********************************************************************************************/
/* 
Technical Notes:

CreatePopup() is a recursive function. It calls itself whenever it comes across a sub-menu.

********************************************************************************************/

wxMenu* CreatePopup(MenuItem* pCamSubMenu)
{                  
	ERROR2IF(pCamSubMenu == 0, 0, "CreatePopup: null pCamSubMenu supplied");

	wxMenu*			pPopupMenu = new wxMenu( /*wxString( pCamSubMenu->GetMenuText() )*/ );	// Popup Menus in Windows

	ERROR2IF(pPopupMenu == 0, 0, "CreatePopup failed to allocate new menu");
	
	MenuItem* pSubMenu = pCamSubMenu->GetFirstMenuItem();                      

	// if menu popup hasn't got any sub menu items then setup a dummy menuitem to allow 
	// the OnMenuPopupInit() function to work when menu is selected!
	
	if (pSubMenu == 0)
	{
    	if (!pPopupMenu->Append( pCamSubMenu->GetMenuId(),		// Use the Group Menu Id
   							   TEXT("")))                  	// Give it an empty string
		{
			TRACEUSER("luke", _T("Append failed\n"));
		}
	}
    
	while (pSubMenu != 0)
	{
		bool	fIsUnimpl = *pSubMenu->GetOpToken() == OPTOKEN_DO_NOTHING;
		bool	fNeedSep  = pSubMenu->IsFollowedBySeparator() && 
							( 0 != pCamSubMenu->GetNextMenuItem( pSubMenu ) );

		if (pSubMenu->IsMenuItemGroup())                    // Distinguish between group &
        {                                                  	// single MenuItems
			wxMenu*	pNewMenu = CreatePopup( pSubMenu );
			if (pNewMenu)
			{
				wxString sName((TCHAR *)pSubMenu->GetMenuText());
				if (!pPopupMenu->Append( pSubMenu->GetMenuId(), sName, pNewMenu ))
				{
					TRACEUSER("luke", _T("Append failed\n"));
				}
			}
		}
		else
		{
			wxItemKind	ItemKind = wxITEM_NORMAL;
			if( pSubMenu->IsCheckable() )
				ItemKind = wxITEM_CHECK;

#if !defined(_DEBUG)
			if( !fIsUnimpl )
			{
#endif
				// Set the leaf's label ... 
				if (!pPopupMenu->Append( pSubMenu->GetMenuId(), 
					wxString( (TCHAR*)pSubMenu->GetMenuText() ), wxEmptyString, ItemKind ))
				{
					TRACEUSER("luke", _T("Append failed\n"));
				}
#if !defined(_DEBUG)
			}
			else
				fNeedSep = false;
#else
			if( fIsUnimpl )
				pPopupMenu->Enable( pSubMenu->GetMenuId(), false );
#endif
			
/*
			// ... and glyph, if any.
			UINT32 nSelBmpID = pSubMenu->GetMenuSelectedBitmapID();
			UINT32 nUnselBmpID = pSubMenu->GetMenuUnselectedBitmapID();
			if (nSelBmpID != 0 && nUnselBmpID != 0)
				pPopupMenu->SetMenuItemBitmaps(pSubMenu->GetMenuId(), 0,
											   &CBitmap(nSelBmpID),
											   &CBitmap(nUnselBmpID));
*/		}

		// Add a separating line if required, BUT...
		// Don't allow a separator to be added if this is the last item in the menu...
    	if( fNeedSep )
		{
     		if (!pPopupMenu->AppendSeparator())
			{
				TRACEUSER("luke", _T("AppendSeparator failed\n"));
			}
		}
		
		pSubMenu = pCamSubMenu->GetNextMenuItem(pSubMenu);
	}
	
	return pPopupMenu;
}

/********************************************************************************************

>	BOOL DestroyCamMenu(MenuItem *CamMenu)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		MenuItem - A platform-independent menu.
	Outputs:	None.
	Returns:	True - If all objects in menu system were destroyed successfully
	Purpose:	Destroys all dynamic objects in the menu system, freeing up memory. 
	Errors:		None
	Scope: 		Private - can only be called from within the OilMenus module
	SeeAlso:	OilMenus
	SeeAlso:	MenuItem          
	SeeAlso:	DestroyWinMenu()
	SeeAlso:	DeInitMenuSystem()

********************************************************************************************/
/* 
Technical Notes:

DestroyCamMenu is a recursive function. It calls itself whenever it comes across a sub-menu.

********************************************************************************************/

BOOL DestroyCamMenu(MenuItem *CamMenu)
{               
	MenuItem *subMenu;
	MenuItem *tempMenu;
	
	subMenu = CamMenu->GetFirstMenuItem();

	while (subMenu != NULL)
	{
		// if MenuItem is a group then delete all MenuItems within it
		if (subMenu->IsMenuItemGroup())
			DestroyCamMenu(subMenu);
 		                   
 		// Get Next MenuItem before the current is deleted                   
 		tempMenu = CamMenu->GetNextMenuItem(subMenu);
 
		// Object pointed to by subMenu is deleted and memory is freed.
		delete CamMenu->RemoveMenuItem(subMenu);
		
		// Submenu now points to next item
		subMenu = tempMenu;
	}
	return TRUE;
}

// Forward Declaration

BOOL DestroyWinSubMenu(wxMenu *WinSubMenu);

/********************************************************************************************

>	BOOL DestroyWinMenu(wxMenu *WinMenu)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		wxMenu - Windows menu object.
	Outputs:	None.
	Returns:	True - If all objects in menu system were destroyed successfully
	Purpose:	Destroys all dynamic objects in the menu system, freeing up memory. 
	Errors:		Does not work with nested, hierarchical menus yet!
	Scope: 		Private - can only be called from within the OilMenus module
	SeeAlso:	OilMenus
	SeeAlso:	MenuItem
	SeeAlso:	DestroyCamMenu()
	SeeAlso:	DeInitMenuSystem()

********************************************************************************************/
/* 
Technical Notes:

Assumes that all sub Menus are popups - only ok for the Menu Bar. Calls DestroyWinSubMenu()
in order to destroy nested/hierarchical menus.
********************************************************************************************/

BOOL DestroyWinMenu(wxMenu *WinMenu)
{               
PORTNOTE("other","Removed menu destruction - temporary!")
#if 0
	wxMenu 	*subMenu;
	INT32		position = 0;
	INT32		noItems = WinMenu->GetMenuItemCount();  
	
	while ((noItems > 0)  && (position < noItems))  
	{                                               
		subMenu = WinMenu->GetSubMenu(position);    
		if (subMenu != NULL)                        
		{
			DestroyWinSubMenu(subMenu);
			subMenu->Detach();
			delete subMenu;
		}
		position++;
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL DestroyWinSubMenu(wxMenu *WinSubMenu)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		wxMenu - Windows menu object.
	Outputs:	None.
	Returns:	True - If all objects in menu system were destroyed successfully
	Purpose:	Destroys all dynamic objects in the menu system, freeing up memory. 
	Errors:		None!
	Scope: 		Private - can only be called from within the OilMenus module
	SeeAlso:	OilMenus
	SeeAlso:	MenuItem
	SeeAlso:	DestroyWinMenu()
	SeeAlso:	DeInitMenuSystem()

********************************************************************************************/
/* 
Technical Notes:

This function calls itself recursively!
********************************************************************************************/

BOOL DestroyWinSubMenu(wxMenu *WinSubMenu)
{               
PORTNOTE("other","Removed menu destruction - temporary!")
#if 0
	wxMenu 	*subMenu;
	UINT32	subMenuId;
	INT32		position = 0;
	INT32		noItems = WinSubMenu->GetMenuItemCount();  
	
	while ((noItems > 0)  && (position < noItems))  
	{                                               
		subMenuId = WinSubMenu->GetMenuItemID(position); 
		if (subMenuId == (UINT32)-1)
		{
			subMenu = WinSubMenu->GetSubMenu(position);    
			if (subMenu != NULL)                        
			{
				DestroyWinSubMenu(subMenu);
				subMenu->Detach();
				delete subMenu;
			}
		}
		position++;
	}
#endif
	return TRUE;
}

// Forward Declaration

BOOL DestroyWinSubMenuMFC(wxMenu *pWinSubMenu);

/********************************************************************************************

>	BOOL DestroyWinMenuMFC(wxMenu *WinMenu)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/97
	Inputs:		wxMenu - Windows menu object.
	Outputs:	None.
	Returns:	True - If all objects in menu system were destroyed successfully
	Purpose:	Destroys all dynamic objects in the menu system, freeing up memory. 
	Errors:		Does not work with nested, hierarchical menus yet!
	Scope: 		Private - can only be called from within the OilMenus module
	SeeAlso:	OilMenus
	SeeAlso:	MenuItem
	SeeAlso:	DestroyCamMenu()
	SeeAlso:	DeInitMenuSystem()

********************************************************************************************/
/* 
Technical Notes:

Assumes that all sub Menus are popups - only ok for the Menu Bar. Calls DestroyWinSubMenu()
in order to destroy nested/hierarchical menus.
********************************************************************************************/

BOOL DestroyWinMenuMFC(wxMenu *pWinMenu)
{               
PORTNOTE("other","Removed menu destruction - temporary!")
#if 0
	ERROR2IF(pWinMenu == NULL, FALSE,"DestroyWinMenuMFC bad pWinMenu supplied");
	wxMenu 	*pSubMenu = NULL;
	INT32		noItems = pWinMenu->GetMenuItemCount();  
	
	// Walk backwards through the list removing items from the end, otherwise
	// we would need to recalculate the position after each menu deletion as the
	// position will have changed.
	BOOL ok = TRUE;
	INT32	position = noItems - 1;
	while (position >= 0)
	{                                               
		pSubMenu = pWinMenu->GetSubMenu(position);    
		if (pSubMenu != NULL)                        
			ok = DestroyWinSubMenuMFC(pSubMenu);
		// now delete the menu item
		ok = pWinMenu->DeleteMenu( position, MF_BYPOSITION );
		if (pSubMenu != NULL)                        
		{
			pSubMenu->Detach();
			delete pSubMenu;
		}

		position--;
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL DestroyWinSubMenuMFC(wxMenu *WinSubMenu)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/97
	Inputs:		wxMenu - Windows menu object.
	Outputs:	None.
	Returns:	True - If all objects in menu system were destroyed successfully
	Purpose:	Destroys all dynamic objects in the menu system, freeing up memory. 
	Errors:		None!
	Scope: 		Private - can only be called from within the OilMenus module
	SeeAlso:	OilMenus
	SeeAlso:	MenuItem
	SeeAlso:	DestroyWinMenu()
	SeeAlso:	DeInitMenuSystem()

********************************************************************************************/
/* 
Technical Notes:

This function calls itself recursively!
********************************************************************************************/

BOOL DestroyWinSubMenuMFC(wxMenu *pWinSubMenu)
{               
PORTNOTE("other","Removed menu destruction - temporary!")
#if 0
	ERROR2IF(pWinSubMenu == NULL, FALSE,"DestroyWinMenuMFC bad pWinSubMenu supplied");
	wxMenu 	*pSubMenu = NULL;
	INT32		noItems = pWinSubMenu->GetMenuItemCount();  
	
	// Walk backwards through the list removing items from the end, otherwise
	// we would need to recalculate the position after each menu deletion as the
	// position will have changed.
	BOOL ok = TRUE;
	INT32	position = noItems - 1;
	while (position >= 0)
	{                                               
		pSubMenu = pWinSubMenu->GetSubMenu(position);    
		if (pSubMenu != NULL)                        
			ok = DestroyWinSubMenuMFC(pSubMenu);
		// now delete the menu item
		ok = pWinSubMenu->DeleteMenu( position, MF_BYPOSITION );
		if (pSubMenu != NULL)                        
		{
			pSubMenu->Detach();
			delete pSubMenu;
		}

		position--;
	}
#endif
	return TRUE;
}


/********************************************************************************************

>	BOOL InitMenuSystem()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		None.
	Outputs:	None.
	Returns:	TRUE if initialisation was successful
	Purpose:	Creates both the platform-independent and windows specific menu systems.
	Errors:		None.
	SeeAlso:	OilMenus
	SeeAlso:	MenuItem
	SeeAlso:	SetupDefaultMenu()
	SeeAlso:	SetupSharedMenu()
    SeeAlso:	ConvertToWinMenu()
	SeeAlso:	DeInitMenuSystem()

********************************************************************************************/

BOOL InitMenuSystem()
{
	CamelotDefMenu = SetupDefaultMenu();			//MainFrame Menu
	CamelotMDIMenu = SetupSharedMenu();        		//MDI Menu
	CamelotContextMenu = NULL;

	if (CamelotDefMenu)
		WinDefMenu = ConvertToWinMenu(CamelotDefMenu);	//Windows MainFrame Menu		
	else
		return FALSE;
		
	if (CamelotMDIMenu)
		WinMDIMenu = ConvertToWinMenu(CamelotMDIMenu);  //Windows MDI Menu
	else
		return FALSE;
	
	return TRUE;
}

/********************************************************************************************

>	void DeInitMenuSystem()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		None.
	Outputs:	None.
	Returns:	Nothing.
	Purpose:	Destroys both the platform-independent and windows specific menu systems.
	Errors:		None.
	SeeAlso:	OilMenus
	SeeAlso:	MenuItem
	SeeAlso:	DestroyCamMenu()	
	SeeAlso:	DestroyWinMenu()	
	SeeAlso:	InitMenuSystem()

********************************************************************************************/

void DeinitMenuSystem()
{
	if (CamelotDefMenu)
	{
		DestroyCamMenu(CamelotDefMenu);     //Destroy Platform-Independent
		delete CamelotDefMenu;
		CamelotDefMenu = NULL;
	}

	if (CamelotMDIMenu)
	{
		DestroyCamMenu(CamelotMDIMenu);     //Menu System
		delete CamelotMDIMenu; 
		CamelotMDIMenu = NULL;
	}

	if (WinMDIMenu)
	{
PORTNOTE("other","Removed menu destruction - may not be needed, lifetime controlled by window!")
#if 0
		DestroyWinMenu(WinMDIMenu);			//Destroy The MDI Frame Menu
#endif
		WinMDIMenu->Detach();
		delete WinMDIMenu;
		WinMDIMenu = NULL;
	}

	//----------------------------------------------------------
	// If the context-sensitive menu is left around delete it...
/*	if (CamelotContextMenu)
	{
		DestroyCamMenu(CamelotContextMenu);     //Menu System
		delete CamelotContextMenu; 
		CamelotContextMenu = NULL;
	}


	if (WinContextMenu)
	{
		DestroyWinMenu(WinContextMenu);			//Destroy The MDI Frame Menu
//		WinContextMenu->Detach();
		WinContextMenu->DestroyMenu();			// DestroyMenu releases Windows resources and calls Detach
		delete WinContextMenu;
		WinContextMenu = NULL;
	}
*/
	ContextMenu::HideCurrent();

}


// Functions Used For Performing Menu Actions and Updates

/********************************************************************************************

>	BOOL IsMenuCommand(WPARAM wParam, LPARAM lParam)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/7/93
	Inputs:		wParam & lParam - Windows API command parameters.
	Outputs:	None.
	Returns:	TRUE if command is a menu command
				FALSE otherwise
	Purpose:	To identify menu item commands
	Errors:		None.
	SeeAlso:	CMainFrame

********************************************************************************************/

BOOL IsMenuCommand( wxCommandEvent& event )
{
	return event.GetEventType() == wxEVT_COMMAND_MENU_SELECTED;
}


/********************************************************************************************

>	MenuItem *FindMenuItem(UINT32 menuId, MenuItem* GroupSubMenu)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/7/93
	Inputs:		Menu ID & Group Sub Menu
	Outputs:	None.
	Returns:	MenuItem if found
				NULL if menu item is not found
	Purpose:	To find a menu item in the platform independent menu system.
	Scope:		Private To OilMenus
	Errors:		None.
	SeeAlso:	MenuItem

********************************************************************************************/
/*
Technical Notes:

Recursively Called For each subMenu;

********************************************************************************************/

MenuItem *FindMenuItem(UINT32 menuId, MenuItem* GroupSubMenu)
{      
	if (GroupSubMenu==NULL)
		return NULL;

	if (GroupSubMenu->GetMenuId() == menuId)
		return GroupSubMenu;

	MenuItem* subsubMenu;
	MenuItem* subMenu = GroupSubMenu->GetFirstMenuItem();

	while (subMenu != NULL)
	{
		if (subMenu->GetMenuId() == menuId)
			return subMenu;

 		if (subMenu->IsMenuItemGroup())
		{
			subsubMenu = FindMenuItem(menuId, subMenu);
			if (subsubMenu != NULL)
				return subsubMenu;			
   	    }
              
 		subMenu = GroupSubMenu->GetNextMenuItem(subMenu);
	}                       
	
	return NULL;
}

/********************************************************************************************

>	MenuItem *GetMenuItem(UINT32 menuId, BOOL* pShowHotKeysFlag = NULL)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		Menu ID
	Outputs:	None.
	Returns:	MenuItem if found
				NULL if menu item is not found
	Purpose:	To find a menu item in one of the platform independent menu systems.
	Errors:		None.
	SeeAlso:	MenuItem
	SeeAlso:	FindMenuItem

********************************************************************************************/

MenuItem *GetMenuItem(UINT32 menuId, BOOL* pShowHotKeysFlag)
{      
	// If There are no active child windows Then search for menu option in default menu
	// Else search for menu option in MDI shared menu.
	
	MenuItem* pItem = NULL;
	if (pShowHotKeysFlag) *pShowHotKeysFlag = TRUE;

PORTNOTE( "other", "We always use MDIMenu" )
/*	if( NULL == ( (wxDocMDIParentFrame *)CCamFrame::GetMainFrame() )->GetActiveChild() )
	{
		// There is no document (MDI child) window, so we want to look only in the 
		// current context-sensitive menu (if any), and then (if that fails), in the
		// default camelot menu.

		pItem = FindMenuItem(menuId, CamelotContextMenu);	// Look in Context sensitive menu first
		if (pItem == NULL)
			pItem = FindMenuItem(menuId, CamelotDefMenu);	// Look in default menu
	}
    else */
	{
		pItem = FindMenuItem(menuId, CamelotContextMenu);	// Look in Context sensitive menu first
		if (pItem == NULL)
		 	pItem = FindMenuItem(menuId, CamelotMDIMenu);
		else
			if (pShowHotKeysFlag) *pShowHotKeysFlag = FALSE;	// ContextMenu shouldn't show hot keys
	}

	return pItem;
}

/********************************************************************************************

>	BOOL PerformMenuCommand(WPARAM wParam, LPARAM lParam)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/7/93
	Inputs:		wParam & lParam - Windows API command parameters.
	Outputs:	None.
	Returns:	TRUE if MenuItem found and command is performed
				FALSE otherwise
	Purpose:	To perform menu item command
	Errors:		None.
	SeeAlso:	CMainFrame
	SeeAlso:	MenuItem
	SeeAlso:	FindMenuItem()
	SeeAlso:	MenuItem::PerformAction()

********************************************************************************************/

BOOL PerformMenuCommand( wxCommandEvent& event )
{                     
	MenuItem* TargetItem;
	                       
	// Get appropriate menu item given the menu id
	TargetItem = GetMenuItem( event.GetId() );

   	if (TargetItem)
	{
		TargetItem->PerformAction();
		return TRUE;
	}               
	
	return FALSE;
}

/********************************************************************************************

>	BOOL FindMenu(MenuItem **TargetMenu, wxMenu* pMenu, BOOL* pShowHotKeysFlag = NULL)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Inputs:		A pointer to a Windows wxMenu
	Outputs:	A pointer to a Camelot MenuItem.
	Returns:	TRUE if found successfully
				FALSE otherwise
	Purpose:	To find kernel menu equivalent to Windows menu that is passed in. 
	Scope:		Private to be called by UpdateMenu() only!
	Errors:		None.
	SeeAlso:	CMainFrame
	SeeAlso:	wxMenu
	SeeAlso:	MenuItem

********************************************************************************************/

BOOL FindMenu(MenuItem **TargetMenu, wxMenu* pMenu, BOOL* pShowHotKeysFlag)
{
	UINT32	TargetMenuId = (UINT32)-1;
	size_t	position = 0;
	size_t	noItems = pMenu->GetMenuItemCount();
	
	// Get id of first non sub-menu item in Windows wxMenu
	// TargetMenuID is -1 if its a sub-menu item and 0 if its a separator
	// No separator check present, added by Neville 31/10/97
	TargetMenuId = pMenu->FindItemByPosition( position )->GetId(); 
	while (
			(noItems > 0) && (position < noItems) &&
			((TargetMenuId == (UINT32)-1) || (TargetMenuId == 0))
			)
	{
		TargetMenuId = pMenu->FindItemByPosition( position )->GetId(); 
		position++;
	}
     
	if (TargetMenuId != (UINT32)-1)
	{                                                           
		//Use ID to get MenuItem object 
		*TargetMenu = GetMenuItem(TargetMenuId, pShowHotKeysFlag);
	
		if (*TargetMenu != NULL)						// couldn't find it, probably
		{												// the system menu on the close box
			//Get Parent Id of MenuItem
			TargetMenuId = (*TargetMenu)->GetMenuItemParentId();

			//Use Parent Id to get menu we want!
			*TargetMenu = GetMenuItem(TargetMenuId, pShowHotKeysFlag);
			if (*TargetMenu)
				return TRUE;
			else
				return FALSE;
		}
	}	
	return FALSE;
}
                                                       
/********************************************************************************************

>	BOOL UpdateMenu(wxMenu* pMenu)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/93
	Inputs:		A pointer to a wxMenu
	Outputs:	None.
	Returns:	FALSE if Menu is not found in the Kernel
				TRUE otherwize.
	Purpose:	To update menu 
	Errors:		None.
	SeeAlso:	CMainFrame
	SeeAlso:	wxMenu
	SeeAlso:	MenuItem

********************************************************************************************/

BOOL UpdateMenu(wxMenu* pMenu)
{                     
	MenuItem*	TargetMenu = NULL;
	BOOL		ShowHotKeys = TRUE;
	
	if (FindMenu(&TargetMenu, pMenu, &ShowHotKeys))
	{
		ERROR3IF(TargetMenu==NULL,"FindMenu returned TRUE but didn't fill in menu pointer");

#ifdef PHOTOSHOPPLUGINS
// Only add in if required - general plug-in removal at present
//#ifdef NEW_NATIVE_FILTER
		// Must check if we are going to drop down the Plug-Ins menu item
		// If so then check that we have parsed the plug-ins.
		PlugInManager* pManager = GetApplication()->GetPlugInManager();
		if (pManager != NULL && !pManager->SearchedPathsYet())
		{
			MenuItem * pPlugInsMenu = pManager->FindPlugInMainMenuItem();
			// a pointer compare should be an adequote test
			if (pPlugInsMenu != NULL && TargetMenu != NULL && pPlugInsMenu == TargetMenu)
			{
				pManager->CheckHaveDetailsOnPlugIns();
				// Now done in UpdatePlugInsWinMenu
				// Force the menu bar to be redrawn as we are in the middle of pulling the
				// menu down and we have changed it.
				//HWND hwnd = GetMainFrame()->GetSafeHwnd();
				//DrawMenuBar(hwnd);
				// Cannot fall through to the code below as the pMenu we have had passed in
				// may now be completely false. This will cause problems in the UpdateWinMenu call.
				// So return now.
				return TRUE;
			}
		}
//#endif
#endif
		// Update Kernel Menu 
		TargetMenu->UpdateMenuState(ShowHotKeys);
	
		// Update OIL Menu 
		UpdateWinMenu(pMenu, TargetMenu);
		
		return TRUE;
	}
	else
		return FALSE;
}

/********************************************************************************************

>	void UpdateWinMenu(wxMenu* pMenu, MenuItem* kernelMenu)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/93
	Inputs:		A pointer to a wxMenu and a kernel menu object
	Outputs:	None.
	Returns:	None.
	Purpose:	To update windows menu system. It is also a Friend of the MenuItem Class.
	Errors:		None.
	SeeAlso:	CMainFrame
	SeeAlso:	wxMenu
	SeeAlso:	MenuItem

********************************************************************************************/

void UpdateWinMenu(wxMenu* pMenu, MenuItem* kernelMenu)
{
	if (kernelMenu == NULL) return;

	MenuItem* subMenu = (kernelMenu->GetFirstMenuItem());
	
	//Read through list of Kernel MenuItems updating their state
	while (subMenu != NULL)
	{
		if (!subMenu->IsMenuItemGroup())
		{
			wxItemKind	ItemKind = wxITEM_NORMAL;
			if( subMenu->IsCheckable() )
				ItemKind = wxITEM_CHECK;
			
			// Make sure the wxMenu has this menu item in it...
			wxMenuItem	*pItem = pMenu->FindItem( subMenu->GetMenuId() );
			if( NULL == pItem )
			{
				// This menu item is missing, probably deleted by the bit of code below
				// We have to put it back in again to be on the safe side
				// Find out the position of the next visible menu item, so we can insert just before it
				MenuItem* NextMenu = subMenu;
				while( NULL != NextMenu && 
					NULL == pMenu->FindItem( NextMenu->GetMenuId() ) )
				{
					NextMenu = (kernelMenu->GetNextMenuItem(NextMenu));
				}
				
				// If there was a next item, insert before it, else append to the end of the menu
				if (NextMenu!=NULL)
				{
					BOOL	MenuAdded = FALSE;
					BOOL	PrevWasSeparator = FALSE;
					size_t	NumMenuItems = pMenu->GetMenuItemCount();
					for( size_t i=0; (!MenuAdded) && (i<NumMenuItems); i++ )
					{
						// loop through all the menu items to try and find our ID
						wxMenuItem* pMenuItem = pMenu->FindItemByPosition( i );
						UINT32 MenuID = pMenuItem->GetId();
						if (MenuID == NextMenu->GetMenuId())
						{
							// We will be wanting to insert just before this one
							size_t InsertPos = i;

							// but was the previous item was a separator
							if (PrevWasSeparator)
								InsertPos--;
							
							// Insert the menu and set the flag to say it was inserted
							pMenu->Insert( InsertPos, subMenu->GetMenuId(), subMenu->GetMenuText(), 
								_T(""), ItemKind );
							MenuAdded = TRUE;
						}

						// Unset the Prev Separator flag
						PrevWasSeparator = FALSE;

						// if this was a separator, then remember for the next loop
						if( pMenuItem->IsSeparator() )
							PrevWasSeparator = TRUE;
					}
				}
				else
					pMenu->Append( subMenu->GetMenuId(), subMenu->GetMenuText(), _T(""),
						ItemKind );
			}

PORTNOTE( "menu", "Remove Unimplemented menuitems from menu, under Release" )
#if !defined(_DEBUG)
			if( subMenu->Description == _T("Unimplemented") )
				subMenu->ControlState.RemoveFromMenu = true;
#endif

			if( !subMenu->ControlState.RemoveFromMenu )
			{
				//Update the Menu Text
				pMenu->FindItem( subMenu->GetMenuId() )->SetText( wxString( (TCHAR *)subMenu->GetMenuText() ) );

				if (subMenu->ControlState.Greyed)
					//Disable and Grey Menu Item 
					pMenu->Enable( subMenu->GetMenuId(), false );
				else
				{
					//Enable Menu Item and UnGrey
					pMenu->Enable( subMenu->GetMenuId(), true );

					if (subMenu->ControlState.Ticked)
					{
						TRACE( _T("Check %s\n"), (PCTSTR)subMenu->GetMenuText() );
						
						//Tick Menu Item
						pMenu->Check( subMenu->GetMenuId(), true );
					}
					else
						//UnTick Menu Item
						pMenu->Check( subMenu->GetMenuId(), false );
				}
			}
			else
			// See if this menu item want itself removed from the menu
			{
				// take it out of the menu
				pMenu->Delete( subMenu->GetMenuId() );
			}
		}
		else
		{
			// It's a menu item group, so we need to recurse down
			wxMenuItem	*pItem = pMenu->FindItem( subMenu->GetMenuId() );
			if (pItem)
			{
				wxMenu * pwxSubMenu = pItem->GetSubMenu();
				if (pwxSubMenu)
				{
					UpdateWinMenu(pwxSubMenu,subMenu);
				}
			}
		}	
		subMenu = (kernelMenu->GetNextMenuItem(subMenu));
	}
}




/********************************************************************************************

>	BOOL CreateContextMenu(MenuItem* pContextMenu)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pContextMenu	Pointer to a context-sensitive menu
	Outputs:	-
	Returns:	TRUE if menu was created and opened on screen succesfully.
				FALSE otherwise
	Purpose:	To put a popup menu on screen.
	Errors:		None.

********************************************************************************************/

BOOL CreateContextMenu(MenuItem* pContextMenu)
{
	BOOL worked = FALSE;

	// If the previous context menu is hanging around for some reason
	// get rid of it.
	DestroyContextMenu();

	// Now setup the new context menu
	CamelotContextMenu = pContextMenu;
	WinContextMenu = CreatePopup(CamelotContextMenu);

	if (WinContextMenu==NULL)
		worked = FALSE;
	else
	{
		// Set the menu states correctly
//		CamelotContextMenu->UpdateMenuState(FALSE);
//		UpdateWinMenu(WinContextMenu, CamelotContextMenu);

		if( CCamFrame::GetMainFrame()->PopupMenu( WinContextMenu ) )
			worked = TRUE;
		else
		{
			DestroyCamMenu(CamelotContextMenu);
			delete CamelotContextMenu; 
			CamelotContextMenu = NULL;
		}
	}

	return worked;
}




/********************************************************************************************

>	BOOL DestroyContextMenu()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/03/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if menu was destroyed succesfully.
				FALSE otherwise
	Purpose:	To remove a popup menu from the screen.
	Errors:		None.

********************************************************************************************/

BOOL DestroyContextMenu()
{
	// If the previous context menu is hanging around for some reason
	// get rid of it.
	if (WinContextMenu)
	{
		DestroyWinMenu(WinContextMenu);
//		WinContextMenu->Detach();
//		WinContextMenu->Destroy();		// DestroyMenu releases Windows resources and calls Detach
		delete WinContextMenu;
		WinContextMenu = NULL;
	}

	if (CamelotContextMenu)
	{
		DestroyCamMenu(CamelotContextMenu);
		delete CamelotContextMenu; 
		CamelotContextMenu = NULL;
	}

	return TRUE;
}




/********************************************************************************************

>	MenuItem* GetCurrentContextMenu()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/03/95
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the current context menu (kernel form) if it exists AND the 
				Windows equivalent exists AND it's currently on screen.
	Purpose:	Find out whether the popup menu is open or not and what it is...
	Errors:		None.

********************************************************************************************/

MenuItem* GetCurrentContextMenu()
{
	if (WinContextMenu!=NULL && CamelotContextMenu!=NULL)
		return CamelotContextMenu;
	else
		return NULL;
}

/********************************************************************************************

>	MenuItem* GetMainMDIMenu()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/97
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the main MDI menu.
	Purpose:	To find the menu item associated with the main kernelly MDI menu.
	Errors:		None.

********************************************************************************************/

MenuItem* GetMainMDIMenu()
{
	return CamelotMDIMenu;
}

/********************************************************************************************

>	BOOL UpdatePlugInsWinMenu(MenuItem* pPlugInsMenu)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/97
	Inputs:		pPlugInsMenu	Pointer to the plug-ins kernel menu structure
	Outputs:	-
	Returns:	TRUE if win menu was updated succesfully.
				FALSE otherwise
	Purpose:	This searches for the equivalent Win menu in the main MDI menu structure,
				destroys anything that is there already and then puts in the new structure.
	Errors:		None.

********************************************************************************************/
BOOL UpdatePlugInsWinMenu(MenuItem* pPlugInsMenu)
{
#ifdef PHOTOSHOPPLUGINS
// Only add in if required - general plug-in removal at present
	ERROR2IF(pPlugInsMenu == NULL,FALSE,"UpdatePlugInsWinMenu null plug-ins menu supplied");

	BOOL worked = FALSE;

	// Find the section in the current MDI main win menu
	INT32 PlugInMenuPos = -1;
	wxMenu * pWinMainMenu = FindSubMenu(WinMDIMenu, PlugInMenuPos, (TCHAR *) pPlugInsMenu->GetMenuText());

	if (pWinMainMenu == NULL)
		return FALSE;
	wxMenu * pPlugInsWinMenu = pWinMainMenu->GetSubMenu(PlugInMenuPos);
	if (pPlugInsWinMenu == NULL)
		return FALSE;

	// If the previous plug-ins menu is hanging around for some reason
	// get rid of it.
	// Try and destroy all the sub-menu items first
	BOOL ok = DestroyWinMenuMFC(pPlugInsWinMenu);
	ok = ok && pWinMainMenu->DeleteMenu( PlugInMenuPos, MF_BYPOSITION );
	pPlugInsWinMenu->Detach();
	delete pPlugInsWinMenu;

	// Now setup the new plug-ins menu inserting it at the old position
	// First create the new windows menu from the kernel version
	wxMenu * pNewWinMenu = CreatePopup(pPlugInsMenu);
	ERROR3IF(pNewWinMenu == NULL,"Failed to create new plug-ins main menu item");
	if (pNewWinMenu)
	{
		// That went ok so now add it in at the old position in the main menu
		pWinMainMenu->InsertMenu(PlugInMenuPos,
								 MF_POPUP | MF_BYPOSITION, 
								 (UINT32)(pNewWinMenu->GetSafeHmenu()), 
								 pPlugInsMenu->GetMenuText()
								 );                      
	}

	// Force the menu bar to be redrawn as we are in the middle of pulling the
	// menu down and we have changed it.
	HWND hwnd = GetMainFrame()->GetSafeHwnd();
	DrawMenuBar(hwnd);

	// This code should happen in the UpdateMenu after our issuing of the DrawMenuBar command
	// Update Kernel Menu 
	pPlugInsMenu->UpdateMenuState(FALSE);

	// Update OIL Menu 
	UpdateWinMenu(pNewWinMenu, pPlugInsMenu);

	return worked;
#else
	return TRUE;
#endif // PHOTOSHOPPLUGINS
}
