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
// This file implements the gallery classes

/*
*/

#include "camtypes.h"

#include "gallery.h"
#include "app.h"
#include "camelot.h"
//#include "galres.h"
//#include "galstr.h"
//#include "jason.h"
#include "mainfrm.h"
//#include "resource.h"	// _R(IDD_GALDISPLAYDLG)
//#include "barsdlgs.h"	

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision: 662 $");

#if FALSE
// **** GALLERIES ARE DEFUNCT - SEE THE NEW SUPERGALLERY CLASS INSTEAD ****

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_MEMDUMP(GalleryBodge, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(GalleryDisplayListPtr, ListItem)
CC_IMPLEMENT_MEMDUMP(GalleryDisplayItem, ListItem)
CC_IMPLEMENT_DYNCREATE(Gallery, DialogBarOp)
CC_IMPLEMENT_DYNCREATE(GalleryDisplayDlg, DialogOp)


// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW


// ---------------------------------------------------------------------------------------------
// GalleryDisplayListPtr methods 


/***********************************************************************************************

>	GalleryDisplayListPtr::GalleryDisplayListPtr(OrderedList *ListPtr)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/94
	Inputs:		ListPtr - The list to set this object to point at
	Purpose:	Constructor for a GalleryDisplayListPtr.
				Sets the ptr item to point at the given OrderedList

***********************************************************************************************/

GalleryDisplayListPtr::GalleryDisplayListPtr(OrderedList *ListPtr)
{
	ERROR3IF(ListPtr == NULL, "Attempt to create NULL GalleryDisplayListPtr!\n");
	DisplayList = ListPtr;
}



/***********************************************************************************************

>	GalleryDisplayListPtr::~GalleryDisplayListPtr()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/9/94
	Purpose:	Destructor for a GalleryDisplayListPtr.

***********************************************************************************************/

GalleryDisplayListPtr::~GalleryDisplayListPtr()
{
	if (DisplayList != NULL)
		delete DisplayList;
}




// ---------------------------------------------------------------------------------------------
// GalleryDisplayItem methods 


/***********************************************************************************************

>	GalleryDisplayItem::GalleryDisplayItem()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Purpose:	Default constructor for a GalleryDisplayItem. UNSUPPORTED. DO NOT USE.
				There is an overloaded constructor which should be used in preference

***********************************************************************************************/

GalleryDisplayItem::GalleryDisplayItem()
{
	// There is currently no way to set the RealItem value other than by the
	// overloaded constructor below.
	ERROR3("Unsupported call to GalleryDisplayItem constructor");
	Selected = FALSE;
	RealItem = NULL;
}



/***********************************************************************************************

>	GalleryDisplayItem::GalleryDisplayItem(ListItem *TheRealItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Inputs:		TheRealItem - The Real Item which the Gallery should display in its
				list box.
	Purpose:	Constructor for a GalleryDisplayItem. Initialises it to reference the
				given ListItem.
	Scope:		private: Use the Gallery interface, which manages the Gallery display
				list internally on your behalf. You should not need to know about the
				internal representation.
	SeeAlso:	Gallery

***********************************************************************************************/

GalleryDisplayItem::GalleryDisplayItem(ListItem *TheRealItem)
{
	ERROR3IF(TheRealItem == NULL, "GalleryDisplayItem initialised with NULL pointer");
	Selected = FALSE;
	RealItem = TheRealItem;
}








		





// ---------------------------------------------------------------------------------------------
// Gallery methods 


/********************************************************************************************

>	Gallery::Gallery(CCRuntimeClass *Class = CC_RUNTIME_CLASS(DialogBarOp)): DialogBarOp(Class) 
												 
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94 - Source derived from InformationBarOp
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Gallery constructor (Identical to DialogOp)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Gallery::Gallery(CCRuntimeClass *Class): DialogBarOp(Class) 
{
	GallerySize = CSize(0,0);

	DisplayList = NULL;
	DisplaySequence = NULL;	
	CurrentDisplayMode = 0;
	IsShaded = FALSE;
} 



/********************************************************************************************

>	Gallery::Gallery(String_32 &NewName,CCRuntimeClass *Class = CC_RUNTIME_CLASS(DialogBarOp): 
	DialogBarOp(NewName, Class) 

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94 - Source derived from InformationBarOp
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Gallery constructor (Identical to DialogOp)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Gallery::Gallery(String_32 &NewName,CCRuntimeClass *Class)
		: DialogBarOp(NewName, Class) 
{
	GallerySize = CSize(0,0);

	DisplayList = NULL;
	DisplaySequence = NULL;	
	CurrentDisplayMode = 0;
	IsShaded = FALSE;
} 



/********************************************************************************************

>	Gallery::~Gallery()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/94
	Purpose:	Gallery destructor.
	Errors:		This method checks that the DisplayList has been properly deallocated
				by derived classes. They should set DisplayList == NULL before they
				exit in order to avoid this ENSURE. If DisplayList is non-NULL, it will
				be deleted and an ENSURE generated.

********************************************************************************************/

Gallery::~Gallery()
{
	// Deselect any sort sequence we might be using
	SelectDisplayList(NULL);

	// Delete any Document DisplayList caches that exist for 'this' gallery
	List *DocList = &GetApplication()->Documents;
	Document *DocPtr = (Document *) DocList->GetHead();
	OrderedList *TheDisplayList;

	while (DocPtr != NULL)
	{
		TheDisplayList = FindDisplayListInDoc(DocPtr);

		if (TheDisplayList != NULL)
		{
			if (TheDisplayList == DisplayList)	// Remember we've deleted THE DisplayList
				DisplayList = NULL;

			DeleteDisplayListFromDoc(DocPtr);	// And delete the cached display list
		}

		DocPtr = (Document *) DocList->GetNext(DocPtr);
	}


	// Ensure that the DisplayList has been deallocated by someone
	if (DisplayList != NULL)
	{
		// Remind the programmer to add code to deallocate the list for himself
		ERROR3("Gallery DisplayList memory leak on delete! Hit Ignore to continue");

		// And clean up
		delete DisplayList;
		DisplayList = NULL;
	}
}


/********************************************************************************************

>	virtual MsgResult Gallery::Message(Msg* Message)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>/Jason
	Created:	22/6/94 - Source derived from InformationBarOp
	Inputs:		Message - The message to handle
	Outputs:	-
	Returns:	-
	Purpose:	A standard message handler, really. If you override this in a derived
				class, you must pass any unhandled event on to this base class method
				after handling the messages you are interested in.
	Errors:		-
	SeeAlso:	DialogOp::Message

********************************************************************************************/

MsgResult Gallery::Message(Msg* Message)
{
	// allow BarDeath messages through even for ops with no window
	if (IS_OUR_DIALOG_MSG(Message)||
		(((Message)->IsKindOf(CC_RUNTIME_CLASS(DialogMsg))) && 
				((DialogMsg*)Message)->DlgMsg == DIM_BAR_DEATH))
	{
		
		DialogMsg* Msg = (DialogMsg*)Message;
	
		DialogBarOp::Message( Msg);
		
		if(Msg->DlgMsg == DIM_BAR_DEATH)
			return OK;

		switch (Msg->DlgMsg)
		{
//			case DIM_COMMIT:						// OK clicked
//			case DIM_SOFT_COMMIT:					// OK "adjust clicked"
//				break;

			case DIM_CREATE:
				ForceRedrawOfList();				// Ensure list is redrawn with correct selection etc
				break;

			case DIM_CANCEL:
				{
				/*	KernelBarPos* 	pKernelBarPos;
					DockBarType 	Dock;
        			if (GetMainFrame()->GetBarPosInfo(WindowID,&Dock,&pKernelBarPos))
					{
						SetDockBarType(Dock);
						SetFloatingCPoint(CPoint(pKernelBarPos->x,pKernelBarPos->y));
					}
					// Report bar is closing to interested parties
					BROADCAST_TO_CLASS(BarMsg(BAR_CLOSE,this),DialogOp);
					WindowID = NULL;
					*/
				
					if (DisplaySequence != NULL )
					{
						if (DisplayList != NULL)
							DisplayList->DeselectOrder(SortFunction, FALSE);
						DisplaySequence = NULL;
					}
				
				}
				break;


			case DIM_SELECTION_CHANGED_COMMIT:		// List item double-clicked
				if (Msg->GadgetID == _R(IDC_GALLERY_LISTBOX))
				{
					// Double-click on an item makes it the only selected item, and
					// then applies it (in the same way as the 'apply' button)
					WORD Index; 
					GetValueIndex(_R(IDC_GALLERY_LISTBOX), &Index);
					if (Index >= 0)
					{
						RescanSelection();
						ApplyApplyItem();
					}
				}
				break;


			case DIM_SELECTION_CHANGED:				// List: Selection change
				if (Msg->GadgetID == _R(IDC_GALLERY_LISTBOX))
				{
					RescanSelection();
					DoSelectionChanged();
				}
				break;


			case DIM_SELECTION_CANCELED:			// List: All items deselected
				if (Msg->GadgetID == _R(IDC_GALLERY_LISTBOX))
				{
					// Update display list selection states to keep in step with listbox
					SequenceItem *Ptr = FindFirstSelected();
					while (Ptr != NULL)
					{
						ERROR3IF(Ptr->GetItem() == NULL, "NULL SequenceItem link in Gallery");
						((GalleryDisplayItem *) Ptr->GetItem())->SetSelected(FALSE);
						Ptr = FindNextSelected(Ptr);
					}
				}
				break;



		case DIM_LFT_BN_CLICKED:
			switch(Msg->GadgetID)
			{
				case _R(IDC_GALLERY_APPLY):
					ApplyApplyItem();
					break;

				case _R(IDC_GALLERY_NEW):
					ApplyNewItem();
					break;

				case _R(IDC_GALLERY_DELETE):
					ApplyDeleteSelection();
					break;

				case _R(IDC_GALLERY_REDEFINE):
					ApplyRedefineItem();
					break;

				case _R(IDC_GALLERY_UNDO):
					ApplyUndo();
					break;

				case _R(IDC_GALLERY_MENU):
					ApplyGalleryMenu();
					break;
			}
			break;

		}	// endswitch

		return (DLG_EAT_IF_HUNGRY(Msg)); 
	}	// endif (IS_OUR_DIALOG_MSG)


	if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg *Msg = (DocChangingMsg *) Message;

		switch (Msg->State)
		{
			case DocChangingMsg::ABOUTTODIE:
					// If a document dies, we should delete any cached display lists
					// from it before it goes. (The doc will clean up, but only as an
					// emergency measure if we failed to do so first)

				if (Msg->pChangingDoc != NULL)
					DeleteDisplayListFromDoc(Msg->pChangingDoc);
				break;

			case DocChangingMsg::SELCHANGED:
					// If the selected document is changing to NULL then there are no
					// documents open/available, so the gallery should shade itself
					// until a new document becomes available
				if (Msg->pNewDoc == NULL)
				{
					SelectDisplayList(NULL);
					ShadeGallery();
				}
				break;
		}
	}


	if (MESSAGE_IS_A(Message,DeathMsg)) 	// We must destroy the dialog if the application is dying
	{
		End();
		return(OK);
	}

	return(OK); 
}    



/********************************************************************************************

>	BOOL Gallery::Create(void)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>/Jason
	Created:	22/6/94 - Source derived from InformationBarOp
	Returns:	TRUE if the window was successfully created
				FALSE => ERROR2
	Purpose:	The Gallery Create method
				This method has been overridden to de-specialize the DialogBarOp. 

	Notes:		Before doing anything else, the PreCreate handler is called.
				The last thing done is to invoke the PostCreate method
				(these should be overridden by derived classes to do stuff)
	SeeAlso:	Gallery::PreCreate; Gallery::PostCreate

********************************************************************************************/

BOOL Gallery::Create(void)
{
	// Call derived class PreCreate handler to set up the DisplayList etc
	if (!PreCreate())
		return(FALSE);

	ERROR2IF(DisplayList == NULL, FALSE, "NULL DisplayList in Gallery::Create");

	// Must set up the SortFunction with defaults, and init DisplaySequence
	EnsureDisplaySequence();

	// Bar initialisation and creation
	SetDockBarType(Dock);
	SetSlot(Slot);
	SetOffset(Offset);
	SetFloatingCPoint(FloatPos);

	BOOL returnval = DialogOp::Create();

	// Call derived class PostCreate handler to fill the DisplayList etc
	if (returnval && !PostCreate())
		return(FALSE);

	return(returnval);
}



/***********************************************************************************************

>	ListItem *Gallery::FindRealItem(SequenceItem *DisplayItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Returns:	NULL, or the related listitem
				(NULL => ERROR2)

	Purpose:	Given a Gallery Display SequenceItem, find the real ListItem object
				which is being displayed by the gallery.
	SeeAlso:	Gallery::FindSequenceItem

***********************************************************************************************/

ListItem *Gallery::FindRealItem(SequenceItem *DisplayItem)
{
	ERROR2IF(DisplayItem == NULL, NULL, "Gallery::FindRealItem - source item is NULL");
	ERROR2IF(DisplayItem->GetItem() == NULL, NULL, "Gallery::FindRealItem - source item has NULL display item");
	return( ((GalleryDisplayItem *) DisplayItem->GetItem())->GetRealItem() );
}



/***********************************************************************************************

>	ListItem *Gallery::FindRealItem(GalleryDisplayItem *DisplayItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Returns:	NULL, or the real ListItem
				NULL => ERROR2

	Purpose:	Given a Gallery Display Item, find the real ListItem object
				which is being displayed by the gallery.
	SeeAlso:	Gallery::FindSequenceItem

***********************************************************************************************/
 
ListItem *Gallery::FindRealItem(GalleryDisplayItem *DisplayItem)
{
	ERROR2IF(DisplayItem == NULL, NULL, "Gallery::FindRealItem - source item is NULL");
	return( DisplayItem->GetRealItem() );
}



/***********************************************************************************************

>	SequenceItem *Gallery::FindSequenceItem(ListItem *TheItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Inputs:		TheItem - The ListItem for which you want to find the related display item
	Returns:	The Display SequenceItem by which the Gallery references TheItem,
				or NULL if it can't be found
	Purpose:	Given a real item, determines what item of the Gallerys Display list
				refers to it. You must use the SequenceItem when asking the Gallery
				to apply most actions to TheItem
	SeeAlso:	Gallery::FindRealItem

***********************************************************************************************/
 
SequenceItem *Gallery::FindSequenceItem(ListItem *TheItem)
{
	// Search for TheItem, and return a pointer to the SequenceItem which
	// references it, if any

	ERROR3IF(TheItem == NULL, "NULL parameter passed to Gallery::FindSequenceItem");
	EnsureDisplaySequence();
	if (DisplaySequence == NULL)
		return(NULL);

	SequenceItem *Ptr = (SequenceItem *) DisplaySequence->GetHead();
	while (Ptr != NULL)
	{
		if (FindRealItem(Ptr) == TheItem)
			break;

		Ptr = (SequenceItem *) DisplaySequence->GetNext(Ptr);
	}

	return(Ptr);
}



/***********************************************************************************************

>	DWORD Gallery::GetSelectedItemCount(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Returns:	The count (0 or positive integer)
	Purpose:	Returns the number of items which are currently selected within the
				gallery displayed listbox.
	SeeAlso:	Gallery::FindFirstSelected

***********************************************************************************************/
 
DWORD Gallery::GetSelectedItemCount(void)
{
	ERROR3IF(DisplaySequence == NULL, "Gallery DisplaySequence is NULL!");

	DWORD Count = 0;

	SequenceItem *Ptr = FindFirstSelected();
	while (Ptr != NULL)
	{
		Count++;
		Ptr = FindNextSelected(Ptr);
	}

	return(Count);
}



/***********************************************************************************************

>	SequenceItem *Gallery::FindFirstSelected(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Returns:	NULL if no selection, else a pointer to the first selected item
	Purpose:	Finds the first item which is selcted in the Gallery's listbox
	SeeAlso:	Gallery::FindLastSelected; Gallery::FindNextSelected;
				Gallery::GetSelectedItemCount; Gallery::FindRealItem

***********************************************************************************************/
 
SequenceItem *Gallery::FindFirstSelected(void)
{
	// **** This may be optimised in future if we actually cache the selstart
	// and selend items...

	EnsureDisplaySequence();

	SequenceItem *Ptr = FindFirstDisplayed();
	while (Ptr != NULL)
	{
		ERROR3IF(Ptr->GetItem() == NULL, "NULL SequenceItem link in Gallery");
		if ( ((GalleryDisplayItem *) Ptr->GetItem())->IsSelected() )
			return(Ptr);

		Ptr = FindNextDisplayed(Ptr);
	}

	return(NULL);
}



/***********************************************************************************************

>	SequenceItem *Gallery::FindLastSelected(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Returns:	NULL if no selection, else a pointer to the last selected item
	Purpose:	Finds the first item which is selcted in the Gallery's listbox
	SeeAlso:	Gallery::FindFirstSelected; Gallery::FindNextSelected;
				Gallery::GetSelectedItemCount; Gallery::FindRealItem

***********************************************************************************************/
 
SequenceItem *Gallery::FindLastSelected(void)
{
	// **** This may be optimised in future if we actually cache the selstart
	// and selend items...

	EnsureDisplaySequence();

	SequenceItem *Ptr = FindLastDisplayed();
	while (Ptr != NULL)
	{
		ERROR3IF(Ptr->GetItem() == NULL, "NULL SequenceItem link in Gallery");
		if ( ((GalleryDisplayItem *) Ptr->GetItem())->IsSelected() )
			return(Ptr);

		Ptr = FindPreviousDisplayed(Ptr);
	}

	return(NULL);
}



/***********************************************************************************************

>	SequenceItem *Gallery::FindNextSelected(SequenceItem *CurItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Returns:	NULL if no selection or have passed the last selected item, else a
				pointer to the selected item
	Purpose:	Given a selected Gallery display item, returns the next selected item
	SeeAlso:	Gallery::FindFistSelected; Gallery::FindLastSelected;
				Gallery::FindPreviousSelected

***********************************************************************************************/
 
SequenceItem *Gallery::FindNextSelected(SequenceItem *CurItem)
{
	ERROR3IF(DisplaySequence == NULL, "Gallery DisplaySequence is NULL!");

	CurItem = FindNextDisplayed(CurItem);
	while (CurItem != NULL)
	{
		ERROR3IF(CurItem->GetItem() == NULL, "NULL SequenceItem link in Gallery");
		if ( ((GalleryDisplayItem *) CurItem->GetItem())->IsSelected() )
			return(CurItem);

		CurItem = FindNextDisplayed(CurItem);
	}

	return(NULL);
}



/***********************************************************************************************

>	SequenceItem *Gallery::FindPreviousSelected(SequenceItem *CurItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Returns:	NULL if no selection or have passed the first selected item, else a
				pointer to the selected item
	Purpose:	Given a selected Gallery display item, returns the previous selected item
	SeeAlso:	Gallery::FindFistSelected; Gallery::FindLastSelected;
				Gallery::FindNextSelected

***********************************************************************************************/
 
SequenceItem *Gallery::FindPreviousSelected(SequenceItem *CurItem)
{
	ERROR3IF(DisplaySequence == NULL, "Gallery DisplaySequence is NULL!");

	CurItem = FindPreviousDisplayed(CurItem);
	while (CurItem != NULL)
	{
		ERROR3IF(CurItem->GetItem() == NULL, "NULL SequenceItem link in Gallery");
		if ( ((GalleryDisplayItem *) CurItem->GetItem())->IsSelected() )
			return(CurItem);

		CurItem = FindPreviousDisplayed(CurItem);
	}

	return(NULL);
}



/***********************************************************************************************

>	void Gallery::DeselectAllItems(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Purpose:	Clears the selection in the Gallery listbox so that there is no selection
	Notes:		May be called even when the gallery window is not visible
	SeeAlso:	Gallery::SetDisplayItemSelState

***********************************************************************************************/
 
void Gallery::DeselectAllItems(void)
{
	SequenceItem *Ptr = FindFirstSelected();

	while (Ptr != NULL)
	{
		ERROR3IF(Ptr->GetItem() == NULL, "NULL SequenceItem link in Gallery");
		((GalleryDisplayItem *) Ptr->GetItem())->SetSelected(FALSE);

		Ptr = FindNextSelected(Ptr);
	}

	// And if the window is open, clear the selection out of the list box
	if (IsVisible())
		SetBoolGadgetSelected(_R(IDC_GALLERY_LISTBOX), FALSE, -1);
}



/***********************************************************************************************

>	INT32 Gallery::CalculateListIndex(SequenceItem *TheItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		TheItem - Points to the item you want the list index of
	Returns:	-1 (item not in display!), or the index (0 .. NumDisplayItems-1)
	Purpose:	Determines what position (0 .. NumDisplayItems-1) the given item
				is in the gallery display ListBox.

***********************************************************************************************/

INT32 Gallery::CalculateListIndex(SequenceItem *TheItem)
{
	INT32 Index = 0;
	SequenceItem *Ptr = FindFirstDisplayed();

	while (Ptr != NULL)
	{
		if (Ptr == TheItem)
			return(Index);

		Index++;
		Ptr = FindNextDisplayed(Ptr);
	}

	return(-1);
}



/***********************************************************************************************

>	SequenceItem *Gallery::FindItemByIndex(INT32 ItemIndex)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/94
	Inputs:		ItemIndex - the index of the item you wish to find
	Returns:	NULL (Out of range index), or a pointer to the indexed item
	Purpose:	Given a position (0 .. NumDisplayItems-1), finds the given item
				in the gallery display ListBox.

***********************************************************************************************/

SequenceItem *Gallery::FindItemByIndex(INT32 ItemIndex)
{
	SequenceItem *Ptr = FindFirstDisplayed();
	INT32 Index = 0;

	while (Ptr != NULL)
	{
		if (Index == ItemIndex)		// Have found the item - return it
			return(Ptr);

		Ptr = FindNextDisplayed(Ptr);
		Index++;
	}

	return(NULL);		// Item not found
}



/***********************************************************************************************

>	void Gallery::SetDisplayItemSelState(SequenceItem *TheItem, BOOL IsSelected)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Inputs:		TheItem - Points to the item to select/deselect
				IsSelected - TRUE to select, FALSE to deselect, the item
	Purpose:	Sets the selection state of a given Gallery display item

	Notes:		May be called even when the gallery window is not open on screen
	Errors:		An ERROR2 is reported if a NULL item is passed in - the function
				returns without doing anything in this case.

	SeeAlso:	Gallery::DeselectAllItems

***********************************************************************************************/
 
void Gallery::SetDisplayItemSelState(SequenceItem *TheItem, BOOL IsSelected)
{
	if (TheItem == NULL)
	{
		ERROR2RAW("NULL GalleryDisplayItem passed to Gallery::SetDisplayItemSelState");
		return;
	}

	((GalleryDisplayItem *) TheItem->GetItem())->SetSelected(IsSelected);

	// And update the listbox selection state
	DWORD Index = CalculateListIndex(TheItem);
	if (Index >= 0 && IsVisible() && !IsShaded)
		SetBoolGadgetSelected(_R(IDC_GALLERY_LISTBOX), IsSelected, (INT32)Index);
}



/***********************************************************************************************

>	void Gallery::SetDisplayItemSelState(INT32 ItemIndex, BOOL IsSelected)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Inputs:		ItemIndex  - Indexes the item to select/deselect in the DisplayList
				IsSelected - TRUE to select, FALSE to deselect, the item

	Purpose:	Sets the selection state of a given Gallery display item

	Notes:		May be called even when the gallery window is not open on screen
	SeeAlso:	Gallery::DeselectAllItems

***********************************************************************************************/
 
void Gallery::SetDisplayItemSelState(INT32 ItemIndex, BOOL IsSelected)
{
	SequenceItem *TheItem = FindItemByIndex(ItemIndex);

	ERROR3IF(ItemIndex < 0 || TheItem == NULL,
				"Out of range item index passed to Gallery::SetDisplayItemSelState");
	if (TheItem != NULL)
	{
		((GalleryDisplayItem *) TheItem->GetItem())->SetSelected(IsSelected);

		// And update the listbox selection state
		if (IsVisible() && !IsShaded)
			SetBoolGadgetSelected(_R(IDC_GALLERY_LISTBOX), IsSelected, (INT32)ItemIndex);
	}
}



/***********************************************************************************************

>	DWORD Gallery::GetDisplayedItemCount(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Returns:	The number of items in the Gallery display (0 or positive number)
	Purpose:	Find out how many items are being displayed by the gallery

***********************************************************************************************/
 
DWORD Gallery::GetDisplayedItemCount(void)
{
	if (DisplayList == NULL)
		return(0);

	EnsureDisplaySequence();
	return(DisplaySequence->GetCount());
}



/***********************************************************************************************

>	BOOL Gallery::AddItemToDisplay(ListItem *TheItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Inputs:		TheItem - The real item which you wish to add to the Gallerys list box
	Returns:	TRUE if it succeeded
				FALSE => ERROR1
	Purpose:	Adds the given item to the displayed list of items. This creates a
				SequenceItem by which you will refer to the real item in the future.
				The item will be added to the Gallery display list (this will *not* affect
				the item's presence in any other lists - the Gallery uses a pointer to
				it from a display list). The exact position of the item i this list
				is determined by the current sort order applied to the gallery, so
				you cannot assume that it will appear anywhere in particular.
	SeeAlso:	Gallery::AddListToDisplay

***********************************************************************************************/
 
BOOL Gallery::AddItemToDisplay(ListItem *TheItem)
{
	// Adds the given item to the display list, returning FALSE if it fails

	ERROR3IF(TheItem == NULL,
		"Gallery::AddItemToDisplay would appreciate being passed a non-NULL item!");

	ERROR3IF(DisplayList == NULL, "Gallery DisplayList is NULL!");

	GalleryDisplayItem *NewDItem = new GalleryDisplayItem(TheItem);
	if (NewDItem == NULL || !DisplayList->AddItem(NewDItem))
		return(FALSE);

	return(TRUE);
}



/***********************************************************************************************

>	BOOL Gallery::AddListToDisplay(List *TheList)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Inputs:		TheList - The list of real items which you wish to add to the Gallerys list box
	Returns:	TRUE if it succeeded
	Purpose:	Adds the given items to the displayed list of items. This creates a
				SequenceItem by which you will refer to the real item in the future.
				The item will be added to the Gallery display list (this will *not* affect
				the item's presence in any other lists - the Gallery uses a pointer to
				it from a display list). The exact position of the items in this list
				is determined by the current sort order applied to the gallery, so
				you cannot assume that they will appear anywhere in particular.
	SeeAlso:	Gallery::AddItemToDisplay

***********************************************************************************************/
 
BOOL Gallery::AddListToDisplay(List *TheList)
{
	// Adds all items in the list to the display list
	// Returns TRUE for success

	ERROR3IF(TheList == NULL,
		"Gallery::AddListToDisplay would appreciate being passed a non-NULL list ptr!");

	ERROR3IF(DisplayList == NULL, "Gallery DisplayList is NULL!");

	ListItem *SourcePtr = TheList->GetHead();
	while (SourcePtr != NULL)
	{
		if (!AddItemToDisplay(SourcePtr))
			return(FALSE);

		SourcePtr = TheList->GetNext(SourcePtr);
	}

	return(TRUE);
}



/***********************************************************************************************

>	BOOL Gallery::RemoveItemFromDisplay(SequenceItem *TheSeqItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Inputs:		TheSeqItem - The display item to remove from the display
	Returns:	TRUE if it succeeds

	Purpose:	Removes the given item from the display. This does NOT affect the
				original (real) item, just stops displaying it in the Gallery list box.

***********************************************************************************************/
 
BOOL Gallery::RemoveItemFromDisplay(SequenceItem *TheSeqItem)
{
	// Deletes the given DisplayItem, effectively removing its master item from display
	// DOES NOT delete the master item

	ERROR3IF(DisplayList == NULL, "Gallery DisplayList is NULL!");
	if (TheSeqItem != NULL)
	{
		DisplayList->RemoveItem(TheSeqItem);
		return(TRUE);
	}

	return(FALSE);
}



/***********************************************************************************************

>	BOOL Gallery::RemoveItemFromDisplay(ListItem *TheItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Inputs:		TheItem - The real item to remove from the display
	Returns:	TRUE if it succeeds

	Purpose:	Removes the given item from the display. This does NOT affect the
				original (real) item, just stops displaying it in the Gallery list box.

	Notes:		If you have a pointer to the SequenceItem associated with TheItem, you
				should pass that into the overloaded function instead, as it is a
				much more efficient way of removing the item.

***********************************************************************************************/
 
BOOL Gallery::RemoveItemFromDisplay(ListItem *TheItem)
{
	return(RemoveItemFromDisplay(FindSequenceItem(TheItem)));
}



/***********************************************************************************************

>	void Gallery::RemoveAllItemsFromDisplay(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Purpose:	Removes all items from the display, effectively clearing the listbox.
				This does NOT affect the original (real) items, just stops displaying
				them in the Gallery list box.

***********************************************************************************************/
 
void Gallery::RemoveAllItemsFromDisplay(void)
{
	if (DisplayList != NULL)
		DisplayList->DeleteAll();
}



/***********************************************************************************************

>	void Gallery::ForceRedrawOfItem(SequenceItem* pThisSeqItem)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (Recoded by Jason)
	Created:	9/8/94
	Inputs:		pThisSeqItem = item that specifies the item to be redrawn
	Outputs:	-
	Returns:	-
	Purpose:	Redraws the given item in the list

***********************************************************************************************/

void Gallery::ForceRedrawOfItem(SequenceItem* pThisSeqItem)
{
	INT32 Index = CalculateListIndex(pThisSeqItem);

	if (Index >= 0)
	{
		StringBase* pText = GetItemText(FindRealItem(pThisSeqItem));

		if (pText != NULL)
			SetStringGadgetValue(_R(IDC_GALLERY_LISTBOX), pText, FALSE, Index);
#ifdef _DEBUG
		else
		{
			String_64 Bob("Unknown Gallery item");
			SetStringGadgetValue(_R(IDC_GALLERY_LISTBOX), &Bob, FALSE, Index);
		}
#endif
	}
}


 
/***********************************************************************************************

>	void Gallery::ForceRedrawOfItem(INT32 Index)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (Recoded by Jason)
	Created:	9/8/94
	Inputs:		Index - index into the DisplayList specifying the item to be redrawn
	Outputs:	-
	Returns:	-
	Purpose:	Redraws the given item in the list

***********************************************************************************************/
 
void Gallery::ForceRedrawOfItem(INT32 Index)
{
	SequenceItem *pSeqItem = FindItemByIndex(Index);

	if (pSeqItem != NULL)
	{
		StringBase* pText = GetItemText(FindRealItem(pSeqItem));

		if (pText != NULL)
			SetStringGadgetValue(_R(IDC_GALLERY_LISTBOX), pText, FALSE, Index);
#ifdef _DEBUG
		else
		{
			String_64 Bob("Unknown Gallery item");
			SetStringGadgetValue(_R(IDC_GALLERY_LISTBOX), &Bob, FALSE, Index);
		}
#endif
	}
}



/***********************************************************************************************

>	void Gallery::ForceRedrawOfList(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Purpose:	Causes the Gallerys list box to be redrawn. After some operations, 
				the list box is not automatically invalidated for redraw so that
				many operations can be applied to it in one go. Maybe.

***********************************************************************************************/
 
void Gallery::ForceRedrawOfList(void)
{
	// re-paints the gallery list box to show the new gallery information
	if (DisplayList == NULL)
		return;

	if (DisplaySequence == NULL)
		DisplaySequence = DisplayList->SelectOrder(SortFunction, FALSE);
	else
		DisplayList->ReSortOrder(SortFunction, FALSE);


	// **** Temporary bodge! The list will be owner-drawn later ****
	// We re-create the entire listbox list from our display list
	if (IsVisible())
	{
//		GadgetRedraw(_R(IDC_GALLERY_LISTBOX), FALSE);	// Disable redraw for a while
		DeleteAllValues(_R(IDC_GALLERY_LISTBOX));

		DWORD Index = 0;
		StringBase *TheText;
		SequenceItem *Ptr = FindFirstDisplayed();
		while (Ptr)
		{
			TheText = GetItemText(FindRealItem(Ptr));
	
			if (TheText != NULL)
				SetStringGadgetValue(_R(IDC_GALLERY_LISTBOX), TheText);
#ifdef _DEBUG
			else
			{
				String_64 Bob("Unknown Gallery item");
				SetStringGadgetValue(_R(IDC_GALLERY_LISTBOX), &Bob);
			}
#endif
			// Chris ..  Commented out for Jason
			// And ensure the item is not marked as selected
			//((GalleryDisplayItem *) Ptr->GetItem())->SetSelected(FALSE);

			Ptr = FindNextDisplayed(Ptr); 
			Index++;
		}

//		GadgetRedraw(_R(IDC_GALLERY_LISTBOX), TRUE);	// Re-enable redraw

		SetSelectionFromDisplayList();
	}
}

			

/********************************************************************************************

>	void Gallery::ShadeGallery(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Purpose:	Shades the gallery - this ensures that all buttons/lists are 
				shaded and non-clickable by the user. If the gallery is already
				in this state, nothing happens. Used by derived gallery classes
				to ensure that they are shaded when no scope document is available.
	SeeAlso:	Gallery::UnshadeGallery

********************************************************************************************/

void Gallery::ShadeGallery(void)
{
	if (IsVisible())
	{
		DeleteAllValues(_R(IDC_GALLERY_LISTBOX));	// Blank out the list box
		UnshadeGallery(FALSE);					// Shade all the buttons and listbox
	}
	IsShaded = TRUE;
}



/********************************************************************************************

>	void Gallery::UnshadeGallery(BOOL UnshadeIt = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/94
	Inputs:		UnshadeIt - You can ignore this. (Used internally to allow
				one generic routine to serve both shading and unshading)

	Purpose:	Unshades the gallery - this ensures that all buttons/lists are 
				unshaded and clickable by the user. If the gallery is already
				in this state, nothing happens. Used by derived gallery classes
				to ensure that they are unshaded when a scope document is available.

	SeeAlso:	Gallery::ShadeGallery

********************************************************************************************/

void Gallery::UnshadeGallery(BOOL UnshadeIt)
{
	if (IsVisible())
	{
		EnableGadget(_R(IDC_GALLERY_APPLY),		UnshadeIt);
		EnableGadget(_R(IDC_GALLERY_NEW),		UnshadeIt);
		EnableGadget(_R(IDC_GALLERY_DELETE),	UnshadeIt);
		EnableGadget(_R(IDC_GALLERY_REDEFINE),	UnshadeIt);
		EnableGadget(_R(IDC_GALLERY_UNDO),		UnshadeIt);
		EnableGadget(_R(IDC_GALLERY_MENU),		UnshadeIt);

		EnableGadget(_R(IDC_GALLERY_LISTBOX),	UnshadeIt);

		DoShadeGallery(!UnshadeIt);		// Call derived gallery to handle its shading stuff
	}
	IsShaded = !UnshadeIt;
}



/********************************************************************************************

>	INT32 Gallery::SortFunction(OrderedList *Parent, ListItem *Item1, ListItem *Item2)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Inputs:		Parent - the OrderedList which the items are in
				Item1, Item2 - The two items to be compared
	Outputs:	-
	Returns:	A Negative/Zero/Positive result to indicate the result of comparing
				the two items.
	Purpose:	Special multi-key list comparator function.
				This provides the sort mode which has been set by the derived Gallery
				class, using the OrderedList::SortMode sort key array provided in the
				Display OrderedList.
	Scope:		private
	SeeAlso:	OrderedList; keyword ListComparator

********************************************************************************************/

INT32 Gallery::SortFunction(OrderedList *Parent, ListItem *Item1, ListItem *Item2)
{
	ListComparator SortFn;
	INT32 result = 0;
	INT32 i = 0;

	ERROR3IF(Parent == NULL || Item1 == NULL || Item2 == NULL, 
			 "NULL parameter(s) passed to Gallery SortFunction!");

	while (i < MaxSortKeys && result == 0)
	{
		if (Parent->SortMode[i].Info != NULL)
		{
			SortFn = Parent->SortMode[i].Info->GetSortFn();

			if (Parent->SortMode[i].Reversed)
				result = SortFn(Parent, Item2, Item1);
			else
				result = SortFn(Parent, Item1, Item2);
		}

		i++;
	}
	return(result);
}



/***********************************************************************************************

>	void Gallery::ApplyApplySelection(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Purpose:	Calls the overridden methods which deal with applying the selection
	Scope:		private subroutine for gallery base class

***********************************************************************************************/
 
void Gallery::ApplyApplyItem(void)
{
	SequenceItem *Ptr = FindFirstSelected();

	if (Ptr != NULL)
		DoApplyItem(FindRealItem(Ptr));
}



/***********************************************************************************************

>	void Gallery::ApplyNewItem(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Purpose:	Calls the overridden methods which deal with creating a new item
	Scope:		private subroutine for gallery base class

***********************************************************************************************/
 
void Gallery::ApplyNewItem(void)
{
	SequenceItem *Ptr = FindFirstSelected();

	ListItem *BasedOnItem = (Ptr == NULL) ? NULL : FindRealItem(Ptr);

	ListItem *NewItem = DoCreateNewItem(BasedOnItem);
}



/***********************************************************************************************

>	void Gallery::ApplyDeleteSelection(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Purpose:	Calls the overridden methods which deal with deletion of the selection
	Scope:		private subroutine for gallery base class

***********************************************************************************************/
 
void Gallery::ApplyDeleteSelection(void)
{
	if (PrepareToDelete() && DoDeleteSelection())
	{
		// Should update the listbox by removing the killed items... **** !!!!
		// (The deleted items are the current selection)
		// Note that this should tie in with ForceRedrawOfList in some way to stop
		// rebuilding of the list all the time
	}
}



/***********************************************************************************************

>	void Gallery::ApplyRedefineSelection(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Purpose:	Calls the overridden methods which deal with redefining (editing) the
				selection.
	Scope:		private subroutine for gallery base class

***********************************************************************************************/

void Gallery::ApplyRedefineItem(void)
{
	SequenceItem *Ptr = FindFirstSelected();

	if (Ptr != NULL)
		DoRedefineItem(FindRealItem(Ptr));
}



/***********************************************************************************************

>	void Gallery::ApplyUndo(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Purpose:	Deals with UNDOing actions which were DOne with this Gallery
	Notes:		Currently not fully implemented - undoes everything just as the normal undo
				button does (i.e. does not stop at stuff this gallery created)
	Scope:		private subroutine for gallery base class

***********************************************************************************************/
 
void Gallery::ApplyUndo(void)
{
	if (GetGalleryDocument() != NULL)
		GetGalleryDocument()->GetOpHistory().UndoPrev();

	// !!!! ToDo: Should only undo actions that were done by this gallery
}



/***********************************************************************************************

>	void Gallery::ApplyGalleryMenu(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Purpose:	Calls the overridden methods which deal with showing the Gallery menu
	Scope:		private subroutine for gallery base class

***********************************************************************************************/
 
void Gallery::ApplyGalleryMenu(void)
{

	// !!!! ToDo
#if FALSE
	DoShowMenu();		// Call the derived DoShowMenu method to show the menu
	
#else
	/// **** For now, we pop up the display dialogue (cos this'll probably be
	// a menu item. maybe. perhaps. dunno really.

	// Create a Gallery display options dialogue
	GalleryDisplayDlg *DisplayDlg = new GalleryDisplayDlg;


	if (DisplayDlg != NULL)
	{
		// Set its parent gallery to be us.
		DisplayDlg->SetParentGallery(this);

		// And attempt to show it.
		if (DisplayDlg->Create())
			DisplayDlg->Open();
		else
			DisplayDlg->End();
	}

#endif
}



/***********************************************************************************************

>	void Gallery::SetSelectionFromDisplayList(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Purpose:	Sets the list box selection to the same state as the DisplayList's
				selection cache.
	SeeAlso:	Gallery:RescanSelection

***********************************************************************************************/

void Gallery::SetSelectionFromDisplayList(void)
{
	if (DisplayList == NULL || !IsVisible())
		return;

//	GadgetRedraw(_R(IDC_GALLERY_LISTBOX), FALSE);				// Disable listbox redraw
	SetBoolGadgetSelected(_R(IDC_GALLERY_LISTBOX), FALSE, -1);	// First, clear the selection

	INT32 Index = 0;
	SequenceItem *Ptr = FindFirstDisplayed();

	while (Ptr != NULL)
	{
		if ( ((GalleryDisplayItem *) Ptr->GetItem())->IsSelected() )
			SetBoolGadgetSelected(_R(IDC_GALLERY_LISTBOX), TRUE, Index);

		Ptr = FindNextDisplayed(Ptr);
		Index++;
	}

//	GadgetRedraw(_R(IDC_GALLERY_LISTBOX), TRUE);				// Re-enable listbox redraw

#ifdef _DEBUG
	INT32 Count;
	GetValueCount(_R(IDC_GALLERY_LISTBOX), &Count);
	ERROR3IF(Index != (INT32) Count,
			"Gallery::SetSelectionFromDisplayList: Disparity between DisplayList and ListBox list!");
#endif
}


/***********************************************************************************************

>	DWORD Gallery::RescanSelection(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Purpose:	Scans the list box to bring the DisplayList's selction cache into
				line with the current list box state.
	SeeAlso:	Gallery::SetSelectionFromDisplayList

***********************************************************************************************/

void Gallery::RescanSelection(void)
{
	if (DisplayList == NULL || !IsVisible())
		return;

	BOOL IsSelected;
	BOOL Valid;
	UINT32 Index = 0;
	SequenceItem *Ptr = FindFirstDisplayed();

	while (Ptr != NULL)
	{
		IsSelected = GetBoolGadgetSelected(_R(IDC_GALLERY_LISTBOX), 0, &Valid, Index);
		ERROR3IF(!Valid, "Gallery::RescanSelection is scanning invalid items!");

		((GalleryDisplayItem *) Ptr->GetItem())->SetSelected(IsSelected);

		Ptr = FindNextDisplayed(Ptr);
		Index++;
	}


#ifdef _DEBUG
	INT32 Count;
	GetValueCount(_R(IDC_GALLERY_LISTBOX), &Count);
	ERROR3IF(Index != (UINT32) Count,
			"Gallery::RescanSelection: Disparity between DisplayList and ListBox list!");
#endif
}






/********************************************************************************************

>	virtual BOOL Gallery::PreCreate(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Returns:	TRUE if it is successful; FALSE to abort creation of the window

	Purpose:	Example Gallery PreCreate handler

	Notes:		This method is called whenever the gallery window is about to be
				created. It should be overridden by derived classes.

				At this point, the gallery should set up data structures - it is
				*essential* that DisplayList is set up by the end of this method.
				(As it is used in the creation of the window)
				Note that you cannot AddItemToDisplay etc in this method, as 
				although the display list exists, the related window controls
				do not yet exist, and ENSURE failures will result.

********************************************************************************************/

BOOL Gallery::PreCreate(void)
{
	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL Gallery::PostCreate(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Returns:	TRUE if it is successful; FALSE to abort creation of the window

	Purpose:	Example Gallery PostCreate handler

	Notes:		This method is called whenever the gallery window is created, AFTER
				the window has been brought into existence. It should be overridden
				by derived classes.

				At this point (and henceforth), the gallery can add items to
				the DisplayList, etc. as the display controls now exist.

********************************************************************************************/

BOOL Gallery::PostCreate(void)
{
	return(TRUE);
}



/***********************************************************************************************

>	virtual BOOL Gallery::DoRedrawAnItem(ListItem *Item, BOOL Large, UINT32 DisplayMode);

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

BOOL Gallery::DoRedrawAnItem(ListItem *Item, BOOL Large, UINT32 DisplayMode)
{
	ERROR3("Unimplemented Gallery::RedrawAnItem override!");	
	return(FALSE);
}



/***********************************************************************************************

>	virtual BOOL Gallery::PrepareToDelete(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Returns:	TRUE if it is OK to go ahead and delete the current selection
	Purpose:	Checks if it is OK to delete the selection. Should be overridden 
				by the derived class. If not overridden, will return TRUE, so that
				it is always "ok" to delete the selection.

***********************************************************************************************/

BOOL Gallery::PrepareToDelete(void)
{
	return(TRUE);
}



/***********************************************************************************************

>	virtual StringBase *Gallery::GetItemText(ListItem *TheItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Returns:	NULL, or a pointer to the item's text

	Purpose:	STAGE 1 - SIMPLE GALLERY IMPLEMENTATION

				This method is overridden by derived galleries to provide text
				for each displayed item. This will not be used once user-redraw
				has been implemented, but is used for the time being.

***********************************************************************************************/

StringBase *Gallery::GetItemText(ListItem *TheItem)
{
	return(NULL);
}



/***********************************************************************************************

>	virtual BOOL Gallery::DoDeleteSelection(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Returns:	TRUE if the selected items were successfully deleted
	Purpose:	Deletes the selection. This function MUST be overridden by the
				derived class. (it would be pure virtual but for DYNCREATE)
	SeeAlso:	ColourGallery::DoDeleteSelection

***********************************************************************************************/

BOOL Gallery::DoDeleteSelection(void)
{
	ERROR3("Unimplemented Gallery::DoDeleteSelection override!");
	return(TRUE);
}



/***********************************************************************************************

>	virtual ListItem *Gallery::DoCreateNewItem(ListItem *BasedOn)

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

***********************************************************************************************/

ListItem *Gallery::DoCreateNewItem(ListItem *BasedOn)
{
	return(NULL);
}



/***********************************************************************************************

>	virtual void Gallery::DoApplyItem(ListItem *ItemToApply)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		ItemToApply - The item to apply
	Purpose:	This function is meant to be overridden in derived classes
				It applies the selected item to the document in whatever way
				is appropriate to its particular data item type. 

***********************************************************************************************/

void Gallery::DoApplyItem(ListItem *ItemToApply)
{
}



/***********************************************************************************************

>	virtual BOOL Gallery::DoRedefineItem(ListItem *ItemToRedefine)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94

	Inputs:		ItemToRedefine - The item which should be redefined

	Returns:	TRUE if the item was changed (or if you're not sure).
				FALSE if the item definitely was not changed (to save us
				re-sorting and drawing the displayed list)

	Purpose:	This function is meant to be overridden in derived classes
				It allows the user to 'redefine' the item in some way, using
				an item editor (such as a colour picker)

***********************************************************************************************/

BOOL Gallery::DoRedefineItem(ListItem *ItemToRedefine)
{
	return(TRUE);
}



/***********************************************************************************************

>	virtual void Gallery::DoSpecialClick(ListItem *ClickedItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/94
	Inputs:		ClickedItem - The item which has been special clicked

	Purpose:	This function is meant to be overridden in derived classes
				It handles a special (alt-) click on a list item. This is generally used
				as a short-cut to bring up an editor on the clicked item, but the exact
				interpretation is left to the derived gallery to decide.

***********************************************************************************************/

void Gallery::DoSpecialClick(ListItem *ClickedItem)
{
}



/***********************************************************************************************

>	virtual void Gallery::DoSelectionChanged(void)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This method is called by the base class when the gallery receives a
				selection change message on the list box, to inform interested derived
				galleries of the change in the selection.

***********************************************************************************************/

void Gallery::DoSelectionChanged(void)
{
}



/***********************************************************************************************

>	virtual void Gallery::DoShadeGallery(BOOL ShadeIt)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Inputs:		ShadeIt -	TRUE if the gallery is being shaded
							FALSE if the gallery is being unshaded
	Purpose:	Called by the base class whenever the shaded status of the gallery
				is changed, to allow derived galleries to shade/unshade any extra
				controls that they provide in the window. ONLY called if the gallery
				is actually open/visible.

	Notes:		Need not be overridden - the default action is to do nothing extra
				to the normal shade/unshade operation.
	SeeAlso:	Gallery::ShadeGallery; Gallery::UnshadeGallery

***********************************************************************************************/

void Gallery::DoShadeGallery(BOOL ShadeIt)
{
}







void Gallery::PaintText(INT32 XOffset, INT32 YOffset, StringBase *TheText)
{
	// Using the current drawing (DC), plot the given text string at the given offset
}





/***********************************************************************************************

>	BOOL Gallery::AddDisplayListToDoc(Document *ScopeDoc, OrderedList *NewList = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/7/94
	Inputs:		ScopeDoc - The document you wish to add your list to
				NewList - NULL, or a pointer to an OrderedList (or derivative) which you want
				added to ScopeDoc's list of cached DisplayLists.

	Returns:	TRUE if it succeeds
				FALSE => ERROR2

	Purpose:	Interface by which derived Gallery classes can add a cached display
				list to the given document. Should be called on document created
				broadcasts to add your gallery's display list to a new doc.

	Notes:		If NewList is NULL, a new (blank) OrderedList will be created on your behalf

				You should call FindDisplayListInDoc first to ensure that you do not
				attempt to add more than one displaylist to any given document.

				The Gallery destructor will remove any display caches owned by your
				gallery when it is deleted.

	SeeAlso:	Gallery::FindDisplayListInDoc; Gallery::SelectMyDocDisplayList

***********************************************************************************************/
 
BOOL Gallery::AddDisplayListToDoc(Document *ScopeDoc, OrderedList *NewList)
{
	ERROR2IF(ScopeDoc == NULL, FALSE, "Gallery::AddDisplayListToDoc needs non-NULL Doc pointer");

	if (ScopeDoc->GalleryDisplays == NULL)
		ScopeDoc->GalleryDisplays = new List;

	if (ScopeDoc->GalleryDisplays == NULL)
		return(FALSE);

	OrderedList *NewDisplayList;
	if (NewList != NULL)
		NewDisplayList = NewList;
	else
		NewDisplayList = new OrderedList;

	if (NewDisplayList == NULL)
		return(FALSE);

	NewDisplayList->SetOwner(this);
	GalleryDisplayListPtr *NewItem = new GalleryDisplayListPtr(NewDisplayList);
	if (NewItem == NULL)
	{
		delete NewDisplayList;
		return(FALSE);
	}

	ScopeDoc->GalleryDisplays->AddTail(NewItem);
	return(TRUE);
}



/***********************************************************************************************

>	BOOL Gallery::DeleteDisplayListFromDoc(Document *ScopeDoc)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/7/94
	Inputs:		ScopeDoc - The document you wish to delete your list from
	Purpose:	Interface by which derived Gallery classes can add a cached display
				list to the given document. Should be called on document created
				broadcasts to add your gallery's display list to a new doc.
	SeeAlso:	Gallery::AddDisplayListToDoc; Gallery::FindDisplayListInDoc;

***********************************************************************************************/
 
void Gallery::DeleteDisplayListFromDoc(Document *ScopeDoc)
{
	ERROR3IF(ScopeDoc == NULL, "Gallery::DeleteDisplayListFromDoc needs non-NULL Doc pointer");

	if (ScopeDoc->GalleryDisplays == NULL)
		return;

	GalleryDisplayListPtr *Ptr = (GalleryDisplayListPtr *) ScopeDoc->GalleryDisplays->GetHead();

	while (Ptr != NULL)
	{
		if (Ptr->DisplayList->GetOwner() == this)
			break;

		Ptr = (GalleryDisplayListPtr *) ScopeDoc->GalleryDisplays->GetNext(Ptr);
	}

	if (Ptr != NULL)
	{
		if (ScopeDoc == GetGalleryDocument())	// If in use, stop using this list!
			SelectDisplayList(NULL);

		ScopeDoc->GalleryDisplays->RemoveItem(Ptr);
		delete Ptr;

		// If this has emptied the list of display caches, delete the list too
		if (ScopeDoc->GalleryDisplays->IsEmpty())
		{
			delete ScopeDoc->GalleryDisplays;
			ScopeDoc->GalleryDisplays = NULL;
		}
	}
}



/***********************************************************************************************

>	OrderedList *Gallery::FindDisplayListInDoc(Document *ScopeDoc)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/7/94
	Inputs:		ScopeDoc - The document you wish to find your list in
	Returns:	NULL, or a pointer to a Gallery DisplayList OrderedList
	Purpose:	Given a document, attempts to find the display list which this gallery
				has added to it. This is an easy way for derived galleries to cache
				a display list for each doc and find it again later, without having
				to check out & change document.cpp or anything

	SeeAlso:	Gallery::AddDisplayListToDoc; Gallery::SelectMyDocDisplayList

***********************************************************************************************/
 
OrderedList *Gallery::FindDisplayListInDoc(Document *ScopeDoc)
{
	ERROR3IF(ScopeDoc == NULL, "Gallery::FindDisplayListInDoc needs non-NULL Doc pointer");

	if (ScopeDoc->GalleryDisplays == NULL)
		return(NULL);

	GalleryDisplayListPtr *Ptr = (GalleryDisplayListPtr *) ScopeDoc->GalleryDisplays->GetHead();

	while (Ptr != NULL)
	{
		if (Ptr->DisplayList->GetOwner() == this)
			return(Ptr->DisplayList);

		Ptr = (GalleryDisplayListPtr *) ScopeDoc->GalleryDisplays->GetNext(Ptr);
	}
	
	return(NULL);
}



/***********************************************************************************************

>	void Gallery::SelectDisplayList(OrderedList *NewDisplayList)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/7/94
	Inputs:		NewDisplayList - The display list which you want the Gallery to
				use from now onwards.

	Purpose:	This deselects the existing Gallery DisplayList and selects the
				new DisplayList for all future operations.
				
	Notes:		1 You can pass in NULL to just deselect the existing list.

				2 This does not affect the OrderedLists in any way, just links/delinks
				them as the current DisplayList.

				3 This does NOT affect the visible state of the  - you should combine
				this call with a ForceRedraw operation.

				4 This shades/unshades the gallery for NULL/non-NULL NewDisplayLists

	SeeAlso:	Gallery::ForceRedrawOfList; Gallery::ShadeGallery

***********************************************************************************************/
 
void Gallery::SelectDisplayList(OrderedList *NewDisplayList)
{
	if (DisplayList != NULL && DisplaySequence != NULL)
	{
		DisplayList->DeselectOrder(SortFunction, FALSE);
		DisplaySequence = NULL;
	}

	DisplayList = NewDisplayList;
	if (DisplayList != NULL)
	{
		DisplaySequence = DisplayList->SelectOrder(SortFunction, FALSE);
		UnshadeGallery();
	}
	else
		ShadeGallery();
}



/***********************************************************************************************

>	void Gallery::SelectMyDocDisplayList(Document *ScopeDoc)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/7/94
	Inputs:		ScopeDoc - The document you wish to find your list in
	Purpose:	Given a document, attempts to find the display list which this gallery
				has added to it, and then swaps the Gallery over to using this new
				display list - should therefore be called on DocChangingMsgs.
				Will shade/unshade the gallery as appropriate

	SeeAlso:	Gallery::AddDisplayListToDoc; Gallery::FindDisplayListInDoc;
				Gallery::SelectDisplayList

***********************************************************************************************/
 
void Gallery::SelectMyDocDisplayList(Document *ScopeDoc)
{
	SelectDisplayList(FindDisplayListInDoc(ScopeDoc));
}



/***********************************************************************************************

>	virtual void Gallery::SortModeHasBeenChanged(BOOL SetDerivedSortsToo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/7/94
	Inputs:		SetDerivedSortsToo - If FALSE, only the base gallery class' Display List
				sort mode should be changed. If TRUE, then any other things that can
				depend upon this sort mode should also be updated (e.g. ColourGallery
				will update the colour bar sortmode if this is TRUE).

				NOTE - this will ALWAYS be false unless you set up an 'also' button
				by providing a derived SetAlsoButton method

	Purpose:	Called when the GalleryDisplayDlg has chnaged the gallery DisplayList
				SortMode array to a new sort. The gallery will cause a re-sort and
				update of its list display.

	Notes:		The base class functionality MUST be provided in derived classes. Thus,
				you should call the base class method from any derived version.
				If SetDerivedSorts == FALSE, derived methods probably should do nothing.
				Otherwise, you should copy the DisplayList->SortMode array into any other
				SortModes you may have for external entities such as the colour bar, and
				cause them to update themselves for the new sort too.

	SeeAlso:	Gallery::SetAlsoButton

***********************************************************************************************/ 

void Gallery::SortModeHasBeenChanged(BOOL SetDerivedSortsToo)
{
	// The display sequence is wrong now, so deselect it to be on the safe side
	if (DisplaySequence != NULL)
	{
		if (DisplayList != NULL)
			DisplayList->DeselectOrder(SortFunction, FALSE);
		DisplaySequence = NULL;
	}
		
	// Force redraw/re-filling of the gallery list in the new sort order
	ForceRedrawOfList();
}



/***********************************************************************************************

>	virtual BOOL Gallery::DoSetDisplayModes(GalleryDisplayDlg *TheDialogue)

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

	SeeAlso:	GalleryDisplayDlg::SetDisplayModeName; Gallery::DoDisplayModeChange

***********************************************************************************************/

BOOL Gallery::DoSetDisplayModes(GalleryDisplayDlg *TheDialogue)
{
	// Example:
	// String_64 Text(_R(IDS_DISPLAY_NAMES_ONLY));
	// TheDialogue->SetDisplayModeName(Text);

	return(FALSE);		// By default, the gallery will have no choice of modes
}



/***********************************************************************************************

>	virtual void Gallery::DoDisplayModeChange(UINT32 NewDisplayMode)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/7/94
	Inputs:		NewDisplayMode - this number is an index [0..MaxDisplayModes] into
				the list of display names you gave in DoSetDisplayModes
	Returns:	-

	Purpose:	This function probably won't be needed, but it is called if the
				display mode is changed in the gallery display dialogue. You may
				want to cache new values or something before the next time your
				redraw code is called, in which case this is the place to do it.

	SeeAlso:	GalleryDisplayDlg::SetDisplayModeName; Gallery::DoSetDisplayModes

***********************************************************************************************/

void Gallery::DoDisplayModeChange(UINT32 NewDisplayMode)
{
	// By default, no action is taken for display mode changes
}



/***********************************************************************************************

>	virtual void Gallery::DoShowMenu(void)

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

void Gallery::DoShowMenu(void)
{
	// By default, no menu is shown
}



/***********************************************************************************************

>	virtual BOOL Gallery::SetAlsoButton(String_64 *TextToPutIn)

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

	SeeAlso:	Gallery::SortModeHasBeenChanged

***********************************************************************************************/

BOOL Gallery::SetAlsoButton(String_64 *TextToPutIn)
{
	return(FALSE);		// By default, Galleries do not support an 'also' button
}



















//-----------------------------------------------------------------------------------------



const CDlgMode GalleryDisplayDlg::Mode = MODELESS; // Mode of the dialog  

const UINT32 GalleryDisplayDlg::IDD = _R(IDD_GALDISPLAYDLG);


// --- GallerySorts
// The GallerySorts array holds the current window state. Similar arrays (of
// SortKeys only) are held by each instantiation of Gallery, which define
// their current sort settings.

GalSortInfo GalleryDisplayDlg::GallerySorts[MaxSortKeys] = 
{
	{ {NULL, FALSE}, _R(IDC_GALSORT_SORT1), _R(IDC_GALSORT_REVERSE1) },
	{ {NULL, FALSE}, _R(IDC_GALSORT_SORT2), _R(IDC_GALSORT_REVERSE2) }
};



/********************************************************************************************

>	static INT32 GalleryDisplayDlg::FindIndexFromCI(List *Sorts, INT32 SortInfoIndex)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/94
	Inputs:		Sorts - The list of available sorts (ComparatorInfos)
				SortInfoIndex - the index in GallerySorts[] of the control you
				need the index for (as each one is a slightly different list)
	Outputs:	-
	Returns:	The index of the given GalSortInfo item in the given list of sorts
				(This is almost a List::GetPosition, but returns 0 if the item is
				not found)
	Purpose:	Conversion between a ComparatorInfo pointer and a combo box index for
				the Gallery Display (Sort) Dialogue.
	Scope:		private
	SeeAlso:	FindCIFromIndex

********************************************************************************************/

INT32 GalleryDisplayDlg::FindIndexFromCI(List *Sorts, INT32 SortInfoIndex)
{
#if FALSE
	ComparatorInfo *SortInfo = GallerySorts[SortInfoIndex].Key.Info;

	if (Sorts == NULL || SortInfo == NULL)
		return(0);

	ComparatorInfo *Ptr = (ComparatorInfo *) Sorts->GetHead();
	INT32 index = 0;
	while (Ptr != NULL)
	{
		if (Ptr == SortInfo)		// Have found this item in the list
			break;

		Ptr = (ComparatorInfo *) Sorts->GetNext(Ptr);
		index++;
	}	

	if (Ptr == NULL)
		return(0);

	if (SortInfoIndex != 0)			// In all but 1st control, add 1 for 'None' entry
		index++;

	return(index);
#else
	return(0);
#endif
}



/********************************************************************************************
	
>	static ComparatorInfo *GalleryDisplayDlg::FindCIFromName(List *Sorts, StringBase *Name)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/94
	Inputs:		Sorts - The list of available sorts (ComparatorInfos)
				Name - The string of the selected sort in the combo box
	Outputs:	-
	Returns:	The appropriate ComparatorInfo for the selected Combo-box item.
				This will return NULL if anything goes wrong, or if the combo box
				is not the primary sort and the 'None' item was selected
	Purpose:	Conversion between a ComparatorInfo pointer and a combo box index for
				the Gallery Display (Sort) Dialogue.
	Scope:		private
	SeeAlso:	FindIndexFromCI

********************************************************************************************/

ComparatorInfo *GalleryDisplayDlg::FindCIFromName(List *Sorts, StringBase &Name)
{
#if FALSE
	// All controls but the first have an extra 'None' field, for which we return NULL
	String_32 Test(_R(IDS_SORTBY_NONE));
	if (Test == Name)
		return(NULL);

	ComparatorInfo *Ptr = (ComparatorInfo *) Sorts->GetHead();
	while (Ptr != NULL)
	{
		if (Ptr->GetSortName() == Name)
			break;

		Ptr = (ComparatorInfo *) Sorts->GetNext(Ptr);
	}

	return(Ptr);
#else
	return(NULL);
#endif
}



/********************************************************************************************

>	GalleryDisplayDlg::GalleryDisplayDlg(): DialogOp(GalleryDisplayDlg::IDD, GalleryDisplayDlg::Mode) 

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the colour manager dialogue class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

GalleryDisplayDlg::GalleryDisplayDlg() : DialogOp(GalleryDisplayDlg::IDD, GalleryDisplayDlg::Mode)
{
	ParentGallery = NULL;
}



/********************************************************************************************

>	void GalleryDisplayDlg::InitValues(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Outputs:	-
	Returns:	-
	Purpose:	Initialises the gallery display dialogue, and sets its controls
	Scope:		private
	Errors:		-
	SeeAlso:	GalleryDisplayDlg::SetControls

********************************************************************************************/

void GalleryDisplayDlg::InitValues(void)
{
#if FALSE
	ERROR3IF(ParentGallery == NULL,
			 "GalleryDisplayDlg MUST be given a valid parent pointer");

	if (ParentGallery->DisplayList == NULL)
		return;

	// Copy the parent gallery's display list sortmode
	for (INT32 i = 0; i < MaxSortKeys; i++)
	{
		GallerySorts[i].Key.Info	 = ParentGallery->DisplayList->SortMode[i].Info;
		GallerySorts[i].Key.Reversed = ParentGallery->DisplayList->SortMode[i].Reversed;
	}

	// Fill in the combo boxes etc with the current settings
	SetControls();
#endif
}



/********************************************************************************************

>	void GalleryDisplayDlg::SetParentGallery(Gallery *Parent)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		Parent - the parent (owner) of this display dialogue. This pointer
				is used to inform the parent of changes to display/sort modes.
	Outputs:	-
	Returns:	-
	Purpose:	Sets the parent gallery of this dialogue. If this is not done
				then ensure failures will occur when you try to open (Do) the dlg.

********************************************************************************************/

void GalleryDisplayDlg::SetParentGallery(Gallery *Parent)
{
	// Remember who our parent Gallery is so we can set their sort mode
	ParentGallery = Parent;
}



/********************************************************************************************

>	void GalleryDisplayDlg::SetDisplayModeName(StringBase *EntryName)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/7/94
	Inputs:		EntryName - The name to set as the next item
	Outputs:	-
	Returns:	-
	Purpose:	Called (multiple times) by derived gallery classes in response to
				a call we make upon opening the dialogue. Each call appends another
				display mode name to the display mode combo box list in the GalDispDlg
				window.

********************************************************************************************/

void GalleryDisplayDlg::SetDisplayModeName(StringBase *EntryName)
{
#if FALSE
	SetStringGadgetValue(_R(IDC_GALSORT_MODES), EntryName, FALSE, 0);	
#endif
}



/********************************************************************************************

>	void GalleryDisplayDlg::SetControls(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	(Re)Initialises the colour manager dialogue controls
				(This simply sets the combo boxes etc up from the current Sort Dlg
				settings)
	Scope:		private
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void GalleryDisplayDlg::SetControls(void)
{
#if FALSE
	ERROR3IF(ParentGallery == NULL, "ParentGallery is NULL!");

	if (ParentGallery->DisplayList == NULL)
		return;


	// Set up the 'also' button; show it and set the text, or hide it as neccesarry
	SetBoolGadgetSelected(_R(IDC_GALSORT_GALONLY), TRUE);	// Always defaults to being on

	String_64 ButtonText;
	if (ParentGallery->SetAlsoButton(&ButtonText))
	{
		HideGadget(_R(IDC_GALSORT_GALONLY), FALSE);
		SetStringGadgetValue(_R(IDC_GALSORT_GALONLY), (StringBase *) &ButtonText);
	}
	else
		HideGadget(_R(IDC_GALSORT_GALONLY), TRUE);


	// Set values in the display combo box
	BOOL Result = ParentGallery->DoSetDisplayModes(this);
	EnableGadget(_R(IDC_GALSORT_MODES), Result);
	if (Result)
		SetSelectedValueIndex(_R(IDC_GALSORT_MODES), (WORD) ParentGallery->CurrentDisplayMode);


	// And set up the sort key lists and 'reversed' option buttons
	List *Sorts = ParentGallery->DisplayList->GetSortOrders();

	if (Sorts == NULL || Sorts->IsEmpty())
	{
		// This generally means that the programmer hasn't init'd the DisplayList correctly...
		TRACE( _T("\nGallery has no sort modes! Derive an OrderedList and call its Init()!\n"));
		return;
	}

	// Add each sort name to all combo boxes...
	String_32 NoSortName(_R(IDS_SORTBY_NONE));	// Add 'None' to all but the 1st combo box
	ComparatorInfo *Info = (ComparatorInfo *) Sorts->GetTail();
	INT32 i;
	while (Info != NULL)
	{
		for (i = 0; i < MaxSortKeys; i++)
			SetStringGadgetValue(GallerySorts[i].ListControlID,
									(StringBase *)&Info->GetSortName(), FALSE, 0);

		Info = (ComparatorInfo *) Sorts->GetPrev(Info);
	}

	for (i = 1; i < MaxSortKeys; i++)
	{
		SetStringGadgetValue(GallerySorts[i].ListControlID, &NoSortName, FALSE, 0);
	}


	// Set the Combo box selections and 'reversed' boxes to the current sort mode
	for (i = 0; i < MaxSortKeys; i++)
	{
		SetBoolGadgetSelected(GallerySorts[i].CheckControlID, GallerySorts[i].Key.Reversed);
		if (i > 0)
			EnableGadget(GallerySorts[i].CheckControlID, (GallerySorts[i].Key.Info != NULL));

		SetSelectedValueIndex(GallerySorts[i].ListControlID,
								(WORD) FindIndexFromCI(Sorts, i));
	}
#endif
}         



/********************************************************************************************

>	void GalleryDisplayDlg::SetSortMode(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Reads the current settings from the window, and sets them as the new
				sort mode for the parent gallery.
	Scope:		private
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void GalleryDisplayDlg::SetSortMode(void)
{
#if FALSE
	if (ParentGallery->DisplayList == NULL)
		return;

	List *Sorts = ParentGallery->DisplayList->GetSortOrders();

	if (Sorts == NULL || Sorts->IsEmpty())
		return;

	// Read the SortBy Combo box selections into the current-sort-mode record
	String_256 Name;
	BOOL SetAllSortModes = GetBoolGadgetSelected(_R(IDC_GALSORT_GALONLY));

	for (INT32 i = 0; i < MaxSortKeys; i++)
	{
		GallerySorts[i].Key.Reversed =
				GetBoolGadgetSelected(GallerySorts[i].CheckControlID);

		Name = GetStringGadgetValue(GallerySorts[i].ListControlID);
		GallerySorts[i].Key.Info = FindCIFromName(Sorts, Name);


		// Copy the new sort mode into the Gallery DisplayList sort mode
		ParentGallery->DisplayList->SortMode[i].Info = GallerySorts[i].Key.Info;
		ParentGallery->DisplayList->SortMode[i].Reversed = GallerySorts[i].Key.Reversed;
	}

	// Ensure that the gallery list is re-sorted on next access
	ParentGallery->DisplayList->InvalidateCaches();

	// And inform the gallery that the sort mode has been changed
	// Pass in a flag indicating if only the DisplayList display sort mode should
	// be changed, or if all derived gallery sort dependencies should also be updated
	// (e.g. if TRUE, the ColourGallery might update the Colour Bar sort mode as well)
	ParentGallery->SortModeHasBeenChanged(SetAllSortModes);
#endif
}         



/********************************************************************************************

>	MsgResult GalleryDisplayDlg::Message( Msg* Message)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		Msg - The dialogue manager message to handle
	Outputs:	-
	Returns:	A MsgResult
	Purpose:	Standard DialogOp message handler, for the Gallery display dialogue
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult GalleryDisplayDlg::Message( Msg* Message)
{
	if (!(IS_OUR_DIALOG_MSG(Message)))
		return(OK);

	DialogMsg* TheMsg = (DialogMsg*)Message;

#if FALSE
	switch(TheMsg->DlgMsg)
	{
		case DIM_COMMIT:						// OK clicked
			SetSortMode();
			// Drop through to DIM_CANCEL case to close the dlg

		case DIM_CANCEL:						// Cancel clicked
			Close();
			End();
			break;

		case DIM_SOFT_COMMIT:					// OK "adjust clicked"
			SetSortMode();
			break;

		case DIM_SELECTION_CHANGED:
			// If select in a combo box other than the primary sort key, then
			// we must shade/unshade the reverse checkbox depending on whether
			// 'None' (item 0) was selected
			{
				for (INT32 i = 1; i < MaxSortKeys; i++)
				{
					if (TheMsg->GadgetID == GallerySorts[i].ListControlID)
					{
						WORD Index;
						GetValueIndex(GallerySorts[i].ListControlID, &Index);
						EnableGadget(GallerySorts[i].CheckControlID, (Index != 0));
					}
				}
			}
			break;
	}
#endif

	return(DLG_EAT_IF_HUNGRY(TheMsg));
}


/********************************************************************************************

>	OpState	GalleryDisplayDlg::GetState(String_256*, OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Get the state of the Colour sort dialogue op
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	GalleryDisplayDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}


/********************************************************************************************

>	BOOL GalleryDisplayDlg::Init()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Initialises the colour sort dialogue op
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GalleryDisplayDlg::Init()
{
	return (RegisterOpDescriptor(
 								0,
 								_R(IDS_GALDISPLAYDLG),
								CC_RUNTIME_CLASS(GalleryDisplayDlg),
 								OPTOKEN_GALDISPLAYDLG,
 								GalleryDisplayDlg::GetState,
 								0,	/* help ID */
 								0,  // **** _R(IDBBL_GALDISPLAYDLG),
 								0	/* bitmap ID */
								));
} 



/********************************************************************************************

>	BOOL GalleryDisplayDlg::Create()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if it fails
	Purpose:	Creates a gallery display dialogue box
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GalleryDisplayDlg::Create()
{
#if FALSE
	if (DialogOp::Create())
	{ 
		InitValues();
		return(TRUE);
	}
#endif

	return(FALSE);
}



/********************************************************************************************

>	void GalleryDisplayDlg::Do(OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Does' a gallery display dialogue op. Shows the dialogue.
	Scope:		private
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void GalleryDisplayDlg::Do(OpDescriptor*)
{
#if FALSE
	if (Create())
		Open();
	else
#endif
		End();
}


#endif

