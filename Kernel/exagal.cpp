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
// An example gallery - implementation

/*
*/


#include "camtypes.h"

//#include "alex.h"
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "exagal.h"
//#include "jason.h"			// For JCWTRACE
#include "oilfiles.h"
//#include "resource.h"

// Jason's personal trace variant - only spews debug for him
#define JCWTRACE if(IsUserName("Jason")) TRACE

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");



#if FALSE
	// Galleries (as opposed to supergalleries) are now defunct


CC_IMPLEMENT_DYNCREATE(ExampleGallery, Gallery)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW



/********************************************************************************************

>	ExampleGallery::ExampleGallery() : Gallery()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/7/94
	Purpose:	Example Gallery constructor

********************************************************************************************/

ExampleGallery::ExampleGallery() : Gallery()
{
JCWTRACE( _T("\nExample gallery constructed\n"));
}



/********************************************************************************************

>	virtual BOOL ExampleGallery::PreCreate(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Returns:	TRUE if it is successful; FALSE to abort creation of the window
				FALSE => ERROR1

	Purpose:	Example Gallery PreCreate handler

	Notes:		This method is called whenever the gallery window is about to be
				created.
				At this point, the gallery should set up data structures - it is
				*essential* that DisplayList is set up by the end of this method.
				(As it is used in the creation of the window)
				Note that you cannot AddItemToDisplay etc in this method, as 
				although the display list exists, the related window controls
				do not yet exist, and ENSURE failures will result.

********************************************************************************************/

BOOL ExampleGallery::PreCreate(void)
{
	if (GetCurrentDisplayList() == NULL)
	{
		OrderedList *NewDisplayList = new OrderedList;
		SelectDisplayList(NewDisplayList);
	}

	return(GetCurrentDisplayList() != NULL);
}



/********************************************************************************************

>	virtual BOOL ExampleGallery::PostCreate(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Returns:	TRUE if it is successful; FALSE to abort creation of the window

	Purpose:	Example Gallery PostCreate handler

	Notes:		This method is called whenever the gallery window is created, AFTER
				the window has been brought into existence.
				At this point (and henceforth), the gallery can add items to]
				the DisplayList, etc. as the display controls now exist.

********************************************************************************************/

BOOL ExampleGallery::PostCreate(void)
{
	if (GetCurrentDisplayList() == NULL)
		return(FALSE);

	// VERY Naughty! But should be safe as a quick test
	for (INT32 i = 1; i <= 20; i++)
		AddItemToDisplay((ListItem *)i);

	return(TRUE);
}



/********************************************************************************************

>	MsgResult ExampleGallery::Message(Msg* Message) 

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/7/94
	Inputs:		Message- The message to handle

	Purpose:	Example Gallery dialog message handler

	Notes:		This should generally only process GalleryMsg messages and things
				like DocChagingMsg messages which might affect what list you want
				your gallery to display. All unprocessed messages must be passed
				down to the base class Gallery::Message method

********************************************************************************************/

MsgResult ExampleGallery::Message(Msg* Message) 
{

	// Note that all Dialogue messages should have been handled by the base class
	// message handler. Unless we're being antisocial, all we should have to
	// process here are the GalleryMsg messages.

JCWTRACE( _T("\nExample gallery message handler\n"));

#if FALSE
	if (MESSAGE_IS_A(Message, GalleryMsg))
	{
		GalleryMsg *Msg = (GalleryMsg *) Message;

		switch(Msg->State)
		{
		}

		return(OK);		// Don't try to go recursive with messages from the base class!
	}
#endif

	return(Gallery::Message(Message));
}    



/***********************************************************************************************

>	BOOL ExampleGallery::DoRedrawAnItem(ListItem *Item, BOOL Large, UINT32 DisplayMode);

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

BOOL ExampleGallery::DoRedrawAnItem(ListItem *Item, BOOL Large, UINT32 DisplayMode)
{
	// Draws a listitem displaying a "Example"!
	return(TRUE);
}



/***********************************************************************************************

>	BOOL ExampleGallery::PrepareToDelete(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Purpose:	Scans the current Gallery selection. Possibly asks the user for
				confirmation about the delete. Possibly adjusts the selection as
				appropriate. Then returns a value to indicate whether the remaining
				selection should be deleted.
	SeeAlso:	ExampleGallery::DoDeleteSelection

***********************************************************************************************/
 
BOOL ExampleGallery::PrepareToDelete(void)
{
	// Scans the selection, changing it if necessary, and returns TRUE if
	// it is OK to delete the resulting selection. The gallery base class 
	// happily handles things like the selection now being empty.

	// eg
	//	SequenceItem *Jobby = FindFirstSelected();
	//	while (Jobby)
	//	{
	//		// Don't allow deletion if any of the items in the selection can't be deleted
	//      if ( ((MyItemType *) FindRealItem(Jobby))->CantBeDeleted() )
	//			return(FALSE);
	//		Jobby = FindNextSelected(Jobby);
	//	}

	return(TRUE);
}



/***********************************************************************************************

>	BOOL ExampleGallery::DoDeleteSelection(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Purpose:	FORCIBLY deletes the current gallery selection. B y the time this function
				is called, PrepareToDelete has made sure that it is safe to continue.
				This must simply invoke a forced delete upon the selection.
	SeeAlso:	ExampleGallery::PrepareToDelete

***********************************************************************************************/
 
BOOL ExampleGallery::DoDeleteSelection(void)
{
	// Force-deletes anything still remaining in the selection
	// returns TRUE if successful

	// eg
	//	SequenceItem *Jobby = FindFirstSelected();
	//	while (Jobby)
	//	{
	//		MyListOfExamples->DeleteItem(FindRealItem(Jobby));
	//		Jobby = FindNextSelected(Jobby);
	//	}

	return(TRUE);
}



/***********************************************************************************************

>	ListItem *ExampleGallery::DoCreateNewItem(ListItem *BasedOn)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		BasedOn - The real item on which the new item should be based
				(this should copy/clone the given item). If BasedOn==NULL then
				you should read the attributes for the current document selection
				and clone the relevant attribute if one is available. If all else
				fails, create a new item based on some default settings.

	Returns:	NULL if it fails to create a new item, else a pointer to the new
				item

	Purpose:	This function is meant to be overridden in derived classes
				It creates a new item and adds it to the Gallery DisplayList
				The item may be created by cloning/deriving/basing-on the
				selected item, depending on the type of Gallery concerned.

	Notes:		Note that the Gallery base class handles addition of the item
				to the display list, and probably also redefinition of the item
				directly after creation.

***********************************************************************************************/

ListItem *ExampleGallery::DoCreateNewItem(ListItem *BasedOn)
{
	return(NULL);
}



/***********************************************************************************************

>	void ExampleGallery::DoApplyItem(ListItem *ItemToApply)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		ItemToApply - The real item which should be applied to the document
				selection in an appropriate manner. If there is no doc selection,
				then you should set the docs default creation attributes instead

	Purpose:	This function is meant to be overridden in derived classes
				It applies the selected item to the document in whatever way
				is appropriate to its particular data item type. 

***********************************************************************************************/

void ExampleGallery::DoApplyItem(ListItem *ItemToApply)
{
}



/***********************************************************************************************

>	BOOL ExampleGallery::DoRedefineItem(ListItem *ItemToRedefine)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94

	Inputs:		ItemToRedefine - The item which should be redefined.

	Returns:	TRUE if the item was changed (or if you're not sure)
				FALSE if the item definitely was not changed (to save us
				re-sorting and drawing the displayed list)

	Purpose:	This function is meant to be overridden in derived classes
				It allows the user to 'redefine' the item in some way, using
				an item editor (such as a colour picker, for example)

***********************************************************************************************/

BOOL ExampleGallery::DoRedefineItem(ListItem *ItemToRedefine)
{
	return(TRUE);
}




/***********************************************************************************************

>	virtual StringBase *ExampleGallery::GetItemText(ListItem *TheItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Returns:	NULL, or a pointer to the item's text

	Purpose:	STAGE 1 - SIMPLE GALLERY IMPLEMENTATION

				This method is overridden by derived galleries to provide text
				for each displayed item. This will not be used once user-redraw
				has been implemented, but is used for the time being.

***********************************************************************************************/

StringBase *ExampleGallery::GetItemText(ListItem *TheItem)
{
	return(NULL);
}



/***********************************************************************************************

>	virtual void ExampleGallery::SortModeHasBeenChanged(BOOL SetDerivedSortsToo)

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
 

void ExampleGallery::SortModeHasBeenChanged(BOOL SetDerivedSortsToo)
{
	// We *MUST* call the base class version of this function
	Gallery::SortModeHasBeenChanged(SetDerivedSortsToo);

	// This function should probaby do nothing if SetDerivedSortsToo == FALSE
	// It is called to say that the DisplayList->SortMode (array of sort keys
	// defining the displayed order of items) has been changed, so if you
	// depend on this sort mode for anything (other than the basic DisplayList
	// dependency, which the base class handles) you should update it now.


	if (SetDerivedSortsToo)
	{
		// See the colour gallery version of this for an example of use.
		// Probably, you'll want to copy the SortMode array into another similar 
		// array (probably stored in an extra member varaible in your derived
		// OrderedList Display List), and then cause the users of that sort mode
		// to update themselves to suit.
	}
}



/***********************************************************************************************

>	BOOL ExampleGallery::DoSetDisplayModes(GalleryDisplayDlg *TheDialogue)

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

	SeeAlso:	GalleryDisplayDlg::SetDisplayModeName; ExampleGallery::DoDisplayModeChange

***********************************************************************************************/

BOOL ExampleGallery::DoSetDisplayModes(GalleryDisplayDlg *TheDialogue)
{
	// Example:
	// String_64 Text(_R(IDS_DISPLAY_NAMES_ONLY));
	// TheDialogue->SetDisplayModeName(Text);

	// ****

	return(FALSE);		// By default, the gallery will have no choice of modes
}



/***********************************************************************************************

>	void ExampleGallery::DoDisplayModeChange(UINT32 NewDisplayMode)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/7/94
	Inputs:		NewDisplayMode - this number is an index [0..MaxDisplayModes] into
				the list of display names you gave in DoSetDisplayModes
	Returns:	-

	Purpose:	This function probably won't be needed, but it is called if the
				display mode is changed in the gallery display dialogue. You may
				want to cache new values or something before the next time your
				redraw code is called, in which case this is the place to do it.

	SeeAlso:	GalleryDisplayDlg::SetDisplayModeName; ExampleGallery::DoSetDisplayModes

***********************************************************************************************/

void ExampleGallery::DoDisplayModeChange(UINT32 NewDisplayMode)
{
	// By default, no action is taken for display mode changes
}



/***********************************************************************************************

>	void ExampleGallery::DoShowMenu(void)

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

void ExampleGallery::DoShowMenu(void)
{
	// By default, no menu is shown
}



/***********************************************************************************************

>	virtual BOOL ExampleGallery::SetAlsoButton(String_64 *TextToPutIn)

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

	SeeAlso:	ExampleGallery::SortModeHasBeenChanged

***********************************************************************************************/

BOOL ExampleGallery::SetAlsoButton(String_64 *TextToPutIn)
{
	return(FALSE);		// By default, Galleries do not support an 'also' button
}


#endif


