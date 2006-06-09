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
// This file is the header file for the gallery classes

#ifndef INC_GALLERY
#define INC_GALLERY

#if FALSE
// GALLERIES ARE DEFUNCT - SEE SuperGallery INSTEAD

//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ordlist.h"


/********************************************************************************************

>	class GalleryBodge : public CC_CLASS_MEMDUMP

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Purpose:	A bodge prototype pure virtual class. This should not be derived in this way.	 

********************************************************************************************/

// Your class definition in here, eg
// This is a dummy definition - please ignore. Soon to be deleted
class GalleryBodge : public CC_CLASS_MEMDUMP
{
	// Give my name in memory dumps
	// If you need a different sort of decare, replace this one. 
	// See CCObject in the help file for info about DECLARE types
	CC_DECLARE_MEMDUMP(GalleryBodge);
	
public:
	// Add some functions here

	// Add some vars here
};



/***********************************************************************************************

>	class GalleryDisplayListPtr : public ListItem

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/94
	Purpose:	The Gallery class provides a mechanism for all derived classes to use to
				store a cached DisplayList in each document.
				This is achieved using a list of GalleryDisplayListPtr's held in
				the Document object, for which the Gallery provides management
				access functions.
	Scope:		private to Gallery
	SeeAlso:	Gallery

***********************************************************************************************/

class GalleryDisplayListPtr : public ListItem
{
	friend class Gallery;

	CC_DECLARE_MEMDUMP(GalleryDisplayListPtr);

public:
	GalleryDisplayListPtr(OrderedList *ListPtr);
	~GalleryDisplayListPtr();

protected:		// Only available from within Gallery and derivatives
	OrderedList *DisplayList;	
};



/***********************************************************************************************

>	class GalleryDisplayItem : public ListItem

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Purpose:	This list item is used by the gallery to keep tabs on the real
				item displayed in a gallery. An OrderedList Sequence will contain
				SequenceItems that point at GalleryDisplayItems. Thus, by a double
				dereference from the original SequenceItem, one can find the actual
				item data related to the displayed Gallery ListBox item.
	Scope:		private middle-man data structure used by the Gallery class
	SeeAlso:	Gallery

***********************************************************************************************/

class GalleryDisplayItem : public ListItem
{
	friend class Gallery;

	CC_DECLARE_MEMDUMP(GalleryDisplayItem);

public:
	GalleryDisplayItem();
	GalleryDisplayItem(ListItem *TheRealItem);

	inline ListItem *GetRealItem(void);
	inline BOOL 	IsSelected(void);
	inline void		SetSelected(BOOL IsSelected);

private:
	ListItem *RealItem;			// Pointer to the actual item we are displaying
	BOOL	 Selected;			// TRUE if this item is selected in the gallery listbox
};



/***********************************************************************************************

>	inline ListItem *GalleryDisplayItem::GetRealItem(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Returns:	A pointer to the real item
	Purpose:	Dereferences a GalleryDisplayItem in order to find the real item 
				to which it refers.
	Scope:		private middle-man used by the Gallery class
	SeeAlso:	Gallery::FindRealItem

***********************************************************************************************/

ListItem *GalleryDisplayItem::GetRealItem(void)
{
	return(RealItem);
}						 



/***********************************************************************************************

>	inline BOOL GalleryDisplayItem::IsSelected(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Returns:	TRUE if this item is selected
	Purpose:	Determines the selection state of a display list item
	Scope:		private middle-man used by the Gallery class
	SeeAlso:	GalleryDisplayItem::SetSelected

***********************************************************************************************/

BOOL GalleryDisplayItem::IsSelected(void)
{
	return(Selected);
}						 



/***********************************************************************************************

>	inline void GalleryDisplayItem::SetSelected(BOOL IsSelected)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Purpose:	Sets the selection state of a GalleryDisplayItem
	Scope:		private middle-man used by the Gallery class
	SeeAlso:	GalleryDisplayItem::IsSelected

***********************************************************************************************/

void GalleryDisplayItem::SetSelected(BOOL IsSelected)
{
	Selected = IsSelected;
}						 





/********************************************************************************************

>	class Gallery : public DialogBarOp

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>, Jason
	Created:	22/6/94
	Purpose:	The kernel version of the gallery.	 

********************************************************************************************/


class GalleryDisplayDlg;		// Pre declare. See below.

class Gallery : public DialogBarOp
{
friend class GalleryDisplayDlg;

	CC_DECLARE_DYNCREATE( Gallery )

public:				// Constructors/Destructors
	Gallery(CCRuntimeClass* Class = NULL);
	Gallery(String_32& NewName,CCRuntimeClass* Class = NULL);
	~Gallery();


public:				// Dialogue operations on Galleries
	BOOL Create(void);
	void SetGallerySize(CSize Size)
			  		{ GallerySize = Size; };

	CSize GetGallerySize(void)
			  		{ return GallerySize; };

	void ShadeGallery(void);
	void UnshadeGallery(BOOL UnshadeIt = TRUE);

	BOOL 			Write(CCLexFile& file);	// Override DialogBarOp read function

protected:
	BOOL IsShaded;	// Current shaded/unshaded status


protected:			// Private dialogue/Bar handlers for galleries
	CSize GallerySize;
	

public:				// Message handlers
	MsgResult Message(Msg *Message);
		// If you override Message, you must pass msgs on to Gallery::Message

	inline Document *GetGalleryDocument(void);


public:				// Display list handlers - available to external users & galleries
	DWORD GetDisplayedItemCount(void);
	inline SequenceItem *FindFirstDisplayed(void);
	inline SequenceItem *FindLastDisplayed(void);
	inline SequenceItem *FindNextDisplayed(SequenceItem *CurItem);
	inline SequenceItem *FindPreviousDisplayed(SequenceItem *CurItem);

	ListItem *FindRealItem(SequenceItem *DisplayItem);


protected:			// Display List handlers - used by derived classes only
	ListItem *FindRealItem(GalleryDisplayItem *DisplayItem);

	SequenceItem *FindSequenceItem(ListItem *TheItem);

	DWORD GetSelectedItemCount(void);
	SequenceItem *FindFirstSelected(void);
	SequenceItem *FindLastSelected(void);
	SequenceItem *FindNextSelected(SequenceItem *CurItem);
	SequenceItem *FindPreviousSelected(SequenceItem *CurItem);

	void DeselectAllItems(void);
	void SetDisplayItemSelState(SequenceItem *TheItem, BOOL IsSelected);
	void SetDisplayItemSelState(INT32 ItemIndex, BOOL IsSelected);


	BOOL AddItemToDisplay(ListItem *TheItem);
		// Adds the given item to the display list, returning TRUE for success
		// If you wish to find the newly added item in the display list, you must
		// then call FindSequenceItem()

	BOOL AddListToDisplay(List *TheList);
		// Adds all items in the given list to the display list, returning TRUE if it
		// succeeds. As the items may be spread apart due to the display sorting
		// it cannot return where in the list the items have been placed.
	
	BOOL RemoveItemFromDisplay(SequenceItem *TheSeqItem);
	BOOL RemoveItemFromDisplay(ListItem *TheItem);
		// Deletes the DisplayItem referring to this item, removing it from the display
		// DOES NOT delete the master item

	void RemoveAllItemsFromDisplay(void);
		// As RemoveItemFromDisplay, but removes ALL items (clears the display)

	void ForceRedrawOfList(void);
		// re-paints the gallery list box to show the new gallery information

	void ForceRedrawOfItem(SequenceItem* pSeqItem);
		// re-paints the gallery list item 

	void ForceRedrawOfItem(INT32 Index);
		// re-paints the gallery list item 

	
protected:			// Display List caching support
	BOOL AddDisplayListToDoc(Document *ScopeDoc, OrderedList *NewList);
		// Adds the given list (or creates one if it's NULL) to the list of cached
		// Gallery displaylists held in the Document. Used to cache derived gallery
		// DisplayLists in each document.

	void DeleteDisplayListFromDoc(Document *ScopeDoc);
		// Removes a DisplayList from the document DisplayList cache

	OrderedList *FindDisplayListInDoc(Document *ScopeDoc);
		// Finds a derived gallery's DisplayList from a document's DisplayList cache.


	inline OrderedList *GetCurrentDisplayList(void);

	void SelectDisplayList(OrderedList *NewDisplayList);
		// Selects the given list as the new displaylist for the gallery

	void SelectMyDocDisplayList(Document *ScopeDoc);
		// Selects the appropriate cached DisplayList fromt the GalleryDocument
		// as the new current DisplayList


private:			// Internal (private) DisplayList caching support
	void RescanSelection(void);
		// Scans the list box, setting the DisplayList selection-state cache to echo the
		// current selection state. Intended only for internal use

	void SetSelectionFromDisplayList(void);
		// Tells the gallery to update the selection state of the list box from the current
		// DisplayList selection-state cache. Intended only for internal use


protected:			// Gallery Display Dialogue information - current display mode
	UINT32 CurrentDisplayMode;

			
private:			// Gallery sort mode information
	static INT32 SortFunction(OrderedList *Parent, ListItem *Item1, ListItem *Item2);
		// This generic method takes the specific instance's GallerySortKey and
		// provides a sort comparator operation with it. The Sort Mode is stored
		// as an array of SortKeys in the OrderedList passed to the sort function.


protected:			// Derived class interface - called by Gallery base class
	virtual BOOL PreCreate(void);
	virtual BOOL PostCreate(void);
		// Initialise display-related stuff before/after the window is created

	virtual BOOL DoRedrawAnItem(ListItem *Item, BOOL Large, UINT32 DisplayMode);
		// Redraws the given item appropriately for large/small mode, and
		// using the given derived class DisplayMode (see DoDisplayMode... below)
		// returns TRUE if it redrew successfully

	virtual BOOL PrepareToDelete(void);
		// returns TRUE if the selection can be deleted, or FALSE if not
		// May alter the selection as appropriate (after getting user confirmation)

	virtual BOOL DoDeleteSelection(void);
		// Force-deletes anything still remaining in the selection
		// returns TRUE if successful

	virtual ListItem *DoCreateNewItem(ListItem *BasedOn);
		// Applies the appropriate action to create a new item, based upon the
		// given item (or if that is NULL, the attribute applied to the document
		// selection, or a default). Returns the new item.

	virtual void DoApplyItem(ListItem *ItemToApply);
		// Applies the appropriate action to apply the selected item to the document

	virtual BOOL DoRedefineItem(ListItem *ItemToRedefine);
		// Redefines the given item (allow the user to edit it for example)
		// Returns FALSE if the value of the item definitely has not changed
		// Returns TRUE if the item has changed, or if it is unknown whether it was changed

	virtual void DoSpecialClick(ListItem *ClickedItem);
		// Handles a special (alt-)click on a list item. This is generally used as a short-
		// cut to bring up an editor on the clicked item, but the exact interpretation is
		// left to the derived gallery to decide.

	virtual StringBase *GetItemText(ListItem *TheItem);
		// Returns NULL, or a pointer to the text to use in the listbox.
		// This is an early development aid while we're waiting for the advent
		// of proper user-redraw.

	virtual void SortModeHasBeenChanged(BOOL SetDerivedSortsToo);
		// Called when the display dialogue has set a new sort mode, to make the
		// Gallery (and related entities) redisplay its list in the new order.
		// Note that the base class method for this MUST be calleded in derivations

	virtual BOOL DoSetDisplayModes(GalleryDisplayDlg *TheDialogue);
		// Called to set up the display mode combo box (large icons, full info etc)
		// This will call TheDialogue->SetDisplayModeName repeatedly to set all
		// available modes. Returns FALSE if there are no display modes.

	virtual void DoDisplayModeChange(UINT32 NewDisplayMode);
		// Called when the user has chosen a display mode, with the index of the
		// mode in the given list. You should not need to take action here, as
		// the display mode is passed through to RedrawAnItem, but this is called
		// in case you wish to cache values before redraw...

	virtual void DoShowMenu(void);
		// Pops up the Gallery menu
		// Menu choices will be handled by the derived class through the Ops system
		// or message handlers - the base class knows nothing of this process.

	virtual	BOOL SetAlsoButton(String_64 *TextToPutIn);
		// Called to set up the generic Gallery Display dialogue
		// Returns FALSE if no 'also set the sort mode for the xxxx' button is wanted
		// Returns TRUE, and fills in the text with an appropriate string if 
		// a button is wanted. See the ColourGallery for an example.

	virtual void DoSelectionChanged();
		// A selection changed message has occurred on the gallery list
		// This is called after the gallery has rescanned the list, so it is up to date
		// when you receive this call

	virtual void DoShadeGallery(BOOL ShadeIt);
		// Called by Gallery::ShadeGallery or UnshadeGallery to allow the derived
		// gallery to add code to shade/unshade any extra controls it may have in
		// its window


protected:			// Supplied methods to support DerivedGallery redraw of items
	void PaintText(INT32 XOffset, INT32 YOffset, StringBase *TheText);


private:			// General data members - only accessible through veneer functions
	OrderedList	*DisplayList;			// The OrderedList of display items
	List		*DisplaySequence;		// The sorted display sequence of the OrdList
										// (this is permanently selected)

private:			// Private winoil interface and related subroutines
	void ApplyApplyItem(void);
	void ApplyNewItem(void);
	void ApplyRedefineItem(void);
	void ApplyDeleteSelection(void);
	void ApplyUndo(void);
	void ApplyGalleryMenu(void);
	void ApplySpecialClick(void);

	INT32 CalculateListIndex(SequenceItem *TheItem);
		// Given an item, determine its index in the DisplayList

	SequenceItem *FindItemByIndex(INT32 ItemIndex);
		// Given an index into the DisplayList, find the item to which that refers

	inline void EnsureDisplaySequence(void);
};



/***********************************************************************************************

inline OrderedList *Gallery::GetCurrentDisplayList(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Returns:	NULL, or a pointer to the current gallery DisplayList
	Purpose:	To find the current display list. This should only be necessary
				for code that checks/sets the DisplayList on document changes.

***********************************************************************************************/

OrderedList *Gallery::GetCurrentDisplayList(void)
{
	return(DisplayList);
}



/***********************************************************************************************

>	Document *Gallery::GetGalleryDocument(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Returns:	NULL, or a pointer to the Gallery doc 
	Purpose:	Finds the document from which all Galleries display information.
				This currently corresponds to the Selected document, but you should
				use this call to determine the doc so that any scheme can be
				implemented if/whenever we feel like it.

***********************************************************************************************/

Document *Gallery::GetGalleryDocument(void)
{
	return(Document::GetSelected());
}



/***********************************************************************************************

>	inline SequenceItem *Gallery::FindFirstDisplayed(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Returns:	NULL, or a pointer to the first item
	Purpose:	Finds the first item displayed in the Gallery.
				This uses the current gallery display order, so this is the item
				displayed at the top of the list box.
	SeeAlso:	Gallery::FindLastDisplayed; Gallery::FindNextDisplayed; 
				Gallery::FindPreviousDisplayed; Gallery::GetDisplayedItemCount

***********************************************************************************************/
 
SequenceItem *Gallery::FindFirstDisplayed(void)
{
	EnsureDisplaySequence();
	return( (SequenceItem *) DisplaySequence->GetHead() );
}



/***********************************************************************************************

>	inline SequenceItem *Gallery::FindLastDisplayed(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Returns:	NULL, or a pointer to the last displayed item
	Purpose:	Finds the last item displayed in the gallery
				This uses the current gallery display order, so this is the item
				displayed at the bottom of the list box.
	SeeAlso:	Gallery::FindFirstDisplayed; Gallery::FindNextDisplayed; 
				Gallery::FindPreviousDisplayed; Gallery::GetDisplayedItemCount

***********************************************************************************************/
 
SequenceItem *Gallery::FindLastDisplayed(void)
{
	EnsureDisplaySequence();
	return( (SequenceItem *) DisplaySequence->GetTail() );
}



/***********************************************************************************************

>	inline SequenceItem *Gallery::FindNextDisplayed(SequenceItem *CurItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Inputs:		CurItem - The current position in the list
	Returns:	NULL (if have passed the last item), or a pointer to the next
				item in the current display ordering.
	Purpose:	Given a displayed gallery item, finds the next item in the display
	SeeAlso:	Gallery::FindFirstDisplayed; Gallery::FindLastDisplayed;
				Gallery::FindPreviousDisplayed

***********************************************************************************************/
 
SequenceItem *Gallery::FindNextDisplayed(SequenceItem *CurItem)
{
	ENSURE(DisplaySequence != NULL, "Gallery DisplaySequence is NULL!");
	return( (SequenceItem *) DisplaySequence->GetNext(CurItem) );
}



/***********************************************************************************************

>	inline SequenceItem *Gallery::FindPreviousDisplayed(SequenceItem *CurItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Inputs:		CurItem - The current position in the list
	Returns:	NULL (if have passed the first item), or a pointer to the previous
				item in the current display ordering.
	Purpose:	Given a displayed gallery item, finds the previous item in the display
	SeeAlso:	Gallery::FindFirstDisplayed; Gallery::FindLastDisplayed;
				Gallery::FindNextDisplayed

***********************************************************************************************/
 
SequenceItem *Gallery::FindPreviousDisplayed(SequenceItem *CurItem)
{
	ENSURE(DisplaySequence != NULL, "Gallery DisplaySequence is NULL!");
	return( (SequenceItem *) DisplaySequence->GetPrev(CurItem) );
}



/***********************************************************************************************

>	void Gallery::EnsureDisplaySequence(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/94
	Purpose:	Ensures that the DisplaySequence member variable is up to date, and points
				at the current display sequence for the gallery.
	Scope:		private

***********************************************************************************************/

void Gallery::EnsureDisplaySequence(void)
{
	if (DisplayList != NULL && DisplaySequence == NULL)
		DisplaySequence = DisplayList->SelectOrder(SortFunction, FALSE);

	ERROR3IF(DisplaySequence == NULL, "Unable to select a valid DisplaySequence!\n");
}






// Sort information for the gallery display dialogue. Holds the currently selected
// sort keys, plus the control IDs of the lists/check boxes which display these
// modes in the dialogue. An array of these is used to store MaxSortKeys of these
// structures, one per sort key.

typedef struct
{
	SortKey Key;
	UINT32 	ListControlID;
	UINT32 	CheckControlID;
} GalSortInfo;



/***********************************************************************************************

>	class GalleryDisplayDlg : public DialogOp

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94 (Colour Sort Dialogue -> Gallery Display Dialogue 21/7/94)
	Purpose:	A dialogue box allowing the user to set the display options (size
				and sort mode) for its parent Gallery display list.
	SeeAlso:	Gallery

***********************************************************************************************/

#define OPTOKEN_GALDISPLAYDLG _T("GalDisplayDialog")


class GalleryDisplayDlg : public DialogOp
{
	CC_DECLARE_DYNCREATE(GalleryDisplayDlg)

public:
	GalleryDisplayDlg();

	MsgResult Message(Msg *Message);

	void Do(OpDescriptor*);
	static BOOL Init();
	BOOL Create();
	static OpState GetState(String_256*, OpDescriptor*);

	static const UINT32 IDD;

	static const CDlgMode Mode;

	void SetParentGallery(Gallery *Parent);

	void SetDisplayModeName(StringBase *EntryName);
		// Called by derived gallery from DoSetDisplayMode() function to set each
		// entry in turn of their display mode combobox list.

protected:
	Gallery *ParentGallery;

	static GalSortInfo GallerySorts[MaxSortKeys];

	void InitValues(void);
	void SetControls(void);
	void SetSortMode(void);

	static ComparatorInfo *FindCIFromName(List *Sorts, StringBase &Name);
	static INT32 FindIndexFromCI(List *Sorts, INT32 SortInfoIndex);
};

#endif	// if FALSE

#endif	// INC_GALLERY
