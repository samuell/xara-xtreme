// $Id: oilmenus.h 662 2006-03-14 21:31:49Z alex $
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
 * */  

/********************************************************************************************

< Menus
< MenuItems
< OIL Menus

COMMENT:

\b OILMENUS Module \b0

The OilMenus module provides a set of functions used for initialising, updating and 
deinitialising the menu system. The new menu consists of two dynamic menu structures. The 
first is a platform independent structure which resides in the Kernel. This structure is 
created and and maintained by functions which reside in the OilMenus Module. The fundamental
component of this structure is the MenuItem class. The second menu structure is a Windows
specific one. It is composed of CMENU objects. The creation and deletion of these objects 
is again handled by functions which reside in the OilMenus module. Conversion between these
menu structures occurs during initialisation and update (i.e.  new menu items are added to 
an existing menu). The reasons for maintaining two menu structures are mainly to do with portability and 
flexibility. 

SeeAlso: 	MenuItem
SeeAlso: 	InitMenuSystem()
SeeAlso:	DeInitMenuSystem()
SeeAlso:	SetupDefaultMenu()
SeeAlso:	SetupSharedMenu()
SeeAlso:	ConvertToWinMenu()
SeeAlso:	PerformMenuCommand()
SeeAlso:	UpdateMenu()

********************************************************************************************/


#ifndef	INC_OILMENUS
#define INC_OILMENUS

#include "menuitem.h"


// A little wxMenuBase derived class to allow the killing of the accelerator table
class wxCamMenuBar : public wxMenuBar
{
public:
	void ClearAccelTable()
	{
#if defined(__WXMSW__)
		m_accelTable = wxAcceleratorTable();
#endif
	}
};

BOOL		InitMenuSystem();                                                
                                                
void		DeinitMenuSystem();

BOOL		DestroyCamMenu(MenuItem *CamMenu);

BOOL 		DestroyWinMenu(wxMenu* WinMenu);

BOOL		IsMenuCommand( wxCommandEvent& event );

BOOL		PerformMenuCommand( wxCommandEvent& event );

BOOL		UpdateMenu(wxMenu*);

MenuItem 	*SetupDefaultMenu();

MenuItem 	*SetupSharedMenu();
                                                
wxCamMenuBar* ConvertToWinMenu(MenuItem* CamMenu);

wxMenu		*CreatePopup(MenuItem* CamSubMenu);

MenuItem 	*GetMenuItem(UINT32 menuId, BOOL* pShowHotKeysFlag = NULL);

BOOL		FindMenu(MenuItem **TargetMenu, wxMenu* pMenu, BOOL* pShowHotKeysFlag = NULL);

BOOL		CreateContextMenu(MenuItem* pContextMenu);
BOOL		DestroyContextMenu();
MenuItem*	GetCurrentContextMenu();

MenuItem*	GetMainMDIMenu();
BOOL		UpdatePlugInsWinMenu(MenuItem* pPlugInsMenu);
BOOL 		DestroyWinMenuMFC(wxMenu* pWinMenu);

// shhh! Global Variables - Standard Camelot Menus

extern wxCamMenuBar*	WinDefMenu;						//Windows Menus
extern wxCamMenuBar*	WinMDIMenu;
extern wxMenu*			WinContextMenu;					// Pointer to current context-sensitive menu

// these define the range of valid menu IDs for automatic ID creation
const	UINT32 AUTO_MENU_ID_MIN = 1000;
const	UINT32 AUTO_MENU_ID_MAX = 2000;

#endif
