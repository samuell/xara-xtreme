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
// SGallery.h - the SuperGallery class

#ifndef INC_SGALLERY
#define INC_SGALLERY

#include "sgtree.h"
#include "sgscan.h"
#include "dlgtypes.h"
#include "dialogop.h"

#define   GSAFECLASS(TheClass)     (TheClass?TheClass:CC_RUNTIME_CLASS(DialogOp))

// Pre-declarations
class DragMessage;

class SGDisplayNode;
class SGDisplayRoot;

class OpState;
class GalleryContextMenu;
class Progress;

struct ReDrawInfoType;

class MenuItem;

/***********************************************************************************************

<	SGActionType

	Comment:	MonoOn
				typedef enum
				{
					SGACTION_CREATE,
					SGACTION_APPLY,
					SGACTION_REDEFINE,
					SGACTION_EDIT,
					SGACTION_DELETE,
					SGACTION_SETOPTIONS,
					SGACTION_SETSORTMODE,
					SGACTION_DISPLAYMODECHANGED
				} SGActionType;
				MonoOff

				This is used by the SuperGallery -> Derived class ApplyAction() upcall.
				This method recieves an action type, from the above list, and applies it
				to the gallery selection. The actions generally correspond to clicks on
				the standard SuperGallery action buttons.

				MonoOn
				Action:				What a conventional gallery would do:

				CREATE				Create a new item, usually copied from the selection
				APPLY				Apply the selected item(s) to the document (select-click)
				APPLYADJUST			Apply the selected item(s) to the document (adjust click)
				REDEFINE			Redefine an item from selection (define by example)
				EDIT				Redefine an item with an editor (e.g. colour editor)
				DELETE				Delete the selected items
				SETOPTIONS			Set options-dialogue values by calling the dialogue
				SETSORTMODE			Set sort-dialogue values by calling the dialogue
				DISPLAYMODECHANGED	Take appropriate action when the display mode changes
				SETLINEPROPERTIES	Set line-properties-dialogue values by calling the dialogue
				MonoOff

	SeeAlso:	SuperGallery::ApplyAction

***********************************************************************************************/

typedef enum
{
	SGACTION_CREATE,
	SGACTION_APPLY,
	SGACTION_APPLYADJUST,
	SGACTION_REDEFINE,
	SGACTION_EDIT,
	SGACTION_DELETE,
	SGACTION_SETOPTIONS,
	SGACTION_SETSORTMODE,
	SGACTION_DISPLAYMODECHANGED,
	SGACTION_SETLINEPROPERTIES,

	SGACTION_ANIMATIONPROPERTIES,
	SGACTION_FRAMEPROPERTIES,

	SGACTION_UNDO		// DEFUNCT - DO NOT USE! This will be removed when possible

} SGActionType;



/***********************************************************************************************

<	SGMenuID

	Comment:	MonoOn
				typedef enum
				{
					SGMENU_OVERITEM,
					SGMENU_OPTIONS
				} SGMenuID;
				MonoOff

				An enum of this type is passed in when a request is made for a gallery to
				build a command menu. The current menu types are:

				MonoOn
				SGMENU_OVERITEM		Build a pop-up menu for the current selection
				SGMENU_OPTIONS		Build a drop-down menu when the options button is clicked
				MonoOff

	SeeAlso:	SuperGallery::BuildCommandMenu

***********************************************************************************************/

typedef enum
{
	SGMENU_OVERITEM,
	SGMENU_OPTIONS
} SGMenuID;

/***********************************************************************************************

>	class SuperGallery : public DialogOp

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/94
	Purpose:	This is the base class for all Camelot "gallery" windows. It is called
				SuperGallery as it replaces the older Gallery class with a new, improved
				system, allowing multiple displaylists to be displayed as categories of
				the gallery display.

	Notes:		SuperGalleries are specced in a number of docs including
					specs\gallib.doc ("super" gallery extensions)
					specs\propui.doc  (gallery basic ui)

	SeeAlso:	SGDisplayNode; SGDisplayRoot; SGDisplayGroup; SGDisplayItem

***********************************************************************************************/

class SGalleryOptionsDlg;			// Defined below
class SGallerySearchDlg;			// Defined below
class SaveRegistryEntries;			// Defined below
class SGallerySortDlg;				// Defined below
class SGalleryLinePropertiesDlg;	// Defined in sglinepr.h

class SuperGallery : public DialogOp
{
friend class SGalleryOptionsDlg;
friend class SGallerySortDlg;
friend class SGallerySearchDlg;
friend class SGalleryLinePropertiesDlg;

	CC_DECLARE_DYNCREATE(SuperGallery)

public:				// Constructors/Destructors
	SuperGallery(CCRuntimeClass *Class = NULL);
	SuperGallery(String_32 &NewName, CCRuntimeClass *Class = NULL);
	~SuperGallery();

private:			// Shared initialisation for constructors
	void InitData(void);


public:				// Dialogue operations on Galleries
	virtual BOOL Create(void);
	virtual void SetVisibility(BOOL Open);

	void SetGallerySize(wxSize Size)
			  		{ GallerySize = Size; };

	wxSize GetGallerySize(void)
			  		{ return GallerySize; };

	// The old write it out to ini file form
	BOOL Write(CCLexFile& file);				// Override DialogBarOp write function
	// The new write it out to the registry form
	BOOL Write(SaveRegistryEntries& Saver);		// Override DialogBarOp write function

protected:			// Private dialogue/Bar handlers for galleries
	wxSize GallerySize;


public:				// Message handling
	virtual MsgResult Message(Msg *Message);
		// If you override Message, you must pass msgs on to SuperGallery::Message


public:				// Derived class interface
	INT32 GetDisplayExtent(void);
		// Returns the minimum y coordinate of the displayed list

	void ForceGroupFolded(SGDisplayGroup *GroupToFold, BOOL FoldedState);
		// Ensures that the given group is {un}folded as desired.
		// Reformats and redraws the DisplayTree if neccessary

	void ScrollToShow(DocRect *Area, BOOL PreferToSeeBottom = FALSE);
		// Scrolls the window, if necessary, to show as much of the specified area
		// as possible. If the area is too large, scrolls to the top of it. Used to
		// scroll to a selected item or items.
		// PreferToSeeBottom indicates that the item at the bottom of the rectangle is
		// more important that the one at the top, so if it doesn't all fit, show the
		// bottom of the rect rather than the top.

	void ScrollArea(DocRect *Area, DocCoord *ScrollBy);
		// Scrolls a given area - uses WINDOW MILLIPOINT values, not virtual coords

	void PaintListNow(void);
		// Just makes the list box update immediately (cf DialogOp::PaintGadgetNow)
	
	void ReformatNow(BOOL ForceReformat);
		// Causes the display tree format to be recalculated immediately if necessary.
		// ForceReformat causes the format to be recalculated even if it is believed
		// to be correctly cached

	void ForceRedrawOfList(void);
		// Forces a reformat and redraw of the entire displayed list

	void ForceRedrawOfArea(DocRect *Area);
		// Forces a redraw of the given area in our virtual coordinate space
		// Only really of use to SGDisplayNode-derived classes!

	void RedrawEverythingBelow(INT32 TopYCoord);
		// Forces a redraw (if necessary) of the region of the display list which
		// occurs below the given virtual coordinate.

	void RedrawDocumentDisplay(Document *TheDocument);
		// Forces a redraw of the given document's display. If the format cache is
		// invalid, a reformat occurs, and all changed areas are redrawn.

	void ReformatAndRedrawIfNecessary(void);
		// If the format cache is invalid, does a reformat of the display. If this changed
		// the cached formatting information at all, the affected area(s) are redrawn

	inline void InvalidateCachedFormat(void);
		// Warns the supergallery that before redrawing/handling a click in this gallery,
		// we must send around an SGEVENT_FORMAT to recache the formatting information.

	void ShadeGallery(BOOL ShadeIt);
		// Shade/Unshade the gallery window - SeeAlso: DoShadeGallery upcall

	void SelectItems(BOOL SelectThem, BOOL Exclusive = FALSE,
						Document *ParentDocument = NULL, Library *ParentLibrary = NULL);
		// Select/Deselect all items, all-items-in-document, all-items-in-library
		// Exclusive indicates if you should apply to the given range or apply to everything
		// outside the given range

	void SelectGroups(BOOL SelectThem, BOOL Exclusive = FALSE,
						Document *ParentDocument = NULL, Library *ParentLibrary = NULL);
		// Select/Deselect all groups, all-groups-in-document, all-groups-in-library
		// Exclusive indicates if you should apply to the given range or apply to everything
		// outside the given range

	void SelectRange(SGDisplayNode *PrimeNode, SGDisplayNode *AnchorNode);
		// Selects the PrimeNode, and if possible, all sibling nodes between it and the
		// Anchor node. If Anchor == NULL or is not found, only PrimeNode is selected.
		// Does not deselect any nodes - you should call SelectItems first to clear the seln.

	INT32 GetSelectedItemCount(Document *TheDocument = NULL, Library *TheLibrary = NULL);
		// Determine how many items are selected in the entire tree (NULL, NULL), or
		// in the given document/library subtree

	INT32 GetSelectedGroupCount(void);
		// Determine how many groups are selected in the entire tree

	virtual void SelectionHasChanged(void);
		// Must be called whenever the selection changes, to make the gallery update itself
		// (For example shade/unshade available option buttons etc)
		// May be overridden by derived galleries to change the shading effects etc.

	inline BOOL AreYouRedrawingNow(void);
		// Returns TRUE if we are currently sitting inside the redraw code. Allows display
		// items to take special action (for scrolling etc) to avoid reentrant calls
		// to the redraw system and unnexcssary invalidations.

	CGadgetID GetListGadgetID(void);
		// Returns the Gadget ID of the list box gadget in the Supergallery dialogue window

	INT32 GetDisplayMode() const { return DisplayMode; }
		// Returns the current value of the gallery DisplayMode variable

	SGDisplayRoot *GetDisplayTree() const { return DisplayTree; }
		// Returns NULL, or a pointer to the display tree root node. Not intended for
		// general use - you should be able to access wha you need via other interfaces.
		// (Used by some drag targets when doing nasty things)

	virtual BOOL CanSearchKeywords(void);
		// Returns FALSE: Overridden by galleries which can supply keywords for searching.

	virtual BOOL GetStatusLineHelp(DocCoord* MousePos, String_256* Result);


public:			// Command interface (for Ops and menu support)
	virtual BOOL PreContextMenu(void);
		// Give the gallery a chance to do something before popping up the context menu
		// At present, used by the bitmap gallery to ensure that the plug-ins have all
		// been parsed.
	
	virtual BOOL InitMenuCommands(void);
		// Called on startup to initialise any desired menu commands. Will make repeated calls
		// to InitMenuCommand(), below.

	BOOL InitMenuCommand(StringBase *CommandID, UINT32 MenuTextID);
		// While initialising, this is called to add a potential menu command. (These are
		// basically OpDescriptors for gallery menu Ops, so must be around permanently)

	virtual BOOL BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID);
		// Builds the command menu for the given menu pop-up (over an item, or options button)

	BOOL AddCommand(GalleryContextMenu *TheMenu, StringBase *CommandID, BOOL Separator = FALSE,
					MenuItem* pRootMenu = NULL);
		// Adds the given command to a menu. Called for each item by BuildMenuCommand.

	virtual OpState GetCommandState(StringBase *CommandID, String_256 *ShadeReason);
		// Returns an OpState indicating the state of this command, much like an Op GetState

	virtual void DoCommand(StringBase *CommandID);
		// Attempts to "do" the given command. Should call down to the base class to handle
		// any commands it can't deal with.

	SGDisplayGroup *FindCommandGroup(void);
		// Determines which group, if any was clicked "in" for the command (i.e. it finds the
		// first selected group or the parent group of the first selected item). May return NULL.


protected:		// Protected derived-class interface
	void InsertCopiedItem(SGDisplayItem *CopiedItem,
								SGDisplayGroup *DestGroup,
								SGDisplayItem *TargetPosition);
		// Asks the gallery to insert the given item into the given group.
		// Optionally, an item in that group may be specified, in which case the
		// CopiedItem will be inserted before/after that item.
		// Otherwise, the copied will should be inserted with DestGroup->AddItem()
		// SEE CopyDisplayItem, below


public:		// Interface generally intended for use by dragging systems
	virtual SGDisplayItem *CopyDisplayItem(SGDisplayItem *SourceItem, 
								SGDisplayGroup *DestGroup,
								SGDisplayItem *TargetPosition = NULL);
		// Asks the gallery to copy (or if appropriate, move) the given item to
		// the given group. This hould be done by copying the given item, and
		// then calling InsertCopiedItem() to insert it appropriately (see above)

	virtual void AllItemsCopied(SGDisplayGroup *DestGroup);
		// Informs the gallery when a sequence of MOVE or COPY actions have been applied
		// to the given gallery (see SGDisplayNode::MoveBefore; SGallery::CopyDisplayItem)
		// (this allows you to cause a message to be broadcast which sums up all of the
		// changes in one go, which is a good thing if a selection of thousands of objects
		// has just been moved up one position in the list!)

	SGDisplayNode *FindNodeUnderPointer(DocCoord *ThePoint);
		// Finds the (usually SGDisplayItem or SGDisplayGroup) node under the given
		// Kernel-rendered-dialogue window coordinate. Used by drag targets to determine
		// which item the mouse pointer lies over.

	void AutoScrollForDrag(DocCoord *MousePosition);
		// Requests that we scroll the list up/down if the given window mouse position
		// is near the top/bottom of the list-box control. Used during drags for autoscroll

	void FillInMiscInfo(SGMiscInfo *MiscInfo,
						ReDrawInfoType *DlgRedrawInfo = NULL);
		// Routine to fill in a MiscInfo struct - intended for internal use, and
		// occasional use by drag targets.


public:		// Interface intended only for use of LibraryFile and Library classes
	virtual SGDisplayGroup *AddLibraryGroup(Library *LibraryToDisplay, INT32 NumItems);
		// Creates a new group (or overwrites an existing group) for the given Library

	virtual void RemoveLibraryGroup(Library *DisplayedLibrary);
		// Ensures that we have no references to the given library in our DisplayTree

	virtual SGDisplayItem *AddLibraryItem(SGDisplayGroup *LibraryGroup, Library *ParentLib,
											LibraryIndex ItemIndex, BOOL bNew = FALSE);
		// Adds an item to the given library display group
		// Must be overridden by derived galleries if they wish to handle Libraries


protected:
	void BroadcastEvent(SGEventType EventType, void *EventInfo, SGMiscInfo *MiscInfo = NULL);
		// Sends the given event around the DisplayTree

	void CalcVirtualSize(ReDrawInfoType *DlgRedrawInfo,
									DocRect *VirtualSize, INT32 *PixelSize);
		// Calculates the virtual coordinate space of the visible portion of the displaylist,
		// and determines the size of a pixel when mapped into this coord space.

	void RenderListBox(ReDrawInfoType *RedrawInfo);
		// Renders the "listbox" region of the dialogue

	void ConvertToVirtualCoords(ReDrawInfoType *DlgRedrawInfo, DocCoord *SourceAndResult);
		// Converts a standard DocCoord, as passed in for kernel-rendered dialogues, into
		// a standard Display-list virtual coordinate, as used by Display items.


public:		// Public version of the above call, for drag targets
	void ConvertToVirtualCoords(SGMiscInfo *MiscInfo, DocCoord *SourceAndResult);
		// Converts a standard DocCoord, as passed in for kernel-rendered dialogues, into
		// a standard Display-list virtual coordinate, as used by Display items.

	BOOL ConvertFromVirtualCoords(SGMiscInfo *MiscInfo, DocRect *SourceAndResult);
		// Converts a DisplayList virtual coordinate rectangle into standard kernel-
		// rendered dialogue coordinates. Returns FALSE if the rect is not visible.
		// (Does NOT clip the returned rectangle to within the visible area)
 

protected:		// Base->Derived class upcalls
	virtual BOOL PreCreate(void);
	virtual BOOL PostCreate(void);
		// Called before/after the window is created (respectively)

	virtual void DoShadeGallery(BOOL ShadeIt);
		// Called by SuperGallery::ShadeGallery to allow the derived gallery to add code
		// to shade/unshade any extra controls it may have in the window

	virtual void HandleDragStart(DragMessage *DragMsg);
		// Called when a drag start message is recieved. Override if you want to
		// ignore drags, or if you want to add handling for special drag types.
		// Default adds a target for the listbox only for SGListDrag drags


public:			// Base->Derived class upcall - also called by display tree nodes sometimes
	virtual BOOL ApplyAction(SGActionType Action);
		// Applies a given action (new, delete, etc) in the gallery. Item is NULL, or
		// a suggested item to apply the action to.


public:			// Interface for the options dialogue
	SGSortKey *GetCurrentSortKeys(void) { return(SortKeys); }
		// Gets a pointer to our array of MaxSGSortModes sort keys

	virtual void ApplySortNow(BOOL ApplyToEntireList);
		// Applies the last selected sort mode (again) to our display tree, or to
		// group(s) of the display tree wqhich contain selection(s)


protected:		// Sorting support
	static INT32 __cdecl SortComparator(const void *Item1, const void *Item2);
		// a 'qsort' comparator function, used internally by ApplySortNow

	static SuperGallery *CurrentSortGallery;

public:			// Virtualisation support
	BOOL DeVirtualiseAllGroups(StringBase *ProgressBarMsg);
		// de-virtualises all the gallery groups

protected:		// RenderRegion replacement - override to provide anything other than OSRndRgn
	virtual RenderRegion *CreateRenderRegion(DocRect *VirtualSize, ReDrawInfoType *DlgRedrawInfo);
	virtual void DestroyRenderRegion(RenderRegion *pRender);


public:		// Access methods for communications from the DisplayTree nodes
	void SetLastSelectedNode(SGDisplayNode *JustSelected);
		// Sets the 'memory' of which node was last made selected

	SGDisplayNode *GetLastSelectedNode(void);
		// Determines which node was last made selected

public:		// Library Gallery overrides
	virtual BOOL GetQuietStatus(void);
		// Get the gallery Quiet button status

	virtual void SetQuietStatus(BOOL Status);
		// Set the gallery Quiet button status

	virtual BOOL IsLibraryGallery(void);
		// TRUE if the SuperGallery is also a LibraryGallery

	virtual BOOL IsLibraryGalleryWithNonLibraryGroups(void);
		// TRUE if the SuperGallery is also a LibraryGallery & has no non-library groups

public:		// Base SGDisplayNode access for implentation of background redraws
	void IncrementPendingRedraws(void);
		// Adds another item to the number enqueued for BG redraw and adds an idle
		// processor to provide BG redraw if necessary.

	void DecrementPendingRedraws(void);
		// Removes an item from the number enqueued for BG redraw and removes any
		// idle processor if idles are no longer needed

	virtual BOOL OnIdleEvent(void);
		// Handles idle events (when enabled) to provide background redraws

	void FlushBackgroundRedraws(void);
		// Ensures that no nodes in the tree are waiting for background redraws,
		// and resets our bg rendering system (so if it gets confused, this call will
		// resurrect it by resetting the state)

	void SetLastBackgroundNode(SGDisplayNode *LastNodeRendered)		\
								{ LastBGNode = LastNodeRendered; }
		// Tell the supergallery which node was last rendered by the BG rendering system

	SGDisplayNode *GetLastBackgroundNode(void)		\
								{ return(LastBGNode); };
		// Retrieve a pointer to the last node which was background rendered


protected:
	INT32			PendingRedraws;	// Counter indicating when we need an idle event handler
									// in order to handle pending background redraw of items

	SGDisplayNode	*LastBGNode;	// Points to the last node we background rendered


public:
	SGDisplayRoot	*DisplayTree;	// Pointer to the Display Tree
protected:			// Display tree information
	INT32				DisplayMode;	// Current display mode identifier
	BOOL			FormatIsCached;	// TRUE if the format info for the display tree is cached

	INT32			LastWindowDX;	// Last size of the display window. If this size changes
	INT32			LastWindowDY;	// then we must make sure the display is reformatted

	BOOL			AmShaded;		// TRUE if the gallery is currently in a shaded state
	BOOL			AmRedrawing;	// Lock: TRUE if the gallery is inside the redraw handler

	SGDisplayNode	*LastSelectedNode;	// Points to the last node which was made selected

	MonotonicTime	LastClickTime;		// Remembers time when a listbox click was last received
	DocCoord		LastClickPos;		// Pos of last click (dbl-click must be close to this)
	BOOL			DblClickPending;	// Indicates we expect 2nd click in double click


protected:			// For access by our friend classes only
	SGalleryOptionsDlg *CurrentOptionsDlg;	// NULL, or the currently open opts dlg
											// (Set by friend class SGalleryOptionsDlg)

	SGallerySortDlg *CurrentSortDlg;		// NULL, or the currently open sort dlg
											// (Set by friend class SGallerySortDlg)

	SGallerySearchDlg *CurrentSearchDlg;	// NULL, or the currently open search dlg
											// (Set by friend class SGallerySearchDlg)

	SGSortKey		SortKeys[MaxSGSortKeys];// An array describing the last applied sort mode

	SGalleryLinePropertiesDlg *CurrentLinePropertiesDlg;	// NULL, or the currently open line properties dlg
											// (Set by friend class SGalleryLinePropertiesDlg)

	virtual void CloseOwnedDialogs(void);	// Closes any of the above dialogs if open

public:		// Global gallery preferences
	static INT32		UseFixedColourSet;		// TRUE (default) to override Windows colours

public:
	virtual BOOL IsABar() {return FALSE; } 		// We are not a bar
	virtual BOOL IsAGallery() {return TRUE; }	// We are a gallery
	static SuperGallery* FindSuperGallery(String_32& SuperGalleryName, INT32 limit = -1);
	static SuperGallery* FindSuperGallery(ResourceID SuperGalleryID);

	String_32		Name;
	String_32&		GetGalName()							{ return Name; }
	void			SetGalName(const String_32& str)		{ Name = str; }

};





/********************************************************************************************

>	inline void SuperGallery::InvalidateCachedFormat(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Purpose:	Indicates to the super gallery that the cached display tree formatting is
				out of date, and must be recalculated before next redraw of the display

********************************************************************************************/

void SuperGallery::InvalidateCachedFormat(void)
{
	FormatIsCached = FALSE;
}



/********************************************************************************************

>	inline BOOL SuperGallery::AreYouRedrawingNow(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/95

	Returns:	TRUE if we are currently processing a redraw event

	Purpose:	To determine if the gallery is currently redrawing. This is necessary
				for two reasons: (1) To ensure we don't go re-entrant in redraw code
				(because we can't have more than one active render region or it'll go
				bang), (2) To allow things like the SGDisplayRootScroll to take special
				action when they try to chnage the scroll position if we are in a redraw
				(obviously at this point, doing a ScrollWindow and force redraws would
				be silly, if not downright dangerous)

********************************************************************************************/

BOOL SuperGallery::AreYouRedrawingNow(void)
{
	return(AmRedrawing);
}








typedef struct
{
	SGSortKey	SortKey;
	UINT32 		ListControlID;
	UINT32		CheckControlID;
} SGOptsSortInfo;


/***********************************************************************************************

>	class SGalleryOptionsDlg : public DialogOp

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Purpose:	A dialogue box allowing the user to set the gallery properties (display
				options) for its parent gallery

	SeeAlso:	SuperGallery; SGalleryOptionsDlg::InvokeDialog

***********************************************************************************************/

#define OPTOKEN_SGOPTIONSDLG _T("GalleryOptionsDlg")


class SGalleryOptionsDlg : public DialogOp
{
	CC_DECLARE_DYNCREATE(SGalleryOptionsDlg)

public:
	SGalleryOptionsDlg();
	~SGalleryOptionsDlg();

	MsgResult Message(Msg *Message);

	void Do(OpDescriptor*);
	void DoWithParam(OpDescriptor*, OpParam *Param);

	static BOOL Init();
	BOOL Create();
	static OpState GetState(String_256*, OpDescriptor*);

	static const UINT32 IDD;
	static const CDlgMode Mode;

	static void InvokeDialog(SuperGallery *Parent);
		// Call this method to create and display a gallery options dialogue

	void AddDisplayModeName(UINT32 NameResourceID);
	void AddDisplayModeName(StringBase *EntryName);
		// Called by derived gallery from ApplyAction(SGACTION_SETOPTIONS) function to set
		// each entry in turn of their display mode combobox list.


protected:
	SuperGallery *ParentGallery;

	void InitValues(void);
	void SetControls(void);

	void Commit(void);
};






/***********************************************************************************************

>	class SGallerySortDlg : public DialogOp

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/95 (separated from opts dlg)

	Purpose:	A dialogue box allowing the user to apply a sort mode to its parent
				SuperGallery DisplayTree

	SeeAlso:	SuperGallery; SGallerySortDlg::InvokeDialog

***********************************************************************************************/

#define OPTOKEN_SGSORTDLG _T("GallerySortDlg")


class SGallerySortDlg : public DialogOp
{
	CC_DECLARE_DYNCREATE(SGallerySortDlg)

public:
	SGallerySortDlg();
	~SGallerySortDlg();

	MsgResult Message(Msg *Message);

	void Do(OpDescriptor*);
	void DoWithParam(OpDescriptor*, OpParam *Param);

	static BOOL Init();
	BOOL Create();
	static OpState GetState(String_256*, OpDescriptor*);

	static const UINT32 IDD;
	static const CDlgMode Mode;

	static void InvokeDialog(SuperGallery *Parent);
		// Call this method to create and display a gallery options dialogue

	void AddSortKeyName(UINT32 NameResourceID);
	void AddSortKeyName(StringBase *EntryName);
		// Called by derived gallery from ApplyAction(SGACTION_SETOPTIONS) function to set
		// each entry in turn of their sort key combobox lists.


protected:
	void ShadeSortFields(void);
		// Shades/unshades the sort fields as appropriate to the do-sort button

protected:
	SuperGallery *ParentGallery;

	void InitValues(void);
	void SetControls(void);

	void Commit(void);

protected:
	static SGOptsSortInfo GallerySorts[MaxSGSortKeys];
};






/***********************************************************************************************

>	class SGallerySearchDlg : public DialogOp

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/3/95

	Purpose:	A dialogue box allowing the user to search a gallery DisplayTree for items
				whose name or fullinfo text contains a given string.

	SeeAlso:	SuperGallery; SGallerySearchDlg::InvokeDialog

***********************************************************************************************/

#define OPTOKEN_SGSEARCHDLG _T("GallerySearchDlg")


class SGallerySearchDlg : public DialogOp
{
	CC_DECLARE_DYNCREATE(SGallerySearchDlg)

public:
	SGallerySearchDlg();
	~SGallerySearchDlg();

	MsgResult Message(Msg *Message);

	void Do(OpDescriptor*);
	void DoWithParam(OpDescriptor*, OpParam *Param);

	static BOOL Init();
	BOOL Create();
	static OpState GetState(String_256*, OpDescriptor*);

	static const UINT32 IDD;
	static const CDlgMode Mode;

	static void InvokeDialog(SuperGallery *Parent);
		// Call this method to create and display a gallery options dialogue

protected:
	SuperGallery *ParentGallery;

	void InitValues(void);
	void SetControls(void);

	void Search(BOOL FindFirst);			// Reads the current window settings and searches

	inline SGDisplayItem *SearchFindNextItem(SGDisplayItem *LastItem, Progress *Hourglass = NULL);	// Get next item
	inline SGDisplayItem *SearchFindStartItem(BOOL FindFirst);			// Get first item

	inline BOOL DoComparison(SGDisplayItem *Item, StringBase *SearchText, BOOL UseKeywords = FALSE,
				BOOL ItsALibrary = FALSE, BOOL FullInfo = FALSE);

	static BOOL LastSearchFullInfo;			// TRUE if the last search was on full info
	static String_64 LastSearchText;		// The text used for the last search
};

#endif // INC_SGALLERY
