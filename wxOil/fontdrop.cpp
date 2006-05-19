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

// fontdrop.cpp - Font drop-down list support

/*
*/


//-----------------------------------------------------------------------------------------
// Include files

#include "camtypes.h"
#include "fontdrop.h"

#include "camelot.h"
#include "errors.h"
#include "fontbase.h"
#include "textinfo.h"
#include "gbrush.h"
//#include "textres.h"
//#include "richard.h"
//#include "richard2.h"

// These required for the document drop font handling class
#include "app.h"		// FONTMANAGER
#include "progress.h"	// BeginSlowJob ...
//#include "mario.h"		// _R(IDS_NO_FONTSINDOC)
#include "fontlist.h"
#include "fontman.h"	// Fontmanager
#include "cartprov.h"
//#include "will2.h"		// _R(IDS_K_FINFODLG_DASH)

class Document;

//-----------------------------------------------------------------------------------------
// Implementation of dynamic classes

CC_IMPLEMENT_DYNCREATE(FontDropItem, CCObject);
CC_IMPLEMENT_DYNCREATE(FontDropEnumFont, OILEnumFonts);
CC_IMPLEMENT_DYNCREATE(FontDropDown, ListItem);
CC_IMPLEMENT_DYNCREATE(DocumentFontDropDown, ListItem);


//-----------------------------------------------------------------------------------------
// This must be defined AFTER all CC_IMPLEMENT_DYNCREATE calls
#define new CAM_DEBUG_NEW


//-----------------------------------------------------------------------------------------
// Enumerating fonts uses a callback function, which in turn needs to access the original FontDropDown caller...
void * FontDropDown::CurrentFontDropDown = 0;

/********************************************************************************************

>	FontDropItem::FontDropItem()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Date:		3/10/95

	Purpose:	Default FontDropDownListItem constructor

********************************************************************************************/

FontDropItem::FontDropItem()
{
	FontName = _T("");
	Type = FC_UNDEFINED;
}

/********************************************************************************************

>	FontDropItem::FontDropItem(TCHAR *Name, FontClass FontType)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Date:		3/10/95

	Inputs:		Name		- Font Typeface name that will be displayed and used
				FontType	- Type of Font this item is describing - FC_ATM for example

	Purpose:	Better FontDropDownListItem constructor

********************************************************************************************/

FontDropItem::FontDropItem(TCHAR *Name, FontClass FontType)
{
	FontName = Name;
	Type = FontType;
}


/********************************************************************************************

>	BOOL FontDropEnumFont::NewFont(Type, ENUMLOGFONT *lpelf)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Date:		3/10/95

	Inputs:		Type		- Type of font
				lpelf		- Logfont pointer
				lpntm		- NewTextMetrics pointer
				lppnse		- Panose pointer

	Returns:	TRUE if the font was added to the list ok.

	Purpose:	Called for each font installed on the system. Effectively creates the FontPullDown
				items from the given info.

********************************************************************************************/

BOOL FontDropEnumFont::NewFont(FontClass Type, ENUMLOGFONT *lpelf)
{
	count++;
	
	ERROR3IF(((FontDropDown *)FontDropDown::CurrentFontDropDown) == NULL, "CurrentFontDropDown is NULL in NewFont");
	if(((FontDropDown *)FontDropDown::CurrentFontDropDown) != NULL)
		return ((FontDropDown *)FontDropDown::CurrentFontDropDown)
				-> AddFontToList((TCHAR *)lpelf->elfLogFont.FaceName, Type);

	return FALSE;
}


/********************************************************************************************

>	FontDropDown::FontDropDown()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Date:		4/10/95

	Purpose:	Default constructor for FontDropDown Class...
	Notes:		If you mistakenly replace the F with a D you get a great song title...

********************************************************************************************/

FontDropDown::FontDropDown()
{
	TheTopItem.FontName = _R(IDS_ATM_DEFAULT); // "Default";
	TheTopItem.Type = FC_UNDEFINED;
}

/********************************************************************************************

>	FontDropDown::~FontDropDown()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Date:		5/10/95

	Purpose:	Default destructor for FontDropDown Class...
	Notes:		If you mistakenly replace the F with a D you get a great song title...

********************************************************************************************/

FontDropDown::~FontDropDown()
{

}


/********************************************************************************************

>	virtual BOOL FontDropDown::Init(CWindowID Window, CGadgetID Gadget)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Date:		26/10/95

	Inputs:		Window - The Window in which your dropdown list gadget resides
				Gadget - The GadgetID of the deropdown list gadget

	Returns:	TRUE if initialised ok...

	Purpose:	FontDropDown initialiser

********************************************************************************************/

BOOL FontDropDown::Init(CWindowID Window, CGadgetID Gadget)
{
	BOOL ok = DropDown::Init(Window, Gadget);

	return ok;
}

/********************************************************************************************

>	void FontDropDown::KillList(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Date:		5/10/95

	Purpose:	Kills all the items in the drop down and deletes them...

********************************************************************************************/

void FontDropDown::KillList(void)
{
	ClearList();

	ListItem *Item = Fonts.GetTail();
    
    // Loop through list, deleting items and reclaiming memory
	while(Item != NULL)
	{              
		ListItem* Prev = Fonts.GetPrev(Item);
		delete( (FontDropItem*)((Fonts.RemoveItem(Item))) );
		Item = Prev;
	}
}


/********************************************************************************************

>	BOOL FontDropDown::AddFontToList(TCHAR *FontName, FontClass Type)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Date:		3/10/95

	Inputs:		FontName	- Name of font
				Type		- Type of font

	Returns:	TRUE if the font was added to the list ok.

	Purpose:	Called for each font installed on the system. Effectively creates the FontPullDown
				items from the given info.

********************************************************************************************/

BOOL FontDropDown::AddFontToList(TCHAR *FontName, FontClass Type)
{
	FontDropItem *Item = new FontDropItem(FontName, Type);
	if(Item == NULL)
	{
		ERROR3("FontDropDown::AddFontToList couldn't create Item");
		return FALSE;
	}

	// Add the item into the list, in a sorted fashion...
	ListItem *TheItem = Fonts.GetHead();

	if(TheItem == NULL)
	{
		Fonts.AddTail(Item);
		return TRUE;
	}
	
	if( ((FontDropItem *)TheItem)->FontName > String_64(FontName))
	{
		Fonts.InsertBefore(TheItem, Item);
		return TRUE;
	}

	while (TheItem != NULL)
	{
		ListItem *TheNextItem = Fonts.GetNext(TheItem);

		if(TheNextItem == NULL)
		{
			Fonts.InsertAfter(TheItem, Item);
			return TRUE;
		}

		if(((FontDropItem *)TheNextItem)->FontName > String_64(FontName))
		{
			Fonts.InsertBefore(TheNextItem, Item);
			return TRUE;
		}

		// Try the next item
		TheItem = TheNextItem;
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL FontDropDown::FillInFontList()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Date:		3/10/95

	Inputs:		

	Returns:	TRUE if it filled in the list successfully

	Purpose:	"Fills in" the attached control with appropriate entries for the current
				FontDropDown settings

	SeeAlso:	FontDropDown::DecodeSelection; FontDropDown::AddSpecialEntry

********************************************************************************************/

BOOL FontDropDown::FillInFontList()
{
	ERROR2IF(ParentDlg == NULL, FALSE, "FontDropDown not properly initialised");

	SetListRedraw(FALSE);										// Disable redraw while updating
	ClearList();												// Delete combobox contents

	// Setup the static class pointer variable so we can add things to this dropdown...
	CurrentFontDropDown = (void *)this;

	if(Fonts.GetCount() == 0)
	{
		FontDropEnumFont EnumObj;
		EnumObj.Execute();

		// Loop through the list, checking for ATM / TTF duplicates, and remove the TTF ones from the list
		FontDropItem *Item = (FontDropItem *)Fonts.GetHead();
		while (Item != NULL)
		{
			FontClass ItemType = Item->Type;
			if(ItemType == FC_ATM)
			{
				// It's an ATM font, go through whole list checking for TTF equiv.
				BOOL Removed = FALSE;
				String_64 ItemName(Item->FontName);
				FontDropItem *InnerLoopItem = (FontDropItem *)Fonts.GetHead();
				while (InnerLoopItem != NULL && Removed == FALSE)
				{
					// Have we found our font ? Remove it if we have...
					if((InnerLoopItem->Type == FC_TRUETYPE) && (InnerLoopItem->FontName == ItemName))
					{
						Fonts.RemoveItem((ListItem *)InnerLoopItem);
						delete InnerLoopItem;
						InnerLoopItem = NULL;
						Removed = TRUE;
					}

					// Get the next item
					if(!Removed)
						InnerLoopItem = (FontDropItem *)Fonts.GetNext((ListItem *)InnerLoopItem);
				}
			}
			// Get the next item
			Item = (FontDropItem *)Fonts.GetNext((ListItem *)Item);
		}
	}

	ListItem *Item = Fonts.GetHead();

	while (Item != NULL)
	{
		// Add the font in the list to the combo box
		AddItem((void *) Item);

		// Try the next item
		Item = Fonts.GetNext(Item);
	}

	SetListRedraw(TRUE);									// Re-enable redraw

	return(TRUE);
}



/********************************************************************************************

>	BOOL FontDropDown::SetSelection(FontDropItem *TheFont)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Date:		4/10/95

	Inputs:		TheFont 	- FontDropItem to select

	Returns:	TRUE if it set the selection OK

	Purpose:	To set the selected item in the list

	SeeAlso:	FontDropDown::DecodeSelection; FontDropDown::FillInColourList

********************************************************************************************/

BOOL FontDropDown::SetSelection(FontDropItem *TheFont)
{
	ERROR2IF(ParentDlg == NULL, FALSE, "FontDropDown not properly initialised");

	INT32 SelectedIndex = -1;

	INT32 Index = 0;
	INT32 MaxIndex = GetNumberOfItems();

	while (Index < MaxIndex)
	{
		if ( (TheFont->FontName == ((FontDropItem *) GetItemData(Index))->FontName)
			 && (TheFont->Type == ((FontDropItem *) GetItemData(Index))->Type) )
		{
			SelectedIndex = Index;
			break;
		}

		Index++;
	}

	SetSelectedIndex(SelectedIndex);						// And set the appropriate selected item
  
	return(TRUE);
}


/********************************************************************************************

>	FontDropItem *FontDropDown::DecodeSelection(INT32 SelectionIndex)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Date:		4/10/95

	Inputs:		SelectionIndex - The index of the item (which is usually the selected one)
				in the combo box.

	Returns:	A FontDropItem which is selected... Or NULL if nothing's selected
				NOTE that if the list is empty, it will return NULL

	Purpose:	Decodes an index in the dropdown list back into a font

	SeeAlso:	FontDropDown::FillInColourList; FontDropDown::AddSpecialEntry

********************************************************************************************/

FontDropItem *FontDropDown::DecodeSelection(INT32 SelectionIndex)
{
	FontDropItem *Fred = (FontDropItem *) GetItemData(SelectionIndex);

	if (Fred==&TheTopItem)
		return(NULL);

	return Fred;
}


/********************************************************************************************

>	virtual BOOL FontDropDown::HasIcon(void * ItemData)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/9/95

	Inputs:		ItemData - Your item data

	Returns:	TRUE if this item needs an icon to the left of it, FALSE if not

	Purpose:	Determine if an item needs an icon next to it

	Notes:		Called by HandleDrawItemInternal when this object has been identified as the 
				owner of the control to be redrawn.

				This method MUST be overridden by derived classes to provide redraw of their
				FontDropDown list items. The base class returns FALSE

				If you return TRUE, you must also provide the DrawIcon method

	SeeAlso:	FontDropDown::DrawIcon; FontDropDown::DrawText

********************************************************************************************/

BOOL FontDropDown::HasIcon(void * ItemData)
{
	// All our items have a space for a truetype icon to their left
	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL FontDropDown::DrawIcon(void * ItemData, wxDC& dc, wxRect& IconRect,
										BOOL Disabled, INT32 flags)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Date:		4/10/95

	Inputs:		ItemData - Your item data
				hDC - The DC to render into
				IconRect - points at a rectangle (square in fact) to be drawn within
				Disabled - TRUE if this item is disabled so the icon should be drawn greyed

	Returns:	TRUE if this item needs an icon to the left of it, FALSE if not

	Purpose:	Draws the icon for an item

	Notes:		Called by HandleDrawItemInternal when this object has been identified as the 
				owner of the control to be redrawn, if HasIcon returned TRUE

				This method MUST be overridden by derived classes to provide redraw of their
				FontDropDown list items. The base class does nothing.

				On entry, the DC is ready for you to draw into, including having the camelot
				palette selected in etc.

	SeeAlso:	FontDropDown::DrawIcon; FontDropDown::DrawText

********************************************************************************************/

BOOL FontDropDown::DrawIcon(void * ItemData, wxDC& dc, wxRect& IconRect, BOOL Disabled, INT32 flags)
{
	ResourceID BitmapID = 0;
	FontClass Type;

	if ((ItemData==&TheTopItem) || !ItemData)
		Type = TheTopItem.Type;
	else
		Type = ((FontDropItem *)ItemData)->Type;

	switch(Type)
	{
		case FC_TRUETYPE:
			BitmapID = _R(IDB_TTF_SYMBOL);
			break;

		case FC_ATM:
			BitmapID = _R(IDB_ATM_SYMBOL);
			break;

		default:
			BitmapID = 0;
			break;
	}

	if(BitmapID == 0)
		return FALSE;

	wxBitmap * pBitmap = CamArtProvider::Get()->FindBitmap(BitmapID, (CamArtFlags)(CAF_DEFAULT|(Disabled?CAF_GREYED:0)));
	if (!pBitmap)
		return FALSE;

	dc.DrawBitmap(*pBitmap, IconRect.GetLeft(), IconRect.GetTop(), TRUE);

	return TRUE;
}



/********************************************************************************************

>	virtual wxString FontDropDown::GetText(void * ItemData, INT32 Item)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Date:		3/10/95

	Inputs:		ItemData - Your item data

	Returns:	The text

********************************************************************************************/

wxString FontDropDown::GetText(void * ItemData, INT32 Item)
{
	ERROR3IF(ItemData == NULL, "NULL Itemdata in FontDropDown::DrawText");

	String_64 TextToDisplay(_T(""));

	if (!ItemData || Item<0 || (ItemData==&TheTopItem))
		TextToDisplay = TheTopItem.FontName;
	else
		TextToDisplay = (((FontDropItem *)ItemData)->FontName);

	// and draw the text...
	return(wxString((TCHAR *)TextToDisplay));
}




/********************************************************************************************

>	BOOL FontDropDown::SetTopFontName(StringBase* StrValue, FontClass Type, BOOL Deselect = TRUE)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Date:		4/10/95

	Inputs:		StrValue	- Font name, or whatever you want in the icon above the pull down
				Type 		- Type of font - TTF, ATM, etc....
				Deselect	- If we do an explicit SetSelectedIndex (Or SetSelected) after this
								call, then set this to FALSE to avoid unneccesary flicker
	Returns:	TRUE if ok

	Purpose:	Changes the string and bmp in the icon above the font pull down to reflect
				whatever you give it...

	SeeAlso:	DropDown::HasIcon; DropDown::DrawIcon; DropDown::DrawText

********************************************************************************************/

BOOL FontDropDown::SetTopFontName(StringBase* StrValue, FontClass Type, BOOL Deselect)
{
	TheTopItem.FontName = *StrValue;
	TheTopItem.Type = Type;

	// Deselect any selected items
	if(Deselect)
		SetSelectedIndex(-1);

	return TRUE;
}       

/********************************************************************************************
***** 			DocumentFontDropDown class												*****
********************************************************************************************/

/********************************************************************************************

>	DocumentFontDropDown::DocumentFontDropDown()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/11/95

	Purpose:	Default constructor for DocumentFontDropDown Class...

********************************************************************************************/

DocumentFontDropDown::DocumentFontDropDown()
{
	TheTopItem.FontName = _R(IDS_NO_FONTSINDOC);
	TheTopItem.Type = FC_UNDEFINED;
}

/********************************************************************************************

>	DocumentFontDropDown::~DocumentFontDropDown()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/11/95

	Purpose:	Default destructor for DocumentFontDropDown Class...

********************************************************************************************/

DocumentFontDropDown::~DocumentFontDropDown()
{
	KillList();
}


/********************************************************************************************

>	BOOL DocumentFontDropDown::FillInFontList(Document * WorkDoc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/11/95

	Inputs:		WorkDoc		pointer to the document to extact the font list from, NULL if none
							which means show blank list

	Returns:	TRUE if it filled in the list successfully

	Purpose:	"Fills in" the attached control with appropriate entries for the current
				DocumentFontDropDown settings. To do this it reads the current fonts from the
				specified document. If the document is NULL then it will show a blank list.
				If no document fonts are in use then it will also show a blank list and fill
				in the top entry with a suitable message.

	SeeAlso:	FontDropDown::DecodeSelection; FontDropDown::AddSpecialEntry

********************************************************************************************/

BOOL DocumentFontDropDown::FillInFontList(Document * WorkDoc)
{
	ERROR2IF(ParentDlg == NULL, FALSE, "FontDropDown not properly initialised");

	BeginSlowJob();

	SetListRedraw(FALSE);	// Disable redraw while updating
	KillList();				// Delete combobox contents and empty the fonts list
	//ClearList();			// Delete combobox contents

	// Setup the static class pointer variable so we can add things to this dropdown...
	CurrentFontDropDown = (void *)this;

	//if(Fonts.GetCount() == 0)
	//{
	if (WorkDoc == NULL)
	{
		// Fill in one item in the list which is no document fonts being used
		String_8 DashString(_R(IDS_K_FINFODLG_DASH));
		TheTopItem.FontName = DashString;
		TheTopItem.Type = FC_UNDEFINED;
		//AddFontToList((TCHAR*)&TheTopItem.FontName, TheTopItem.Type);
	}
	else
	{
		// Build the font list for the specified document
		// This will NOT be alphabetical
		FontList DocFonts;
		DocFonts.Build(WorkDoc);

		FontListItem* FontItem = DocFonts.GetFirstItem();
		if (FontItem == NULL)
		{
			// Fill in one item in the list which is no document fonts being used
			TheTopItem.FontName = _R(IDS_NO_FONTSINDOC);
			TheTopItem.Type = FC_UNDEFINED;
			//AddFontToList((TCHAR*)&TheTopItem.FontName, TheTopItem.Type);
		}
		else
		{
			//BOOL FirstInList = TRUE;
			
			// fill up the list
			while (FontItem != NULL)
			{
				// get the name
				String_64 Name = FontItem->GetFontName();
				
				WORD Handle = FONTMANAGER->GetFontHandle(&Name);

				// check the style
				INT32 Style = FontItem->GetFontStyle();
				if(Style & 1)
					Name += _T(" -Bold");
				if(Style & 2)
					Name += _T(" -Italic");
				
				if(Handle > 0)
				{
					if (FONTMANAGER->IsFontReplaced(Handle))
						Name += _T(" *");
				}
				FontClass Type = FontItem->GetFontClass();

				AddFontToList(Name, Type);
				
		 		FontItem = DocFonts.GetNextItem(FontItem);

				ContinueSlowJob();
			}
		}
	}
	//}

	// Work out the top item, if any, on the alphabetically sorted list
	// ready for putting in the top combo box item
	FontDropItem *pTopItem = (FontDropItem *)Fonts.GetHead();
	if (pTopItem != NULL)
	{
		// Fill in the top item in the list
		TheTopItem.FontName = pTopItem->FontName;
		TheTopItem.Type = pTopItem->Type;
	}

	ListItem *Item = Fonts.GetHead();

	while (Item != NULL)
	{
		// Add the font in the list to the combo box
		AddItem((void *) Item);

		// Try the next item
		Item = Fonts.GetNext(Item);
	}

	// Re-enable redraw
	SetListRedraw(TRUE);

	// We have no concept of a selected font
	SetSelectedIndex(-1);

	EndSlowJob();

	return(TRUE);
}
