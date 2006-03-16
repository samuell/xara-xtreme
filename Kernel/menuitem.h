// $Id: menuitem.h 662 2006-03-14 21:31:49Z alex $
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
 *  */

#ifndef INC_MENUITEM
#define INC_MENUITEM

#include "cmdctrl.h" 

/********************************************************************************************
>   class OpMenuParam

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/20/2004

	Purpose:	Contain all the info transferred from the menu UI to Ops

	SeeAlso:	Operation::DoWithParam

********************************************************************************************/

class OpMenuParam : public OpParam
{
CC_DECLARE_DYNAMIC(OpMenuParam)

public:
	OpMenuParam() : OpParam()
	{
		strOpUnique = String( _T("") ); 
		strMenuText = String( _T("") ); 
		strMenuPath = String( _T("") );
	}

// Properties
public:
	String_256	strOpUnique;
	String_64	strMenuText;
	String_256	strMenuPath;

// Working vars
private:
};




/********************************************************************************************

>	Class MenuItem : public Control

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/6/93
	Purpose:	
		MenuItems are the things that appear in menus, but in camelot they are also the menus 
		themselves. The idea is that the menu system is a dynamically growing, living-thing 
		where all the elements are connected in a tree like structure. The elements of the 
		menu system or the nodes in the tree are MenuItems. MenuItems can therefore be 
		simple or composite i.e. a menu option or a sub menu. In windows there is a third 
		type of MenuItem: the separator, in our new improved menuing system separators will
		not be modelled as MenuItems. Instead a MenuItem may be specified as having a menu 
		separator following it.  
	Errors:		None yet.
	SeeAlso:	Control

********************************************************************************************/

class CCAPI MenuItem : public CmdControl
{ 

friend void UpdateWinMenu( CNativeMenu* pMenu, MenuItem* kernelMenu );

	CC_DECLARE_MEMDUMP(MenuItem);

public:
	virtual void PerformAction();

private:                        

	UINT32 ParentMenuId;
	List* SubMenuItems;
	String_256 Description;
	BOOL FollowedBySeparator;
	UINT32 m_nSelectedBitmapID;
	UINT32 m_nUnselectedBitmapID;
	OpMenuParam* pMenuParam;
	
public:
	
	MenuItem()
	  : ParentMenuId(0), SubMenuItems(0), FollowedBySeparator(FALSE),
	    m_nSelectedBitmapID(0), m_nUnselectedBitmapID(0), pMenuParam(0)
			{ /* empty */ }

	// new prototype, requires OpDescriptor only (almost)
	MenuItem(
			UINT32 parentMenuID,
			OpDescriptor* Owner,
			BOOL separator = FALSE
			);

	// new prototype for group MenuItem
	MenuItem(	const String_256& description, 
				UINT32 parentMenuID = 0,
				BOOL separator = FALSE
			);

    ~MenuItem();

	BOOL IsMenuItemGroup();
	BOOL IsFollowedBySeparator();
	BOOL IsCheckable();
	BOOL SetIsFollowedBySeparator(BOOL NewValue);

	BOOL UpdateControlState(BOOL AppendHotKeyName = TRUE);
	BOOL UpdateMenuState(BOOL AppendHotKeyNames = TRUE);

	UINT32 GetMenuId();
	UINT32 GetMenuItemParentId();
	UINT32 GetHelpId();
	String_256 GetMenuText();
	String_256* GetMenuDesc();
	String_32* GetOpToken();
	void SetMenuText(const String_256& strNewText) {Description=strNewText;}
	void SetMenuParam(OpMenuParam* pNewParam) {pMenuParam=pNewParam;}
	
	UINT32 GetMenuSelectedBitmapID();
	UINT32 GetMenuUnselectedBitmapID();

	MenuItem* GetFirstMenuItem();
	MenuItem* GetNextMenuItem(MenuItem* curItem);

	BOOL AddMenuItem(MenuItem *newItem);
	MenuItem* RemoveMenuItem(MenuItem* Item);

	static UINT32 AutomaticMenuID;
	static UINT32 GetNextAutomaticMenuID();
	static UINT32 GetAutomaticIDState();
	static BOOL SetAutomaticIDState(UINT32 newvalue);

	static const OpState DefaultMenuState;

private:
	void PopulateMenuParam();

};

#endif
