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

#include "bfxop.h"			// useful BFX plug-in related operations
//#include "plugres.h"		// _R(IDS_PLUGIN_FAMILY)

#include "plugmngr.h"		// main plug-in manager
#include "bfxitem.h"		// useful Bfx plug-in related items
//#include "app.h"			// GetApplication() - in camtypes.h [AUTOMATICALLY REMOVED]

//#include "bitmap.h"			// KernelBitmap - in camtypes.h [AUTOMATICALLY REMOVED]
#include "menuitem.h"		// MenuItem

// Place any IMPLEMENT type statements here
CC_IMPLEMENT_DYNCREATE(BfxPlugInOp, PlugInOp);
CC_IMPLEMENT_MEMDUMP(BfxPlugInContextMenu, PlugInsContextMenu)

// We want better memory tracking
#define new CAM_DEBUG_NEW

// Set up our statics
KernelBitmap*	BfxPlugInOp::m_pBitmap		= NULL;
Document*		BfxPlugInOp::m_pDocument	= NULL;

/********************************************************************************************

>	BfxPlugInOp::BfxPlugInOp()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Inputs:
	Outputs:
	Returns:
	Purpose:	Constructor for BfxPlugInOp operation. It is not undoable.
	Errors:		None

********************************************************************************************/

BfxPlugInOp::BfxPlugInOp()
{
}

/********************************************************************************************

>	static BOOL BfxPlugInOp::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Returns:	TRUE if the Op was started ok
	Purpose:	Creates all the opDescriptors that call this operation.

********************************************************************************************/

BOOL BfxPlugInOp::Init()
{
	BOOL ok = TRUE;
	// Set up some standard operations
/*	ok = ok && RegisterOpToken(OPTOKEN_Bfx_PLUGINS, _R(IDS_Bfx_PLUGINS),
							   CC_RUNTIME_CLASS(BfxPlugInOp), BfxPlugInOp::GetState);
	ok = ok && RegisterOpToken(OPTOKEN_Bfx_APPLYLAST, _R(IDS_Bfx_APPLYLAST),
							   CC_RUNTIME_CLASS(BfxPlugInOp), BfxPlugInOp::GetState);
*/
	return ok;
}

/********************************************************************************************

>	static BOOL BfxPlugInOp::CheckBitmapWithPlugIn(const String_256& OpName,
												   BOOL IncludeUniqueName = TRUE,
												   BOOL IncludeUndoAbleSig = FALSE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Inputs:		OpName				- the name of the OpToken being requested.
				IncludeUniqueName	- Set to True (Default) if want the
						unique name of the plug-in added to the OpToken
				IncludeUndoAbleSig	- Set to True is want the unique undoable
						sig added to the OpToken. (Default = FALSE.)
	Returns:	True if the item is ok with the plug-in, False otherwise
	Purpose:	To ask the plug-in which matches the OpName whether the
				bitmap is ok.
				Assumes only working on Bfx plug-ins. Not that this should
				make much difference at present.
	Errors:		None

********************************************************************************************/

BOOL BfxPlugInOp::CheckBitmapWithPlugIn(const String_256& OpName, BOOL IncludeUniqueName,
										BOOL IncludeUndoAbleSig)
{
	// If no bitmap then always return False
	if (m_pBitmap == NULL)
		return FALSE;

	// Search the plug-ins list for the specified plug-in and check
	// that this colour depth is ok with it
	INT32 ColourDepth = m_pBitmap->GetBPP();
	BOOL GreyScale = m_pBitmap->IsGreyscale();
	PlugInManager* pManager = GetApplication()->GetPlugInManager();
	if (pManager)
	{
		PlugInItem * pPlugIn = pManager->GetFirstPlugIn();
		String_32 OpToken;
		// Add the unique undo signature
		String_32 UndoSig(TEXT(PLUGIN_UNDO_SIG));
		while (pPlugIn)
		{
			// If we are to include the unique name in the test then do so
			if (IncludeUniqueName)
				OpToken = pPlugIn->GetUniqueID();
			else
				OpToken.Empty();
			if (IncludeUndoAbleSig)
				OpToken += UndoSig;
			OpToken += pPlugIn->GetPlugInName();
			if (OpName == OpToken)
			{
				// Ask the plug-in item whether it likes this colour depth
				// Returns True if likes it, False otherwise, so we must negate this to
				// set the greyed state.
				return pPlugIn->IsBitmapModeOk(ColourDepth, GreyScale);
			}
			
			pPlugIn = pManager->GetNextPlugIn(pPlugIn);
		}
	}

	// Cannot find plug-in so return False
	return FALSE;
}

/********************************************************************************************

>	static OpState BfxPlugInOp::GetState(String_256* pDesc, OpDescriptor* pOpDesc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Inputs:		pOpDesc	- the OpDescriptor being used.
	Outputs:	pDesc - if MenuItem is enabled Then return the name of the operation to 
							be undone Else return the reason why it is disabled
	Returns:	OpState the state of the operation
	Purpose:	To provide greying and ticking functionality for the
				operation.
				Assumes only working on Bfx plug-ins. Not that this should
				make much difference at present.
	Errors:		None

********************************************************************************************/

OpState BfxPlugInOp::GetState(String_256* pDesc, OpDescriptor* pOpDesc)
{
	// At present, this item is always available unless there is
	// no bitmap and document to apply it to.
	OpState OpSt;
	if (m_pBitmap == NULL || m_pDocument == NULL)
		OpSt.Greyed = TRUE;

	if (m_pBitmap)
	{
		// If there is a bitmap then we are always available
		OpSt.Greyed = FALSE;

/*		// If we have a valid bitmap then checks that it is of the correct type
		INT32 ColourDepth = m_pBitmap->GetBPP();

		// Only allow bitmaps of 24bpp or of 8bpp greyscale
		if (ColourDepth == 24 || (ColourDepth == 8 && m_pBitmap->IsGreyscale()))
		{
			// Check the bitmap depth with the plug-in itself
			// Returns True if likes it, False otherwise, so we must negate this to
			// set the greyed state.
			OpSt.Greyed = !CheckBitmapWithPlugIn(pOpDesc->Token);
		}
		else
			OpSt.Greyed = TRUE;
*/
	}

	return OpSt;
}

/********************************************************************************************

>	void BfxPlugInOp::Do(OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Inputs:		pOpDesc		the OpDescriptor of the item to invoke
	Outputs:	None
	Returns:	None
	Purpose:	Invokes the plug-in specified by the OpDescriptor.
	Errors:		if no OpDescriptor is supplied

********************************************************************************************/

void BfxPlugInOp::Do(OpDescriptor* pOpDesc)
{
	if (pOpDesc == NULL || m_pBitmap == NULL || m_pDocument == NULL)
	{
		ERROR3("BfxPlugInOp::Do null OpDescriptor/bitmap/document");
		FailAndExecute(); 
		End();
		return;
	}
	//ERROR3("PlugInOp - do");

	// Search the plug-ins list for the specified plug-in and invoke it
	PlugInManager* pManager = GetApplication()->GetPlugInManager();
	if (pManager == NULL)
		return;

	PlugInItem * pPlugIn = pManager->GetFirstPlugIn();

	String_32 OpToken;
	while (pPlugIn)
	{
		OpToken = pPlugIn->GetUniqueID();
		OpToken += pPlugIn->GetPlugInName();
		if (pOpDesc->Token == OpToken)
		{
			BOOL ok = pPlugIn->Apply(m_pBitmap, m_pDocument);
			if (!ok)
			{
				FailAndExecute(); 
			}
				
			// and finish as we have done our work
			End();
			return;
		}

		pPlugIn = pManager->GetNextPlugIn(pPlugIn);
	}

	// We must have failed to find the plug-in operation so fail and finish
	ERROR3("BfxPlugInOp::Do Failed to find plug-in operation");
	FailAndExecute(); 
	End();
}

/********************************************************************************************

>	static BOOL BfxPlugInOp::SetBitmapAndDocument(KernelBitmap* pBitmap, Document * pDocument)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Inputs:		pBitmap		the bitmap to apply the effect to
				pDocument	the document the bitmap is stored in
	Purpose:	Provides the parameters that the operation will apply to.

********************************************************************************************/

BOOL BfxPlugInOp::SetBitmapAndDocument(KernelBitmap* pBitmap, Document * pDocument)
{
	m_pBitmap = pBitmap;
	m_pDocument = pDocument;
	return TRUE;
}

/********************************************************************************************
// BfxPlugInContextMenu
********************************************************************************************/

/********************************************************************************************

>	BfxPlugInContextMenu::BfxPlugInContextMenu()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Inputs:		pBitmap		the bitmap to apply the effect to
				pDocument	the document the bitmap is stored in
	Outputs:	-
	Purpose:	Default constructer.

********************************************************************************************/

BfxPlugInContextMenu::BfxPlugInContextMenu(KernelBitmap* pBitmap, Document * pDocument)
{
	BfxPlugInOp::SetBitmapAndDocument(pBitmap, pDocument);
	
	// remember them for later use
	m_pBitmap = pBitmap;
	m_pDocument = pDocument;

}

/********************************************************************************************

>	virtual BOOL BfxPlugInContextMenu::Build(void)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Returns:	TRUE if menu built OK
				FALSE (and sets error) otherwise
	Purpose:	To build the Bfx plug-ins pop-up menu.

********************************************************************************************/

BOOL BfxPlugInContextMenu::Build()
{
	return BfxPlugInContextMenu::BuildMenu(m_pBitmap, m_pDocument, this);
}

/********************************************************************************************

>	static BOOL BfxPlugInContextMenu::BuildMenu(KernelBitmap * pBitmap, Document * pDocument,
												ContextMenu * pContextMenu, MenuItem * pMainRoot = NULL,
												BOOL AddSeparator = FALSE, BOOL UseUndoable = FALSE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Inputs:		pBitmap			bitmap the effect is to be applied to
				pDocument		the document the bitmap is in
				pContextMenu	the menu we want to add to
				pMainRoot		the root menu to add this menu to, defaults to no parent
				AddSeparator	True if want a separator added to the last item on the menu
				UseUndoable		True if want the undoable operations instead of the non-undoable
								Defaults to False
	Returns:	TRUE if menu built OK
				FALSE (and sets error) otherwise
	Purpose:	To build the a Bfx plug-ins pop-up menu either directly or tacked on
				as a sub-menu item to the specified pMainRoot.

********************************************************************************************/

BOOL BfxPlugInContextMenu::BuildMenu(KernelBitmap * pBitmap, Document * pDocument,
									 ContextMenu * pContextMenu, MenuItem * pMainRoot,
									 BOOL AddSeparator, BOOL UseUndoable)
{
	ERROR2IF(pContextMenu == NULL,FALSE,"BfxPlugInContextMenu::BuildMenu null ContextMenu to add to");

	// Inform the Bfx operation what bitmap and document it will be working on
	BfxPlugInOp::SetBitmapAndDocument(pBitmap, pDocument);

	BOOL ok = TRUE;
	// In the non-undoable case we require the bitmap and document pointers.
	// In the undoable case these will be null.
	if ((pBitmap != NULL && pDocument != NULL) || UseUndoable)
	{
		//ok = ok && pContextMenu->BuildCommand(OPTOKEN_Bfx_APPLYLAST, TRUE, pMainRoot);

		PlugInManager* pManager = GetApplication()->GetPlugInManager();
		if (pManager != NULL)
		{
			String_32 OpToken;
			String_32 FamilyOpToken;
			String_32 LastFamilyOpToken;
			MenuItem* pFamilyRoot = NULL;
			// Add the unique undo signature
			String_32 UndoSig(TEXT(PLUGIN_UNDO_SIG));
			// Go through the list of plug-ins registered to the system and make up a
			// menu structure from the OpTokens that they have registered.
			// Assumes the plug-ins are sorted correctly on family names and then
			// plug-in names.
			PlugInItem * pPlugIn = pManager->GetFirstPlugIn();
			while (pPlugIn)
			{
				// Only allow Bfx plug-ins on this menu. We are very plug-inist!
				if (pPlugIn->GetTypeOfPlugIn() == PLUGIN_BFX)
				{
					OpToken = pPlugIn->GetUniqueID();
					if (UseUndoable)
						OpToken += UndoSig;
					FamilyOpToken = OpToken;
					OpToken += pPlugIn->GetPlugInName();
					FamilyOpToken += pPlugIn->GetFamilyName();
					// check if we already have 
//					if (FamilyOpToken != LastFamilyOpToken)
//					{
//						ok = ok && pContextMenu->BuildCommand(FamilyOpToken, FALSE, pMainRoot);
//						pFamilyRoot = pContextMenu->GetLastItem();
//					}
//					ok = ok && pContextMenu->BuildCommand(OpToken, FALSE, pFamilyRoot);
					ok = ok && pContextMenu->BuildCommand(OpToken, FALSE, pMainRoot);

					LastFamilyOpToken = FamilyOpToken;
				}

				// Go to the next plug-in in the list
				pPlugIn = pManager->GetNextPlugIn(pPlugIn);
			}

			// Tell the last item that it should have a separator 
			if (pFamilyRoot != NULL && AddSeparator)
				pFamilyRoot->SetIsFollowedBySeparator(TRUE);
		}
	}
	
	return ok;
}
