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
// sglbase.cpp - SuperGallery Library Base class (standard library gallery stuff)

#include "camtypes.h"
#include "sglbase.h"
#include <io.h>
#include <string.h>
#include <strstrea.h>
#include "pathnmex.h" 
#include "sgscanf.h"
//#include "sgscan.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "galstr.h"
//#include "pathname.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "sginit.h"
#include "sglcart.h"	// This .cpp file's corresponding header
#include "sglib.h"
//#include "sgtree.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "thumb.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "thumbmsg.h"
#include "grnddib.h"
//#include "galres.h"
//#include "richard.h"
//#include "simon.h"		// _R(IDC_CONTINUE)
#include "sgliboil.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "filedlgs.h"
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "progress.h"
#include "camelot.h"

#include "dragmgr.h"
//#include "viewrc.h"		// FOR _R(IDC_CANDROPONPAGE)
//#include "resource.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "spread.h"

#include "sgrmfile.h"	// For Lib settings
//#include "richard2.h"
//#include "richard3.h"

#include "registry.h"	// SaveRegistryEntries LoadRegistryEntries
#include "product.h"	// PRODUCT_GALLERY_REGISTRYKEY
//#include "webster.h"
#include "inetop.h"

// Implement the dynamic class bits...
CC_IMPLEMENT_DYNCREATE(LibraryGallery, SuperGallery)

// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW

using namespace InetUtils;

// Statics...

// Ask the user if they want to 'remove all existing groups before adding this one or not'
BOOL LibraryGallery::AskAboutRemoving = FALSE;

// Offset into folders of the first one removed by RemoveSimilarLibraryGroups
INT32 LibraryGallery::OffsetOfFirstGroupRemoved = -1;

// Was the first removed group selected or not ?
BOOL LibraryGallery::FirstGroupRemovedSelectionState = FALSE;

// Temporary bitmap used for dragging
KernelBitmap *LibraryGallery::TmpDraggingBitmap = NULL;


/***********************************************************************************************

>	class LibraryGallery : public SuperGallery

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95 (Finally)
	Purpose:	A base library gallery class with common library gallery stuff in

	Notes:		SuperGalleries are specced in a number of docs including
					specs\sgallery.doc ("super" gallery extensions)
					specs\propui.doc  (gallery basic ui)
				There is also howtouse\sgallery.doc which describes creating a supergallery

	SeeAlso:	SuperGallery; SGDisplayLibClipart

***********************************************************************************************/

LibraryGallery::LibraryGallery()
{
	if (!OpAddWebFolders::Init() || !OpAddWebLibrary::Init() || !OpThumbDownload::Init())
	{
		ERROR3("Failed to init web operations");
	}

	// Initialising to true means Camelot will try and download thumbnails without you having to press the 'Get Fills' 
	// (or equivalent) button. Because downloading is now suspended when an error occurs, offline users (with a
	// partial thumb stock) no longer get continual popups when scrolling down a page of unloaded thumbnails.
	// Fixes, with inetop.cpp change, #10471
	bThumbDownloading = TRUE;
	m_bDiscardWebFolders = FALSE;
}

LibraryGallery::~LibraryGallery()
{
	TRACEUSER( "Richard", _T("~LibraryGallery called\n"));
}



/********************************************************************************************

>	virtual MsgResult LibraryGallery::Message(Msg* Message)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95
	Inputs:		Message - The message to handle

	Purpose:	Standard library gallery message handler

	Notes:		Any messages that this does not handle must be passed down to the
				SuperGallery base class message handler.

				NOTE WELL that the SuperGallery base class handler does some funky things
				for us - see SuperGallery::Message - such as deleting our display subtree
				for any document which dies (which, uncannily, would explain why they go
				away like that when you close documents ;-), and shading the gallery when
				there are no documents present. [To override this behaviour in these cases,
				you should respond to the message, and return OK rather than calling the
				base class message handler]

	SeeAlso:	SuperGallery::Message; LibClipartSGallery::Message; etc...

********************************************************************************************/

MsgResult LibraryGallery::Message(Msg* Message)
{
	// If we have no displaytree, then we have not been shown, or something terrible has
	// happened, so we don't bother handling any of these messages.
	if (DisplayTree == NULL)
		return(SuperGallery::Message(Message));

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		switch (Msg->DlgMsg)
		{
			case DIM_LFT_BN_CLICKED:
				switch (Msg->GadgetID)
				{
					case _R(IDC_LIBGAL_REMOVE):
						RemoveSelectedLibraries(TRUE);
						UpdateGRMFile();
						break;
// >>webster (adrian 15/12/96)
					case _R(IDC_LIBGAL_WEB_CLIPART):
					case _R(IDC_LIBGAL_WEB_FILLS):
					case _R(IDC_LIBGAL_WEB_FONTS):
						OnGetButtonClicked();
						break;
					case _R(IDC_LIBGAL_STOP_LOADING):
						SuspendThumbnailDownloading();
						break;
// <<webster
				}
		}
	}

	return SuperGallery::Message(Message);
}

/********************************************************************************************

>	virtual BOOL LibraryGallery::CanSearchKeywords(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95

	Returns:	TRUE

	Purpose:	Used to determine if this type of gallery supports keyword searching.
				At the moment, all except the fonts gallery do.
				
********************************************************************************************/

BOOL LibraryGallery::CanSearchKeywords(void)
{
	return SuperGallery::CanSearchKeywords();
}


/***********************************************************************************************

>	virtual SGDisplayGroup *LibClipartSGallery::AddLibraryGroup(Library *LibraryToDisplay, INT32 NumItems)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95

	Inputs:		LibraryToDisplay - Pointer to the library associated with the group
				NumItems - Number of items in the group
	Returns:	The newly created library group, or NULL if there were problems
	Purpose:	Create a library group, as opposed to a display group

***********************************************************************************************/

SGDisplayGroup *LibraryGallery::AddLibraryGroup(Library *LibraryToDisplay, INT32 NumItems)
{
	return SuperGallery::AddLibraryGroup(LibraryToDisplay, NumItems);
}


/********************************************************************************************

>	void LibrarybGallery::SelectionHasChanged(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95

	Purpose:	To inform the gallery that the selection has changed in some way.
				This function handles the default library gallery buttons		

********************************************************************************************/

void LibraryGallery::SelectionHasChanged(void)
{
	// Always ungrey the library menu and browse buttons
	EnableGadget(_R(IDC_GALLERY_MENU), TRUE);
	EnableGadget(_R(IDC_LIBGAL_BROWSE), TRUE);

	// Check if we should grey the 'remove' button
	INT32 Count = DisplayTree->GetSelectedGroupCount();
	EnableGadget(_R(IDC_LIBGAL_REMOVE), (Count > 0));
	EnableGadget(_R(IDC_LIBGAL_STOP_LOADING), !m_lstPendingThumbs.empty());
}

/********************************************************************************************

>	virtual void LibraryGallery::DoShadeGallery(BOOL ShadeIt)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95

	Purpose:	Handles the standard library gallery button shading in the gallery
	Notes:

********************************************************************************************/

void LibraryGallery::DoShadeGallery(BOOL ShadeIt)
{
	// make sure the standard gallery buttons are updated properly
	LibraryGallery::SelectionHasChanged();
} 

/********************************************************************************************

>	virtual BOOL LibraryGallery::PreCreate(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95

	Returns:	TRUE if the Gallery initialised successfully
				FALSE if it should not be opened due to a failure to initialise

	Purpose:	The LibraryGallery PreCreate handler. This overrides the base class
				PreCreate function. It is called at the very beginning of the
				SuperGallery::Create method, before the window has been created.

	Notes:		As this is called before the window is open, it must not attempt to touch
				any of the button gadgets in the window, or force redraws, etc. Also,
				events cannot be passed to the tree, as the tree formatting relies on
				knowing the window size - however, the tree will be reformatted and
				redrawn automatically when the window is opened - this will happen shortly.

********************************************************************************************/

BOOL LibraryGallery::PreCreate(void)
{
	return SuperGallery::PreCreate();
}

/********************************************************************************************

>	virtual BOOL LibraryGallery::ApplyAction(SGActionType Action)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95

	Inputs:		Action - Indicates what action to apply

	Returns:	TRUE to indicate successful handling of the action, or
				FALSE to indicate failure

	Purpose:	Applies certain conventional gallery actions (usually associated with
				gallery buttons, for new, edit, delete, etc)

	SeeAlso:	SGActionType

********************************************************************************************/

BOOL LibraryGallery::ApplyAction(SGActionType Action)
{
	return SuperGallery::ApplyAction(Action);
}


/********************************************************************************************

>	virtual BOOL LibraryGallery::AddNewLibrary(PathName *LibPath, BOOL WipeOld, SGLibType LibType, BOOL Update = FALSE)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95

	Inputs:		LibPath	- should be set to the new library location
				WipeOld	- will wipe the old groups from the gallery if TRUE
				LibType	- should be set the the library type for adding
				Update	- set to true if updating rather than adding a new library. This just causes the
						  new group to keep the position where the old group was.

	Returns:	TRUE if things went OK

	Purpose:	This call will optionally kill all the old library groups and then recreate
				library groups for the given libpath
	Notes:		
	SeeAlso:

********************************************************************************************/

BOOL LibraryGallery::AddNewLibrary(PathName *LibPath, BOOL WipeOld, SGLibType LibType, BOOL Update)
{
/*	String_256 P(LibPath->GetPath());
	ERROR3_PF(("LibraryGallery::AddNewLibrary -> %s", (TCHAR *)P));*/

	ERROR3IF(DisplayTree==NULL, "No display tree!");
	if(DisplayTree == NULL) return FALSE;

	if(LibPath == NULL)
	{
		ERROR3("LibraryGallery::AddNewLibrary given null LibPath");
		return FALSE;
	}

	if(WipeOld)
	{
		// Kill all existing lib groups
		OpenLibFiles.DeleteAll();
		LibraryGallery::SelectionHasChanged();
	}

	// Open the new index and add it to the gallery
	LibraryFile *LibFile = new LibraryFile;
	if (LibFile != NULL)
	{
		// OK. Lets initialise the library. This will call us back via base-class
		// SuperGallery methods to add a group and items for each {sub}library
		// found in the given library file.
		if(!LibPath->IsValid())
		{
			ERROR3("LibraryGallery::AddNewLibrary given an invalid default library path");
			delete LibFile;
			InvalidateCachedFormat();
			ReformatAndRedrawIfNecessary();
			return FALSE;
		}

		String_64 SlowJob(_R(IDS_LIBRARY_SCANNING));
		BeginSlowJob(-1, FALSE, &SlowJob);

		InvalidateCachedFormat();

		// This will give the option of generating a new index file - Don't do the scroll redraw here...
		if (!LibFile->Init(this, LibPath, LibType, Update, FALSE))
		{
			// InformError if it returned FALSE? **** !!!!
			TRACEUSER( "Richard", _T("Problems with this new library... -> LibFile::Init returned false\n"));
			delete LibFile;
			EndSlowJob();
			// And force a redraw of the entire list
			ReformatAndRedrawIfNecessary();
			return FALSE;
		}
		else
		{
			SortOutGroupsAfterLibraryAdd(LibFile, Update);
		}

		EndSlowJob();
	}
	
	// And force a redraw of the entire list
	InvalidateCachedFormat();
	ReformatAndRedrawIfNecessary();
	return TRUE;
}





/********************************************************************************************

>	virtual BOOL LibraryGallery::SortOutGroupsAfterLibraryAdd(LibraryFile *LibFile, BOOL Update)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/95
	Inputs:		LibFile	- LibraryFile just created and initialised
				Update	- set to true if updating rather than adding a new library. This just causes the
						  new group to keep the position where the old group was.
	Returns:	TRUE if things went OK
	Purpose:	This call will scroll to and redraw the newly added groups. Also, if we've just updated
				the group which is being added, we'll work out where it should be placed in the
				gallery...
	Notes:		We assume the last group added to the gallery was placed at the end of the list...
	
********************************************************************************************/

BOOL LibraryGallery::SortOutGroupsAfterLibraryAdd(LibraryFile *LibFile, BOOL Update)
{
	// If updating a group then adding it back to the gallery, we want it added in the position
	// that the old group was in...
	BOOL AddedLibFileToOpenLibList = FALSE;

	if(Update && LibraryGallery::OffsetOfFirstGroupRemoved > -1 && GetDisplayTree() != NULL)
	{
		// If we were updating a group, then adding it, we need to move the new group into the old group's
		// position			
		// Get the last group, and move it to where it should be...
		SGLibGroup *LastGroup = (SGLibGroup *)GetDisplayTree()->GetChild();	
		if(LastGroup != NULL)
		{
			while(LastGroup->GetNext() != NULL)
				LastGroup = (SGLibGroup *)LastGroup->GetNext();
							
			LastGroup->SetSelected(LibraryGallery::FirstGroupRemovedSelectionState);

			// Find the group at offset 'OffsetOfFirstGroupRemoved'
			INT32 GroupCount = 0;
			SGLibGroup *Group = (SGLibGroup *)GetDisplayTree()->GetChild();		
			while(Group != NULL && GroupCount < LibraryGallery::OffsetOfFirstGroupRemoved)
			{
				Group = (SGLibGroup *)Group->GetNext();
				GroupCount ++;
			}

			// If we were the last item anyhow, don't worry about moving the groups and stuff...
			if(Group != NULL && Group->GetNext() != NULL)
			{
				Group->MoveBefore(LastGroup);
				OpenLibFiles.InsertBefore((LISTPOS)LibraryGallery::OffsetOfFirstGroupRemoved, LibFile);
				AddedLibFileToOpenLibList = TRUE;
			}
		}
	}

	// Just a simple, add to bottom of list, type addition...
	if(!AddedLibFileToOpenLibList)
		OpenLibFiles.AddTail(LibFile);

	// Now we've fiddled about with the groups, it should be safe to redraw properly...
	LibFile->DoScrollRedraw();

	UpdateGRMFile();

	return TRUE;
}

/********************************************************************************************

>	virtual SGDisplayItem *LibraryGallery::AddLibraryItem(SGDisplayGroup *LibraryGroup,
												Library *ParentLib,
												LibraryIndex ItemIndex)
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95

	Inputs:		LibraryGroup - The group to add the item into
				ParentLib - (For cross checking inputs) the library you allege the above
				group is for.
				ItemIndex - The Library generated index for this item

	Returns:	NULL, or a pointer to the created item

	Purpose:	Called by the Library class to create a display item for every item in 
				a newly-scanned library file. It is essentially a callback to the gallery
				which requested that the library be scanned.
				
	Notes:		This method MUST BE OVERRIDDEN by the derived gallery that opens the library,
				in order to create appropriate SGDisplayItem-derived nodes for the things
				in the library (e.g. a clipart library gallery will have to create items
				that display clipart thumbnails)

	SeeAlso:	SuperGallery::AddLibraryGroup; SuperGallery::RemoveLibraryGroup

********************************************************************************************/

SGDisplayItem *LibraryGallery::AddLibraryItem(SGDisplayGroup *LibraryGroup, Library *ParentLib, LibraryIndex ItemIndex, BOOL bNew)
{
	return SuperGallery::AddLibraryItem(LibraryGroup, ParentLib, ItemIndex, bNew);
}


/********************************************************************************************

>	virtual void LibraryGallery::HandleDragStart(DragMessage *DragMsg)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95

	Inputs:		DragMsg - The DRAGSTARTED message that we've just recieved, indicating
				the type of drag being started

	Purpose:	Checks a DragMessage to see if it is a colour drag.
				If it is, then it creates a drag target for this gallerys listbox.

	Notes:		Overrides the default base-class action. Calls down to the base class
				if it is not a colour drag, so that dragging of gallery groups is allowed

********************************************************************************************/

void LibraryGallery::HandleDragStart(DragMessage *DragMsg)
{
	SuperGallery::HandleDragStart(DragMsg);
}

/********************************************************************************************

>	virtual SGDisplayItem *LibraryGallery::CopyDisplayItem(SGDisplayItem *SourceItem, 
													SGDisplayGroup *DestGroup,
													SGDisplayItem *TargetPosition = NULL)
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95

	Inputs:		SourceItem - The item to copy elsewhere in the tree (see below)

				DestGroup - The group into which the item should be inserted

				TargetPosition - NULL (to add the copied item to the end of the sibling
				list), or points to an item BEFORE which the copied item will be inserted.

	Returns:	NULL (failed) or a pointer to the new (copied) display item

	Purpose:	"Copies" the existing node in the tree in an appropriate fashion.
				
				This method is normally called when a gallery-organising drag completes,
				and it is discovered that the dragged item(s) have been dragged to a 
				different display group.

	Notes:		This method should be overridden by derived galleries to provide
				appropriate behaviour (some galleries (e.g colour) will copy the real-item
				that the given display-item references to the new group (document), while
				other galleries (layer) may just move the item after all).

				Note the handy InsertCopiedItem and MoveBefore/After methods which
				are available to take all of the hard work out of copying/moving items!

				See the body of this method in the source code for example pseudocode.
				For real code, see the Colour Gallery (sgcolour.cpp)

	SeeAlso:	SuperGallery::InsertCopiedItem; SGDisplayItem::MoveBefore;
				SGDisplayItem::MoveAfter; ColourSGallery::CopyDisplayItem

********************************************************************************************/

SGDisplayItem *LibraryGallery::CopyDisplayItem(SGDisplayItem *SourceItem, 
								SGDisplayGroup *DestGroup, SGDisplayItem *TargetPosition)
{
	return SuperGallery::CopyDisplayItem(SourceItem, DestGroup, TargetPosition);
}

/********************************************************************************************

>	virtual BOOL LibraryGallery::BrowseClicked(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95

	Purpose:	Handle clicks on the browse button

********************************************************************************************/

BOOL LibraryGallery::BrowseClicked(void)
{
	return FALSE;
}


/********************************************************************************************

>	virtual BOOL LibraryGallery::BrowseClicked(String_256 *DefaultPath, SGLibType Type, INT32 Message)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95

	Inputs:		DefaultPath	- Default path for Browse dialog to open up in
				Type		- Type of Browse dialog
				Message		- Message to ask whether we should kill all the groups or not
	Outputs:	DefaultPath	- Updated if browse dialog added a new library
	
	Purpose:	Shared Library Gallery Browse Clicked procedure

********************************************************************************************/

BOOL LibraryGallery::BrowseClicked(String_256 *DefaultPath, SGLibType Type, INT32 Message)
{
	BOOL Updated = FALSE;
	PathName ThePath(*DefaultPath);	

	// This returns FALSE if Cancel was hit, or an error occurred.
	if(SGLibOil::GetLibPath(this, &ThePath, CanCreateIndexes(), Type, &Updated))
	{
		if(!ThePath.IsValid())
		{
			String_256 Tmp(ThePath.GetPath());
			ERROR3_PF(("LibraryGallery::BrowseClicked GetLibPath returned an invalid pathname - '%s'", (TCHAR *)Tmp));
		}
		else
		{							
			// Remove by default
			INT32 ButtonPressed = 2;

			if(LibraryGroupsInGallery() > 0 && LibraryGallery::AskAboutRemoving)
			{
				// Find out whether we should kill all the other groups
				ButtonPressed = InformMessage(Message, _R(IDS_REMOVE), _R(IDS_KEEP), _R(IDS_CANCEL)/*, _R(IDS_HELP)*/);
				Error::ClearError();

				if(ButtonPressed == 3)
					return FALSE;
			}

			// Remove clicked if button pressed == 1
			if(AddNewLibrary(&ThePath, ButtonPressed == 1, Type, Updated))
			{							
				// Remember the new path for the next time
				*DefaultPath = ThePath.GetPath();
			}

			// Sort the entire gallery alphabetically
			SortGallery();

			ReformatAndRedrawIfNecessary();

			return TRUE;
		}
	}
	return FALSE;
}

/********************************************************************************************

>	virtual BOOL LibraryGallery::CanCreateIndexes(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95

	Returns:	TRUE to if index generation is possible

	Purpose:	To determine if this gallery can generate indexes or not
				If you want to create indexes, override and return TRUE

********************************************************************************************/

BOOL LibraryGallery::CanCreateIndexes(void)
{
	return FALSE;
}


/********************************************************************************************

>	virtual BOOL GetDefaults(String_256 *DefaultIndex, String_256 *IndexDesc, SGLibType *Type)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95

	Outputs:	DefaultIndex	- The filename for the default index file (Xaraclip.txt)
				IndexDesc		- Description of the index / gallery (Clipart)
				Type			- Default library type associated with this gallery

	Returns:	TRUE if this was overridden successfully 

	Purpose:	To determine various library gallery default properties

********************************************************************************************/

BOOL LibraryGallery::GetDefaults(String_256 *DefaultIndex, String_256 *IndexDesc, SGLibType *Type)
{
/*	case SGLib_ClipArt:
		*DefaultIndex = _R(IDS_LIBRARIES_CLIPART_FILENAME);	// "XaraClip.txt";
		*IndexDesc = _R(IDS_LIBRARIES_CLIPART_DESC);		// "Clipart";
		break;
	
	case SGLib_Bitmap:
		*DefaultIndex = _R(IDS_LIBRARIES_BITMAPS_FILENAME); // "XaraBitm.txt";
		*IndexDesc = _R(IDS_LIBRARIES_BITMAPS_DESC);		// "Bitmap clipart";
		break;
		
	case SGLib_Font:
		*DefaultIndex = _R(IDS_LIBRARIES_FONTS_FILENAME); 	// "XaraFont.txt";
		*IndexDesc = _R(IDS_LIBRARIES_FONTS_DESC);			// "Font";
		break;

	case SGLib_Texture:
		*DefaultIndex = _R(IDS_LIBRARIES_FILLS_FILENAME); 	// "XaraText.txt";
		*IndexDesc = _R(IDS_LIBRARIES_FILLS_DESC);			// "Fills";
		break;
	
	case SGLib_Fractal:
		*DefaultIndex = _R(IDS_LIBRARIES_FRACTALS_FILENAME);// "XaraFrac.txt";
		*IndexDesc = _R(IDS_LIBRARIES_FRACTALS_DESC);		// "Fractal";
		break;

	case SGLib_Colour:
		*DefaultIndex = _R(IDS_LIBRARIES_COLOURS_FILENAME);	// "XaraColo.txt";
		*IndexDesc = _R(IDS_LIBRARIES_COLOURS_DESC);		// "Colour";
		break;*/

	return FALSE;
}



/********************************************************************************************

>	virtual BOOL LibraryGallery::GetLibraryDirectoryName(String_256 *LibDirName)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95

	Outputs:	LibDirName	- Returns the Default directory (on the clipart CD) for the gallery
	Returns:	TRUE if overridden and directory obtained...

	Purpose:	Get the default CD directory name for the gallery

********************************************************************************************/

BOOL LibraryGallery::GetLibraryDirectoryName(String_256 *LibDirName)
{
	/*case SGLib_Font:
		CDPath += String_16(_R(IDS_LIBRARIES_FONTS_DIRNAME));
		break;
	case SGLib_ClipArt:
	case SGLib_Blank:
		CDPath += String_16(_R(IDS_LIBRARIES_CLIPART_DIRNAME));
		break;
	case SGLib_Bitmap:
		CDPath += String_16(_R(IDS_LIBRARIES_BITMAPS_DIRNAME));
		break;
	case SGLib_Texture:
		CDPath += String_16(_R(IDS_LIBRARIES_FILLS_DIRNAME));
		break;
	case SGLib_Fractal:
		CDPath += String_16(_R(IDS_LIBRARIES_FRACTALS_DIRNAME));
		break;
	case SGLib_Colour:
		CDPath += String_16(_R(IDS_LIBRARIES_COLOURS_DIRNAME));
		break;*/

	return FALSE;
}


/********************************************************************************************

>	virtual BOOL LibraryGallery::CheckForIndexMatch(StringBase *Txt)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95

	Inputs:		Kind	- Last column entry in the index.txt file (" F")
	Returns:	TRUE if this signifies the gallery in question...

	Purpose:	To see whether we should add this line of the index.txt file to this gallery

********************************************************************************************/

BOOL LibraryGallery::CheckForIndexMatch(StringBase *Txt)
{
/*	if(Type == SGLib_ClipArt && ((Kind.Sub(String_8("A"))!=-1) || (Kind.Sub(String_8("a"))!=-1)) ) Match = TRUE;
	if(Type == SGLib_ClipArt && ((Kind.Sub(String_8("B"))!=-1) || (Kind.Sub(String_8("b"))!=-1)) ) Match = TRUE;				
	//if(Type == SGLib_Bitmap  && ((Kind.Sub(String_8("B"))!=-1) || (Kind.Sub(String_8("b"))!=-1)) ) Match = TRUE;
	if(Type == SGLib_Font    && ((Kind.Sub(String_8("F"))!=-1) || (Kind.Sub(String_8("f"))!=-1)) ) Match = TRUE;
	if(Type == SGLib_Texture && ((Kind.Sub(String_8("T"))!=-1) || (Kind.Sub(String_8("t"))!=-1)) ) Match = TRUE;
	if(Type == SGLib_Fractal && ((Kind.Sub(String_8("R"))!=-1) || (Kind.Sub(String_8("r"))!=-1)) ) Match = TRUE;
	if(Type == SGLib_Colour  && ((Kind.Sub(String_8("C"))!=-1) || (Kind.Sub(String_8("c"))!=-1)) ) Match = TRUE;*/

	return FALSE;
}


/********************************************************************************************

>	virtual BOOL LibraryGallery::GetQuietStatus(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Returns:	TRUE if Quiet has been pressed (SetQuiet status called with TRUE)
	Purpose:	Get the Quiet status of the gallery

********************************************************************************************/

BOOL LibraryGallery::GetQuietStatus(void)
{
	return FALSE;
}

/********************************************************************************************

>	virtual void LibraryGallery::SetQuietStatus(BOOL Status)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Inputs:		Status - Set to TRUE and call if Quiet has just been pressed on an 'adding
						 indexes' dialog
	Purpose:	Set the Quiet status of the gallery

********************************************************************************************/
		
void LibraryGallery::SetQuietStatus(BOOL Status)
{
}

/***********************************************************************************************

>	virtual BOOL LibraryGallery::ScanForLocation(SGLibType Type, StringBase *Result = NULL);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95

	Inputs:		Type of library to scan for
	Outputs:	(We assume the library static strings exist and use them - bit yucky)
				If a Result pointer if given then we copy this into there as well..
	Returns:	FALSE if it fails; TRUE if we got a path

	Purpose:	Searches all the drives for a CDROM drive. If it finds the Camelot CD
				mount here at Xara HQ, we point to that instead.
	Notes:

***********************************************************************************************/

BOOL LibraryGallery::ScanForLocation(SGLibType Type, StringBase *Result)
{
	return FALSE;
}

/********************************************************************************************

>	virtual void LibraryGallery::SortGallery(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95

	Purpose:	Sort the entire gallery alphabetically

********************************************************************************************/

void LibraryGallery::SortGallery(void)
{
	// auto sorting disabled
	return;
	
	// Sort alphabetically (and keep old sort keys)...
	SGSortKey TmpSortKeys[MaxSGSortKeys];
	for (INT32 i = 0; i < MaxSGSortKeys; i++)
	{
		TmpSortKeys[i] = SortKeys[i];
		SortKeys[i].SortKey  = 0;
		SortKeys[i].Reversed = FALSE;
	}
	SortKeys[0].SortKey = 1;

	ApplySortNow(TRUE);

	for (i = 0; i < MaxSGSortKeys; i++)
		SortKeys[i] = TmpSortKeys[i];

}

/********************************************************************************************

>	INT32 LibraryGallery::GetDisplayMode(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95

	Purpose:	Get the current gallery display mode as passing in miscinfo structures...

********************************************************************************************/

INT32 LibraryGallery::GetDisplayMode(void)
{
	// Current display mode identifier
	return DisplayMode;
}

/********************************************************************************************

>	void LibraryGallery::RemoveSelectedLibraries(BOOL Warn)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95
	Inputs:		Warn	- Warn user before removing groups (doesn't warn if no groups)

	Purpose:	Removes all the selected groups in the library

********************************************************************************************/

void LibraryGallery::RemoveSelectedLibraries(BOOL Warn)
{
	BOOL FullRedrawNecessary = FALSE;

	ERROR3IF(DisplayTree == NULL, "LibraryGallery::RemoveSelectedLibraries called with null display tree");

	INT32 GroupCount = GetSelectedGroupCount();

	//ERROR3_PF(("Selected Group Count %d", GroupCount));

	if(GroupCount > 0)
	{
		INT32 ButtonPressed = 1;
		
		if(Warn)
			ButtonPressed = InformMessage(_R(IDS_LIBRARY_CHECK_REMOVE), _R(IDS_REMOVE), _R(IDS_CANCEL));

		if(ButtonPressed == 1)
		{
			SGDisplayGroup *Group = DisplayTree->FindNextSelectedGroup(NULL);
			SGDisplayGroup *NextGroup = NULL;
			BOOL DoingSlowJob = FALSE;

			INT32 ActualCount = 0;
			if(GroupCount > 0)
			{
				String_64 SlowJob(_R(IDS_LIBRARY_REMOVING_FOLDERS));
				BeginSlowJob(GroupCount, FALSE, &SlowJob);
				DoingSlowJob = TRUE;
			}

			while(Group != NULL)
			{
				NextGroup = DisplayTree->FindNextSelectedGroup(Group);

				INT32 LibFileCount = OpenLibFiles.GetCount();
				BOOL DeletedGroup = FALSE;

				if(LibFileCount > 0)
				{
					// Count so we can tell where we are in the libfile list
					INT32 LibFilesDeleted = 0;
					INT32 OriginalLibFileCount = LibFileCount;

					// Loop through library files
					for(INT32 LibFileLoop = 0; LibFileLoop < OriginalLibFileCount; LibFileLoop ++)
					{
						LibraryFile *LibFile = (LibraryFile *)OpenLibFiles.FindItem(LibFileLoop - LibFilesDeleted);

						INT32 LibCount = LibFile->Libraries.GetCount();

						if(LibCount > 0)
						{
							// Count so we can tell where we are in the libfile list
							INT32 LibsDeleted = 0;
							INT32 OriginalLibCount = LibCount;

							// Loop through library groups
							for(INT32 LibLoop = 0; LibLoop < OriginalLibCount; LibLoop ++)
							{
								Library *Lib = (Library *)LibFile->Libraries.FindItem(LibLoop - LibsDeleted);

								if(Lib->ParentGroup == Group)
								{
									if(Lib->ParentGroup->GetNext() == NULL)
										FullRedrawNecessary = TRUE;

									Lib->ParentGroup->DestroySubtree(TRUE);
									// >>webster
									if (Lib->IsWebLibrary())
									{
										OpAddWebFolders::Success[Lib->GetType()] = FALSE;
										// Remove the files from the hard disk
										DownloadCache::RemoveFile(Lib->ReturnIndexLocation()->GetPath());
									}
									// <<webster
									// Found our group
									delete(LibFile->Libraries.RemoveItem(Lib));
									LibsDeleted ++;

									LibCount = LibFile->Libraries.GetCount();
									if(LibCount == 0)
									{
										delete(OpenLibFiles.RemoveItem(OpenLibFiles.FindItem(LibFileLoop)));
										LibFilesDeleted ++;
									}

									DeletedGroup = TRUE;
								}

								if(DeletedGroup)
									break;
							}
						}
						if(DeletedGroup)
							break;
					}
				}

				// Get the next selected group from the gallery
				Group = NextGroup;

				// Update the progress bar for each group
				if(DoingSlowJob)
					ContinueSlowJob(++ActualCount);
			}

			if(DoingSlowJob)
				EndSlowJob();

			// Update the button shading status - 'Remove' should now be greyed out
			SelectionHasChanged();

			// Update the gallery
			InvalidateCachedFormat();
			if(FullRedrawNecessary)
				ForceRedrawOfList();
			else
				ReformatAndRedrawIfNecessary();
		}
	}
}

/********************************************************************************************

>	INT32 LibraryGallery::RemoveSimilarLibraryGroups(PathName *IndexFile, BOOL RemoveFirstOne = TRUE, BOOL Redraw = TRUE
				, Library *DontKill = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/95
	Inputs:		IndexFile		- Path of index for group with which similar ones should be removed
				RemoveFirstOne 	- Set to TRUE if you want ALL the groups with the same index removed
								- Set to FALSE and all but the first will be removed...
				Redraw			- Set to TRUE if you want the new gallery redrawn...
				DontKill		- Pointer to a Library not to kill

	Returns:	Number of groups removed

	Purpose:	Removes all the groups with the same Index path...

********************************************************************************************/

INT32 LibraryGallery::RemoveSimilarLibraryGroups(PathName *FilesPath, String_256 *IndexFileName, BOOL RemoveFirstOne,
						 BOOL Redraw, Library *DontKill)
{
	ERROR3IF(FilesPath == NULL || IndexFileName == NULL, "LibraryGallery::RemoveSimilarLibraryGroups given null params");
	ERROR3IF(DisplayTree == NULL, "LibraryGallery::RemoveSimilarLibraryGroups called with null display tree");

	INT32 RemovedGroups = 0;

	BOOL FullRedrawNecessary = FALSE;
	INT32 GroupCount = LibraryGroupsInGallery();

	LibraryGallery::OffsetOfFirstGroupRemoved = -1;
	INT32 OffsetCount = 0;
	
	if(GroupCount > 0)
	{
		// Sort out the path we're checking against
		String_256 UpperIndex(*IndexFileName);
		UpperIndex.toUpper();
		String_256 UpperFiles(FilesPath->GetPath());
		LibraryFile::TidyUpSubPath(&UpperFiles);
		UpperFiles.toUpper();
	
		SGDisplayGroup *Group = (SGDisplayGroup *)DisplayTree->GetChild();
		SGDisplayGroup *NextGroup = NULL;

		// Loop through groups...
		while(Group != NULL)
		{
			NextGroup = (SGDisplayGroup *)Group->GetNext();

			INT32 LibFileCount = OpenLibFiles.GetCount();
			BOOL DeletedGroup = FALSE;

			if(LibFileCount > 0)
			{
				// Count so we can tell where we are in the libfile list
				INT32 LibFilesDeleted = 0;
				INT32 OriginalLibFileCount = LibFileCount;

				// Loop through library files
				for(INT32 LibFileLoop = 0; LibFileLoop < OriginalLibFileCount; LibFileLoop ++)
				{
					LibraryFile *LibFile = (LibraryFile *)OpenLibFiles.FindItem(LibFileLoop - LibFilesDeleted);
					INT32 LibCount = LibFile->Libraries.GetCount();

					if(LibCount > 0)
					{
						// Count so we can tell where we are in the libfile list
						INT32 LibsDeleted = 0;
						INT32 OriginalLibCount = LibCount;

						// Loop through library groups
						for(INT32 LibLoop = 0; LibLoop < OriginalLibCount; LibLoop ++)
						{
							Library *Lib = (Library *)LibFile->Libraries.FindItem(LibLoop - LibsDeleted);

							if(Lib->ParentGroup == Group && Lib->SubLibPath != NULL && Lib->IndexFile != NULL && Lib != DontKill)
							{
								String_256 LibraryPath(Lib->SubLibPath->GetPath());
								String_256 LibraryIndex(Lib->IndexFile->GetFileName(TRUE));
								LibraryPath.toUpper();
								LibraryIndex.toUpper();
								
								// Found a duplicate group - vape city !
								if(UpperFiles == LibraryPath && UpperIndex == LibraryIndex)
								{
									// Keep the offset count in a static so we can do a proper insert for updated groups
									if(LibraryGallery::OffsetOfFirstGroupRemoved == -1)
									{
										LibraryGallery::OffsetOfFirstGroupRemoved = OffsetCount;
										LibraryGallery::FirstGroupRemovedSelectionState = Group->IsSelected();
									}

									RemovedGroups++;

									if(Lib->ParentGroup->GetNext() == NULL)
										FullRedrawNecessary = TRUE;

									Lib->ParentGroup->DestroySubtree(TRUE);

									delete(LibFile->Libraries.RemoveItem(Lib));
									LibsDeleted ++;

									LibCount = LibFile->Libraries.GetCount();
									if(LibCount == 0)
									{
										delete(OpenLibFiles.RemoveItem(OpenLibFiles.FindItem(LibFileLoop)));
										LibFilesDeleted ++;
									}

									DeletedGroup = TRUE;
								}
							}

							if(DeletedGroup)
								break;
						}
					}
					if(DeletedGroup)
						break;
				}
			}

			// Get the next group from the gallery
			Group = NextGroup;
			OffsetCount++;
		}

		// Update the button shading status - 'Remove' should now be greyed out
		SelectionHasChanged();

		// Update the gallery
		if(Redraw)
		{
			InvalidateCachedFormat();
			if(FullRedrawNecessary)
				ForceRedrawOfList();
			else
				ReformatAndRedrawIfNecessary();
		}
	}

	return RemovedGroups;
}


/********************************************************************************************

>	virtual void LibraryGallery::WorkOutSectionName(String_256 *Section)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/95
	Inputs:
	Outputs:	A section name to use for the library gallery

	Purpose:	Works out which library gallery we are and returns a section name as appropriate

	Notes:		Ideally this function should be overidden by the individual galleries...

********************************************************************************************/

void LibraryGallery::WorkOutSectionName(String_256 *Section)
{
	ERROR3IF(Section == NULL, "LibraryGallery::WorkOutSectionName given null section");

	*Section = _R(IDS_SGLBASE_SECTION_NAME); // TEXT("LibraryGallery");
}

/********************************************************************************************

>	virtual void LibraryGallery::WorkOutDescriptionSectionName(String_256 *Section)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/96
	Inputs:
	Outputs:	A section name to use for the library gallery which denotes group descriptions

	Purpose:	Works out which library gallery we are and returns a description section name
				as appropriate

********************************************************************************************/

void LibraryGallery::WorkOutDescriptionSectionName(String_256 *Section)
{
	ERROR3IF(Section == NULL, "LibraryGallery::WorkOutDescriptionSectionName given null section");

	WorkOutSectionName(Section);
	*Section += String_256(_R(IDS_GRMFILE_DESCRIPTIONS));
}


/********************************************************************************************

>	virtual void LibraryGallery::WorkOutLibraryTypeSectionName(String_256 *Section)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/96
	Inputs:
	Outputs:	A section name to use for the library gallery which denotes library types

	Purpose:	Works out which library gallery we are and returns a library type section name
				as appropriate

********************************************************************************************/

void LibraryGallery::WorkOutLibraryTypeSectionName(String_256 *Section)
{
	ERROR3IF(Section == NULL, "LibraryGallery::WorkOutLibraryTypeSectionName given null section");

	WorkOutSectionName(Section);
	*Section += String_256(_R(IDS_GRMFILE_LIBRARYTYPES));
}


/********************************************************************************************

>	static BOOL LibraryGallery::WipeGallerySettings()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/2/97
	Inputs:		-
	Purpose:	Function to remove all the current gallery settings

********************************************************************************************/

BOOL LibraryGallery::WipeGallerySettings(void)
{

#ifdef SAVE_PREFS_TO_REGISTRY
	// Wipe the gallery settings from the registry
	// need to use this delete function so that works on Windows 95 and NT
	DeleteRegKeyAndSubKeys(hAppStateRegKey, PRODUCT_REGISTRYKEY_GALLERY);
	
	// We must wipe the ini file as well as otherwise when the code checks to 
	// see if the registry settings and then falls back on the ini file if they
	// are not there, then we wont be using the default settings.
#endif

	// Wipe the GRM ini file
	LibSettings::NukeSettings();

	return TRUE;
}

/********************************************************************************************

>	void LibraryGallery::UpdateGRMFile(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/95
	Inputs:		

	Purpose:	Updates the GRM file to reflect the status of the gallery (all the groups)

********************************************************************************************/

void LibraryGallery::UpdateGRMFile(void)
{
	if(DisplayTree == NULL)
		ERROR3("LibraryGallery::UpdateGRMFile called with null display tree");

	String_64 SlowJob(_R(IDS_LIBRARY_UPDATING_GRMF));
	BeginSlowJob(-1, TRUE, &SlowJob);

	INT32 Pass = 0;
	String_256 SectionName;
	
	// We now do 3 passes (webster), the first builds the pathnames section, the second does the descriptions
	// the third lists the index files corresponding to web folders. Note - doing the section first is important so we 
	// can detect if the descriptions are out of date,
	// or rather that the SectionName bit was generated by an old version (which will always add it to
	// the end of the file...)

	//OK, we now do 4 passes - the extra pass adds the Library type for each item - needed for clipart gallery to
	//support multiple download and cache locations...
	while(++Pass <= 3)
	{
		if(Pass == 1)
			WorkOutSectionName(&SectionName);
		else if (Pass == 2)
			WorkOutDescriptionSectionName(&SectionName);
		else if (Pass == 3)
			WorkOutLibraryTypeSectionName(&SectionName);

#ifdef SAVE_PREFS_TO_REGISTRY
		// This is the main section name in the registry for gallery settings.
		String_256 KeySectionName(PRODUCT_REGISTRYKEY_GALLERY TEXT("\\"));
		// add in the required sub-section name that we are going to use
		KeySectionName += SectionName;

		SaveRegistryEntries Saver;
		BOOL StartedOk = Saver.StartSaving(&KeySectionName);
		ERROR3IF(!StartedOk,"LibraryGallery::UpdateGRMFile couldn't start saving the settings");

		if(!OpenLibFiles.IsEmpty() && StartedOk)
		{
			INT32 LibFileCount = OpenLibFiles.GetCount();

			if(LibFileCount > 0)
			{
				// Loop through library files
				for(INT32 LibFileLoop = 0; LibFileLoop < LibFileCount; LibFileLoop ++)
				{
					LibraryFile *LibFile = (LibraryFile *)OpenLibFiles.FindItem(LibFileLoop);
					INT32 LibCount = LibFile->Libraries.GetCount();
					if(LibCount > 0)
					{
						// Loop through library groups
						for(INT32 LibLoop = 0; LibLoop < LibCount; LibLoop ++)
						{
							Library *Lib = (Library *)LibFile->Libraries.FindItem(LibLoop);

							// Add the pathname of the index file to the grm file
							if(Lib != NULL && Lib->IndexFile != NULL)
							{
								String_256 Line;
								if(Pass == 1)
									Line = Lib->IndexFile->GetPath();
								else if (Pass == 2)
									Lib->GetLibraryTitle(&Line);
								else if (Pass == 3)
								{
									INT32 libtype = Lib->Type;
									String_256 libstringtype;
									libstringtype._MakeMsg("#1%d",libtype);
									Line = libstringtype;
								}

								if (Line.Length())
								{
									if (!Saver.SaveNextEntry(&Line))
									{
										// Possibly write access denied on grm file... or disk full...
										EndSlowJob();
										return;
									}
								}
							}
						}
					}
				}
			}
		}
		
		// Close down the saving
		Saver.StopSaving();
#else
		LibSettings::StartOutput(&SectionName);
		LibSettings::AddSection(&SectionName);

		if(!OpenLibFiles.IsEmpty())
		{
			INT32 LibFileCount = OpenLibFiles.GetCount();

			if(LibFileCount > 0)
			{
				// Loop through library files
				for(INT32 LibFileLoop = 0; LibFileLoop < LibFileCount; LibFileLoop ++)
				{
					LibraryFile *LibFile = (LibraryFile *)OpenLibFiles.FindItem(LibFileLoop);
					INT32 LibCount = LibFile->Libraries.GetCount();
					if(LibCount > 0)
					{
						// Loop through library groups
						for(INT32 LibLoop = 0; LibLoop < LibCount; LibLoop ++)
						{
							Library *Lib = (Library *)LibFile->Libraries.FindItem(LibLoop);

							// Add the pathname of the index file to the grm file
							if(Lib != NULL && Lib->IndexFile != NULL)
							{
								String_256 Line;
								if(Pass == 1)
									Line = Lib->IndexFile->GetPath();
								else if (Pass == 2)
									Lib->GetLibraryTitle(&Line);
								else if (Pass == 3)
								{
									INT32 libtype = Lib->Type;
									String_256 libstringtype;
									libstringtype._MakeMsg("#1%d",libtype);
									Line = libstringtype;
								}

								if(!LibSettings::AddLine(&Line))
								{
									// Possibly write access denied on grm file... or disk full...
									EndSlowJob();
									return;
								}
							}
						}
					}
				}
			}
		}
#endif
	}
#ifdef SAVE_PREFS_TO_REGISTRY
	String_256 RootURLKeySectionName(PRODUCT_REGISTRYKEY_GALLERY TEXT("\\"));
	RootURLKeySectionName += String_256(_R(IDS_ROOTURL));
	SaveRegistryEntries Saver;	
	BOOL StartedOk = Saver.StartSaving(&RootURLKeySectionName);
	ERROR3IF(!StartedOk,"LibraryGallery::UpdateGRMFile couldn't start saving the settings");
	Saver.SaveNextEntry(&Library::URLBase);
#else
	String_256 strRootURLSectionName(_R(IDS_ROOTURL));
	LibSettings::StartOutput(&strRootURLSectionName);
	LibSettings::AddSection(&strRootURLSectionName);
	LibSettings::AddLine(&Library::URLBase);
#endif

	EndSlowJob();
}



/********************************************************************************************

>	BOOL LibraryGallery::AddLibraryGroups(SGLibType LibType, String_256 *DefaultLibraryPath);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/95

	Inputs:		LibType - The type of library we're adding
				DefaultLibraryPath	- Library path to use if non found in grm file
	Returns:	TRUE if new groups added

	Purpose:	Creates all teh library groups described in the GRM file, or passed in if
				non were in the GRM...

********************************************************************************************/

BOOL LibraryGallery::AddLibraryGroups(SGLibType LibType, String_256 *DefaultLibraryPath)
{
	TRACEUSER( "Matt", _T("AddLibraryGroups called in Base Class\n"));
//	ERROR3_PF(("LibraryGallery::AddLibraryGroups -> %s", (TCHAR *)*DefaultLibraryPath));

	BOOL AddedGroups = FALSE;

	if(AddGRMGroups(LibType))
	{
		AddedGroups = TRUE;
	}
	else
	{

// Matt - 03/01/2001
// This method is only ever called from derived classes and the check below is made in each one of them before calling...
// Therefore, this change will not affect their operation - it DOES mean that we can now pre-create  a gallery with CD stuff
// from multiple sources... used in the clipart/web themes precreation...

//		if (OpenLibFiles.IsEmpty())		// We have no libraries, so open the default library
//		{
			String_64 SlowJob(_R(IDS_LIBRARY_SCANNING));
			BeginSlowJob(-1, FALSE, &SlowJob);

			// open the library file
			LibraryFile *LibFile = new LibraryFile;
			if (LibFile != NULL)
			{
				// OK. Lets initialise the library. This will call us back via base-class
				// SuperGallery methods to add a group and items for each {sub}library
				// found in the given library file.			
				PathName ThePath(*DefaultLibraryPath);
				if (!LibFile->Init(this, &ThePath, LibType))
				{
					// InformError if it returned FALSE? **** !!!!
					delete LibFile;
				}
				else
				{
					OpenLibFiles.AddTail(LibFile);
					AddedGroups = TRUE;
					//SortGallery();
				}
			}

			EndSlowJob();
//		}
	}

	return AddedGroups;
}

/********************************************************************************************

>	BOOL LibraryGallery::AddGRMGroups(SGLibType LibType);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/95

	Inputs:		LibType - The type of library we're adding
	Returns:	TRUE if new groups added

	Purpose:	Creates all the library groups described in the GRM file

********************************************************************************************/

BOOL LibraryGallery::AddGRMGroups(SGLibType LibType)
{
	TRACEUSER( "Matt", _T("AddGRMGroups Called in base class\n"));
	String_256 SectionName;
	WorkOutSectionName(&SectionName);

	// The two lists we are going to use to keep track of the two section that we require
	// Errr.. now there's three lists... Sorry. (Matt 25/09/2000)
	List *PathList = NULL;
	List *DescList = NULL;
	List *TypeList = NULL;

	BOOL bRetVal = FALSE;

#ifdef SAVE_PREFS_TO_REGISTRY
	// Check to see if we have the gallery settings section in the registry
	// if we do then use that.
	// Otherwise, use check for the older ini file version and if present use that
	
	// This is the main section name in the registry for gallery settings.
	String_256 KeySectionName(PRODUCT_REGISTRYKEY_GALLERY TEXT("\\"));
	// add in the required sub-section name that we are going to use
	KeySectionName += SectionName;

	BOOL SectionPresent = CheckForRegistryEntry(KeySectionName);
	if (SectionPresent)
	{
		// We need to read the data from the registry as the section is present
		
		// First do the pathnames list section
		LoadRegistryEntries Loader;
		BOOL StartedOk = Loader.StartLoading(&KeySectionName);
		ERROR3IF(!StartedOk,"LibraryGallery::UpdateGRMFile couldn't start saving the settings");

		PathList = new List;

		// read all the lines from that section
		BOOL AddedItems = FALSE;
		INT32 NumberSectionItems = 0;
		if (PathList && StartedOk)
		{
			String_256 Entry;
			while (Loader.LoadNextEntry(&Entry))
			{
				LibSettingsListItem *LibSettingsItem = new LibSettingsListItem(&Entry);
				PathList->AddTail((ListItem *)LibSettingsItem);
				AddedItems = TRUE;
			}
			// remember how many items we got
			NumberSectionItems = Loader.GetNumberOfItemsLoaded();
			Loader.StopLoading();
		}

		// See if we've got descriptions section as well
		String_256 DescriptionSectionName;
		WorkOutDescriptionSectionName(&DescriptionSectionName);
		// This is the main section name in the registry for gallery settings.
		String_256 DescriptionKeySectionName(PRODUCT_REGISTRYKEY_GALLERY TEXT("\\"));
		// add in the required sub-section name that we are going to use
		DescriptionKeySectionName += DescriptionSectionName;
		BOOL DescriptionsPresent = FALSE;
		String_256 DescEntry;

		// First do the pathnames list section
		LoadRegistryEntries DescLoader;
		DescriptionsPresent = DescLoader.StartLoading(&DescriptionKeySectionName);
		ERROR3IF(!DescriptionsPresent,"LibraryGallery::UpdateGRMFile couldn't start saving the settings");

		// build up a list of the items in this section
		DescList = new List;
		INT32 NumberDescriptionItems = 0;
		if (DescList && DescriptionsPresent)
		{
			while (DescLoader.LoadNextEntry(&DescEntry))
			{
				LibSettingsListItem *LibSettingsItem = new LibSettingsListItem(&DescEntry);
				DescList->AddTail((ListItem *)LibSettingsItem);
				AddedItems = TRUE;
			}
			// remember how many items we got
			NumberDescriptionItems = DescLoader.GetNumberOfItemsLoaded();
			DescLoader.StopLoading();
		}


		// See if we've got library types section as well
		String_256 LibraryTypesSectionName;
		WorkOutLibraryTypeSectionName(&LibraryTypesSectionName);
		// This is the main section name in the registry for gallery settings.
		String_256 LibraryTypesKeySectionName(PRODUCT_REGISTRYKEY_GALLERY TEXT("\\"));
		// add in the required sub-section name that we are going to use
		LibraryTypesKeySectionName += LibraryTypesSectionName;
		BOOL LibraryTypesPresent = FALSE;
		String_256 TypesEntry;

		// First do the pathnames list section
		LoadRegistryEntries TypesLoader;
		LibraryTypesPresent = TypesLoader.StartLoading(&LibraryTypesKeySectionName);
		ERROR3IF(!LibraryTypesPresent,"LibraryGallery::UpdateGRMFile couldn't start saving the LibraryTypes");

		// build up a list of the items in this section
		TypeList = new List;
		INT32 NumberLibraryTypesItems = 0;
		if (TypeList && LibraryTypesPresent)
		{
			while (TypesLoader.LoadNextEntry(&TypesEntry))
			{
				LibSettingsListItem *LibSettingsItem = new LibSettingsListItem(&TypesEntry);
				TypeList->AddTail((ListItem *)LibSettingsItem);
				AddedItems = TRUE;
			}
			// remember how many items we got
			NumberLibraryTypesItems = TypesLoader.GetNumberOfItemsLoaded();
			TypesLoader.StopLoading();
		}
		
		
// >>webster		
		// This is the main section name in the registry for gallery settings.
		String_256 RootURLKeySectionName(PRODUCT_REGISTRYKEY_GALLERY TEXT("\\"));
		// add in the required sub-section name that we are going to use
		RootURLKeySectionName += String_256(_R(IDS_ROOTURL));
		LoadRegistryEntries URLLoader;
		if (URLLoader.StartLoading(&RootURLKeySectionName))	
			URLLoader.LoadNextEntry(&Library::URLBase);
		URLLoader.StopLoading();
	// <<webster

		// If we have a matching set of pathname and description sections then add those items
		// Otherwise, just leave the gallery blank
		if (NumberDescriptionItems == NumberSectionItems && 
			NumberSectionItems != 0 && NumberDescriptionItems != 0 &&
			PathList != NULL && DescList != NULL)
		{
			// Add the sections to the gallery
			bRetVal = GoAndAddGRMGroups(/*LibType*/TypeList , PathList, DescList, DescriptionsPresent, &SectionName, &DescriptionSectionName);
		}
	}
	else
#endif
	{
		// read the data from the ini file
		String_256 Entry;

		// Right, only return false if the section wasn't found in the file
		// The contents of the section could be null, this would mean there are no libraries
		// shown, but if that's what they want then fair enough
		FilePos SectionNamePos = 0;
		if(!LibSettings::GetEntry(&SectionName, 0, &Entry, &SectionNamePos))
			return FALSE;

		// See if we've got descriptions as well
		String_256 DescriptionSectionName(SectionName);
		WorkOutDescriptionSectionName(&DescriptionSectionName);
		BOOL DescriptionsPresent = FALSE;
		String_256 DescEntry;
		FilePos DescriptionPos = 0;
		if(LibSettings::GetEntry(&DescriptionSectionName, 0, &DescEntry, &DescriptionPos))
			DescriptionsPresent = TRUE;
		
		// If the [SectionName] appears later than the [DescriptionSectionName] then the
		// [Description] one is invalid...
		// Perhaps in the future we should change the SectionNames so that old versions of Camelot
		// won't affect new things like this...
		if(DescriptionPos < SectionNamePos)
			DescriptionsPresent = FALSE;

		// See if we've got library types as well
		String_256 LibraryTypesSectionName(SectionName);
		WorkOutLibraryTypeSectionName(&LibraryTypesSectionName);
		BOOL LibraryTypesPresent = FALSE;
		String_256 TypeEntry;
		FilePos LibraryTypesPos = 0;
		if(LibSettings::GetEntry(&LibraryTypesSectionName, 0, &TypeEntry, &LibraryTypesPos))
			LibraryTypesPresent = TRUE;


		PathList = new List;
		if(PathList != NULL && !LibSettings::BuildList(&SectionName, PathList))
		{
			PathList->DeleteAll();
			delete PathList;
			PathList = NULL;
		}

		DescList = NULL;
		if(DescriptionsPresent)
		{
			DescList = new List;
			if(DescList != NULL && !LibSettings::BuildList(&DescriptionSectionName, DescList))
			{
				DescList->DeleteAll();
				delete DescList;
				DescList = NULL;
			}
		}

		TypeList = new List;
		if(LibraryTypesPresent)
		{
			TypeList = new List;
			if(TypeList != NULL && !LibSettings::BuildList(&LibraryTypesSectionName, TypeList))
			{
				TypeList->DeleteAll();
				delete TypeList;
				TypeList = NULL;
			}
		}



		//>> webster (Adrian 3/01/97)
		String_256 strURLBaseSectionName(_R(IDS_ROOTURL));
		String_256 strRootURL;
		if (LibSettings::GetEntry(&strURLBaseSectionName, 1, &strRootURL, NULL))	
			Library::URLBase = strRootURL;
		//<< webster

		bRetVal = ActuallyAddGRMGroups(TypeList /*LibType*/, PathList, DescList, DescriptionsPresent, &SectionName, &DescriptionSectionName);

	}

	// Free up memory entailed in lists
	if(PathList != NULL)
	{
		PathList->DeleteAll();
		delete PathList;
	}

	if(DescList != NULL)
	{
		DescList->DeleteAll();
		delete DescList;
	}

	if(TypeList != NULL)
	{
		TypeList->DeleteAll();
		delete TypeList;
	}

	return bRetVal;
}

/********************************************************************************************

>	BOOL LibraryGallery::GoAndAddGRMGroups(SGLibType LibType, List *PathList, List *DescList, BOOL DescPresent,
										   String_256 *SectionName, String_256 *DescriptionSectionName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/2/97

	Inputs:		LibType		- The type of library we're adding
				PathList	- List of Pathnames for the groups (optional, can be NULL, but slower...)
				DescList	- List of Descriptions to display for the groups (optional, can be NULL, but slower...)
				DescPresent	- Are descriptions present ?
				SectionName	- Section name for the paths
				DescriptionSectionName - Section name for the descriptions

	Returns:	TRUE if new groups added

	Purpose:	Given two lists, create all the groups in the gallery from them.
				Same as ActuallyAddGRMGroups but if the lists are null, then it doesn't
				go through the grmfile itself, slowly... as this is no longer relevent.

	SeeAlso:	ActuallyAddGRMGroups;			
				

********************************************************************************************/

BOOL LibraryGallery::GoAndAddGRMGroups(/*SGLibType LibType*/ List *TypeList, List *PathList, List *DescList, BOOL DescPresent,
									   String_256 *SectionName, String_256 *DescriptionSectionName)
{
	ERROR2IF(PathList == NULL || DescList == NULL,FALSE,"GoAndAddGRMGroups bad lists!");
	ERROR2IF(SectionName == NULL || DescriptionSectionName == NULL,FALSE,"GoAndAddGRMGroups bad section names!");

	ListItem *TypeItem = NULL;
	ListItem *PathItem = NULL;
	ListItem *DescItem = NULL;
	String_256 Entry;
	INT32 LineCount = 0;
	INT32 Line = 1;
	BOOL ValidLine = TRUE;
	BOOL GroupsAdded = FALSE;

	LineCount = PathList->GetCount() + 1;

	// Work out the location of web folders so we can flag them as such
	String_256 strWebFoldersPath;
	GetAppCachePath(&strWebFoldersPath);

	if (LineCount > 0)
	{
		String_64 SlowJob(_R(IDS_LIBRARY_SCANNING));
		BeginSlowJob(LineCount, FALSE, &SlowJob);

		// Need to reset the Quiet status before a stream of Library::Init calls
		SetQuietStatus(FALSE);

		Line = 1;
		ValidLine = TRUE;

		BOOL EscapePressed = FALSE;
	
		while (ValidLine && !EscapePressed)
		{
			EscapePressed = !ContinueSlowJob(Line);

			if(PathItem == NULL)
				PathItem = PathList->GetHead();
			else
				PathItem = PathList->GetNext(PathItem);
			ValidLine = (PathItem != NULL && ((LibSettingsListItem *)PathItem)->Line != NULL);
			if (ValidLine)
				Entry = *((LibSettingsListItem *)PathItem)->Line;
			Line ++;

			if (ValidLine)
			{
				// See if we've got descriptions as well
				String_256 *pDescString = NULL;
				String_256 DescString;
				if (DescPresent)
				{
					if(DescItem == NULL)
						DescItem = DescList->GetHead();
					else
						DescItem = DescList->GetNext(DescItem);

					if(DescItem != NULL && ((LibSettingsListItem *)DescItem)->Line != NULL)
					{
						DescString = *((LibSettingsListItem *)DescItem)->Line;
						pDescString = &DescString;
					}
				}

				Line ++;


				// See if we've got library types as well
				SGLibType m_SGLibType;
				String_256 TypeString;
				INT32 TypeInt;
				if (DescPresent)
				{
					if(TypeItem == NULL)
						TypeItem = TypeList->GetHead();
					else
						TypeItem = TypeList->GetNext(TypeItem);

					if(TypeItem != NULL && ((LibSettingsListItem *)TypeItem)->Line != NULL)
					{
						INT32 pos = 0;
						TypeString = *((LibSettingsListItem *)TypeItem)->Line;
						TypeInt = TypeString.ConvertToInteger(pos);
						m_SGLibType = (SGLibType)TypeInt;
					}
				}
				//>> webster (Adrian 3/01/96)
				// Check if the catalog file still exists and is accessible - the web clipart folder
				// might have been deleted in the meantime - in which case we'll skip it
				BOOL bIsWebFolder = !_tcsncicmp(Entry, strWebFoldersPath, _tcsclen(strWebFoldersPath));
				// BUT if it is a CD based folder and the CD ROM is not present then this test will fail.
				// This is very very very bad. We need to only check this if we are in the web case.
				// Otherwise, if the user opens the gallery without the CD ROM present, they loose all their
				// CD gallery entries. Older versions always show the glalery entries and it is only when
				// the user opens a section that it errors and tells them about the problem, giving them
				// a chance to fix it.
				// Neville 12/11/97
				BOOL bIsIndexPresent = (_access((TCHAR*) Entry, 0) != -1);
				if (!bIsWebFolder || (bIsWebFolder && bIsIndexPresent))
				{
					// open the library file
					LibraryFile *LibFile = new LibraryFile;
					if (LibFile != NULL)
					{
						PathName path(Entry);
						if(!LibFile->AddSpecificIndex(this, &path, m_SGLibType, pDescString, 0, bIsWebFolder))
							delete LibFile;											
						else
						{
							OpenLibFiles.AddTail(LibFile);
							GroupsAdded = TRUE;
						}
					}
				}
			}	
		}
		EndSlowJob();
	}

	return GroupsAdded;
}

/********************************************************************************************

>	BOOL LibraryGallery::ActuallyAddGRMGroups(SGLibType LibType, List *PathList, List *DescList, BOOL DescPresent,
	String_256 *SectionName, String_256 *DescriptionSectionName)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/96

	Inputs:		LibType		- The type of library we're adding
				PathList	- List of Pathnames for the groups (optional, can be NULL, but slower...)
				DescList	- List of Descriptions to display for the groups (optional, can be NULL, but slower...)
				DescPresent	- Are descriptions present ?
				SectionName	- Section name for the paths
				DescriptionSectionName - Section name for the descriptions

	Returns:	TRUE if new groups added

	Purpose:	Given two lists, create all the groups in the gallery from them. If the lists are null, then
				we go through the grmfile itself, slowly...

********************************************************************************************/

BOOL LibraryGallery::ActuallyAddGRMGroups(/*SGLibType LibType*/ List *TypeList, List *PathList, List *DescList, BOOL DescPresent,
	String_256 *SectionName, String_256 *DescriptionSectionName)
{
	ListItem *TypeItem = NULL;
	ListItem *PathItem = NULL;
	ListItem *DescItem = NULL;
	String_256 Entry;
	INT32 LineCount = 0;
	INT32 Line = 1;
	BOOL ValidLine = TRUE;
	BOOL GroupsAdded = FALSE;

	// Work out the location of web folders so we can flag them as such
	String_256 strWebFoldersPath;
	GetAppCachePath(&strWebFoldersPath);

	if(PathList != NULL)
		LineCount = PathList->GetCount() + 1;
	else
		while(ValidLine)
		{
			ValidLine = LibSettings::GetEntry(SectionName, Line++, &Entry);
			LineCount ++;
		};

	if(LineCount > 0)
	{
		String_64 SlowJob(_R(IDS_LIBRARY_SCANNING));
		BeginSlowJob(LineCount, FALSE, &SlowJob);

		// Need to reset the Quiet status before a stream of Library::Init calls
		SetQuietStatus(FALSE);

		Line = 1;
		ValidLine = TRUE;

		BOOL EscapePressed = FALSE;
	
		while(ValidLine && !EscapePressed)
		{
			EscapePressed = !ContinueSlowJob(Line);

			if(PathList != NULL)
			{
				if(PathItem == NULL)
					PathItem = PathList->GetHead();
				else
					PathItem = PathList->GetNext(PathItem);
				ValidLine = (PathItem != NULL && ((LibSettingsListItem *)PathItem)->Line != NULL);
				if(ValidLine)
					Entry = *((LibSettingsListItem *)PathItem)->Line;
				Line ++;
			}
			else
				ValidLine = LibSettings::GetEntry(SectionName, Line++, &Entry);

			if(ValidLine)
			{
				// See if we've got descriptions as well
				String_256 *pDescString = NULL;
				String_256 DescString;
				if(DescPresent)
				{
					if(DescList != NULL)
					{
						if(DescItem == NULL)
							DescItem = DescList->GetHead();
						else
							DescItem = DescList->GetNext(DescItem);

						if(DescItem != NULL && ((LibSettingsListItem *)DescItem)->Line != NULL)
						{
							DescString = *((LibSettingsListItem *)DescItem)->Line;
							pDescString = &DescString;
						}
						Line ++;
					}
					else
					{
						if(LibSettings::GetEntry(DescriptionSectionName, (Line-1), &DescString))
							pDescString = &DescString;
					}
				}

				// See if we've got library types as well
				SGLibType m_SGLibType;
				String_256 TypeString;
				INT32 TypeInt;
				if(DescPresent)
				{
					if(TypeList != NULL)
					{
						if(TypeItem == NULL)
							TypeItem = TypeList->GetHead();
						else
							TypeItem = TypeList->GetNext(DescItem);

						if(TypeItem != NULL && ((LibSettingsListItem *)TypeItem)->Line != NULL)
						{
							INT32 pos = 0;
							TypeString = *((LibSettingsListItem *)TypeItem)->Line;
							TypeInt = TypeString.ConvertToInteger(pos);
							m_SGLibType = (SGLibType)TypeInt;
						}
					}
					else
					{
						String_256 LibraryTypesSectionName;
						WorkOutLibraryTypeSectionName(&LibraryTypesSectionName);
						if(LibSettings::GetEntry(&LibraryTypesSectionName, (Line-1), &TypeString))
						{
							INT32 pos = 0;
							TypeInt = TypeString.ConvertToInteger(pos);
							m_SGLibType = (SGLibType)TypeInt;
						}
					}
				}


				//>> webster (Adrian 3/01/96)
				// Check if the catalog file still exists and is accessible - the web clipart folder
				// might have been deleted in the meantime - in which case we'll skip it
				BOOL bIsWebFolder = (camStrstr((TCHAR*) Entry, (TCHAR*) strWebFoldersPath) != NULL);
				// BUT if it is a CD based folder and the CD ROM is not present then this test will fail.
				// This is very very very bad. We need to only check this if we are in the web case.
				// Otherwise, if the user opens the gallery without the CD ROM present, they loose all their
				// CD gallery entries. Older versions always show the glalery entries and it is only when
				// the user opens a section that it errors and tells them about the problem, giving them
				// a chance to fix it.
				// Neville 12/11/97
				BOOL bIsIndexPresent = (_access((TCHAR*) Entry, 0) != -1);
				if (!bIsWebFolder || (bIsWebFolder && bIsIndexPresent))
				{
					// open the library file
					LibraryFile *LibFile = new LibraryFile;
					if (LibFile != NULL)
					{
						// 'Entry' now points to a specific index file. We have to add this to the
						// gallery...
						PathName path(Entry);
						if(!LibFile->AddSpecificIndex(this, &path, m_SGLibType, pDescString, 0, bIsWebFolder))
							delete LibFile;											
						else
						{
							OpenLibFiles.AddTail(LibFile);
							GroupsAdded = TRUE;
						}
					}
				}
			}	
		}
		EndSlowJob();
	}

	return GroupsAdded;
}

/********************************************************************************************

>	INT32 LibraryGallery::LibraryGroupsInGallery(void);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/95

	Inputs:		
	Returns:	Number of library groups in the gallery

	Purpose:	Count the number of library groups in the gallery

********************************************************************************************/

INT32 LibraryGallery::LibraryGroupsInGallery(void)
{
	return (OpenLibFiles.GetCount());
}

/********************************************************************************************

>	void LibraryGallery::GalleryAboutToClose(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/95

	Purpose:	The gallery is about to close, so free any excess memory taken up by the indexes
				and things...

********************************************************************************************/

void LibraryGallery::GalleryAboutToClose(void)
{
	TRACEUSER( "Richard", _T("Gallery about to close\n"));

	// Update the GRM file
	UpdateGRMFile();

	String_64 ProgMsg(_R(IDS_GALLERY_FREEING_MEMORY_ON_CLOSE));
	BeginSlowJob(-1, TRUE, &ProgMsg);

	if(FALSE)
	{
		// This will delete ALL the memory used by galleries when they close...
		OpenLibFiles.DeleteAll();
	}
	else
	{
		if(!OpenLibFiles.IsEmpty())
		{
			INT32 LibFileCount = OpenLibFiles.GetCount();

			if(LibFileCount > 0)
			{
				// Loop through library files
				for(INT32 LibFileLoop = 0; LibFileLoop < LibFileCount; LibFileLoop ++)
				{
					LibraryFile *LibFile = (LibraryFile *)OpenLibFiles.FindItem(LibFileLoop);
					INT32 LibCount = LibFile->Libraries.GetCount();
					if(LibCount > 0)
					{
						// Loop through library groups
						for(INT32 LibLoop = 0; LibLoop < LibCount; LibLoop ++)
						{
							Library *Lib = (Library *)LibFile->Libraries.FindItem(LibLoop);
							if(Lib != NULL)
								Lib->FreeCachedIndexes();
						}
					}
				}
			}
		}
	}

	EndSlowJob();

	VirtualiseAllFoldedGroups();
}

/********************************************************************************************

>	virtual void LibraryGallery::VirtualiseAllFoldedGroups(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/1/96

	Purpose:	Go through the gallery tree and virtualise all folded groups which can be
				virtualised.

	Notes:		There is now a time-out facility if we find we are taking too long...

********************************************************************************************/

void LibraryGallery::VirtualiseAllFoldedGroups(void)
{
	const INT32 TimeLimit = 1 * 1000; // 1 second time out...
	
	SGDisplayGroup *Group = (SGDisplayGroup *)GetDisplayTree()->GetChild();	
	if(Group != NULL)
	{
		INT32 Groups = 0;
		while(Group != NULL)
		{
			Groups++;
			Group = (SGDisplayGroup *)Group->GetNext();
		}

		String_64 ProgMsg(_R(IDS_GALLERY_FREEING_MEMORY_ON_CLOSE));
		BeginSlowJob(Groups, TRUE, &ProgMsg);

		INT32 Count = 0;
		Group = (SGDisplayGroup *)GetDisplayTree()->GetChild();

		MonotonicTime TimeCheck;
		BOOL DoneAGroup = FALSE;

		while(Group != NULL)
		{
			if(ContinueSlowJob(Count++))
			{
				if(Group->Flags.Folded && !Group->IsVirtualised() && Group->CanVirtualise() && !Group->IsSelected()
					&& !Group->Flags.Modified)
				{
					Group->Virtualise();
					DoneAGroup = TRUE;
				}
				Group = (SGDisplayGroup *)Group->GetNext();
			}
			else
				Group = NULL; // Escape pressed - stop virtualising right now !

			// Do it here so we ALWAYS release at least one group (if there are any)
			if(DoneAGroup && TimeCheck.Elapsed(TimeLimit))
				Group = NULL; // Time limit exceeded - stop virtualising right now !
		}

		EndSlowJob();
	}
}


/********************************************************************************************

>	void LibraryGallery::GalleryAboutToReOpen(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/95

	Purpose:	The gallery is about to reopen, so recache any indexes or other information
				that we might need...

	Notes: 		This is called every time the gallery opens, so the static at the start stops
				the code from being used the first time... (Since the initialisation routines
				are supposed to handle everything correctly)

********************************************************************************************/

void LibraryGallery::GalleryAboutToReOpen(void)
{
	TRACEUSER( "Richard", _T("Gallery about to open\n"));

	if(!OpenLibFiles.IsEmpty())
	{
		INT32 LibFileCount = OpenLibFiles.GetCount();

		if(LibFileCount > 0)
		{
			// Dummy run to count the groups for the progress bar
			INT32 GroupCount = 0;
			INT32 LibFileLoop = 0;
			for(LibFileLoop = 0; LibFileLoop < LibFileCount; LibFileLoop ++)
			{
				LibraryFile *LibFile = (LibraryFile *)OpenLibFiles.FindItem(LibFileLoop);
				if(LibFile != NULL)
					GroupCount += LibFile->Libraries.GetCount();
			}

			if(GroupCount > 0)
			{
				String_64 SlowJob(_R(IDS_LIBRARY_CACHING_INDEXES));
				BeginSlowJob(GroupCount, FALSE, &SlowJob);
			}

			// Count so we can tell where we are in the LibFile list
			INT32 LibFilesDeleted = 0;
			INT32 OriginalLibFileCount = LibFileCount;
			INT32 ActualCount = 0;	

			// Used by the error box if the CD's not there...			
			INT32 LastButtonPressed = 2;	

			// Loop through library files
			for(LibFileLoop = 0; LibFileLoop < OriginalLibFileCount; LibFileLoop ++)
			{
				LibraryFile *LibFile = (LibraryFile *)OpenLibFiles.FindItem(LibFileLoop - LibFilesDeleted);
				if(LibFile != NULL)
				{
					INT32 LibCount = LibFile->Libraries.GetCount();
					if(LibCount > 0)
					{
						// Count so we can tell where we are in the lib list
						INT32 LibsDeleted = 0;
						INT32 OriginalLibCount = LibCount;
					
						// Loop through library groups
						for(INT32 LibLoop = 0; LibLoop < OriginalLibCount; LibLoop ++)
						{
							// Found a library, possibly without its index file in memory...
							Library *Lib = (Library *)LibFile->Libraries.FindItem(LibLoop - LibsDeleted);
							if(Lib != NULL)
							{
								if(LastButtonPressed != 3)
									LastButtonPressed = 2;

								BOOL RepeatTheLoop = FALSE;
								BOOL Remove = FALSE;
								BOOL Recache = TRUE;
								do {
									RepeatTheLoop = FALSE;
									
									Recache = Lib->ReCacheIndexes();
									if(!Recache)
									{
										if(LastButtonPressed != 3)
										{
											String_256 WarnMsg;
											String_256 Section;						
											String_64 SmallSection;						
											Lib->GetLibraryTitle(&Section);
											Section.Left(&SmallSection, 36);
											// Be very careful that this message does not exceed 255 bytes...
											WarnMsg.MakeMsg(_R(IDS_LIBRARY_CD_REMOVED), (TCHAR *)SmallSection);

											Error::SetError(0, WarnMsg, 0);
											LastButtonPressed = InformWarning(0, _R(IDS_CONTINUE), _R(IDS_RETRY), _R(IDS_QUIET), NULL, 2, 1);

											// RETRY
											if(LastButtonPressed == 2)
												RepeatTheLoop = TRUE;
										}							
										// Continue or Quiet				
										if(LastButtonPressed == 1 || LastButtonPressed == 3)
											Remove = TRUE;
									}
								} while(RepeatTheLoop);

								if(Remove)
								{
									// If there were problems recaching the index, delete the group from the gallery
									Lib->ParentGroup->DestroySubtree(TRUE);

									// Delete the relevant library
									delete(LibFile->Libraries.RemoveItem(Lib));
									LibsDeleted ++;

									// And delete the libfile structure
									LibCount = LibFile->Libraries.GetCount();
									if(LibCount == 0)
									{
										delete(OpenLibFiles.RemoveItem(LibFile));
										LibFilesDeleted ++;
									}
								}
							}
							// Update the progress bar for each group
							ContinueSlowJob(++ActualCount);		
						}
					}
				}
			}
			EndSlowJob();
		}
	}
}


/********************************************************************************************

>	BOOL LibraryGallery::RemoveWebFolders(SGLibType type)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/01/97

	Inputs:		type - one of 	SGLib_ClipArt,	SGLib_Font,	SGLib_Fractal
	Returns:	TRUE if the folders are removed, FALSE otherwise 

	Purpose:	Reclaims space taken up by downloaded files of a certain type
						on the local machine

********************************************************************************************/

BOOL LibraryGallery::RemoveWebFolders(SGLibType type)
{
	if (AskQuestion(_R(IDS_WARNREMOVE), _R(IDS_EMPTY), _R(IDS_CANCEL)) == _R(IDS_CANCEL))
		return FALSE;

	// If the application is installed on the local hard disk, web folders will be in the cache
	String_256 strLocation;
	GetAppDataPath(&strLocation);
	strLocation += GetStringField(type, _R(IDS_CACHEDIR));

	if (_access((TCHAR*) strLocation, 0) != -1) // found it
	{
		DownloadCache::Traverse(strLocation, TRUE);
		DownloadCache::Refresh();
		SelectionHasChanged();
		return TRUE;
	}
	else 
		return FALSE;
}




/********************************************************************************************
>	void LibraryGallery::SuspendThumbnailDownloading()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/01/97

	Inputs:		none
	Returns:	-

	Purpose:	Prevents further thumbnail downloading and cancels the downloads
					already queued up. Used in response to a user cancel or a serious 
					error.

********************************************************************************************/
void LibraryGallery::SuspendThumbnailDownloading()

{
	bThumbDownloading = FALSE;
	if (!m_lstPendingThumbs.empty()) // we have downloads pending, so we must end them
	{
		OpThumbDownload::Suspend(); // temporarily suspend executing downloads so that the pending download list is frozen
		// Iterate through the download list and end all download operation
		std::list<OpThumbDownload*>::iterator listIterator = m_lstPendingThumbs.begin();
		INT32 nListSize = m_lstPendingThumbs.size();
		for (INT32 i = 0; i < nListSize; i++)
		{
			OpThumbDownload* pDownloadOp = *listIterator++;
			ERROR3IF(!pDownloadOp || !pDownloadOp->IsKindOf(CC_RUNTIME_CLASS(OpThumbDownload)), "Ooops! Invalid pointer - we're about to crash");
			pDownloadOp->End();
		}
		OpThumbDownload::Resume(); // we're done - let the thumbs requested by other galleries continue downloading
		ERROR3IF(!m_lstPendingThumbs.empty(),"Failed to remove all pending downloads"); // just in case
	}
}

	
/********************************************************************************************

>	void LibraryGallery::ResumeThumbnailDownloading()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/01/97

	Inputs:		none
	Returns:	-

	Purpose:	Restarts thumbnail downloading and clears all previous download erorrs

********************************************************************************************/

void LibraryGallery::ResumeThumbnailDownloading()
{
	OpThumbDownload::ClearErrors();
	bThumbDownloading = TRUE;
}



/********************************************************************************************

>	BOOL LibraryGallery::OnGetButtonClicked()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/96

	Inputs:		
	Returns:	TRUE if web folders have been added successfully

	Purpose:	Attempts to download the main web resource index file from our web site,
					parse it and create corresponding gallery folders

********************************************************************************************/

BOOL LibraryGallery::OnGetButtonClicked()

{
	// Connect to the Net if not yet connected
	// This works only to some extent i.e. AttemptConnection() returns TRUE if the dial-up dialog has been created
	// successfully but it can't tell if a TCP/IP connection is eventually established. Anyway, all network operations
	// will eventually time out if nothing seems to be happening.
	if (!InternetManager::AttemptConnection())
	{
		InformError(_R(IDS_NOTCONNECTED), _R(IDS_OK));
		return FALSE;
	}
	// (re)enable thumbnail downloading in case it has been stopped
	ResumeThumbnailDownloading();
	ForceRedrawOfList();
	if (OpAddWebFolders::Success[GetGalleryType()] && !m_bDiscardWebFolders) // nothing else to do
		return TRUE;
	m_bDiscardWebFolders = FALSE;
	String_256 strIndexURL(GetStringField((UINT32) GetGalleryType(), _R(IDS_MAININDEXURL)));
	// We initiate the download of the web clipart index file to a temporary file in $WINDIR\Temporary Internet Files
	String_256 strTempFile;
	GetTempInetDir(&strTempFile);
	strTempFile += GetStringField((UINT32) GetGalleryType(), _R(IDS_TMPFILES));
	OpDescriptor* pOpDesc =	OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpAddWebFolders));
	if (pOpDesc != NULL)
	{
		AddWebFoldersParam* Param = new AddWebFoldersParam;
		ERROR2IF(!Param, FALSE, "Memory allocation error");
		// Set up the op's param
		Param->localFile.SetPathName((TCHAR *) strTempFile); // local path of index file
		Param->strURL = strIndexURL; // URL of index file on our web server
		Param->pLibGal = this; // pointer to the gallery to add folders to
		Param->type = GetGalleryType();
		// Invoke the operation - this will initiate the download of the index file
		// No result is returned, but the Op will report itself any error that might occur
		pOpDesc->Invoke((OpParam*) Param);
	}
	else
	{
		ERROR3("OpAddWebFolders not found");
		return FALSE;
	}
	return TRUE;
}




/********************************************************************************************

>	Library* LibraryGallery::FindLibrary(const String_256& strLibPath)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/96

	Inputs:		strLibPath - full path to the library index file
	Returns:	a pointer to the library if found, NULL otherwise

********************************************************************************************/


Library* LibraryGallery::FindLibrary(const String_256& rLibPath)
{
	String_256 strLibPath(rLibPath);
	LibraryFile* pLibFile = (LibraryFile*) OpenLibFiles.GetHead();
	while (pLibFile)
	{
		Library* pLib = (Library*) pLibFile->Libraries.GetHead();
		while (pLib)
		{
			if (strLibPath.CompareTo(pLib->ReturnIndexLocation()->GetPath(), FALSE))
				pLib = (Library*) pLibFile->Libraries.GetNext(pLib);
			else
				return pLib; // we found it
		}
		pLibFile = (LibraryFile*) OpenLibFiles.GetNext(pLibFile);
	}
	return NULL;
}
