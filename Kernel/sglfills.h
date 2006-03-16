// $Id: sglfills.h 662 2006-03-14 21:31:49Z alex $
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
// SGLFills.h - the Fills library gallery class


#ifndef INC_SGLFILLS
#define INC_SGLFILLS

#include "sgallery.h"
#include "sgtree.h"
#include "sgscan.h"
#include "sglib.h"
#include "dragbmp.h"
#include "sgdrag.h"
#include "scrcamvw.h"

// LibraryGallery
#include "sglbase.h"

enum CALLRESULT {FAILURE = 0, SUCCESS, PENDING}; 

/***********************************************************************************************

>	class LibFillsSGallery : public LibraryGallery

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com> (Based on Clipart gallery code by Richard / Jason)
	Created:	23/3/95
	Purpose:	The LibFills SuperGallery class

	Notes:		SuperGalleries are specced in a number of docs including
					specs\sgallery.doc ("super" gallery extensions)
					specs\propui.doc  (gallery basic ui)
				There is also howtouse\sgallery.doc which describes creating a supergallery

	SeeAlso:	SuperGallery; SGDisplayLibFills

***********************************************************************************************/

class LibFillsSGallery : public LibraryGallery
{
	CC_DECLARE_DYNCREATE(LibFillsSGallery)

public:
	LibFillsSGallery();
	~LibFillsSGallery();

	// TRUE if the SuperGallery is also a LibraryGallery & has no non-library groups
	virtual BOOL IsLibraryGalleryWithNonLibraryGroups(void) {return TRUE;};

public:

	// Initialise the fills gallery
	static BOOL Init(void);

	// Fills gallery message handler
	virtual MsgResult Message(Msg* Message);

	// Can we search keywords ? Yes, I think we can
	virtual BOOL CanSearchKeywords(void);

public:
	// These _MUST_ be overridden by LibraryGalleries

	// Can we create indexes ?
	virtual BOOL CanCreateIndexes(void);

	// Get various default bits 'n' pieces
	virtual BOOL GetDefaults(String_256 *DefaultIndex, String_256 *IndexDesc, SGLibType *Type);

	// Get the default CD directory name for the gallery
	virtual BOOL GetLibraryDirectoryName(String_256 *LibDirName);

	// Check the entry in the index.txt file with our gallery, and return TRUE if it's a match
	virtual BOOL CheckForIndexMatch(StringBase *Txt);

	// Returns the section name to use in the grm file
	virtual void WorkOutSectionName(String_256 *Section);

	// Get the quiet status of the gallery
	virtual BOOL GetQuietStatus(void);

	// Set the quiet status of the gallery
	virtual void SetQuietStatus(BOOL Status);

	// Scan for location of default CD library
	virtual BOOL ScanForLocation(SGLibType Type, StringBase *Result);

	// Return gallery type
	virtual SGLibType GetGalleryType() { return SGLib_Fractal;}

	virtual String_256* GetDefaultLibraryPath() {return &DefaultLibraryPath;}

	void SelectionHasChanged(void);

protected:		// Overridden upcall methods

	void DoShadeGallery(BOOL ShadeIt);

	virtual BOOL PreCreate(void);

	virtual BOOL ApplyAction(SGActionType Action);

	// Override for creating fill library groups rather than display groups
	virtual SGDisplayGroup *AddLibraryGroup(Library *LibraryToDisplay, INT32 NumItems);

	// Adds an item to the given library display group
	// Must be overridden by derived galleries if they wish to handle Libraries
	virtual SGDisplayItem *AddLibraryItem(SGDisplayGroup *LibraryGroup, Library *ParentLib,
											LibraryIndex ItemIndex, BOOL bNew = FALSE);

	void HandleDragStart(DragMessage *DragMsg);

	// Use the currently selected item in the fills gallery to do a fill
	// of type FillType
	bool FillUsingFillGallerySelection(FillTypeEnum FillType);

public:			// Overridden Command interface (for Ops and menu support)
	virtual BOOL InitMenuCommands(void);
		// Called on startup to initialise any desired menu commands. Will make repeated calls
		// to InitMenuCommand(), below.

	virtual BOOL BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID);
		// Builds the command menu for the given menu pop-up (over an item, or options button)

	virtual OpState GetCommandState(StringBase *CommandID, String_256 *ShadeReason);
		// Returns an OpState indicating the state of this command, much like an Op GetState

	virtual void DoCommand(StringBase *CommandID);
		// Attempts to "do" the given command. Should call down to the base class to handle
		// any commands it can't deal with.


protected:
	// Handle clicks on the browse button
	BOOL BrowseClicked(void);

	// Sort the entire gallery alphabetically
	void SortGallery(void);

	SGDisplayItem *CopyDisplayItem(SGDisplayItem *SourceItem, 
								SGDisplayGroup *DestGroup, SGDisplayItem *TargetPosition);

	KernelBitmap* GetSelectedBitmap(void);

public:			// ini file options
	// Where to find the default library
	static String_256 DefaultLibraryPath;

	// For keeping the display modes constant when next loaded
	static INT32 DefaultDisplayMode;

	// For storing sort modes across quits
	static UINT32 DefaultSortKeys;

public:
	// This gallery
	static LibFillsSGallery *ThisGallery;

	// Quiet status of the gallery
	static BOOL QuietStatus;
};
								  

/********************************************************************************************

>	class OpDisplayLibFillsGallery: public Operation

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95 (Based on the Clipart SGallery code)

	Purpose:	Displays the LibFills gallery.
				This op can be attached to a menu, button or keypress
	SeeAlso:	LibFillsSGallery

********************************************************************************************/

#define OPTOKEN_DISPLAYFILLSGALLERY _T("DisplayFillsGallery")

class CCAPI OpDisplayLibFillsGallery: public Operation
{         
	CC_DECLARE_DYNCREATE( OpDisplayLibFillsGallery );

public:
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
    void 			Do(OpDescriptor*);

private:
	static DialogBarOp *FindGallery(void);
			// Finds the LibFills gallery class instance
};  

/********************************************************************************************

>	class SGFillsItem: public SGLibDisplayItem

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95

	Purpose:	A fills gallery item
	SeeAlso:
	
********************************************************************************************/

class SGFillsItem: public SGLibDisplayItem
{         
	CC_DECLARE_DYNCREATE( SGFillsItem );
public:
	SGFillsItem();
	~SGFillsItem();
	SGFillsItem(LibraryIndex LibraryIndexToDisplay, BOOL bNew = FALSE);
	LibDisplayType GetDisplayType(SGMiscInfo *MiscInfo);

protected:
	virtual	BOOL HandleEvent(SGEventType EventType, void *EventInfo,
							 SGMiscInfo *MiscInfo);
};

/********************************************************************************************

>	class GalleryFillsDragInfo : public BitmapDragInformation

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/4/95
	Purpose:	A fill drag, which was started from the fill gallery
	SeeAlso:	BitmapDragInformation
				
********************************************************************************************/

class GalleryFillsDragInfo : public BitmapDragInformation
{ 
	CC_DECLARE_DYNCREATE(GalleryFillsDragInfo)
 
public:
	GalleryFillsDragInfo();
	~GalleryFillsDragInfo();
 	GalleryFillsDragInfo(	SGFillsItem *pSourceItem,
							SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 							BOOL IsAdjust = FALSE, INT32 XSize = 0, INT32 YSize = 0);

	virtual void OnClick(INT32 Flags, POINT Point);

	virtual UINT32 GetCursorID(DragTarget* pDragTarget);
	virtual BOOL GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget);

	virtual BOOL CanDropOnPage() { return TRUE; };
	virtual BOOL OnPageDrop(ViewDragTarget*);

	SGFillsItem*	GetDraggedFill()	{ return(SourceItem); }

protected:
	SGFillsItem*			SourceItem;
	SGMouseInfo				MouseInfo;
	SGMiscInfo				MiscInfo;
};



/*********************************************************************************************

>	class 	SGFillsDragTarget : public SGListDragTarget

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/4/95

	Purpose:	An instantiation of this class is created by each entity which wishes to 
				provide a 'destination' to which the mouse can go to complete a drag.

				This particular target is used for handling drags of BitmapSGalllery items
				within their displayed list. It automatically handles drags with draginfo
				derived from BitmapDragInformation.

	Notes:		Drag targets are destructed automatically when a drag finishes by the drag
				manager with which they were registered.

				To remove a drag target at any time, destruct it - it automatically deregisters
				and cleans up.
				
	SeeAlso:	DragManagerOp::StartDrag; DragInformation; DragTarget;
				SGBitmapDragTarget::ProcessEvent

	Documentation:	Docs\Specs\DragMan.doc; Docs\HowToUse\SGallery.doc

*********************************************************************************************/

class SGFillsDragTarget : public SGListDragTarget
{
friend class DragManagerOp;
	
CC_DECLARE_DYNAMIC(SGFillsDragTarget)

public:	// Public interface
	SGFillsDragTarget(DialogOp *TheDialog, CGadgetID TheGadget = NULL);

protected:
		// Process a drag-related event
	virtual BOOL ProcessEvent(DragEventType Event,
								DragInformation *pDragInfo,
								OilCoord *pMousePos, KeyPress* pKeyPress);
};


#endif

