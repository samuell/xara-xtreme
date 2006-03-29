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
//
// Class that represents pop-up menus in the kernel.
//

#ifndef INC_CONTEXTMENU
#define INC_CONTEXTMENU

#include "doccoord.h"
#include "clikmods.h"

class MenuItem;
class Spread;
class OpMenuParam;

/********************************************************************************************

>	class ContextMenu : public CCObject

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	Encapsulate the concept of pop-up, context-sensitive menus at the kernel
				level.

				To create and use a new context-sensitive menu:
				*	Derive a new subclass from ContextMenu, e.g. ViewContextMenu.
				*	Create the operations that you want to use from the menu or use existing
					operations.
				*	Implement a new Build function in your subclass to build your menu using
					the helper functions provided (see func headers for more details).
				*	That's all there is to it - the menu system handles everything else!

				Note: There are guidelines about how to construct a pop-up menu and when to
				open it.
				*	The menu should be formed something like this:
						Clicked object's primary commands
						Clicked object's transfer commands (e.g. cut, copy, etc...)
						"Properties..." command
				*	The menu should be opened in response to a BUTTON UP event!!!

********************************************************************************************/

class ContextMenu : public CC_CLASS_MEMDUMP
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(ContextMenu);

public:
						ContextMenu();
	virtual 			~ContextMenu();

// Major functions...
	BOOL				Show();
	BOOL				ShowOverView(Spread* pSpread, DocCoord ClickPos, ClickModifiers ClickMods);
	BOOL				Hide();
	static BOOL			HideCurrent();

// Functions that MUST be over-ridden in derived classes...
// Either:
	virtual	BOOL		Build();
// Or:
	virtual	BOOL		BuildOverView(Spread* pSpread, DocCoord ClickPos, ClickModifiers ClickMods);


//protected:
// Helper functions...
	BOOL 				BuildTransferCommands(BOOL NodeCommands);
	BOOL				BuildEffectCommands();
	BOOL 				BuildCommand(TCHAR* pOpToken,
									 BOOL Separator = FALSE,
									 MenuItem* pRootMenu = NULL,
									 const String& strNewText = String( _T("") ),
									 OpMenuParam* pParam = NULL);
	MenuItem*			GetLastItem();

protected:
	MenuItem*			pRootItem;
	MenuItem*			pLastItem;
	UINT32				AutoIDStash;

	static ContextMenu* pCurrentMenu;

};

#endif  // INC_CONTEXTMENU


