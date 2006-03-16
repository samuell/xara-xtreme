// $Id: sglcart.h 662 2006-03-14 21:31:49Z alex $
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
// SGLibClipart.h - the LibClipartSGallery (LibClipart SuperGallery) class


#ifndef INC_SGLCART
#define INC_SGLCART

#include "sgallery.h"
#include "sgtree.h"
#include "sgscan.h"
#include "sglib.h"
#include "oilfltrs.h"
#include "dragbmp.h"
#include "sgdrag.h"
#include "sglbase.h"

/***********************************************************************************************

>	class LibClipartSGallery : public LibraryGallery

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (Based on template code by Jason)
	Created:	27/1/95 (Based on the Colour SGallery code)
	Purpose:	The LibClipart SuperGallery class

	Notes:		SuperGalleries are specced in a number of docs including
					specs\sgallery.doc ("super" gallery extensions)
					specs\propui.doc  (gallery basic ui)
				There is also howtouse\sgallery.doc which describes creating a supergallery

	SeeAlso:	SuperGallery; SGDisplayLibClipart; LibraryGallery

***********************************************************************************************/

class LibClipartSGallery : public LibraryGallery
{
	CC_DECLARE_DYNCREATE(LibClipartSGallery)

public:
	LibClipartSGallery();
	~LibClipartSGallery();
	static LibClipartSGallery* Instance();

	// Item properties.
	INT32 GetClipTheme() const;
	INT32 SetClipTheme(INT32 nNewIndex);


	// TRUE if the SuperGallery is also a LibraryGallery & has no non-library groups
	virtual BOOL IsLibraryGalleryWithNonLibraryGroups(void) {return TRUE;};

public:

	// Initialise the clipart gallery
	static BOOL Init(void);

	// Clipart gallery message handler
	virtual MsgResult Message(Msg* Message);

	// Function for profiling redraw (normally contains no code and isn't called)
	void ProfileRedraw(ReDrawInfoType *Param);

	// Can we support keywords ?
	virtual BOOL CanSearchKeywords(void);

	// Override for creating clipart library groups rather than display groups
	virtual SGDisplayGroup *AddLibraryGroup(Library *LibraryToDisplay, INT32 NumItems);

public:
	// Library Galleries must override these

	// Can we create indexes ?
	virtual BOOL CanCreateIndexes(void);

	// Get various default bits 'n' pieces
	virtual BOOL GetDefaults(String_256 *DefaultIndex, String_256 *IndexDesc, SGLibType *Type);

	// Get the default CD directory name for the gallery
	virtual BOOL GetLibraryDirectoryName(String_256 *LibDirName);

	// Check the entry in the index.txt file with our gallery, and return TRUE if it's a match
	virtual BOOL CheckForIndexMatch(StringBase *Txt);

	// Return a Section name for this gallery...
	virtual void WorkOutSectionName(String_256 *Section);

	// Get the quiet status of the gallery
	virtual BOOL GetQuietStatus(void);

	// Set the quiet status of the gallery
	virtual void SetQuietStatus(BOOL Status);

	// Scan for location of default CD library
	virtual BOOL ScanForLocation(SGLibType Type, StringBase *Result);

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

	// Return gallery type
	virtual SGLibType GetGalleryType()
	{
		if (GetClipTheme() == 0)
		{
			return SGLib_ClipArt;
		}
		else
		{
			return SGLib_ClipArt_WebThemes;
		}
	}

	virtual String_256* GetDefaultLibraryPath() {return &DefaultLibraryPath;}

	// Override for button greying
	void SelectionHasChanged(void);

	void ModeHasChanged(void);


	// Import all the currently selected clipart
	static BOOL ImportClipart(BOOL NewDocument, LibraryGallery* pSourceGallery);

	virtual BOOL PreCreate(void);

	virtual BOOL OnGetButtonClicked(void);

	//Added so that they may be suitably overridden for our multiple source downloads
	virtual BOOL RemoveWebFolders(SGLibType type);



protected:
	void DoShadeGallery(BOOL ShadeIt);
	

	virtual BOOL ApplyAction(SGActionType Action);

	// Adds an item to the given library display group
	// Must be overridden by derived galleries if they wish to handle Libraries
	virtual SGDisplayItem *AddLibraryItem(SGDisplayGroup *LibraryGroup, Library *ParentLib,
											LibraryIndex ItemIndex, BOOL bNew = FALSE);


	// Load and save all the currently selected clipart
	BOOL LoadAndSave(void);				 

	void HandleDragStart(DragMessage *DragMsg);

	SGDisplayItem *CopyDisplayItem(SGDisplayItem *SourceItem, 
								SGDisplayGroup *DestGroup, SGDisplayItem *TargetPosition);

protected:

	// Handle clicks on the browse button
	BOOL BrowseClicked(void);

	// Sort the entire gallery alphabetically
	void SortGallery(void);

public: // ini file stuff
	// Where to find the default library
	static String_256 DefaultLibraryPath;
	static String_256 ClipartPath;
	static String_256 WebThemePath;

	// For keeping the last display mode
	static INT32 DefaultDisplayMode;

	// For storing sort modes across quits
	static UINT32 DefaultSortKeys;

	// Optional location for batching system to save its babies
	static String_256 BatchSaveLocation;

	// If the index contains sounds, should we play them ?
	static BOOL DoSounds;


public:	// general statics

	// Pointer to the last created clipart gallery (or null)
	static LibClipartSGallery *ThisGallery;

	// Quiet status of the gallery
	static BOOL QuietStatus;
	bool m_bRedraw;
	bool m_bRestore;


private:
	static LibClipartSGallery* m_pInstance;			// the global instance of the gallery
	INT32	m_nClipTheme;	// whether displaying clipart or web themes
};



/********************************************************************************************
>	class CCAPI OpLibGalClipTheme : public OpDescriptor

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/09/2000
	Purpose:	Manages the clipart gallery's clipart/web themes combo-box.
********************************************************************************************/

#define OPTOKEN_LIBGAL_CLIPTHEME _T("LibGalClipTheme")

class CCAPI OpLibGalClipTheme : public OpDescriptor
{
	CC_DECLARE_DYNCREATE(OpLibGalClipTheme);

public:
	// Creation & destruction.
	OpLibGalClipTheme();

private:
	// Event handlers.
	virtual void OnControlCreate(OpDescControlCreateMsg* pCreateMsg);
	virtual void OnSelectionChange(OpDescControlMsg* pSelChangedMsg, List* pGadgetList);

	// Helpers.
	void UpdateGadgets();

};






/********************************************************************************************

>	class OpDisplayLibClipartGallery: public Operation

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (Based on the Colour SGallery code)

	Purpose:	Displays the LibClipart gallery.
				This op can be attached to a menu, button or keypress
	SeeAlso:	LibClipartSGallery

********************************************************************************************/

#define OPTOKEN_DISPLAYCLIPARTGALLERY _T("DisplayClipartGallery")

class CCAPI OpDisplayLibClipartGallery: public Operation
{         
	CC_DECLARE_DYNCREATE( OpDisplayLibClipartGallery );

public:
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
    void 			Do(OpDescriptor*);

private:
	static DialogBarOp *FindGallery(void);
			// Finds the LibClipart gallery class instance
};  

/********************************************************************************************

>	class SGClipartItem: public SGLibDisplayItem

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/3/95

	Purpose:	A clipart gallery item
	SeeAlso:	SGClipartItem
	
********************************************************************************************/

class SGClipartItem: public SGLibDisplayItem
{         
	CC_DECLARE_DYNCREATE( SGClipartItem );
	
public:

	SGClipartItem();
	~SGClipartItem();

	SGClipartItem(LibraryIndex LibraryIndexToDisplay, BOOL bNew = FALSE);

	LibDisplayType GetDisplayType(SGMiscInfo *MiscInfo);

//	void DragWasReallyAClick(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo);

protected:
	
	virtual	BOOL HandleEvent(SGEventType EventType, void *EventInfo,
							 SGMiscInfo *MiscInfo);

};

/********************************************************************************************

>	class GalleryClipartDragInfo : public BitmapDragInformation

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/4/95
	Purpose:	A fill drag, which was started from the fill gallery
	SeeAlso:	BitmapDragInformation
				
********************************************************************************************/

class GalleryClipartDragInfo : public BitmapDragInformation
{ 
	CC_DECLARE_DYNCREATE(GalleryClipartDragInfo)
 
public:
	GalleryClipartDragInfo();
 	GalleryClipartDragInfo(	SGClipartItem *pSourceItem,
							SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 							BOOL IsAdjust = FALSE, INT32 XSize = 0, INT32 YSize = 0);
	~GalleryClipartDragInfo();

	virtual void OnClick(INT32 Flags, POINT Point);

	virtual UINT32 GetCursorID(DragTarget* pDragTarget);
	virtual BOOL GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget);

	virtual BOOL CanDropOnPage() { return TRUE; }
	virtual BOOL OnPageDrop(ViewDragTarget*);

	virtual BOOL OnMainFrameDrop(DragTarget* pDragTarget);

	SGClipartItem*	GetDraggedClipart()		{ return(SourceItem); }

public:
	SGClipartItem*			SourceItem;
protected:
	SGMouseInfo				MouseInfo;
	SGMiscInfo				MiscInfo;
};

/*********************************************************************************************

>	class 	SGClipartDragTarget : public SGListDragTarget

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

class SGClipartDragTarget : public SGListDragTarget
{
friend class DragManagerOp;
	
CC_DECLARE_DYNAMIC(SGClipartDragTarget)

public:	// Public interface
	SGClipartDragTarget(DialogOp *TheDialog, CGadgetID TheGadget = NULL);

protected:
		// Process a drag-related event
	virtual BOOL ProcessEvent(DragEventType Event,
								DragInformation *pDragInfo,
								OilCoord *pMousePos, KeyPress* pKeyPress);
};



/*********************************************************************************************

>	class SGOilClipartDragTarget : public WinoilDragTarget

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/95

	Purpose:	Drag Target for the mainframe, so dragging clipart into a null document
				situation (behind all the docs) will load a new document

	Notes:		Drag targets are destructed automatically when a drag finishes by the drag
				manager with which they were registered.

				To remove a drag target at any time, destruct it - it automatically deregisters
				and cleans up.
				
	SeeAlso:	DragManagerOp::StartDrag; DragInformation; DragTarget;
				SGColourDragTarget::ProcessEvent

	Documentation:	Docs\Specs\DragMan.doc; Docs\HowToUse\SGallery.doc

*********************************************************************************************/

class SGOilClipartDragTarget : public WinoilDragTarget
{
friend class DragManagerOp;
	
CC_DECLARE_DYNAMIC(SGOilClipartDragTarget)


public:	// Public interface
	SGOilClipartDragTarget(HWND TheWindow, CRect *ClientArea = NULL);
	
	virtual UINT32 GetCursorID();
	virtual BOOL GetStatusLineText(String_256 * TheText);

protected:
		// Process a drag-related event
	virtual BOOL ProcessEvent(DragEventType Event,
								DragInformation *pDragInfo,
								CPoint *pMousePos, KeyPress* pKeyPress);
};


#endif

