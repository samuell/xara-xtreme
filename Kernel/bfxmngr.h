// $Id: bfxmngr.h 662 2006-03-14 21:31:49Z alex $
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

// bfxmngr.h : header file
//

// This file contains specific BFX plug-in manager related items
// At present this includes:-

// BfxHandler
// Defines the characteristics of a BFX plug-in and provides a function
// for parsing them.

#ifndef INC_BFXPLUGINMANAGER
#define INC_BFXPLUGINMANAGER

#include "plugmngr.h"	// PlugInHandler

/**************************************************************************************

>	class BfxHandler : public PlugInHandler

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Purpose:	Provides BFX plug-in handling characteristics.
				It handles defining what file types BFX plug-ins have and how to
				parse them. As these are the built in filters, they are just added when
				the handler is inited.

**************************************************************************************/

class BfxHandler : public PlugInHandler
{
	// Declare the class for memory tracking
	CC_DECLARE_DYNAMIC(BfxHandler);

public:
	BfxHandler();
	~BfxHandler();

	virtual BOOL Init();

public:
	// Useful helper functions
	virtual BOOL AddFixedPlugIns();

	// Add a plug-in to the list in the plug-in manager
	virtual BOOL AddPlugIn(PlugInItem * pPlugIn, CCRuntimeClass* pClass = CC_RUNTIME_CLASS(PlugInOp),
						   pfnGetState gs = PlugInOp::GetState);

	// go and parse the specified plug-in
	virtual BOOL ParsePlugIn(const PathName& Path, const INT32 PathID);

	// go and add in the any main menu items that this handler requires
	virtual BOOL CreateMainMenuItems(PlugInManager * pManager, MenuItem * pRootMenu,
									 BOOL AddSeparator = FALSE);
	// a handler is asked whether it is going to add menu items so that the manager
	// can decide whether to ask the previous handler to add a separator to the end
	// of its added menus
	// Need to override this if the above function is overriden
	virtual BOOL GoingToAddMainMenuItems() { return TRUE; }

public:
	// define this handler's unique characteristics

	// return a string describing a unique identifier for the plug-ins this handler is interested in
	virtual String_32 GetUniqueID() const; 
	// return a unique type for this kind of plug-in
	virtual TypeOfPlugIn GetTypeOfPlugIn() const;
	// return a string descibing what filetypes the plug-in handler is interested in
	virtual String_32 GetFileTypes() const; 

public:
};

#endif // INC_BFXPLUGINMANAGER
