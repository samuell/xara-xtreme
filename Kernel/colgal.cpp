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
// colgal.cpp - Implementation of the ColourGallery class

/*
*/


#include "camtypes.h"



#include "attrmgr.h"
#include "ccolbar.h"
#include "coldlog.h"
#include "colgal.h"
#include "colmsg.h"
#include "colormgr.h"
#include "colpick.h"
#include "ensure.h"
#include "fillattr.h"
//#include "galstr.h"
//#include "jason.h"
#include "lineattr.h"
#include "oilfiles.h"
//#include "markn.h"
//#include "resource.h"
#include "sginit.h"

//#include "will2.h"

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");






#if FALSE
// The old Colour Gallery no longer exists. THIS FILE IS DEFUNCT


CC_IMPLEMENT_DYNCREATE(ColourGallery, Gallery)
CC_IMPLEMENT_DYNCREATE(OpDisplayColourGallery,Operation);

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW





// Static member variables
ColourGallery *ColourGallery::TheColourGallery = NULL;

static BOOL LockColourChange = FALSE;



/********************************************************************************************

>	ColourGallery::ColourGallery() : Gallery()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Purpose:	Colour Gallery constructor

********************************************************************************************/

ColourGallery::ColourGallery() : Gallery()
{
	TheColourGallery = this;				// Set up our static instance pointer

	// Just in case we arrive at the party late, ensure that the SelectedDoc
	// has a display cache - this is needed for the colour bar if it is to be able
	// to display a colour list
	if (Document::GetSelected() != NULL)
		CreateCachedDisplayList(Document::GetSelected());
}



/********************************************************************************************

>	ColourGallery::~ColourGallery()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/94
	Purpose:	Colour Gallery destructor

********************************************************************************************/

ColourGallery::~ColourGallery()
{
	ERROR3IF(TheColourGallery != this,
		"There seem to have been several ColourGallery instantiations?!\n");

	TheColourGallery = NULL;
}



/********************************************************************************************

>	virtual BOOL ColourGallery::PreCreate(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Returns:	TRUE if it is successful; FALSE to abort creation of the window
				FALSE => ERROR1

	Purpose:	Colour Gallery PreCreate handler

	Notes:		This method is called whenever the gallery window is about to be
				created.
				At this point, the gallery should set up data structures - it is
				*essential* that DisplayList is set up by the end of this method.
				(As it is used in the creation of the window)
				In the case of the colour gallery, the display list has already
				been set up, so this function needs take no action. (However, if
				the DisplayList is NULL, it refuses to allow the Gallery window
				to be created)

				Note that you cannot AddItemToDisplay etc in this method, as 
				although the display list exists, the related window controls
				do not yet exist, and ENSURE failures will result.

********************************************************************************************/

BOOL ColourGallery::PreCreate(void)
{
	SwitchTheDisplayList(NULL); 		// Ensure there's a DisplayList for the GalleryDoc
	return(GetCurrentDisplayList() != NULL);
}



/********************************************************************************************

>	virtual BOOL ColourGallery::PostCreate(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Returns:	TRUE if it is successful; FALSE to abort creation of the window

	Purpose:	Colour Gallery PostCreate handler

	Notes:		This method is called whenever the gallery window is created, AFTER
				the window has been brought into existence.
				At this point (and henceforth), the gallery can add items to]
				the DisplayList, etc. as the display controls now exist.

********************************************************************************************/

BOOL ColourGallery::PostCreate(void)
{
	if (GetCurrentDisplayList() == NULL)
		return(FALSE);

	ReadNewColourList();

	return(TRUE);
}



/********************************************************************************************

>	static void ColourGallery::ColourListHasChanged(ColourList *TheList)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Inputs:		TheList - The list which has been changed

	Purpose:	The ColourManager calls this function just prior to broadcasting a
				LISTUPDATED ColourChangingMsg. This allows us to update our display list
				so that entities (such as the colour bar), which interrogate us upon
				recieving the message, get correct information (as if we sat on the
				msg broadcast, we could not guarantee that we'd get it first)

	Notes:		This method is called whenever the gallery window is created, AFTER
				the window has been brought into existence.
				At this point (and henceforth), the gallery can add items to]
				the DisplayList, etc. as the display controls now exist.

********************************************************************************************/

void ColourGallery::ColourListHasChanged(ColourList *TheList)
{
	if (TheColourGallery == NULL)
		return;

	TheColourGallery->UnshadeGallery();
	if (TheColourGallery->GetCurrentDisplayList() != NULL)
		TheColourGallery->ReadNewColourList();
}



/********************************************************************************************

>	void ColourGallery::SetSelectionFromDocument(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94

	Purpose:	Sets the colour gallery listbox selection state based upon the currently
				selected object(s) in the document (to show the current fill/stroke colours)

	Scope:		private

********************************************************************************************/

void ColourGallery::SetSelectionFromDocument(void)
{
	DocColour *SelColour[2];
	BOOL AreCurrentAttrs =
			ColourManager::GetCurrentLineAndFillColours(&SelColour[0], &SelColour[1]);

	DeselectAllItems();					// Clear the current selection
	if (!AreCurrentAttrs)
	{
		// The line/fill colours returned are those of the selection, not the current attrs
		IndexedColour *Parent;
		
		for (INT32 i = 0; i < 2; i++)		// For each colour (line, fill)
		{
			if (SelColour[i] != NULL)	// If there is a single selected colour
			{
				Parent = SelColour[i]->FindParentIndexedColour();		// Find the named IxColour
				if (Parent != NULL && Parent->IsNamed())				// (if any)
				{
					SequenceItem *SeqItem = FindSequenceItem(Parent);	// And select it

					if (SeqItem != NULL)
						SetDisplayItemSelState(SeqItem, TRUE);
				}
			}
		}
	}
}



/********************************************************************************************

>	MsgResult ColourGallery::Message(Msg* Message) 

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Inputs:		Message- The message to handle

	Purpose:	Colour Gallery dialog message handler

	Notes:		This should generally only process GalleryMsg messages and things
				like DocChagingMsg messages which might affect what list you want
				your gallery to display. All unprocessed messages must be passed
				down to the base class Gallery::Message method

********************************************************************************************/

MsgResult ColourGallery::Message(Msg* Message) 
{
	// Note that all Dialogue messages should have been handled by the base class
	// message handler. Unless we're being antisocial, all we should have to
	// process here are list changing messages such as Doc- or Colour-ChangingMsg

	if (MESSAGE_IS_A(Message, ColourChangingMsg))
	{
		ColourChangingMsg *Msg = (ColourChangingMsg *) Message;

		switch(Msg->State)
		{
			case ColourChangingMsg::LISTPAGED:
				SwitchTheDisplayList(Msg->ScopeDoc);
				if (GetCurrentDisplayList() != NULL &&	// cache exists, but not been filled in
					GetCurrentDisplayList()->GetCount() < 1)
					ReadNewColourList();
				break;

			case ColourChangingMsg::COLOURUPDATED:
			case ColourChangingMsg::COLOURUPDATEDINVISIBLE:
				// If we weren't responsible for the colour being updated, then we must
				// re-read the colour list to ensure we've got it right.
				// **** This could be optimised by just re-sorting the displaylist...
//				if (!LockColourChange && GetCurrentDisplayList() != NULL)
//					ReadNewColourList();
				ForceRedrawOfList();		// Cause list to be resorted & redisplayed
				break;

#if FALSE
/*
	// We NO LONGER process this message: The Colour Manager calls our special
	// ColourGallery::ColourListHasChanged() function in order to do this, so that we
	// have updated our DisplayList before others (the colour bar) start making
	// requests on it in response to the LISTUPDAETD message.
			case ColourChangingMsg::LISTUPDATED:
				UnshadeGallery();
				if (GetCurrentDisplayList() != NULL)
					ReadNewColourList();	// Re-create the display list from the new list
				break;
*/
#endif

			case ColourChangingMsg::LISTDESELECTED:
				// Gallery should grey out - there is no current colour list
				ShadeGallery();
				break;
		}
		// Drop through so the Gallery base class gets to see this message too
	}
	else if (MESSAGE_IS_A(Message, SelChangingMsg))
	{
		SelChangingMsg *Msg = (SelChangingMsg *) Message;
		switch ( Msg->State )
		{
			case SelChangingMsg::SelectionState::COLOURATTCHANGED:
			case SelChangingMsg::SelectionState::SELECTIONCHANGED:
			case SelChangingMsg::SelectionState::NODECHANGED:
				if (IsVisible() && !IsShaded)	// If we're open & active, set listbox selection
					SetSelectionFromDocument();
				break;
		}
		// Drop through so the Gallery base class gets to see this message too
	}
	else if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg *Msg = (DocChangingMsg *) Message;

		switch (Msg->State)
		{
			case DocChangingMsg::BORN:
				if (Msg->pChangingDoc != NULL)
					CreateCachedDisplayList(Msg->pChangingDoc);
				break;
		}
		// Drop through so the Gallery base class gets to see this message too
	}
	

	return(Gallery::Message(Message));
}    



/***********************************************************************************************

>	BOOL ColourGallery::DoRedrawAnItem(ListItem *Item, BOOL Large, UINT32 DisplayMode);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Inputs:		Item - The item to be redrawn
				Large - TRUE for large mode, FALSE for small mode redraw
				DisplayMode - derived gallery defined display mode index which
				determines how the item is redrawn (icon only, name only, full info)

	Returns:	TRUE if it redraws successfully
	Purpose:	Redraws a display list item. MUST be overridden by derived class
				(it would be pure virtual but for DYNCREATE)

***********************************************************************************************/

BOOL ColourGallery::DoRedrawAnItem(ListItem *Item, BOOL Large, UINT32 DisplayMode)
{
	PaintText(0, 0, ((IndexedColour *)Item)->GetName());
	return(TRUE);
}



/***********************************************************************************************

>	BOOL ColourGallery::PrepareToDelete(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Purpose:	Scans the current Gallery selection. Possibly asks the user for
				confirmation about the delete. Possibly adjusts the selection as
				appropriate. Then returns a value to indicate whether the remaining
				selection should be deleted.
	SeeAlso:	ColourGallery::DoDeleteSelection

***********************************************************************************************/
 
BOOL ColourGallery::PrepareToDelete(void)
{
	SequenceItem *Ptr = FindFirstSelected();
	DWORD ItemsInUse = 0;
	DWORD TotalItems = 0;

	// Determine how many items are selected, and how many of these are in use
	while (Ptr != NULL)
	{
		if ( ((IndexedColour *)FindRealItem(Ptr))->IsInUse() )
			ItemsInUse++;
		TotalItems++;
		Ptr = FindNextSelected(Ptr);
	}

	// If some are in use, determine if the user really wants to delete them
	INT32 ButtonPressed = 2;
	if (ItemsInUse != 0)
	{
		ButtonPressed = AskQuestion(_R(IDS_COLGAL_COLINUSE),
									_R(IDS_COLGAL_KILL), _R(IDS_COLGAL_NOKILL), _R(IDS_CANCEL), 0,
									2, 3);	// NOKILL=default, CANCEL=cancel

		if (ButtonPressed == 3)		// Delete was cancelled - return FALSE
			return(FALSE);
	}

	if (ButtonPressed == 1)		// Force-delete all items, so go ahead and delete
		return(TRUE);

	if (ButtonPressed == 2 && TotalItems - ItemsInUse <= 0)
		return(FALSE);			// No items left to delete (all in use) so abort

	// Remove all in-use items from the selection so we only delete 'free' colours
	Ptr = FindFirstSelected();
	while (Ptr != NULL)
	{
		if ( ((IndexedColour *) FindRealItem(Ptr))->IsInUse() )
			SetDisplayItemSelState(Ptr, FALSE);	// Item won't be deleted- deselect it
		//else
		//	the item will be deleted - leave it selected

		Ptr = FindNextSelected(Ptr);
	}

	return(TRUE);
}



/***********************************************************************************************

>	BOOL ColourGallery::DoDeleteSelection(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Returns:	FALSE if it couldn't delete the selection. (This should, if at all possible
				leave the entire selection undeleted in this case)

	Purpose:	FORCIBLY deletes the current gallery selection. By the time this function
				is called, PrepareToDelete has made sure that it is safe to continue.
				This must simply invoke a forced delete upon the selection.
				Deletion of colours occurs by hiding (with undo)

	SeeAlso:	ColourGallery::PrepareToDelete; ColourManager::HideColours

***********************************************************************************************/
 
BOOL ColourGallery::DoDeleteSelection(void)
{
	DWORD TotalItems = GetSelectedItemCount();

	IndexedColour **KillList = new IndexedColourPtr[TotalItems+1];
	if (KillList == NULL)
		return(FALSE);
	
	DWORD KillIndex = 0;
	SequenceItem *Ptr = FindFirstSelected();
	while (Ptr != NULL)
	{
		KillList[KillIndex++] = (IndexedColour *) FindRealItem(Ptr);

		Ptr = FindNextSelected(Ptr);
	}
	
	KillList[KillIndex] = NULL;		// NULL terminate the list

	// Delete (hide, with undo actually) the given colours
	ColourManager::HideColours(ColourManager::GetColourList(), KillList, TRUE);

	// HideColours has made a copy of this list for itself, so we are responsible
	// for deleting our original list
	delete [] KillList;

	return(TRUE);
}



/***********************************************************************************************

>	ListItem *ColourGallery::DoCreateNewItem(ListItem *BasedOn)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		BasedOn - This is the item in the gallery display on which the
				new item should be based. If there is no Gallery selection, this parameter
				will be NULL, in which case you should read the sttributes of the current
				selection of object(s) in the document to determine what to base your
				new item on. If this fails, then you should create a new item with some
				kind of default settings.

	Returns:	NULL, or the newly created real item
	Purpose:	This function is meant to be overridden in derived classes
				It creates a new item and adds it to the Gallery DisplayList
				The item may be created by cloning/deriving/basing-on the
				selected item, depending on the type of Gallery concerned.

	Notes:		Generally, a gallery should do the following:
				If BasedOn != NULL, then the gallery is supplying you with an item which
				should be copied to make the item.
				If BasedOn == NULL, then you should try to find something useful to copy-
				usually, this means something from the attributes of the selection or
				whatever.
				If all of this fails, then you should create an item from some default
				template (e.g. the ColourGallery creates a mid-grey HSV colour)

				The ColourGallery also has another set of options: The source colour can
				either be copied, or (if it is an IndexedColour) used to create a tint
				or linked colour. This is determined by asking the user if necessary.

***********************************************************************************************/

ListItem *ColourGallery::DoCreateNewItem(ListItem *BasedOn)
{
	ColourList *ColList = ColourManager::GetColourList();

	if (ColList == NULL)
		return(NULL);

	IndexedColour *NewCol;
	NewCol = ColourManager::GenerateNewNamedColour(ColList, (IndexedColour *) BasedOn);

	if (NewCol != NULL)
	{
		// Bring up the colour editor on the new colour
		DoSpecialClick(NewCol);

		// And make the new item the selected one in the list
		DeselectAllItems();
		SetDisplayItemSelState(FindSequenceItem(NewCol), TRUE);
		ForceRedrawOfList();
	}

	// Return NULL or the new item to let Gallery know what happened
	return(NewCol);
}



/***********************************************************************************************

>	void ColourGallery::DoApplyItem(ListItem *ItemToApply)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		ItemToApply - The item to apply
	Purpose:	This function is meant to be overridden in derived classes
				It applies the selected item to the document in whatever way
				is appropriate to its particular data item type. 

***********************************************************************************************/

void ColourGallery::DoApplyItem(ListItem *ItemToApply)
{
	if (ItemToApply == NULL)		// Check for stupidity just in case
		return;

	// Fill colour selected	so create a fill colour attribute
	NodeAttribute *Attrib = new AttrColourChange;
	if (Attrib == NULL)
		return;

	DocColour Ref = *((AttrColourChange *)Attrib)->GetStartColour();
	Ref.MakeRefToIndexedColour((IndexedColour *) ItemToApply);

	((AttrColourChange *)Attrib)->SetStartColour(&Ref);

	// AttributeSelected knows what to do with a selected attribute
	AttributeManager::AttributeSelected(NULL, Attrib);
}



/***********************************************************************************************

>	BOOL ColourGallery::DoRedefineItem(ListItem *ItemToRedefine)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94

	Inputs:		ItemToRedefine - The item which should be redefined

	Returns:	TRUE if the item was changed (or if you're not sure).
				FALSE if the item definitely was not changed (to save us
				re-sorting and drawing the displayed list)

	Purpose:	This function is meant to be overridden in derived classes
				It allows the user to 'redefine' the item in some way; in the case
				of the colour gallery, this redefines the colour from the selected
				object on the page

***********************************************************************************************/

BOOL ColourGallery::DoRedefineItem(ListItem *ItemToRedefine)
{
	// Bring up the colour editor on the new colour
	if (ItemToRedefine != NULL)
		DoSpecialClick(ItemToRedefine);

	return(TRUE);

#if FALSE
	ColourList *ColList = ColourManager::GetColourList();
	if (ColList == NULL)
		return(FALSE);

	DocColour *SourceColour = NULL;
	if (ColourManager::GetCurrentLineAndFillColours(NULL, &SourceColour))
		SourceColour = NULL;				// Sourcecolour returned was current attr - don't use

	if (SourceColour == NULL)
	{
		// Nothing to copy to redefine the colour. Report this to the user
		InformError(_R(IDE_COLGAL_NOSOURCE), _R(IDS_OK));
		return(FALSE);
	}

	// We have a colour to copy...
	INT32 Result;
	IndexedColour *ParentColour = SourceColour->FindParentIndexedColour();
	if (ParentColour == NULL)
	{
		Result = AskQuestion(_R(IDS_COLGAL_QREDEFINE), _R(IDS_COLGAL_YREDEFINE), _R(IDS_CANCEL));

		if (Result == 1)		// The user said to go ahead...
		{
			// Source is an immediate colour - generate an IndexedColour from its definition
			ColourGeneric Bob;
			SourceColour->GetSourceColour(&Bob);
			*((IndexedColour *)ItemToRedefine) = IndexedColour(SourceColour->GetColourModel(), &Bob);
			return(TRUE);
		}

		return(FALSE);			// The user cancelled
	}

	if (ParentColour == (IndexedColour *) ItemToRedefine)
	{
		InformError(_R(IDE_COLGAL_SAMEITEM));
		return(FALSE);
	}

	Result = 3;			// Default to making a copy of the colour

	{
		IndexedColour *NewCol = (IndexedColour *)ItemToRedefine;
		
		*NewCol = *ParentColour;
		if (ParentColour->IsNamed())
		{
			String_128 NewName;		// Accomodate overflow- SetName clips this to String_64
			NewName.MakeMsg(_R(IDS_COPYOFCOLOUR), (TCHAR *) (*ParentColour->GetName()));
			((IndexedColour *)ItemToRedefine)->SetName(NewName);

			// If the parent was named, then remember it as the 'parent' colour, even
			// if it was not tint/linked. This will allow the colour editor to default
			// to linking to a useful colour if tint/link is turned on at a later date.
			NewCol->SetLinkedParent(ParentColour, NewCol->GetType());
		}
	}

	if (Result != 0)				// Finally, inform the world if this item has changed
		ColourManager::ColourHasChanged(GetGalleryDocument(),
										ColList, (IndexedColour *) ItemToRedefine);

	return(Result != 0);			// if Cancelled, return FALSE (no change), else TRUE

#endif


}



/***********************************************************************************************

>	void ColourGallery::DoSpecialClick(ListItem *ClickedItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/94
	Inputs:		ClickedItem - The item which has been special clicked

	Purpose:	This function is meant to be overridden in derived classes
				It handles a special (alt-) click on a list item. This is generally used
				as a short-cut to bring up an editor on the clicked item, but the exact
				interpretation is left to the derived gallery to decide.

				The ColourGallery pops up the colour editor on the given colour.

	Notes:		This is called internally to EDIT a colour. It is thus vital that you
				do not change this function without also changing the calling code in this
				file (see ColourGallery::DoCreateNewItem())

***********************************************************************************************/

void ColourGallery::DoSpecialClick(ListItem *ClickedItem)
{
	ColourList *ColList = ColourManager::GetColourList();
	if (ColList == NULL)
		return;

LockColourChange = TRUE;

	ColourPicker ColPicker;
	ColPicker.EditColour(ColList, (IndexedColour *) ClickedItem);

LockColourChange = FALSE;
}



/***********************************************************************************************

>	virtual StringBase *ColourGallery::GetItemText(ListItem *TheItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Returns:	NULL, or a pointer to the item's text

	Purpose:	STAGE 1 - SIMPLE GALLERY IMPLEMENTATION

				This method is overridden by derived galleries to provide text
				for each displayed item. This will not be used once user-redraw
				has been implemented, but is used for the time being.

***********************************************************************************************/

StringBase *ColourGallery::GetItemText(ListItem *TheItem)
{
	if (TheItem != NULL)
		return( ((IndexedColour *)TheItem)->GetName() );

	return(NULL);
}



/***********************************************************************************************

>	virtual void ColourGallery::SortModeHasBeenChanged(BOOL SetDerivedSortsToo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/7/94
	Inputs:		SetDerivedSortsToo - If FALSE, only the base gallery class' Display List
				sort mode should be changed. If TRUE, then any other things that can
				depend upon this sort mode should also be updated (e.g. ColourGallery
				will update the colour bar sortmode if this is TRUE).

	Purpose:	Called when the GalleryDisplayDlg has chnaged the gallery DisplayList
				SortMode array to a new sort. The gallery will cause a re-sort and
				update of its list display.

	Notes:		The base class functionality MUST be provided in derived classes. Thus,
				you should call the base class method from any derived version.
				If SetDerivedSorts == FALSE, derived methods probably should do nothing.
				Otherwise, you should copy the DisplayList->SortMode array into any other
				SortModes you may have for external entities such as the colour bar, and
				cause them to update themselves for the new sort too.

***********************************************************************************************/
 

void ColourGallery::SortModeHasBeenChanged(BOOL SetDerivedSortsToo)
{
	ENSURE(GetCurrentDisplayList() != NULL,
			"Inconceivably, the DisplayList is NULL");

	// We *MUST* call the base class version of this function
	Gallery::SortModeHasBeenChanged(SetDerivedSortsToo);

	// This function should probaby do nothing if SetDerivedSortsToo == FALSE
	// It is called to say that the DisplayList->SortMode (array of sort keys
	// defining the displayed order of items) has been changed, so if you
	// depend on this sort mode for anything (other than the basic DisplayList
	// dependency, which the base class handles) you should update it now.


	if (SetDerivedSortsToo)
	{
		// Set the sort mode for the colour bar as well as the gallery
		// First, copy the SortMode array...

		ColGalDisplayList *CurDisplayList = (ColGalDisplayList *) GetCurrentDisplayList();

		if (CurDisplayList != NULL)
		{
			SortKey *ColourBarSortMode = CurDisplayList->GetColourBarSortMode();

			for (INT32 i = 0; i < MaxSortKeys; i++)
			{
				ColourBarSortMode[i].Info		= CurDisplayList->SortMode[i].Info;
				ColourBarSortMode[i].Reversed	= CurDisplayList->SortMode[i].Reversed;
			}
		}

		// Now, tell the Colour Bar to re-sort its display sequence and redraw...
//		CColourBar::SetSortMode(ColourBarSortFunction);
	}
}



/***********************************************************************************************

>	BOOL ColourGallery::DoSetDisplayModes(GalleryDisplayDlg *TheDialogue)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/7/94
	Inputs:		TheDialogue - The Gallery Display dialogue to call
				SetDisplayModeName for.

	Returns:	TRUE if the gallery has a choice of display modes, 
				FALSE if not (in which case the combo box list is greyed)

	Purpose:	This derived function is called by the base class when the
				GallerySortDlg is opened. It is a request from the gallery
				for you to fill in the list of available display modes. e.g
				you may provide icon-only, name-only, icon-and-name, full info
				modes for displaying the gallery items.

				For each mode you support, call SetDisplayModeName to add it to
				the list. Return TRUE when you're done.

				If you don't support anything other than one display mode, then you
				should just return FALSE (the base class default does this for you)

	SeeAlso:	GalleryDisplayDlg::SetDisplayModeName; ColourGallery::DoDisplayModeChange

***********************************************************************************************/

BOOL ColourGallery::DoSetDisplayModes(GalleryDisplayDlg *TheDialogue)
{
	// Example:
	// String_64 Text(_R(IDS_DISPLAY_NAMES_ONLY));
	// TheDialogue->SetDisplayModeName(Text);

	// ****

	return(FALSE);		// By default, the gallery will have no choice of modes
}



/***********************************************************************************************

>	void ColourGallery::DoDisplayModeChange(UINT32 NewDisplayMode)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/7/94
	Inputs:		NewDisplayMode - this number is an index [0..MaxDisplayModes] into
				the list of display names you gave in DoSetDisplayModes
	Returns:	-

	Purpose:	This function probably won't be needed, but it is called if the
				display mode is changed in the gallery display dialogue. You may
				want to cache new values or something before the next time your
				redraw code is called, in which case this is the place to do it.

	SeeAlso:	GalleryDisplayDlg::SetDisplayModeName; ColourGallery::DoSetDisplayModes

***********************************************************************************************/

void ColourGallery::DoDisplayModeChange(UINT32 NewDisplayMode)
{
	// By default, no action is taken for display mode changes
}



/***********************************************************************************************

>	void ColourGallery::DoShowMenu(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/7/94
	Inputs:		-
	Returns:	-

	Purpose:	Creates and shows your gallery-specific menu.
				This is in reaction to clicking the gallery menu button - you should
				pop up a menu under the pointer. How to do this is a complete mystery
				to me! I guess the menu items should be connected to actions via the
				standard Op system, so this is the only call the Gallery provides
				for you.

***********************************************************************************************/

void ColourGallery::DoShowMenu(void)
{
	// By default, no menu is shown
}



/***********************************************************************************************

>	virtual BOOL ColourGallery::SetAlsoButton(String_64 *TextToPutIn)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/7/94
	Inputs:		-
	Outputs:	TextToPutIn - Fill this in with the text you want on the
				'also' option button
	Returns:	TRUE if it wants an 'also' button, FALSE if not

	Purpose:	If your gallery sort mode can also be applied to other entities
				(e.g. the Colour Gallery display mode can also be applied to the
				colour bar) then return TRUE from this function after filling in
				the text parameter with a string (e.g. "Also apply this sort to
				the colour bar"

				If you want more control than this, you'll need to override the
				GalleryDisplayDlg class and get your hands dirty.

				By default (if you don't override this method) there is no button

	SeeAlso:	ColourGallery::SortModeHasBeenChanged

***********************************************************************************************/

BOOL ColourGallery::SetAlsoButton(String_64 *TextToPutIn)
{
	TextToPutIn->MakeMsg(_R(IDS_COLGAL_ALSOCOLOURBAR));
	return(TRUE);
}






// ---------------------------------------------------------------------------------------------
// ColourGallery specific methods


/***********************************************************************************************

>	static ColGalDisplayList *ColourGallery::FindColourDisplayList(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/8/94

	Returns:	The current colour gallery DisplayList pointer, or NULL

	Purpose:	To find the ColGalDisplayList list in which the Colour Gallery
				stores its displayed IndexedColour items. (Used by the colour bar
				to redraw itself with)

***********************************************************************************************/
 
ColGalDisplayList *ColourGallery::FindColourDisplayList(void)
{
	if (TheColourGallery == NULL)
		return(NULL);

	// This may be called by things like the colour bar on LISTUPDATED colour change messages
	// Unfortunately, they may be called before we are, so we have to assume that the
	// current display list could be invalid now, and search for the right one for the
	// selected doc. This may be slow, but the message system doesn't let us grab the
	// message first, so we have to bodge our way around it...
	TheColourGallery->SwitchTheDisplayList(NULL);
	if (TheColourGallery->GetCurrentDisplayList() != NULL &&	// cache exists, but not been filled in
		TheColourGallery->GetCurrentDisplayList()->GetCount() < 1)
		TheColourGallery->ReadNewColourList();

	return((ColGalDisplayList *) TheColourGallery->GetCurrentDisplayList());
}



/***********************************************************************************************

>	static ColourGallery *ColourGallery::FindTheGallery(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94

	Returns:	The current colour gallery (or NULL)

	Purpose:	To find the current Colour Gallery object
				Used by the colour bar to use Gallery display list traversal routines

***********************************************************************************************/
 
ColourGallery *ColourGallery::FindTheGallery(void)
{
	return(TheColourGallery);
}



/***********************************************************************************************

>	void ColourGallery::CreateCachedDisplayList(Document *ScopeDoc = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Inputs:		ScopeDoc - The document to cache a displaylist for.
	Purpose:	Creates (if necessary) a new DisplayList cache for the given Doc.
				If this doc is the SelectedDoc, it also sets the current gallery
				Display List to the (new) cached list for this doc

***********************************************************************************************/
 
void ColourGallery::CreateCachedDisplayList(Document *ScopeDoc)
{
	if (ScopeDoc != NULL)
	{
		OrderedList *NewDisplayList = FindDisplayListInDoc(ScopeDoc);

		if (NewDisplayList == NULL)		// Not previously cached a DList in this doc
		{
			NewDisplayList = new ColGalDisplayList;

			if (NewDisplayList != NULL)
			{
				((ColGalDisplayList *) NewDisplayList)->Init(ScopeDoc); // Init list
				AddDisplayListToDoc(ScopeDoc, NewDisplayList);			// Cache it in the doc

				if (ScopeDoc == Document::GetSelected())
					SelectDisplayList(NewDisplayList);
			}
		}
	}
}



/***********************************************************************************************

>	void ColourGallery::SwitchTheDisplayList(Document *ScopeDoc = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/8/94

	Inputs:		ScopeDoc - The document to switch to. NULL if you want to just use
				the GalleryDocument (As we swap lists on ColourChangingMsgS, there
				may be a brief discrepancy between GalleryDoc and SelectedDoc while
				the message is being processed)

	Returns:	TRUE if the Display List pointer changed as a result of this call

	Purpose:	Switches the DisplayList to the appropriate list for the current 
				GalleryDocument.
				
	Notes:		If a cached list is not available in the document, a new list is
				created and the gallery is redrawn.

				If a cached list is available, the DisplayList is swapped to it,
				and the gallery will be redrawn.

***********************************************************************************************/

BOOL ColourGallery::SwitchTheDisplayList(Document *ScopeDoc)
{
	if (ScopeDoc == NULL)
		ScopeDoc = GetGalleryDocument();

	OrderedList *OldDisplayList = GetCurrentDisplayList();

	OrderedList *NewDisplayList = NULL;
	if (ScopeDoc != NULL)
		NewDisplayList = FindDisplayListInDoc(ScopeDoc);

	if (NewDisplayList == NULL && ScopeDoc != NULL)		// No cached DisplayList for this doc
	{
		NewDisplayList = new ColGalDisplayList;

		if (NewDisplayList != NULL)
		{
			((ColGalDisplayList *) NewDisplayList)->Init(ScopeDoc);
															// Init lists sort modes etc
			SelectDisplayList(NewDisplayList);				// Swap to the new list
			AddDisplayListToDoc(ScopeDoc, NewDisplayList);	// Cache list in its doc
			ReadNewColourList();	   						// Fill in the display colours
		}
	}
	else
	{
		if (OldDisplayList != NewDisplayList)	// If list changed, redraw the gallery
		{
			UnshadeGallery();
			SelectDisplayList(NewDisplayList);
			ForceRedrawOfList();
		}
	}			

	return(OldDisplayList != NewDisplayList);
}



/***********************************************************************************************

>	void ColourGallery::ReadNewColourList(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Purpose:	Reads the (new) ColourList for the (new) Document, causing the 
				gallery to display it. Called when the ColourList either changes
				or is paged. (Inefficient in that it recalculates the entire
				display list - this will be improved upon later, with a cached
				DisplayList in each document)

***********************************************************************************************/
 
void ColourGallery::ReadNewColourList(void)
{
	ColourList *Colours = ColourManager::GetColourList();

	RemoveAllItemsFromDisplay();

	if (Colours == NULL)
	{
		ForceRedrawOfList();
		return;
	}

	IndexedColour *Ptr = (IndexedColour *) Colours->GetHead();
	while (Ptr != NULL)
	{
		if (!Ptr->IsDeleted())
			AddItemToDisplay(Ptr);

		Ptr = (IndexedColour *) Colours->GetNext(Ptr);
	}

	SetSelectionFromDocument();
	ForceRedrawOfList();
}



/********************************************************************************************

>	static INT32 ColourGallery::ColourBarSortFunction(OrderedList *Parent,
												ListItem *Item1, ListItem *Item2)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/94 (Based on Gallery::SortFunction of 12/7/94)
	Inputs:		Parent - the OrderedList which the items are in
				Item1, Item2 - The two items to be compared
	Outputs:	-
	Returns:	A Negative/Zero/Positive result to indicate the result of comparing
				the two items.
	Purpose:	Special multi-key list comparator function.
				This provides the sort mode which has been set by the ColourGallery
				class for the ColourBar, using the ColourBarSortKeys held by the
				ColourGallery
	Scope:		private
	SeeAlso:	OrderedList; keyword ListComparator; Gallery::SortFunction

********************************************************************************************/

INT32 ColourGallery::ColourBarSortFunction(OrderedList *Parent,
											ListItem *Item1, ListItem *Item2)
{
	ListComparator SortFn;
	INT32 result = 0;
	INT32 i = 0;

	ENSURE(Parent != NULL && Item1 != NULL && Item2 != NULL, 
			"NULL parameter(s) passed to Gallery SortFunction!");

	SortKey *ColourBarSortMode = ((ColGalDisplayList *)Parent)->GetColourBarSortMode();

	while (i < MaxSortKeys && result == 0)
	{
		if (ColourBarSortMode[i].Info != NULL)
		{
			SortFn = ColourBarSortMode[i].Info->GetSortFn();

			if (ColourBarSortMode[i].Reversed)
				result = SortFn(Parent, Item2, Item1);
			else
				result = SortFn(Parent, Item1, Item2);
		}

		i++;
	}
	return(result);
}





//-----------------------------------------------------------------------------------------------

// OpDisplayColourGallery - the operation that is used to display the colours gallery

/********************************************************************************************

>	BOOL OpDisplayColourGallery::Init()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpDisplayColourGallery initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpDisplayColourGallery::Init()
{
	return (RegisterOpDescriptor(
	 							0,
	 							_R(IDS_DISPLAY_COLOUR_GALLERY),
								CC_RUNTIME_CLASS(OpDisplayColourGallery),
	 							OPTOKEN_DISPLAYCOLOURGALLERY,
	 							OpDisplayColourGallery::GetState,
	 							0,									// Help ID
	 							_R(IDBBL_DISPLAY_COLOUR_GALLERY),	// bubble help
	 							0,									// bitmap ID		
								0,									// control ID
								SYSTEMBAR_ILLEGAL,					// Bar ID
								TRUE,								// Recieve system messages
								FALSE,								// Smart duplicate operation
								TRUE,								// Clean operation
								NULL,								// No vertical counterpart
								0,									// String for one copy only error
								0,									// Auto state flags
								TRUE								// Tickable
								));
}               
    
/********************************************************************************************

>	OpState	OpDisplayColourGallery::GetState(String_256*, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpDisplayColourGallery operation
	Purpose:	For finding the OpDisplayColourGallery's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpDisplayColourGallery::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;  

	// If the gallery is currenty open, then the menu item should be ticked
	String_32 Name(_R(IDS_K_COLGAL_GALLNAME));
	DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(Name);

	if (pDialogBarOp != NULL)
	{
		if (pDialogBarOp->GetRuntimeClass() == CC_RUNTIME_CLASS(ColourSGallery))
			OpSt.Ticked = pDialogBarOp->IsVisible();
	}

	// If there are no open documents, you can't toggle the gallery
	OpSt.Greyed = (Document::GetSelected() == NULL);

 	return(OpSt);   
}



/********************************************************************************************

>	void OpDisplayColourGallery::Do(OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Displays the colours gallery
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpDisplayColourGallery::Do(OpDescriptor*)
{
	String_32 Name(_R(IDS_K_COLGAL_GALLNAME));
	DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(Name);

	if (pDialogBarOp != NULL)
	{
		if (pDialogBarOp->GetRuntimeClass() == CC_RUNTIME_CLASS(ColourSGallery))
		{
			// Toggle the visibility of the gallery window
			pDialogBarOp->SetVisibility( !pDialogBarOp->IsVisible() );
		}
#if _DEBUG
		else
			ERROR3("The Colour Gallery isn't a ColourSGallery! Woss goin' on?");
#endif

		SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYCOLOURGALLERY, pDialogBarOp->IsVisible());
	}

	ERROR3IF(pDialogBarOp == NULL,"Couldn't find the colour gallery bar");

	End();
}

#endif

