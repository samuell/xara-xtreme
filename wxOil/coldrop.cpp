// $Id: coldrop.cpp 662 2006-03-14 21:31:49Z alex $
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

// coldrop.cpp - Colour drop-down list support

/*
*/


//-----------------------------------------------------------------------------------------
// Include files

#include "camtypes.h"

#include "coldrop.h"
#include "colormgr.h"
#include "devcolor.h"
#include "dlgmgr.h"
#include "docview.h"
#include "errors.h"
#include "progress.h"


//-----------------------------------------------------------------------------------------
// Implementation of dynamic classes

class SpecialEntry : public ListItem
{
	CC_DECLARE_DYNCREATE(SpecialEntry);

public:
	BOOL Init(StringBase *Text, DocColour *Colour);

public:
	String_256	ItemText;
	BOOL		ItemHasColour;
	DocColour	ItemColour;
};

CC_IMPLEMENT_DYNCREATE(SpecialEntry, ListItem);
CC_IMPLEMENT_DYNCREATE(ColourDropDown, ListItem);
CC_IMPLEMENT_DYNAMIC(ColourDropMsgHandler, MessageHandler);



//-----------------------------------------------------------------------------------------
// This must be defined AFTER all CC_IMPLEMENT_DYNCREATE calls
#define new CAM_DEBUG_NEW



BOOL SpecialEntry::Init(StringBase *Text, DocColour *Colour)
{
	ItemText = *Text;
	if (Colour == NULL)
		ItemHasColour = FALSE;
	else
	{
		ItemHasColour = TRUE;
		ItemColour = *Colour;
	}

	return(TRUE);
}


// --- Statics
ColourDropMsgHandler *ColourDropDown::MessageHandler = NULL;		// One msg handler for all coldropdowns
INT32 ColourDropDown::MsgHandlerCount = 0;




/********************************************************************************************

>	ColourDropDown(BOOL AutoUpdateOnColourListChange = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		6/10/95

	Inouts:		AutoUpdateOnColourListChange - Cor! What a whopper! ;-)
				er... where was I? Oh, yeah.

				TRUE (default) if you want the colour list to automatically update
				when the current colour list (selected document) changes. This includes
				addition/removal/change of colours, as well as changing document, and
				auto-shadin/unshading of the list gadget when there are no documents
				or colours present.

				FALSE if you want to poke the list yourself. (This should only really
				be used by the Colour Editor, which already had lots of careful handling
				code built in- everyone else should just use the default setting)

	Purpose:	ColourDropDown constructor

********************************************************************************************/

ColourDropDown::ColourDropDown(BOOL AutoUpdateOnColourListChange)
{
	AutoUpdateOnChange = AutoUpdateOnColourListChange;
}



/********************************************************************************************

>	virtual ColourDropDown::~ColourDropDown()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		29/8/95

	Purpose:	ColourDropDown destructor

********************************************************************************************/

ColourDropDown::~ColourDropDown()
{
	ClearAllSpecialEntries();

	if (MessageHandler != NULL)
	{
		MsgHandlerCount--;
		if (MsgHandlerCount <= 0)
		{
			delete MessageHandler;		// Get rid of msghandler when not needed
			MessageHandler = NULL;
		}
	}
}



/********************************************************************************************

>	virtual BOOL ColourDropDown::Init(CWindowID Window, CGadgetID Gadget)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		29/8/95

	Inputs:		Window - The Window in which your dropdown list gadget resides
				Gadget - The GadgetID of the deropdown list gadget

	Returns:	TRUE if it succeeded in connecting itself to your gadget
				FALSE if it failed

	Purpose:	DropDown initialiser

********************************************************************************************/

BOOL ColourDropDown::Init(CWindowID Window, CGadgetID Gadget)
{
	BOOL AlreadyInitialised = Initialised;	// Remember if this is the first time we've been init'd

	if (DropDown::Init(Window, Gadget))		// Call the base class
	{
		if (AlreadyInitialised)				// Only increment handler count on first initialisation
			return(TRUE);

		if (MessageHandler == NULL)			// And ensure we have a message handler
		{
			MessageHandler = new ColourDropMsgHandler;
			MsgHandlerCount = 0;
		}

		if (MessageHandler != NULL)			// And increment its usage count
		{
			MsgHandlerCount++;
			return(TRUE);
		}
	}

	return(FALSE);
}






/********************************************************************************************

>	void ColourDropDown::ClearAllSpecialEntries(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		29/8/95

	Purpose:	Forgets about any special entries you may have added in the past

	SeeAlso:	ColourDropDown::AddSpecialEntry

********************************************************************************************/

void ColourDropDown::ClearAllSpecialEntries(void)
{
	SpecialEntries.DeleteAll();
}



/********************************************************************************************

>	BOOL ColourDropDown::AddSpecialEntry(StringBase *Text, DocColour *Colour = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		29/8/95

	Inputs:		Text - The text to put in the entry (this will be copied)
				Colour - NULL or a DocColour. If non-NULL, this describes the colour of
				the colour patch placed to the left of the text. If NULL, no colour patch
				will be drawn.

	Returns:	TRUE if it added to the list successfully

	Purpose:	Adds a "special entry" to the top of the colour list

	Notes:		All special items MUST be added before calling FillInColourList

				The colour list can be augmented with a number of "special" entries.
				Each call to this method adds another entry to the bottom of the special
				entries list. If there are special entries, they are added to to top
				of the colour list, with a divider item between them and the colours.
				There can only be one divider.

				Special entries are indexed by their order of addition, with the first
				entry being number 0. When decoding a selection, DecodeSelection will
				return an IndexedColour pointer (if a colour was selected), or NULL,
				in which case, use the Index you passed in to determine which Special
				Entry was selected. Note that the divider will be treated as a special
				entry (e.g. with 2 special items, you can get an entry 2 selected - this
				corresponds to the divider, and should be ignored)

	SeeAlso:	ColourDropDown::DecodeSelection; ColourDropDown::FillInColourList

********************************************************************************************/

BOOL ColourDropDown::AddSpecialEntry(StringBase *Text, DocColour *Colour)
{
	ERROR2IF(ParentDlg == NULL, FALSE, "ColourDropDown not properly initialised");

	SpecialEntry *NewEntry = new SpecialEntry;
	if (NewEntry != NULL && NewEntry->Init(Text, Colour))	
	{
		SpecialEntries.AddTail(NewEntry);
		return(TRUE);
	}
	return(FALSE);
}



/********************************************************************************************

>	BOOL ColourDropDown::UpdateSpecialEntry(StringBase *Text, DocColour *NewColour)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		20/10/95

	Inputs:		Text - The text in the existing entry
				NewColour - The colour to chnage the display to

	Returns:	TRUE for success, FALSE if the entry was not found

	Purpose:	Modifies a "special entry" to display a different colour.
				Cannot be used if the entry has no colour associated with it.

	Notes:		Also causes a redraw of the dropdown control to update the screen display.

	SeeAlso:	ColourDropDown::DecodeSelection; ColourDropDown::FillInColourList

********************************************************************************************/

BOOL ColourDropDown::UpdateSpecialEntry(StringBase *Text, DocColour *NewColour)
{
	ERROR3IF(Text == NULL || NewColour == NULL, "Illegal NULL params");

	SpecialEntry *Ptr = (SpecialEntry *) SpecialEntries.GetHead();
	while (Ptr != NULL)
	{
		if (Ptr->ItemHasColour && Ptr->ItemText == *Text)
		{
			// Set the new colour
			Ptr->ItemColour = *NewColour;
			
			// And cause the combobox to redraw
			HWND hGadget = ::GetDlgItem(ParentDlg, ParentGadget);
			if (hGadget != NULL)
				::InvalidateRect(hGadget, NULL, FALSE);

			return(TRUE);
		}

		Ptr = (SpecialEntry *) SpecialEntries.GetNext(Ptr);
	}

	return(FALSE);
}



/********************************************************************************************

>	BOOL ColourDropDown::FillInColourList(IndexedColour *Selected = NULL,
											INT32 SelectIndex = -1,
											IndexedColour *NotDescendantOf = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		29/8/95

	Inputs:		Selected - NULL or a pointer to the selected colour
					(Only one of Selected, SelectedIndex should indicate the selection)

				SelectIndex - -1 or the 0-based index of the special item to select
					(Only one of Selected, SelectedIndex should indicate the selection.
					SelectIndex will actually override Selected if it is valid)

				NotDescendantOf - NULL, or a pointer to an IndexedColour. If this is non-null
				then this does not include "NotADescendantOf" or any of its children in the
				list (this allows a list of all available parent colours for a colour to be
				compiled by passing that colour in this field)

	Returns:	TRUE if it filled in the list successfully

	Purpose:	"Fills in" the attached control with appropriate entries for the current
				ColourDropDown settings, and (if neccessary) a separated set of special
				entries at the top thereof.

	Notes:		Only special entries added before this call is made will appear in the
				list, so add them first.

				Call this after initialising the control, and after any subsequent calls
				to change the contents of the list.

	SeeAlso:	ColourDropDown::DecodeSelection; ColourDropDown::AddSpecialEntry

********************************************************************************************/

BOOL ColourDropDown::FillInColourList(IndexedColour *Selected, INT32 SelectIndex,
										IndexedColour *NotDescendantOf)
{
	ERROR2IF(ParentDlg == NULL, FALSE, "ColourDropDown not properly initialised");

	BOOL Result = FALSE;		// Assume we've failed until we add an item!

	Progress Hourglass;			// Start an hourglass running

	ColourList *ParentList = ColourManager::GetCurrentColourList();

	SetListRedraw(FALSE);										// Disable redraw while updating
	ClearList();												// Delete combobox contents

	INT32 SelectedIndex = 0;

	if (!SpecialEntries.IsEmpty())
	{
		// Add an entry for each special entry
		ListItem *Ptr = SpecialEntries.GetHead();

		while (Ptr != NULL)
		{
			AddItem((DWORD) Ptr);
			Ptr = SpecialEntries.GetNext(Ptr);
		}

		AddItem(NULL);											// And add a separator
		Result = TRUE;		// There are items in the list, so we can return "OK" now
	}

	// If we have a "NotDescendantOf" colour, then we can only add parent colours to the list
	// if the "NotDescendantOf" colour is tint/shade/linked - normal/spot colours cannot have parents

	// Of course, we have to rely on the caller to be sensible here, because they might be about
	// to change the type of the colour (e.g. New Colour dialogue shows potential parents and
	// converts the colour to a shade if a parent is chosen. Thus, we don't want to present a blank
	// list of parent colours!)

	if (ParentList != NULL)
	{
		// Determine (precalculate) if NotADescendant (if any) is a tint colour
		BOOL NotATint = TRUE;
		if (NotDescendantOf != NULL &&
			NotDescendantOf->GetType() == COLOURTYPE_TINT &&
			!NotDescendantOf->TintIsShade())
		{
			NotATint = FALSE;
		}

		// Now scan the colour list, adding any legal parent colours to the list
		IndexedColour *Ptr = (IndexedColour *) ParentList->GetHead();
		INT32 Index = (SpecialEntries.IsEmpty()) ? 0 : (SpecialEntries.GetCount() + 1);
		
		while (Ptr != NULL)
		{
			Hourglass.Update();		// Update the houglass

			// Is the colour a named, live colour and not descended from NotADescendant? (Note: we
			// can safely pass NULL into IsADescendantOf())
			if (!Ptr->IsDeleted() && Ptr->IsNamed() && !Ptr->IsADescendantOf(NotDescendantOf))
			{
				AddItem((DWORD) Ptr);
				Result = TRUE;			// We've succeeded in adding at least 1 item
				if (Ptr == Selected)
					SelectedIndex = Index;

				Index++;
			}

			Ptr = (IndexedColour *) ParentList->GetNext(Ptr);
		}
	}

	SetListRedraw(TRUE);									// Re-enable redraw

	// Override any selection index we have with the caller-provided selection index, if
	// it indicates a valid "special item"
	if (SelectIndex >= 0 && SelectIndex < (INT32)SpecialEntries.GetCount())
		SelectedIndex = SelectIndex;

	SetSelectedIndex(SelectedIndex);						// And set the appropriate selected item

	// Finally, enable the gadget, and trim its length down if necessary to fit the
	// number of displayed items.
//	DialogManager::SetComboListLength(ParentDlg, ParentGadget);  -- causes nasty redraw of document behind!
	DialogManager::EnableGadget(ParentDlg, ParentGadget, TRUE);

	return(Result);
}



/********************************************************************************************

>	BOOL ColourDropDown::SetSelection(IndexedColour *Selected, INT32 SelectIndex = -1)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		1/9/95

	Inputs:		Selected - NULL or a pointer to the selected colour
					(Only one of Selected, SelectedIndex should indicate the selection)

				SelectIndex - -1 or the 0-based index of the special item to select
					(Only one of Selected, SelectedIndex should indicate the selection.
					SelectIndex will actually override Selected if it is valid)

	Returns:	TRUE if it set the selection OK

	Purpose:	To set the selected item in the list

	SeeAlso:	ColourDropDown::DecodeSelection; ColourDropDown::FillInColourList

********************************************************************************************/

BOOL ColourDropDown::SetSelection(IndexedColour *Selected, INT32 SelectIndex)
{
	ERROR2IF(ParentDlg == NULL, FALSE, "ColourDropDown not properly initialised");

	INT32 SelectedIndex = 0;

	if (SelectIndex >= 0 && SelectIndex < (INT32)SpecialEntries.GetCount())		// Use special item
		SelectedIndex = SelectIndex;
	else if (Selected != NULL)													// Use colour ptr
	{
		INT32 Index = (SpecialEntries.IsEmpty()) ? 0 : (SpecialEntries.GetCount() + 1);
		INT32 MaxIndex = GetNumberOfItems();

		while (Index < MaxIndex)
		{
			if (Selected == (IndexedColour *) GetItemData(Index))
			{
				SelectedIndex = Index;
				break;
			}

			Index++;
		}
	}

	SetSelectedIndex(SelectedIndex);						// And set the appropriate selected item
	return(TRUE);
}



/********************************************************************************************

>	IndexedColour *ColourDropDown::DecodeSelection(INT32 SelectionIndex)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		29/8/95

	Inputs:		SelectionIndex - The index of the item (which is usually the selected one)
				in the combo box.

	Returns:	NULL if the SelectionIndex should be treated as a "special item", else
				a pointer to the selected IndexedColour.
				(Note: If the list is empty, it will also return NULL)

	Purpose:	Decodes an index in the dropdown list back into either a special-item index
				or an IndexedColour pointer. (See AddSpecialEntry for details)

	Notes:		The colour list can be augmented with a number of "special" entries.
				Each call to this method adds another entry to the bottom of the special
				entries list. If there are special entries, they are added to to top
				of the colour list, with a divider item between them and the colours.
				There can only be one divider.

				Special entries are indexed by their order of addition, with the first
				entry being number 0. When decoding a selection, DecodeSelection will
				return an IndexedColour pointer (if a colour was selected), or NULL,
				in which case, use the Index you passed in to determine which Special
				Entry was selected. Note that the divider will be treated as a special
				entry (e.g. with 2 special items, you can get an entry 2 selected - this
				corresponds to the divider, and should be ignored)

	SeeAlso:	ColourDropDown::FillInColourList; ColourDropDown::AddSpecialEntry

********************************************************************************************/

IndexedColour *ColourDropDown::DecodeSelection(INT32 SelectionIndex)
{
	// If it's at the top of the list, it could be a "special" (extra) entry
	if (!SpecialEntries.IsEmpty() && SelectionIndex <= (INT32)SpecialEntries.GetCount())
		return(NULL);

	INT32 Data = GetItemData(SelectionIndex);
	if (Data == -1)				// If the list is empty, we will get a -1 back, so weed it out
		return(NULL);

	// Otherwise, it must be a colour, so fetch the colour pointer...
	return( (IndexedColour *) Data);
}



/********************************************************************************************

>	static void ColourDropDown::UpdateForNewColourList(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		6/10/95

	Purpose:	Called by the ColourDropMsgHandler whenever a colour list changes or
				we swap to a different document, etc. Will automatically clear the
				list and shade it if there are no documents around.

				Basically just calls FillInColourList for all active ColourDropDowns

	Scope:		sorta private

********************************************************************************************/

void ColourDropDown::UpdateForNewColourList(void)
{
	Document *SelDoc = Document::GetSelected();

	if (SelDoc != NULL)		// No need to shade, so return now
		return;

	DropDown *Ptr = (DropDown *) CurrentDropDowns.GetHead();
	while (Ptr != NULL)
	{
		if (Ptr->IsKindOf(CC_RUNTIME_CLASS(ColourDropDown)))
		{
			ColourDropDown *CPtr = (ColourDropDown *)Ptr;
			if (CPtr->AutoUpdateOnChange)
			{
				CPtr->ClearList();
				DialogManager::EnableGadget(CPtr->ParentDlg, CPtr->ParentGadget, FALSE);	// Shade
			}
		}

		Ptr = (DropDown *) CurrentDropDowns.GetNext(Ptr);
	}
}



/********************************************************************************************

>	static void ColourDropDown::RedrawColourDropDowns(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		23/5/96

	Purpose:	Called by the ColourDropMsgHandler whenever the selected view's colour
				context is changed, so we redraw using the new correction/separation
				options.

	Scope:		sorta private

********************************************************************************************/

void ColourDropDown::RedrawColourDropDowns(void)
{
	DropDown *Ptr = (DropDown *) CurrentDropDowns.GetHead();
	while (Ptr != NULL)
	{
		if (Ptr->IsKindOf(CC_RUNTIME_CLASS(ColourDropDown)))
		{
			ColourDropDown *CPtr = (ColourDropDown *)Ptr;
			DialogManager::InvalidateGadget(CPtr->ParentDlg, CPtr->ParentGadget);
		}

		Ptr = (DropDown *) CurrentDropDowns.GetNext(Ptr);
	}
}



/********************************************************************************************

>	virtual BOOL ColourDropDown::HasIcon(DWORD ItemData)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/9/95

	Inputs:		ItemData - Your item data

	Returns:	TRUE if this item needs an icon to the left of it, FALSE if not

	Purpose:	Determine if an item needs an icon next to it

	Notes:		Called by HandleDrawItemInternal when this object has been identified as the 
				owner of the control to be redrawn.

				This method MUST be overridden by derived classes to provide redraw of their
				ColourDropDown list items. The base class returns FALSE

				If you return TRUE, you must also provide the DrawIcon method

	SeeAlso:	ColourDropDown::DrawIcon; ColourDropDown::DrawText

********************************************************************************************/

BOOL ColourDropDown::HasIcon(DWORD ItemData)
{
	// All our items have a colour splodge
	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL ColourDropDown::DrawIcon(DWORD ItemData, HDC hDC, RECT *IconRect,
											BOOL Disabled)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/9/95

	Inputs:		ItemData - Your item data
				hDC - The DC to render into
				IconRect - points at a rectangle (square in fact) to be drawn within
				Disabled - TRUE if this item is disabled so the icon should be drawn greyed

	Returns:	TRUE if this item needs an icon to the left of it, FALSE if not

	Purpose:	Draws the icon for an item

	Notes:		Called by HandleDrawItemInternal when this object has been identified as the 
				owner of the control to be redrawn, if HasIcon returned TRUE

				This method MUST be overridden by derived classes to provide redraw of their
				ColourDropDown list items. The base class does nothing.

				On entry, the DC is ready for you to draw into, including having the camelot
				palette selected in etc.

	SeeAlso:	ColourDropDown::DrawIcon; ColourDropDown::DrawText

********************************************************************************************/

BOOL ColourDropDown::DrawIcon(DWORD ItemData, HDC hDC, RECT *IconRect, BOOL Disabled)
{
	GDrawBrush.Init(hDC);
	GDrawBrush.Start();

	LOGBRUSH LogBrush;
	HGDIOBJ OldPen;
	DWORD rgb;

	BOOL IsSpotColour = FALSE;

	if (Disabled)
	{
		// The item is disabled, so just put a light grey splodge in the place of the colour
		rgb = RGB(192, 192, 192);
	 	OldPen = ::SelectObject(hDC, ::GetStockObject(NULL_PEN));
	}
	else
	{
		ColourContextRGBT *CCrgbt = (ColourContextRGBT *)
				ColourManager::GetColourContext(COLOURMODEL_RGBT, DocView::GetSelected());

		ERROR3IF(CCrgbt == NULL, "No available RGB colour context?!");

		CCObject *Item = (CCObject *)ItemData;
		if (Item->IsKindOf(CC_RUNTIME_CLASS(IndexedColour)))
		{
			DocColour Bob;
			Bob.MakeRefToIndexedColour((IndexedColour *)ItemData);
			rgb = ConvertColourToScreenWord(CCrgbt, &Bob);

			if ( ((IndexedColour *) ItemData)->IsSpotOrTintOfSpot() )
				IsSpotColour = TRUE;
		}
		else
			rgb = ConvertColourToScreenWord(CCrgbt, &(((SpecialEntry *)ItemData)->ItemColour));

	 	OldPen = ::SelectObject(hDC, ::GetStockObject(BLACK_PEN));
	}

	if (GDrawBrush.Available())
		GDrawBrush.GetLogBrush(rgb, &LogBrush);
	else
	{
		LogBrush.lbStyle = BS_SOLID;
		LogBrush.lbColor = rgb;
		LogBrush.lbHatch = 0;
	}

	HBRUSH TheBrush  = ::CreateBrushIndirect(&LogBrush);
	HGDIOBJ OldBrush = ::SelectObject(hDC, TheBrush);

	if (IsSpotColour)
		::Ellipse(hDC, IconRect->left, IconRect->top, IconRect->right, IconRect->bottom);
	else
		::Rectangle(hDC, IconRect->left, IconRect->top, IconRect->right, IconRect->bottom);

	::SelectObject(hDC, OldBrush);
	::DeleteObject(TheBrush);

 	::SelectObject(hDC, OldPen);

	// Finish with GBrush
	GDrawBrush.Stop();

	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL ColourDropDown::DrawText(DWORD ItemData, HDC hDC, RECT *TextRect)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/9/95

	Inputs:		ItemData - Your item data
				HDC - the DC to draw into
				TextRect - points at a rectangle in which the text should be rendered

	Returns:	TRUE if redraw went well

	Purpose:	Draws the text for an item

	Notes:		Called by HandleDrawItemInternal when this object has been identified as the 
				owner of the control to be redrawn, if HasIcon returned TRUE

				This method MUST be overridden by derived classes to provide redraw of their
				ColourDropDown list items. The base class draws nothing.

				Note that on entry, the text FG/BG colours have been set up appropriately
				for the state of the item (shaded, selected, etc)
				Basically, all you have to do is find the text and do a DrawText call.

	SeeAlso:	ColourDropDown::DrawIcon; ColourDropDown::DrawText

********************************************************************************************/

BOOL ColourDropDown::DrawText(DWORD ItemData, HDC hDC, RECT *TextRect)
{
	ERROR3IF(ItemData == NULL || TextRect == NULL, "NULL Itemdata/TextRect in ColourDropDown::DrawText");

	// Determine if it's a colour or a special item, and find the text to draw
	LPCTSTR TextToDraw = NULL;

	CCObject *Item = (CCObject *) ItemData;
	if (Item->IsKindOf(CC_RUNTIME_CLASS(IndexedColour)))
		TextToDraw = (LPCTSTR) ( (TCHAR *) (*((IndexedColour *)ItemData)->GetName()) );
	else
		TextToDraw = (LPCTSTR) ( (TCHAR *) ((SpecialEntry *)ItemData)->ItemText );

	// Draw the text...
	if (TextToDraw != NULL)
		::DrawText(hDC, TextToDraw, -1, TextRect, DT_LEFT | DT_VCENTER);

	return(TRUE);
}








/********************************************************************************************

>	ColourDropMsgHandler::ColourDropMsgHandler()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/95
	Purpose:	Constructs a ColourDropMsgHandler, an object which, surprisingly,
				handles messages for its parent ColourDropDown.
	Errors:		-
	SeeAlso:	MessageHandler

********************************************************************************************/

ColourDropMsgHandler::ColourDropMsgHandler()
  : MessageHandler(CC_RUNTIME_CLASS(MessageHandler), TRUE)
{
}



/********************************************************************************************

>	virtual MsgResult ColourDropMsgHandler::Message(Msg* Message)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/95
	Inputs:		Message: The message

	Purpose:	Process messages on behalf of ColourDropDowns

********************************************************************************************/

MsgResult ColourDropMsgHandler::Message(Msg* Message)
{
	if (MESSAGE_IS_A(Message, ColourChangingMsg))
	{
		ColourChangingMsg *Msg = (ColourChangingMsg *) Message;
		switch ( Msg->State )
		{
			case ColourChangingMsg::ColourState::LISTDESELECTED:
//			case ColourChangingMsg::ColourState::LISTPAGED:
//			case ColourChangingMsg::ColourState::LISTUPDATED:
//			case ColourChangingMsg::ColourState::COLOURUPDATED:
				ColourDropDown::UpdateForNewColourList();
				break;

			case ColourChangingMsg::ColourState::SELVIEWCONTEXTCHANGE:
				// The colour context for the selected view has chnaged, so we must immediately
				// force a redraw to make sure we show the colours with correction/separation.
				ColourDropDown::RedrawColourDropDowns();
				break;
		}
	}

	return OK; 
}

