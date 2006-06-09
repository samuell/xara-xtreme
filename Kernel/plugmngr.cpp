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


#include "camtypes.h"
//#include "app.h"			// GetApplication() - in camtypes.h [AUTOMATICALLY REMOVED]

#include "plugmngr.h"		// main header file
#include "plugin.h"			// useful plug-in related items
#include "fileutil.h"		// useful file utilities

#include "pshpmngr.h"		// PhotoShopHandler
#include "bfxmngr.h"		// Bfxhandler

#include "plugbrws.h"
#include "plugop.h"			// PlugInOp
#include "plugopun.h"		// PlugInUndoOp

//#include "plugres.h"		// _R(IDS_PLUGIN_ONE)
#include "progress.h"		// Progress
//#include "fixmem.h"			// CCMalloc CCFree - in camtypes.h [AUTOMATICALLY REMOVED]
#include "sgliboil.h"		// SGLibOil::AppendSlashIfNotPresent
#include "sgrmfile.h"		// LibSettings
#include "product.h"		// PRODUCT_PLUGINS_INIFILE

// Only include the freehand plug-ins if required
#ifdef FREEHANDPLUGINS
#include "xtrahand.h"		// Freehand Xtra Plugin Handler
#endif // FREEHANDPLUGIN

#include "menuitem.h"		// MenuItem
#include "oilmenus.h"		// GetMainMDIMenu
#include "menupref.h"		// CreateMenuItem
#include "pshpop.h"			// OPTOKEN_PHOTOSHOP_APPLYLAST

#include "registry.h"		// SaveRegistryEntries


#include "appprefs.h"		// OPTOKEN_PLUGSDLG



// Place any IMPLEMENT type statements here
CC_IMPLEMENT_DYNAMIC(PlugInHandler, ListItem)
CC_IMPLEMENT_DYNAMIC(PlugInManager, CCObject)

// We want better memory tracking
#define new CAM_DEBUG_NEW

// declare any statics

/********************************************************************************************

	Preference:		ParseAtStartUp
	Section:		Plug-ins
	Range:			TRUE or FALSE
	Purpose:		If TRUE, then the plug-in pathnames will be searched at start up for 
					plug-ins. If FALSE, then the searching happens the first time any plug-in
					UI is invoked.
	SeeAlso:		-

********************************************************************************************/

BOOL PlugInManager::m_ParseAtStartUp	= TRUE;

/**************************************************************************************

>	PlugInHandler::PlugInHandler()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/12/96
	Purpose:	Main constructor for the plug-in handler in Camelot. 

**************************************************************************************/

PlugInHandler::PlugInHandler()
{
}

/**************************************************************************************

>	PlugInHandler::~PlugInHandler()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/12/96
	Purpose:	Main destructor for the plug-in handler in Camelot. 

**************************************************************************************/

PlugInHandler::~PlugInHandler()
{
}

/**************************************************************************************

>	virtual BOOL PlugInHandler::AddFixedPlugIns()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Inputs:		-
	Returns:	True if worked, False otherwise
	Purpose:	Add any fixed plug-ins to the list in the plug-in manager and register the op
				descriptor to the system. By fixed, I mean those which are not file
				related and hence will be loaded when the specified paths are parsed
				for plug-ins.
				This baseclass version does nothing.

**************************************************************************************/

BOOL PlugInHandler::AddFixedPlugIns()
{
	return TRUE;
}

/**************************************************************************************

>	virtual BOOL PlugInHandler::AddPlugIn(PlugInItem * pPlugIn, CCRuntimeClass* pClass = CC_RUNTIME_CLASS(PlugInOp),
										  pfnGetState gs = PlugInOp::GetState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/96
	Inputs:		pPlugIn		the new PlugInItem to add to the list
				pClass		the operation handling class to use
				gs			the GetState function to use
	Returns:	True if worked, False otherwise
	Purpose:	Add a plug-in to the list in the plug-in manager and register the op
				descriptor to the system. 

**************************************************************************************/

BOOL PlugInHandler::AddPlugIn(PlugInItem * pPlugIn, CCRuntimeClass* pClass,	pfnGetState gs)
{
	PlugInManager* pManager = GetApplication()->GetPlugInManager();
	if (pManager == NULL)
		return FALSE;
	
	if (!pManager->AddPlugIn(pPlugIn))
		ERROR2(FALSE,"Failed to register a plug-in with plug-in manager");

	// Make up a unique name for the plug in
	String_32 OpToken = GetUniqueID();
	OpToken += pPlugIn->GetPlugInName();
	// Register this name as an OpDescriptor to the system so that we can use it
	// on menus etc.
	if (!PlugInOp::RegisterOpToken(OpToken, pPlugIn->GetPlugInName(), pClass, gs))
		ERROR2(FALSE,"Failed to register OpToken for a plug-in");

	// Now see if the family name is registered, if not then register it
	OpToken = GetUniqueID();
	OpToken += pPlugIn->GetFamilyName();
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OpToken);
	if (pOpDesc == NULL)
	{
		// Register this name as an OpDescriptor to the system so that we can use it
		// on menus etc.
		if (!PlugInOp::RegisterOpToken(OpToken, pPlugIn->GetFamilyName(), pClass, gs))
			ERROR2(FALSE,"Failed to register OpToken for a plug-in family name");
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL PhotoShopHandler::CreateMainMenuItems(PlugInManager * pManager, MenuItem * pRootMenu,
											   BOOL AddSeparator = FALSE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/97
	Inputs:		pManager	the plug-in manager to get at useful things like the plug-in list
				pRootMenu	The root of the main menu to add the items to
				AddSeparator	True if want a separator added to the last item on the menu
	Outputs:	-
	Returns:	True if the operation completed successfully
				False if it failed in some way.
	Purpose:	After the plug in list has been sorted alphabetically we must add each item
				in the list to the main plug-in menu. This is a handler's chance to insert
				menu options into this menu.
				This baseclass version does nothing and so needs to be overridden by a handler
				to do something useful.

********************************************************************************************/

BOOL PlugInHandler::CreateMainMenuItems(PlugInManager * pManager, MenuItem * pRootMenu,
										BOOL AddSeparator)
{
	ERROR2IF(pManager == NULL || pRootMenu == NULL,FALSE,"CreateMainMenuItems bad parameters!");

	// This baseclass version does nothing
	return TRUE;
}

/**************************************************************************************
**************************************************************************************/

/**************************************************************************************

>	PlugInManager::PlugInManager()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/96
	Purpose:	Main constructor for the plug-in manager in Camelot. 

**************************************************************************************/

PlugInManager::PlugInManager()
{
	// Flag that we have not been initialised yet
	m_Initialised = FALSE;
	// Default to parsing paths at start up
	m_ParseAtStartUp = TRUE;
	// Flag that we have not searched the plug-ins path yet
	m_SearchedPaths = FALSE;
}

/**************************************************************************************

>	PlugInManager::~PlugInManager()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/96
	Purpose:	Main destructor for the plug-in manager in Camelot. 

**************************************************************************************/

PlugInManager::~PlugInManager()
{
	// Clean out our list of pathnames
	/* PlugInPath* pPath = (PlugInPath *)Paths.GetHead();
	while (pPath != NULL)
	{
		delete pBitmap->pBmp;

		// Try the next pathname in the list
		pPath = (PlugInPath *)Paths.GetNext(pPath);
	} */

	//Paths.DeleteAll();

	// Clean out the list of plug-in handlers that Camelot understands.
	m_HandlersList.DeleteAll();

	// Clean out the list of plug-in items
	DestroyPlugInsList();
}

/**************************************************************************************

>	BOOL PlugInManager::DestroyPlugInsList()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Returns:	True if worked ok, False otherwise.
	Purpose:	Destroy the list of plugs that the plug-in manager in Camelot currently
				has. 

**************************************************************************************/

BOOL PlugInManager::DestroyPlugInsList()
{
	// We need to write the code to destruct an OpDescriptor and delink it from the
	// list before we can do this.

	// We must go through the list of plug-ins and deregister things like the
	// allocated op-descriptors that we allocated when the plug-in was created.
	PlugInItem* pPlugIn = (PlugInItem *)m_PlugInsList.GetHead();
	String_32 OpToken;
	String_32 FamilyOpToken;
	String_32 LastFamilyOpToken;
	while (pPlugIn != NULL)
	{
		OpToken = pPlugIn->GetUniqueID();
		FamilyOpToken = OpToken;
		OpToken += pPlugIn->GetPlugInName();
		FamilyOpToken += pPlugIn->GetFamilyName();
		// Search for the OpDescriptor
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OpToken);
		if (pOpDesc != NULL)
		{
			OpDescriptor::DelinkDescriptor(pOpDesc);
			delete pOpDesc;
		}
		// If the family optoken changes then this is the time to delete it
		if (FamilyOpToken != LastFamilyOpToken)
		{
			// Search for the OpDescriptor
			pOpDesc = OpDescriptor::FindOpDescriptor(FamilyOpToken);
			if (pOpDesc != NULL)
			{
				OpDescriptor::DelinkDescriptor(pOpDesc);
				delete pOpDesc;
			}
		}
		
		LastFamilyOpToken = FamilyOpToken;

		// Try the next plug-in in the list
		pPlugIn = (PlugInItem *)m_PlugInsList.GetNext(pPlugIn);
	}

	// Clean out the list of plug-in items
	m_PlugInsList.DeleteAll();

	// Lastly, flag that we have now not parsed all the paths
	m_SearchedPaths = FALSE;

	return TRUE;
}

/**************************************************************************************
**************************************************************************************/

#define ADD_HANDLER(Classname) \
	{																					\
		/* Create, initialise and install a new plug-in handler */						\
		Classname * pHandler = new Classname;											\
																						\
		if (pHandler == NULL) return FALSE;												\
																						\
		if (!pHandler->Init())															\
		{																				\
			/* Error occured - report it and stop trying to initialise filters. */		\
			InformError();																\
			return TRUE;																\
		}																				\
																						\
		m_HandlersList.AddTail(pHandler);												\
	}

/**************************************************************************************

>	BOOL PlugInManager::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/96
	Returns:	True if initialised ok, False otherwise.
	Purpose:	Main initialisation code for the plug-in manager in Camelot. 

**************************************************************************************/

BOOL PlugInManager::Init()
{
#ifndef WEBSTER
#ifndef EXCLUDE_BFX
//#ifdef NEW_NATIVE_FILTER
	ADD_HANDLER(BfxHandler);
//#endif
#endif
#endif
	// Only include the freehand plug-ins if required
#ifdef PHOTOSHOPPLUGINS	
	ADD_HANDLER(PhotoShopHandler);
#endif
	// Only include the freehand plug-ins if required
#ifdef FREEHANDPLUGINS
	ADD_HANDLER(XtraHandler);
#endif

	if (!PlugInOp::Init())
		return FALSE;

	// Declare any preferences that we require.
	if ( Camelot.DeclareSection(TEXT("Plug-ins"), 3) )
	{
		// section declared ok so now define the preference option  
		Camelot.DeclarePref(TEXT("Plug-ins"), TEXT("ParseAtStartUp"), &m_ParseAtStartUp, 0, 1);
	}

	// load stored items from preference system
	BOOL ok = ReadPathNameList();

	ok = ok && PlugInOp::RegisterOpToken(OPTOKEN_PLUGINS_MENU, _R(IDS_PLUGINS));
	ok = ok && PlugInOp::RegisterOpToken(OPTOKEN_PLUGINS_MANAGER, _R(IDS_PLUGINS));
	ok = ok && PlugInOp::RegisterOpToken(OPTOKEN_PLUGINS_PARSING, _R(IDS_PLUGSIN_PARSING));
	ok = ok && PlugInOp::RegisterOpToken(OPTOKEN_PLUGINS_PLUGINSMNGR, _R(IDS_PLUGINS_PLUGINSMNGR));

	ok = ok && PlugInUndoOp::RegisterOpToken(OPTOKEN_PLUGINS_UNDO_MENU, _R(IDS_PLUGINS));

	// Lastly, note that initialised has been called
	m_Initialised = TRUE;

	return TRUE;
}


/********************************************************************************************

>	static BOOL PlugInManager::WipePlugInsSettings()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/2/97
	Purpose:	Wipes the plug-in pathnames list from the ini file or from the registry.
	SeeAlso:	LibSettings::NukeSettings;


********************************************************************************************/

BOOL PlugInManager::WipePlugInsSettings()
{
#ifdef SAVE_PREFS_TO_REGISTRY
	// Check if the registry entry is present or not. Look for the our special plugin related
	// items which should be stored in a 'PlugIns\\Plug-in Pathname List' key.
	// This is the section and sub-section name that we are going to use
	String_256 KeySectionName(PRODUCT_REGISTRYKEY_PLUGINS TEXT("\\Plug-in Pathname List"));
	DeleteRegKey(hAppStateRegKey, KeySectionName);

	// We must wipe the ini file as well as otherwise when the code checks to 
	// see if the registry settings and then falls back on the ini file if they
	// are not there, then we wont be using the default settings.
#endif

	// Vape the plug-ins ini file
	String_256 PlugInsIni(PRODUCT_PLUGINS_INIFILE);
	LibSettings::NukeSettings(&PlugInsIni);

	return TRUE;
}

/********************************************************************************************

>	BOOL PlugInManager::ReadPathNameList()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/12/96
	Purpose:	Reads the plug-in pathnames list in from the ini file.
				This has to be a new ini file as the main ini file code cannot cope with
				the concept of sections filled with an arbitrary number of items which need
				cleaning out when resaving to remove old items no longer required.
				The code is based on LibSettings::BuildList
	SeeAlso:	CFileList::ReadList(); LibSettings::BuildList;


********************************************************************************************/

BOOL PlugInManager::ReadPathNameList()
{
#ifdef SAVE_PREFS_TO_REGISTRY
	// Check if the registry entry is present or not. Look for the our special plugin related
	// items which should be stored in a 'PlugIns\\Plug-in Pathname List' key.
	// This is the section and sub-section name that we are going to use
	String_256 KeySectionName(PRODUCT_REGISTRYKEY_PLUGINS TEXT("\\Plug-in Pathname List"));
	BOOL EntryPresent = CheckForRegistryEntry(KeySectionName);

	if (EntryPresent)
	{
		LoadRegistryEntries Loader;
		Loader.StartLoading(&KeySectionName);

		BOOL AddedItems = FALSE;
		String_256 FileName;
		while (Loader.LoadNextEntry(&FileName))
		{
			// only interested in items which contain some data!
			// If blank then we may parse the entire disc or currently selected directory!
			if (!FileName.IsEmpty())
			{
				// Add a trailing slash if it hasn't got one
				SGLibOil::AppendSlashIfNotPresent(&FileName);
				PathName Path(FileName);
				m_Paths.AddPathName(Path);
				// Flag that we have added something
				AddedItems = TRUE;
			}
		}
		Loader.StopLoading();
	}
	else
#endif
	{
		// Read in the data from the older ini file that may be kicking around
		// This is the section name that we are going to use
		String_256 SectionName(TEXT("Plug-in Pathname List"));
		// This is the ini file that we are going to be using
		String_256 PlugInsIni(PRODUCT_PLUGINS_INIFILE);

		BOOL AddedItems = FALSE;
		String_256 FileName;
		CCDiskFile GetEntryFile(1024, FALSE, TRUE);

		if (LibSettings::GetEntryStart(&GetEntryFile, &PlugInsIni))
		{
			if (LibSettings::SeekToSection(&GetEntryFile, &SectionName))
			{
				while (LibSettings::GetNextLine(&GetEntryFile, &FileName))
				{
					// Add a trailing slash if it hasn't got one
					SGLibOil::AppendSlashIfNotPresent(&FileName);
					PathName Path(FileName);
					m_Paths.AddPathName(Path);
					// Flag that we have added something
					AddedItems = TRUE;
				}
			}

			LibSettings::GetEntryFinish(&GetEntryFile);
		}
	}

	// Everything is ok
	return TRUE;
}

/********************************************************************************************

>	BOOL PlugInManager::WritePathNameList()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/12/96
	Purpose:	Writes the list of plug-in pathnames to the ini file
				This has to be a new ini file as the main ini file code cannot cope with
				the concept of sections filled with an arbitrary number of items which need
				cleaning out when resaving to remove old items no longer required.
	SeeAlso:	CFileList::WriteList()

********************************************************************************************/

BOOL PlugInManager::WritePathNameList()
{
#ifdef SAVE_PREFS_TO_REGISTRY
	BOOL ok = TRUE;

	// This is the section and sub-section name that we are going to use
	String_256 SectionName(PRODUCT_REGISTRYKEY_PLUGINS TEXT("\\Plug-in Pathname List"));
	SaveRegistryEntries Saver;
	ok = Saver.StartSaving(&SectionName);
	ERROR3IF(!ok,"PlugInManager::WritePathNameList() couldn't start saving the settings");
	// Go through all our stored paths writing them out in the preferences.
	// DO NOT USE GetHead/GetNext as we would see all the paths, which is more than we should
	// be seeing. Use the offical GetFirstPath/GetNextPath routines which give us what we 
	// should be seeing, i.e. the user entered paths.
	PlugInPath* pPath = (PlugInPath *)m_Paths.GetFirstPath();
	PathName FileName;
	String_256 Line;

	while (pPath != NULL && ok)
	{
		// Get the pathname for this entry
		FileName = pPath->GetPathName();
		// Get the string that represents this
		Line = FileName.GetPath();
		// only write out the line if non-blank
		// If blank then we may parse the entire disc or currently selected directory!
		if (!Line.IsEmpty())
			ok = ok && Saver.SaveNextEntry(&Line);
		// if !ok then possibly write access denied on in file... or disk full...
		// So flag that we should stop outputting any more data

		// Try the next pathname in the list
		pPath = (PlugInPath *)m_Paths.GetNextPath(pPath);
	}

	Saver.StopSaving();
#else
	// This is the section name that we are going to use
	String_256 SectionName(TEXT("Plug-in Pathname List"));
	// This is the ini file that we are going to be using
	String_256 PlugInsIni(PRODUCT_PLUGINS_INIFILE);
	
	LibSettings::StartOutput(&SectionName, &PlugInsIni);
	LibSettings::AddSection(&SectionName, &PlugInsIni);

	// Go through all our stored paths writing them out in the preferences.
	// DO NOT USE GetHead/GetNext as we would see all the paths, which is more than we should
	// be seeing. Use the offical GetFirstPath/GetNextPath routines which give us what we 
	// should be seeing, i.e. the user entered paths.
	PlugInPath* pPath = (PlugInPath *)m_Paths.GetFirstPath();
	PathName FileName;
	String_256 Line;

	BOOL ok = TRUE;
	while (pPath != NULL && ok)
	{
		// Get the pathname for this entry
		FileName = pPath->GetPathName();
		// Get the string that represents this
		Line = FileName.GetPath();
		if (!LibSettings::AddLine(&Line, &PlugInsIni))
		{
			// Possibly write access denied on in file... or disk full...
			// So flag that we should stop outputting any more data
			ok = FALSE;
		}
		// Try the next pathname in the list
		pPath = (PlugInPath *)m_Paths.GetNextPath(pPath);
	}
#endif
	return TRUE;
}

/**************************************************************************************

>	BOOL PlugInManager::SearchForPlugIns()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/12/96
	Returns:	True if parsed ok, False otherwise.
	Purpose:	Starts up the process of parsing all the paths specified to the plug-in
				manager for plug-ins and stroing information on them in a list.
				This is called during start up so needs to check the flag to see whether
				the user has requested this to be done at the start, before actually
				trying to parse.

**************************************************************************************/

BOOL PlugInManager::SearchForPlugIns()
{
	// If the user has requested plug-in parsing at start up then now is the time to do it
	// Otherwise, delay until the first call
	if (m_ParseAtStartUp)
		return ParseAllPaths();

	// Otherwise we do the job later
	return TRUE;
}

/**************************************************************************************

>	BOOL PlugInManager::CheckHaveDetailsOnPlugIns()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/12/96
	Returns:	True if parsed ok, False otherwise.
	Purpose:	Checks to see if the pathnames have been searched for plug-ins. If not
				then do it now.
				This should be called by any UI code before they try and put up any UI
				of plug-ins, e.g. a menu of available plug-ins. If the user requested
				not to parse pathnames at start up then this is where it happens.

**************************************************************************************/

BOOL PlugInManager::CheckHaveDetailsOnPlugIns()
{
	// If we have not already parsed the paths then go and do it now as we need it.
	if (!m_SearchedPaths)
		return ParseAllPaths();
	
	// Otherwise we do the job later
	return TRUE;
}

/**************************************************************************************

>	BOOL PlugInManager::ParseAllPaths()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/96
	Returns:	True if parsed ok, False otherwise.
	Purpose:	Starts up the process of parsing all the paths specified to the plug-in
				manager for plug-ins and storing information on them in a list.
				Assumes the list is either empty before we are called or the caller
				has cleared it using 

**************************************************************************************/

BOOL PlugInManager::ParseAllPaths()
{
	// First, flag that we have parsed all the paths
	m_SearchedPaths = TRUE;

	BOOL ParsedOk = TRUE;

//#if NEW_NATIVE_FILTER
	// Now, go and make sure that we have no plug-ins main menu items
	DeleteMainMenuItems();
//#endif

	// Start adding the plug-ins into the list
	// First any ones which are fixed or built in and not file related
	// Go through each of the handlers asking them to add fixed plug-ins.
	PlugInHandler* pHandler = (PlugInHandler *)m_HandlersList.GetHead();
	while (pHandler != NULL)
	{
		pHandler->AddFixedPlugIns();

		// Try the next handler in the list
		pHandler = (PlugInHandler *)m_HandlersList.GetNext(pHandler);
	}

	// Now look through all the directories that the user has specified
	
	// First remove all hidden items from the list
	// Put up an hourglass to show we are doing something
	Progress InitialPathsProgress(_R(IDS_PLUGSIN_PARSING));
	m_Paths.RemoveHiddenItems();

	// Now go through all the pathnames looking for sub-directories recursively
	// and add these to the list of items
	// The items we use MUST only ever see non-hidden paths, as otherwise
	// we will get very recursive!!!!!
	PlugInPath* pPath = (PlugInPath *)m_Paths.GetFirstPath();
	INT32 FilesCount = 0;
	while (pPath != NULL)
	{
		ParsedOk = ParsedOk && ParsePathNameForDirectories(pPath->GetPathName(), &FilesCount);
		pPath = (PlugInPath *)m_Paths.GetNextPath(pPath);
	}

	// Put up a progress bar to show we are doing something
	Progress PathsProgress(_R(IDS_PLUGSIN_PARSING), FilesCount);

	// Go through all our stored paths and check them for plug-ins stored in them.
	pPath = (PlugInPath *)m_Paths.GetHead();
	INT32 Count = 0;
	PathsProgress.Update(Count);

	while (pPath != NULL)
	{
		ParsedOk = ParsedOk && ParsePathName(pPath->GetPathName(), pPath->GetPathNameID(), &Count, PathsProgress);

		// Try the next pathname in the list
		pPath = (PlugInPath *)m_Paths.GetNext(pPath);
		PathsProgress.Update(Count);
	}

	// Sort those plug-ins that we have loaded
	SortPlugInsAlphabetically();

//#if NEW_NATIVE_FILTER
	// Now, go and create the plug-ins main menu items required
	CreateMainMenuItems();
//#endif

	return ParsedOk;
}

/**************************************************************************************

>	BOOL PlugInManager::ParsePathNameForDirectories( const PathName& SearchPath, INT32 * pFilesCount)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/96
	Inputs:		ParsePath		the pathname to parse for new plug-ins
	Outputs:	pFilesCount		an updated count of the files discovered
	Returns:	True if parsed ok, False otherwise.
	Purpose:	Parse the given path for all directories. If found then add them as
				hidden items to our list of paths so that the user cannot see them.
				We do it this way so that we only ever store a PathID with a plug-in
				record rather than having to store the pathname or sub-directories to
				the pathname with the plug-in.
	Note:		Must be re-entrant!
	
**************************************************************************************/

BOOL PlugInManager::ParsePathNameForDirectories( const PathName& SearchPath, INT32 * pFilesCount)
{
	PathName File = SearchPath;
	String_256 SearchDir = SearchPath.GetPath();
	// If the path is blank then ignore it otherwise we will search the entire hard drive
	// or currently selected directory!
	if (SearchDir.IsEmpty())
		return TRUE;

	SearchDir += "*.*";
	TRACEUSER( "Neville", _T("ParsePathNameForDirectories Search path is %s\n"), (TCHAR *)SearchDir);
	FindFiles Finder;
	BOOL IsDirectory = FALSE;
	if (Finder.StartFindingFiles(&SearchDir))
	{
		String_256 LeafName;
		while (TRUE)
		{
			if (!Finder.FindNextFile(&LeafName, &IsDirectory))
				break;

			if (pFilesCount)
				(*pFilesCount)++;

			// We have found a file or a directory.
			// In this case we only want to look for directories
			if (IsDirectory)
			{
				// Do something with the directory "LeafName"
				TRACEUSER( "Neville", _T("ParsePathNameForDirectories add directory %s\n"), (TCHAR *)LeafName);
				String_256 NewSearchDir = SearchPath.GetPath();
				NewSearchDir += LeafName;
				// Add a trailing slash if it hasn't got one, which it shouldn't have
				SGLibOil::AppendSlashIfNotPresent(&NewSearchDir);
				PathName NewSearchPath(NewSearchDir);
				// Add the new pathname to the list, but as a hidden one so that
				// people not in the know do not see them.
				PlugInPath * pPath = m_Paths.AddPathName(NewSearchPath, TRUE);
				if (pPath)
				{
					// recover the id associated with the path
					ParsePathNameForDirectories(NewSearchPath, pFilesCount);
				}
			}
		}

		// Stop the process
		Finder.StopFindingFiles();
	}

	return TRUE;
}

/**************************************************************************************

>	BOOL PlugInManager::ParsePathName( const PathName& SearchPath, const INT32 PathID, INT32 * pFilesCount,
									   const Progress& PathsProgress )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/96
	Inputs:		ParsePath	the pathname to parse for new plug-ins
				pathID		the ID assigned to this path
				PathsProgress	the progress object to call for progress update
	Outputs:	pFilesCount	an updated count of the files discovered
	Returns:	True if parsed ok, False otherwise.
	Purpose:	Parse the given path for all files and pass the files of the registered
				types to the appropriate plug-in handler. The handler should then check
				to see whether it likes this type of file and if so then create a new 
				plug-in record of the correct type and then place useful details into
				the record. Finally it should register the new plug-in record with the
				plug-in manager i.e. this class.
				The PathID should be stored in the plug-in record so that we do not
				have to store the full path with all plug-in records.
	
**************************************************************************************/

BOOL PlugInManager::ParsePathName( const PathName& SearchPath, const INT32 PathID, INT32 * pFilesCount,
								   const Progress& PathsProgress )
{
	PathName File = SearchPath;
	String_256 SearchDir = SearchPath.GetPath();
	// If the path is blank then ignore it otherwise we will search the entire hard drive
	// or currently selected directory!
	if (SearchDir.IsEmpty())
		return TRUE;

	// We must set up the operating system current directory so that extra DLLs beside the
	// plug-ins can be picked up.
	BOOL ok = FileUtil::SetCurrentDirectory( SearchPath );

	SearchDir += "*.*";
	TRACEUSER( "Neville", _T("ParsePathName Search path is %s, found:-\n"), (TCHAR *)SearchDir);
	FindFiles Finder;
	BOOL IsDirectory = FALSE;
	if (Finder.StartFindingFiles(&SearchDir))
	{
		String_256 LeafName;
		while (TRUE)
		{
			if (!Finder.FindNextFile(&LeafName, &IsDirectory))
				break;

			if (pFilesCount)
				(*pFilesCount)++;

			// Do something with file "LeafName"
			TRACEUSER( "Neville", _T("ParsePathName parse plug-in file %s\n"), (TCHAR *)LeafName);

			// We have found a file or a directory.
			// In this case we only want to look for files
			if (!IsDirectory)
			{
				File.SetFileNameAndType(LeafName);
				String_32 FileType = File.GetType();
				FileType.toLower();
				PlugInHandler* pHandler = (PlugInHandler *)m_HandlersList.GetHead();
				BOOL Found = FALSE;
				while (pHandler != NULL && !Found)
				{
					String_32 HandlerFileType = pHandler->GetFileTypes();
					// If the handler returns a filetype which is null then ignore it
					// as this generally indicates that it will do its own thing.
					// Handlers can specify a list of file types by splitting them with commas
					if (!HandlerFileType.IsEmpty() && (HandlerFileType.Sub(FileType) != -1)) //FileType == HandlerFileType)
						Found = TRUE;
					else
					{
						// Try the next handler in the list
						pHandler = (PlugInHandler *)m_HandlersList.GetNext(pHandler);
					}
				}
				// If we have found this filetype in our list of handlers then
				// ask that handler to parse the file.
				if (Found && pHandler)
				{
					pHandler->ParsePlugIn(File, PathID);
				}

				// Update our progress bar for the file just parsed
				if (pFilesCount)
					PathsProgress.Update(*pFilesCount);
			}
		}

		// Stop the process
		Finder.StopFindingFiles();
	}

	return TRUE;
}

/**************************************************************************************

>	BOOL PlugInManager::GetPlugInPath( PathName* NewSearchPath )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/96
	Inputs:		-
	Outputs:	NewSearchPath	the pathname to parse for new plug-ins
	Returns:	True if parsed ok, False otherwise.
	Purpose:	Allows the requesting of a pathname to parse plug-ins for from the user.
				Its pops up a modified form of the stanadard file dialog box set up to
				browse explicitly for paths rather than files.
				Used by the plug-ins options tab.

**************************************************************************************/

BOOL PlugInManager::GetPlugInPath( PathName* NewSearchPath )
{
	// Ask out special class to pop up a common file dialog box to the user and then
	// to return the path that the user has requested back to us.
	BOOL ok = PlugInsBrowser::GetLibPath(NewSearchPath);							
	return ok;
}

/**************************************************************************************

>	BOOL PlugInManager::AddPlugInPath( const PathName& NewSearchPath )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Inputs:		NewSearchPath	the pathname to add to the list
	Returns:	True if added ok, False otherwise.
	Purpose:	Adds a new pathname to the list of pathnames stored in the plug-in manager.

**************************************************************************************/

BOOL PlugInManager::AddPlugInPath( const PathName& NewSearchPath )
{
	// First check to see if its blank or not. If so then ignore it
	String_256 SearchDir = NewSearchPath.GetPath();
	if (SearchDir.IsEmpty())
	{
		ERROR3("AddPlugInPath asked to add blank path!");
		return FALSE;
	}

	m_Paths.AddPathName(NewSearchPath);
	return TRUE;
}

/**************************************************************************************

>	BOOL PlugInManager::DeletePlugInPath( const PathName& SearchPath )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Inputs:		SearchPath	the pathname to delete from the list
	Returns:	True if added ok, False otherwise.
	Purpose:	Deletes a pathname from the list of pathnames stored in the plug-in manager.

**************************************************************************************/

BOOL PlugInManager::DeletePlugInPath( const PathName& SearchPath )
{
	return m_Paths.DeletePathName(SearchPath);
}

/**************************************************************************************

>	PlugInPath * PlugInManager::GetFirstPathName()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Returns:	Pointer to a plug-in pathname in the list.
	Purpose:	Get the first plug-in pathname in the list stored in the plug-in manager.
				This is just the list of user defined paths and does not include the
				hidden paths that are stored in the list as well.

**************************************************************************************/

PlugInPath * PlugInManager::GetFirstPathName()
{
	return (PlugInPath *)m_Paths.GetFirstPath();
}

/**************************************************************************************

>	PlugInPath * PlugInManager::GetNextPathName(PlugInPath * pPath)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Inputs:		pPlugIn		pointer to the current plug-in in the list
	Returns:	Pointer to a plug-in pathname in the list.
	Purpose:	Get the next plug-in pathname in the list stored in the plug-in manager.
				This is just the list of user defined paths and does not include the
				hidden paths that are stored in the list as well.

**************************************************************************************/

PlugInPath * PlugInManager::GetNextPathName(PlugInPath * pPath)
{
	return (PlugInPath *)m_Paths.GetNextPath(pPath);
}

/**************************************************************************************

>	BOOL PlugInManager::AddPlugIn(PlugInItem * pPlugIn)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/96
	Returns:	True if worked, False otherwise
	Purpose:	Add a plug-in to the list in the plug-in manager and register the op
				desciptor to the system. 

**************************************************************************************/

BOOL PlugInManager::AddPlugIn(PlugInItem * pPlugIn)
{
	m_PlugInsList.AddTail(pPlugIn);

	return TRUE;
}

/**************************************************************************************

>	BOOL PlugInManager::RemovePlugIn(PlugInItem * pPlugIn)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/97
	Inputs:		pPlugIn		- Pointer to plugin item
	Returns:	True if worked, False otherwise
	Purpose:	Removes a plug-in from the list in the plug-in manager (if it's there)

**************************************************************************************/

BOOL PlugInManager::RemovePlugIn(PlugInItem * pPlugIn)
{
	if(m_PlugInsList.RemoveItem(pPlugIn) == NULL)
		return FALSE;

	return TRUE;
}



/**************************************************************************************

>	PlugInItem * PlugInManager::GetFirstPlugIn()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/96
	Returns:	Pointer to a plug-in item in the list.
	Purpose:	Get the first plug-in item in the list stored in the plug-in manager.

**************************************************************************************/

PlugInItem * PlugInManager::GetFirstPlugIn()
{
	return (PlugInItem *)m_PlugInsList.GetHead();
}

/**************************************************************************************

>	PlugInItem * PlugInManager::GetNextPlugIn(PlugInItem * pPlugIn)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/96
	Inputs:		pPlugIn		pointer to the current plug-in in the list
	Returns:	Pointer to a plug-in item in the list.
	Purpose:	Get the next plug-in item in the list stored in the plug-in manager.

**************************************************************************************/

PlugInItem * PlugInManager::GetNextPlugIn(PlugInItem * pPlugIn)
{
	return (PlugInItem *)m_PlugInsList.GetNext(pPlugIn);
}

/**************************************************************************************

>	PlugInItem * PlugInManager::GetPreviousPlugIn(PlugInItem * pPlugIn)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/96
	Inputs:		pPlugIn		pointer to the current plug-in in the list
	Returns:	Pointer to a previous plug-in item in the list.
	Purpose:	Get the previous plug-in item in the list stored in the plug-in manager.

**************************************************************************************/

PlugInItem * PlugInManager::GetPreviousPlugIn(PlugInItem * pPlugIn)
{
	return (PlugInItem *)m_PlugInsList.GetPrev(pPlugIn);
}

/**************************************************************************************

>	BOOL PlugInManager::GetPlugInPathname(INT32 PathNameID, String_256 * pPathName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/96
	Inputs:		PathNameID	id of the pathname to search for
				pPathName	pointer to a string to fill in with the pathname
	pPlugIn		pointer to the current plug-in in the list
	Returns:	True if worked ok, False otherwise.
	Purpose:	Converts a pathanme id which is stored with each plug-in into the real
				pathname that it corresponds to by searching the pathname list in the
				plug-in manager.

**************************************************************************************/

BOOL PlugInManager::GetPlugInPathname(INT32 PathNameID, String_256 * pPathName)
{
	ERROR2IF(pPathName == NULL,FALSE,"PlugInManager::GetPlugInPathname null pPath");

	// Make the string empty
	pPathName->Empty();

	// Go through all our stored paths and check them for plug-ins stored in them.
	PlugInPath* pPath = (PlugInPath *)m_Paths.GetHead();
	while (pPath != NULL)
	{
		if (pPath->GetPathNameID() == PathNameID)
		{
			*pPathName = pPath->GetPathNameAsString();
			return TRUE;
		}

		// Try the next pathname in the list
		pPath = (PlugInPath *)m_Paths.GetNext(pPath);
	}

	// Nothing found so  return this to the user
	return FALSE;
}

/********************************************************************************************

>	BOOL PlugInManager::SortPlugInsAlphabetically()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/96
	Inputs:		-
	Outputs:	-
	Returns:	True if the operation completed successfully
				False if it failed in some way.
	Purpose:	Sorts the list of plugs-in alphabetically
				Borrowed code from SuperGallery::ApplySortNow as we need a much simpler version.
	Errors:		-
	SeeAlso:	ImportedColours::SortColoursByEntryNumber(); SuperGallery::ApplySortNow

********************************************************************************************/

BOOL PlugInManager::SortPlugInsAlphabetically()
{
	INT32 NumItems = 0;

	// Count the number of items we have to sort
	PlugInItem * pPlugIn = GetFirstPlugIn();
	while (pPlugIn != NULL)
	{
		NumItems ++;
		pPlugIn = GetNextPlugIn(pPlugIn);
	}

	// Start progress indicators, with a percentage based upon the number of items.
	// We will update twice for each group (after qsort and shuffle-items stages)
	Progress SortProgress(_R(IDS_PLUGSIN_SORTING), NumItems * 2, FALSE);
	INT32 NumItemsToSort = 0;

	if (NumItems > 1)
	{
		// Get memory for an array of pointers to these items
		PlugInItem **SortArray = (PlugInItem **)CCMalloc(NumItems * sizeof(PlugInItem *));
		if (SortArray == NULL)
			return FALSE;

		// Fill in the array with pointers to display items to sort
		INT32 i = 0;
		PlugInItem * pPlugIn = GetFirstPlugIn();
		while (pPlugIn != NULL)
		{
			SortArray[i++] = pPlugIn;
			pPlugIn = GetNextPlugIn(pPlugIn);
		}

		// Sort the array of pointers
		qsort(SortArray, NumItems, sizeof(PlugInItem *), PlugInManager::SortComparator);

		NumItemsToSort += NumItems;
		// Update percentage complete for the number of items processed
		SortProgress.Update(NumItemsToSort);

		// Now, take the sorted array, and rearrange the items to be in that order		
		// Special case the first item
		PlugInItem *pPrevPlugIn = GetPreviousPlugIn(SortArray[0]);
		if (pPrevPlugIn != NULL)
		{
			m_PlugInsList.RemoveItem(SortArray[0]);
			m_PlugInsList.InsertBefore(SortArray[1], SortArray[0]);
		}

		// Then whip through the rest of the items
		for (i = 1; i < NumItems; i++)
		{
			pPrevPlugIn = GetPreviousPlugIn(SortArray[i]);
			if (pPrevPlugIn != SortArray[i-1])
			{
				m_PlugInsList.RemoveItem(SortArray[i]);
				m_PlugInsList.InsertAfter(SortArray[i-1], SortArray[i]);
			}
		}

		// Free our temporary workspace
		CCFree(SortArray);
	}
	
	// End the progress bar that we started
	//EndSlowJob();

	// We seem to have sorted the items ok
	return TRUE;
}

/********************************************************************************************

>	static INT32 __cdecl PlugInManager::SortComparator(const void *Item1, const void *Item2)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/96
	Inputs:		Item1, Item2 - the items to be compared
	Returns:	negative (I am lesser), 0 (we are equal), or positive (I am greater)
				result of comparing the items with the EntryNumber stored in each item in
				the loaded colour list.
	Purpose:	'qsort' comparator function, used when quicksorting the loaded colour list
	SeeAlso:	PlugInManager::SortPlugInsAlphabetically();
				ImportedColours::SortColoursByEntryNumber();
				SuperGallery::SortComparator; SuperGallery::ApplySortNow;

********************************************************************************************/

INT32 __cdecl PlugInManager::SortComparator(const void *Item1, const void *Item2)
{
//	if (Item1 == NULL || Item2 == NULL)
//		return 0;

	PlugInItem *pItem1 = *((PlugInItem **)Item1);
	PlugInItem *pItem2 = *((PlugInItem **)Item2);

	if (pItem1 == NULL || pItem2 == NULL)
	{
		ERROR3("PlugIn::SortComparator bad pointers!");	
		return 0;
	}

	INT32 Result = 0;

	// Sort on family name as the highest priority
	if (pItem1->GetFamilyName() < pItem2->GetFamilyName())
		return(-1);

	if ((pItem1->GetFamilyName() == pItem2->GetFamilyName()) &&
		(pItem1->GetPlugInName() < pItem2->GetPlugInName())
		)
		return(-1);

	if ((pItem1->GetFamilyName() == pItem2->GetFamilyName()) &&
		(pItem1->GetPlugInName() == pItem2->GetPlugInName())
		)
		return 0;
	
	// Must be greater than
	return 1;
}

/********************************************************************************************

>	MenuItem * PlugInManager::FindPlugInMainMenuItem()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> & Re-writen by sjk 2/8/00 so it doesn't have to be a top level menu!
	Created:	20/1/97
	Inputs:		-
	Outputs:	-
	Returns:	Pointer to the plug-in main menu item if the operation completed successfully
				NULL if it failed in some way.
	Purpose:	Finds the main plug-in menu item and returns a pointer to it.

********************************************************************************************/
MenuItem * PlugInManager::FindPlugInMainMenuItem(MenuItem * pMainMenu)
{
	if (pMainMenu == NULL)
		pMainMenu = GetMainMDIMenu();

	MenuItem * pMenu = pMainMenu->GetFirstMenuItem();

	String_256 PlugInMenu(OPTOKEN_PLUGINS_UNDO_MENU);
	String_32 Desc;
	while (pMenu)
	{
		MenuItem * pSub = pMenu->GetFirstMenuItem();

		if (pSub)
		{
			// check sub menu
			pSub = FindPlugInMainMenuItem(pMenu);
		}

		// Get the OpToken for the current menu item or returned menu item
		String_32 * pDesc = pSub ? pSub->GetOpToken() : pMenu->GetOpToken();
		if (pDesc)
			Desc = *pDesc;
		else
			Desc.Empty();

		// If the OpTokens match then it is the one that we desire
		if (Desc == PlugInMenu)
			return pSub ? pSub : pMenu;

		// Go to the next main menu item in the list
		pMenu = pMainMenu->GetNextMenuItem(pMenu);
	}

	return NULL;
}

/********************************************************************************************

>	BOOL PlugInManager::DeleteMainMenuItems()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/97
	Inputs:		-
	Outputs:	-
	Returns:	True if the operation completed successfully
				False if it failed in some way.
	Purpose:	Before the pathnames are parsed, we must clear out the main menu items of
				all the current plug-ins.

********************************************************************************************/

BOOL PlugInManager::DeleteMainMenuItems()
{
//#if NEW_NATIVE_FILTER
	// Search the main menu items until we find the plug-ins one
	MenuItem * pRootMenu = FindPlugInMainMenuItem();
	if (pRootMenu == NULL)
		return FALSE;

	// Completely destroy this menu
	BOOL ok = DestroyCamMenu(pRootMenu);
	
	// We must add back an item so that the menu is not empty otherwise
	// all hell will break loose if its left in this state!!!!
	// Create a menu item for the apply last item
//	MenuItem * pApplyLastItem = CreateMenuItem(OPTOKEN_PLUGINS_PARSING, pRootMenu->GetMenuId(), FALSE);
	// If created ok then add this to the main plug-in's menu
//	if (pApplyLastItem)
//		pRootMenu->AddMenuItem(pApplyLastItem);

	// Just in case anything goes wrong between here and adding the new plug-ins back in
	// we must make sure that the plug-ins win menu is updated to mirror the kernel
	// menu structure that we have just blatted
	BOOL update = UpdatePlugInsWinMenu(pRootMenu);

//#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL PlugInManager::CreateMainMenuItems()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/97
	Inputs:		-
	Outputs:	-
	Returns:	True if the operation completed successfully
				False if it failed in some way.
	Purpose:	After the plug in list has been sorted alphabetically we must add each item
				in the list to the main plug-in menu.

********************************************************************************************/

BOOL PlugInManager::CreateMainMenuItems()
{
/*
//#if NEW_NATIVE_FILTER
	// Search the main menu items until we find the plug-ins one
	MenuItem * pRootMenu = FindPlugInMainMenuItem();
	if (pRootMenu == NULL)
		return FALSE;

//	// Create the plug-ins manager menu item...
//	MenuItem * pPlugDlgMngr = CreateMenuItem(OPTOKEN_PLUGSDLG, pRootMenu->GetMenuId(), TRUE);
//	// If created ok then add this to the main plug-in's menu
//	if (pPlugDlgMngr)
//		pRootMenu->AddMenuItem(pPlugDlgMngr);


//	// Create a menu item for the apply last item
	MenuItem * pApplyLastItem = CreateMenuItem(OPTOKEN_PHOTOSHOP_APPLYLAST, pRootMenu->GetMenuId(), FALSE);
	// If created ok then add this to the main plug-in's menu
	if (pApplyLastItem)
		pRootMenu->AddMenuItem(pApplyLastItem);
//


	// Go through all the handlers asking them to add in sections to the main menu
	PlugInHandler* pHandler = (PlugInHandler *)m_HandlersList.GetHead();
	PlugInHandler* pNextHandler = NULL;
	BOOL AddedOk = TRUE;
	BOOL AddSeparator = TRUE;
	while (pHandler != NULL)
	{
		// Try the next handler in the list
		pNextHandler = (PlugInHandler *)m_HandlersList.GetNext(pHandler);

		// If there is a next handler then we must add a separator to the
		// end of the list.
		// This does assume that only one handler will not want to add items to the end.
		// but this is good enough for now
		if (pNextHandler && pNextHandler->GoingToAddMainMenuItems())
			AddSeparator = TRUE;
		else
			AddSeparator = FALSE;

		// Ask the handler to add its required section of the menu to the list
		AddedOk = pHandler->CreateMainMenuItems(this, pRootMenu, AddSeparator);
	
		// Try the next handler in the list
		pHandler = pNextHandler;
	}
	

	// Make sure that the plug-ins win menu is updated to mirror the kernel
	// menu structure that we have just created
	BOOL update = UpdatePlugInsWinMenu(pRootMenu);
*/

//#endif
	return TRUE;
}
