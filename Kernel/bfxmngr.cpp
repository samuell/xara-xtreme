// $Id$
// bfxpmngr.cpp : implementation file
//
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

// This file contains specific BFX plug-in manager related items
// At present this includes:-

// BfxHandler
// Defines the characteristics of a BFX plug-in and provides a function
// for parsing them. These type of plug-ins are at present built in 
// and so are not parsed but added using the AddFixedPlugIns virtual function.

#include "camtypes.h"

#include "plugmngr.h"		// main header file
#include "bfxmngr.h"		// BfxHandler
#include "bfxitem.h"		// BfxPlugIn
#include "bfxop.h"			// BfxPlugInOp BfxContextMenu
#include "bfxopun.h"		// BfxPlugInUndoOp
#include "bfxdlg2.h"		// BfxPlugInDlg

#include "app.h"			// GetApplication()
#include "menuitem.h"		// MenuItem
#include "menupref.h"		// CreateMenuItem

#include "appprefs.h"		// OPTOKEN_PLUGSDLG


// Place any IMPLEMENT type statements here
CC_IMPLEMENT_DYNAMIC(BfxHandler, PlugInHandler)

// We want better memory tracking
#define new CAM_DEBUG_NEW

/**************************************************************************************
**************************************************************************************/

/**************************************************************************************

>	BfxHandler::BfxHandler()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Purpose:	Main constructor for the Bfx plug-in handler in Camelot. 

**************************************************************************************/

BfxHandler::BfxHandler()
{
}

/**************************************************************************************

>	BfxHandler::~BfxHandler()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Purpose:	Main destructor for the Bfx plug-in handler in Camelot. 

**************************************************************************************/

BfxHandler::~BfxHandler()
{
}

/**************************************************************************************

>	virtual BOOL BfxHandler::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Returns:	True if initialised ok, False otherwise.
	Purpose:	Main initialisation code for the Bfx plug-in handler in Camelot. 

**************************************************************************************/

BOOL BfxHandler::Init()
{
	// Initialise the operations we are going to need
	if (!BfxPlugInOp::Init())
		return FALSE;
//#if NEW_NATIVE_FILTER
	if (!BfxPlugInUndoOp::Init())
		return FALSE;
//#endif
	if (!BfxPlugInDlg::Init())
		return FALSE;

	return TRUE;
}

/**************************************************************************************

>	virtual String_32 BfxHandler::GetUniqueID() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Returns:	A string containing a filetype. 
	Purpose:	A string which defines the unique identifier for the plug-ins that this
				handler is interested in.
				In this case being Bfx it is 'Bfx:'.

**************************************************************************************/

String_32 BfxHandler::GetUniqueID() const
{
	return TEXT("BFX:");
}

/**************************************************************************************

>	virtual TypeOfPlugIn BfxHandler::GetTypeOfPlugIn() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Returns:	Type of the plug-in. 
	Purpose:	A unique type for the plug-ins that this handler is interested in.
				In this case being Bfx it is PLUGIN_BFX.

**************************************************************************************/

TypeOfPlugIn BfxHandler::GetTypeOfPlugIn() const
{
	return PLUGIN_BFX;
}

/**************************************************************************************

>	virtual String_32 BfxHandler::GetFileTypes() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Returns:	A string containing a filetype. 
	Purpose:	A string which defines the filetype or a list of filetypes that this
				type of plug-in is interested in.
				In this case being Bfx they are built in and have no filetype.
				Always supplied in lowercase.

**************************************************************************************/

String_32 BfxHandler::GetFileTypes() const
{
	return TEXT("");
}

/**************************************************************************************

>	virtual BOOL BfxHandler::AddFixedPlugIns()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Inputs:		-
	Returns:	True if worked, False otherwise
	Purpose:	Add any fixed plug-ins to the list in the plug-in manager and register the op
				descriptor to the system. By fixed, I mean those which are not file
				related and hence will be loaded when the specified paths are parsed
				for plug-ins.
				This is the main way that BFX type plug-ins are added to the system.

**************************************************************************************/

BOOL BfxHandler::AddFixedPlugIns()
{
	// Add our fixed plug-ins to the system
//	BfxBrightnessContrast * pBrightnessContrast = new BfxBrightnessContrast;
//	if (pBrightnessContrast)
//		AddPlugIn(pBrightnessContrast, CC_RUNTIME_CLASS(BfxPlugInOp), BfxPlugInOp::GetState);

	BfxSpecialEffects * pSpecialEffects = new BfxSpecialEffects;
	if (pSpecialEffects)
		AddPlugIn(pSpecialEffects, CC_RUNTIME_CLASS(BfxPlugInOp), BfxPlugInOp::GetState);

//	BfxResize * pBfxResize = new BfxResize;
//	if (pBfxResize)
//		AddPlugIn(pBfxResize, CC_RUNTIME_CLASS(BfxPlugInOp), BfxPlugInOp::GetState);

//	BfxFlipAndRotate * pBfxFlipAndRotate = new BfxFlipAndRotate;
//	if (pBfxFlipAndRotate)
//		AddPlugIn(pBfxFlipAndRotate, CC_RUNTIME_CLASS(BfxPlugInOp), BfxPlugInOp::GetState);

//	BfxColourDepth * pBfxColourDepth = new BfxColourDepth;
//	if (pBfxColourDepth)
//		AddPlugIn(pBfxColourDepth, CC_RUNTIME_CLASS(BfxPlugInOp), BfxPlugInOp::GetState);

	return TRUE;
}

/**************************************************************************************

>	virtual BOOL BfxHandler::ParsePlugIn(const PathName& Path, const INT32 PathID)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Inputs:		Path	the pathname of the file to parse
				PathID	the id assigned to this pathname
	Returns:	True if parsed ok and False otherwise. 
	Purpose:	Parses the specified plug-in and sees whether it is a Bfx plug-in.
				If so, then adds it to the list of available plug-ins, noting any useful
				details which will be required later.
				The PathID should be added to the plug-in record so that it can be
				translated back to the pathname at the appropriate time.

**************************************************************************************/

BOOL BfxHandler::ParsePlugIn(const PathName& Path, const INT32 PathID)
{
//	TRACEUSER( "Neville", _T("ParsePlugIn = %s\n"), (TCHAR*)Path.GetPath());
	// They are built in and so do not need parsing

	return TRUE;
}

/**************************************************************************************

>	virtual BOOL BfxHandler::AddPlugIn(PlugInItem * pPlugIn, CCRuntimeClass* pClass = CC_RUNTIME_CLASS(PlugInOp),
								  pfnGetState gs = PlugInOp::GetState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Inputs:		pPlugIn		the new PlugInItem to add to the list
				pClass		the operation handling class to use
				gs			the GetState function to use
	Returns:	True if worked, False otherwise
	Purpose:	Add a plug-in to the list in the plug-in manager and register the op
				descriptor to the system.
				We override the baseclass version as we need to register an undoable
				operation and a normal version for each plug-in. The undoable version
				is used in the document. The normal version is used in the bitmap
				gallery.

**************************************************************************************/

BOOL BfxHandler::AddPlugIn(PlugInItem * pPlugIn, CCRuntimeClass* pClass,	pfnGetState gs)
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

//#if NEW_NATIVE_FILTER
	// Now do the same to register the undoable operation to the system
	// Make up a unique name for the plug in
	OpToken = GetUniqueID();
	// Add the unique undo signature
	String_32 UndoSig(TEXT(PLUGIN_UNDO_SIG));
	OpToken += UndoSig;
	OpToken += pPlugIn->GetPlugInName();
	// Register this name as an OpDescriptor to the system so that we can use it
	// on menus etc.
	if (!PlugInUndoOp::RegisterOpToken(OpToken, pPlugIn->GetPlugInName(), CC_RUNTIME_CLASS(BfxPlugInUndoOp), BfxPlugInUndoOp::GetState))
		ERROR2(FALSE,"Failed to register undo OpToken for a plug-in");
//#endif

	// Now see if the family name is registered, if not then register it
	OpToken = GetUniqueID();
	OpToken += pPlugIn->GetFamilyName();
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OpToken);
	if (pOpDesc == NULL)
	{
		// Register this name as an OpDescriptor to the system so that we can use it
		// on menus etc.
		// Use the default OpDescriptor as this will have enough functionality for tick
		// greying i.e. none!
		if (!PlugInOp::RegisterOpToken(OpToken, pPlugIn->GetFamilyName())) //, pClass, gs))
			ERROR2(FALSE,"Failed to register OpToken for a plug-in family name");
	}

	// Now see if the undo version of the family name is registered, if not then register it
	OpToken = GetUniqueID();
	OpToken += UndoSig;
	OpToken += pPlugIn->GetFamilyName();
	pOpDesc = OpDescriptor::FindOpDescriptor(OpToken);
	if (pOpDesc == NULL)
	{
		// Register this name as an OpDescriptor to the system so that we can use it
		// on menus etc.
		// Use the default OpDescriptor as this will have enough functionality for tick
		// greying i.e. none!
		if (!PlugInUndoOp::RegisterOpToken(OpToken, pPlugIn->GetFamilyName(), CC_RUNTIME_CLASS(BfxPlugInUndoOp), BfxPlugInUndoOp::GetState))
			ERROR2(FALSE,"Failed to register OpToken for a plug-in family name");
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL BfxHandler::CreateMainMenuItems(PlugInManager * pManager, MenuItem * pRootMenu,
										 BOOL AddSeparator = FALSE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Inputs:		pManager	the plug-in manager to get at useful things like the plug-in list
				pRootMenu	The root of the main menu to add the items to
				AddSeparator	True if want a separator added to the last item on the menu
	Outputs:	-
	Returns:	True if the operation completed successfully
				False if it failed in some way.
	Purpose:	After the plug in list has been sorted alphabetically we must add each item
				in the list to the main plug-in menu. This is a handler's chance to insert
				menu options into this menu.

********************************************************************************************/

BOOL BfxHandler::CreateMainMenuItems(PlugInManager * pManager, MenuItem * pRootMenu,
									 BOOL AddSeparator)
{
	ERROR2IF(pManager == NULL || pRootMenu == NULL,FALSE,"CreateMainMenuItems bad parameters!");

//#if NEW_NATIVE_FILTER
	// Create a menu item for the apply last item
	//MenuItem * pApplyLastItem = CreateMenuItem(OPTOKEN_PLUGINS_PARSING, pRootMenu->GetMenuId(), FALSE);
	// If created ok then add this to the main plug-in's menu
	//if (pApplyLastItem)
	//	pRootMenu->AddMenuItem(pApplyLastItem);

	// Create the plug-ins manager menu item...
	MenuItem * pPlugDlgMngr = CreateMenuItem(OPTOKEN_PLUGSDLG, pRootMenu->GetMenuId(), TRUE);
	// If created ok then add this to the main plug-in's menu
	if (pPlugDlgMngr)
		pRootMenu->AddMenuItem(pPlugDlgMngr);

	// Did we find the plug-in main menu item?
	// If we did then we need to add the plug-ins to the item
	String_32 OpToken;
	String_32 FamilyOpToken;
	String_32 LastFamilyOpToken;
//	MenuItem* pFamilyRoot = NULL;
	MenuItem* pFamilyRoot = pRootMenu;
	String_32 UndoSig(TEXT(PLUGIN_UNDO_SIG));
	// Go through the list of plug-ins registered to the system and make up a
	// menu structure from the OpTokens that they have registered.
	// Assumes the plug-ins are sorted correctly on family names and then
	// plug-in names.
	PlugInItem * pPlugIn = pManager->GetFirstPlugIn();
	BOOL ok = TRUE;
	while (pPlugIn != NULL)
	{
		// Only allow Bfx plug-ins on this menu. We are very plug-inist!
		if (pPlugIn->GetTypeOfPlugIn() == PLUGIN_BFX)
		{
			OpToken = pPlugIn->GetUniqueID();
			FamilyOpToken = OpToken;
			// Add the unique undo signature
			OpToken += UndoSig;
			FamilyOpToken += UndoSig;
			// Now add the menu names themselves
			OpToken += pPlugIn->GetPlugInName();
			FamilyOpToken += pPlugIn->GetFamilyName();
			// check if we already have 
//			if (FamilyOpToken != LastFamilyOpToken)
//			{
//				MenuItem * pNewFamilyItem = CreateMenuItem(FamilyOpToken, pRootMenu->GetMenuId(), FALSE);
//				if (pNewFamilyItem)
//				{
//					// Created ok so add this to the main plug-in's menu
//					ok = ok && pRootMenu->AddMenuItem(pNewFamilyItem);
//					// This becomes the new family root
//					pFamilyRoot = pNewFamilyItem;
//				}
//			}
//
			if (pFamilyRoot)
			{
				// Create a menu item for the plug-in itself
				MenuItem * pNewItem = CreateMenuItem(OpToken, pFamilyRoot->GetMenuId(), FALSE);
				// If created ok then add this to the main plug-in's menu
				if (pNewItem)
					ok = ok && pFamilyRoot->AddMenuItem(pNewItem);
			}

			LastFamilyOpToken = FamilyOpToken;
		}

		pPlugIn = pManager->GetNextPlugIn(pPlugIn);
	}


	// Tell the last item that it should have a separator 
//	if (pFamilyRoot != NULL && AddSeparator)
//		pFamilyRoot->SetIsFollowedBySeparator(TRUE);

//#endif
	return TRUE;
}
