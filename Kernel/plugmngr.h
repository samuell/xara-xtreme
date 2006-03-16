// $Id: plugmngr.h 662 2006-03-14 21:31:49Z alex $
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

// This is the header file for the main plug-in manager code.

#ifndef INC_PLUGINMANAGER
#define INC_PLUGINMANAGER

#include "list.h"
#include "listitem.h"
#include "plugin.h"
#include "plugop.h"

class Progress;
class MenuItem;

#define OPTOKEN_PLUGINS_MENU		_T("PlugInsMenu")
#define OPTOKEN_PLUGINS_MANAGER		_T("PlugInsManager")
#define OPTOKEN_PLUGINS_PARSING		_T("PlugInsParsing")
#define OPTOKEN_PLUGINS_PLUGINSMNGR	_T("PlugInsPluginManager")

#define PLUGIN_UNDO_SIG	"UNDO:"
#define OPTOKEN_PLUGINS_UNDO_MENU	_T("PlugInsUndoMenu")

class PlugInManager;
class MenuItem;

/**************************************************************************************

>	class PlugInHandler : public ListItem

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/96
	Purpose:	Provides specific characteristics of a type of plug-in. This includes
				the type of file which the plug-in in usually stored in. This provides
				the basis of which a handler is derived from. 

**************************************************************************************/

class PlugInHandler : public ListItem
{
	// Declare the class for memory tracking
	CC_DECLARE_DYNAMIC(PlugInHandler);

public:
	PlugInHandler();
	~PlugInHandler();

	virtual BOOL Init()		= 0;

public:
	// Useful helper functions
	virtual BOOL AddFixedPlugIns();

	// Add a plug-in to the list in the plug-in manager
	virtual BOOL AddPlugIn(PlugInItem * pPlugIn, CCRuntimeClass* pClass = CC_RUNTIME_CLASS(PlugInOp),
						   pfnGetState gs = PlugInOp::GetState);

	// go and parse the specified plug-in
	virtual BOOL ParsePlugIn(const PathName& Path, const INT32 PathID)	= 0;

	// go and add in the any main menu items that this handler requires
	virtual BOOL CreateMainMenuItems(PlugInManager * pManager, MenuItem * pRootMenu,
									 BOOL AddSeparator = FALSE);
	// a handler is asked whether it is going to add menu items so that the manager
	// can decide whether to ask the previous handler to add a separator to the end
	// of its added menus
	// Need to override this if the above function is overriden
	virtual BOOL GoingToAddMainMenuItems() { return FALSE; }

public:
	// define this handler's unique characteristics

	// return a string describing a unique identifier for the plug-ins this handler is interested in
	virtual String_32 GetUniqueID() const	= 0; 
	// return a unique type for this kind of plug-in
	virtual TypeOfPlugIn GetTypeOfPlugIn() const = 0;
	// return a string describing what filetypes the plug-in handler is interested in
	virtual String_32 GetFileTypes() const	= 0; 

};

/**************************************************************************************

>	class PlugInManager : public CCObject

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/96
	Purpose:	Provide general handling facilities for plug-ins in Camelot. 

**************************************************************************************/

class PlugInManager : public CCObject
{
	// Declare the class for memory tracking
	CC_DECLARE_DYNAMIC(PlugInManager);

public:
	PlugInManager();
	~PlugInManager();

	BOOL Init();

	// Allow access to the parse at start up switch
	BOOL GetParseAtStartUp() { return m_ParseAtStartUp; }
	void SetParseAtStartUp( BOOL NewState ) { m_ParseAtStartUp = NewState; }

	// Request a pathname using the plug-in browser form of common file dialog box
	BOOL GetPlugInPath( PathName* NewSearchPath );

	// Add a plug-in to the list in the plug-in manager
	BOOL AddPlugIn(PlugInItem * pPlugIn);

	// Remove a plug-in from the list in the plug-in manager
	BOOL RemovePlugIn(PlugInItem * pPlugIn);

	PlugInItem * GetFirstPlugIn();
	PlugInItem * GetNextPlugIn(PlugInItem * pPlugIn);

	BOOL AddPlugInPath( const PathName& NewSearchPath );
	BOOL DeletePlugInPath( const PathName& SearchPath );

	PlugInPath * GetFirstPathName();
	PlugInPath * GetNextPathName(PlugInPath * pPath);
	PlugInItem * GetPreviousPlugIn(PlugInItem * pPlugIn);
	
	// Convert an id stored in a plug-in details record into the real pathname
	BOOL GetPlugInPathname(INT32 PathNameID, String_256 * pPathName);

public:
	// Useful routines for setting up and destroying the plug-ins list
	// This is the one used during start up which only searches for plug-ins
	// if this is what the user has requested.
	BOOL SearchForPlugIns();
	// This should be called by UI items before they try and put up say a menu
	// of available plug-ins. If the user has requested parsing at this point
	// then this is where it will happen.
	BOOL CheckHaveDetailsOnPlugIns();
	BOOL SearchedPathsYet() { return m_SearchedPaths; }

	// Routines to load and save out the list of pathnames to a preferences file
	BOOL ReadPathNameList();
	BOOL WritePathNameList();
	
	// Function to remove all the current plug-ins settings
	static BOOL WipePlugInsSettings();

	// Use with care.
	BOOL DestroyPlugInsList();
	// This always searches the paths
	BOOL ParseAllPaths();

	// Finds the main plug-ins menu item
	MenuItem * FindPlugInMainMenuItem(MenuItem * pMainMenu = NULL);

protected:
	// This parses a particular pathname for plug-ins
	BOOL ParsePathName( const PathName& SearchPath, const INT32 PathID, INT32 * FilesCount,
						const Progress& PathsProgress );
	// This parses a pathname for directories
	BOOL ParsePathNameForDirectories( const PathName& SearchPath, INT32 * FilesCount );

private:
	// Code for sorting plug-ins into alphabetical order
	BOOL SortPlugInsAlphabetically();
	static INT32 __cdecl SortComparator(const void *Item1, const void *Item2);

	// Code for deleting all the main plug-in menu items associated with individual plug-ins
	BOOL DeleteMainMenuItems();
	// Code for creating the main plug-in menu items associated with the individual plug-ins
	BOOL CreateMainMenuItems();

private:
	// List of the paths the user has set which we should look in for plug-ins
	PlugInPathList m_Paths;

	// The list of plug-in hanlders that Camelot understands.
	List m_HandlersList;

	// The list of plug-in items
	List m_PlugInsList;

	// Have we been initialised yet
	BOOL m_Initialised;
	// Have we searched the paths for plug-ins
	BOOL m_SearchedPaths;
	// Should we search the specified paths at start up for plug-ins?
	static BOOL m_ParseAtStartUp;
};

#endif	// INC_PLUGINMANAGER


